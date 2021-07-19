/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <algorithm>
#include <boost/algorithm/clamp.hpp>
#include "Config/Config.h"
#include "Maps/MapManager.h"
#include "Items/PcInventory.h"
#include "Packets/S_AddSkill.hpp"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "SQLStorages.h"
#include "WorldSession.h"
#include "Corpse.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "Player.h"

const uint8 HEALTH_REGEN_INTERVAL_TICKS[] = {30, 25, 20, 16, 14, 12, 11, 10, 9, 3, 2};

const uint16 CLASSID_KNIGHT_MALE = 61;
const uint16 CLASSID_KNIGHT_FEMALE = 48;
const uint16 CLASSID_ELF_MALE = 138;
const uint16 CLASSID_ELF_FEMALE = 37;
const uint16 CLASSID_WIZARD_MALE = 734;
const uint16 CLASSID_WIZARD_FEMALE = 1186;
const uint16 CLASSID_DARK_ELF_MALE = 2786;
const uint16 CLASSID_DARK_ELF_FEMALE = 2796;
const uint16 CLASSID_PRINCE = 0;
const uint16 CLASSID_PRINCESS = 1;
const uint16 CLASSID_DRAGON_KNIGHT_MALE = 6658;
const uint16 CLASSID_DRAGON_KNIGHT_FEMALE = 6661;
const uint16 CLASSID_ILLUSIONIST_MALE = 6671;
const uint16 CLASSID_ILLUSIONIST_FEMALE = 6650;

static const std::map<Classes, std::map<Gender, uint16>> CLASS_GFX_IDS = {
	{CLASS_PRINCE, {{GENDER_MALE, CLASSID_PRINCE}, {GENDER_FEMALE, CLASSID_PRINCESS}}},
	{CLASS_KNIGHT, {{GENDER_MALE, CLASSID_KNIGHT_MALE}, {GENDER_FEMALE, CLASSID_KNIGHT_FEMALE}}},
	{CLASS_ELF, {{GENDER_MALE, CLASSID_ELF_MALE}, {GENDER_FEMALE, CLASSID_ELF_FEMALE}}},
	{CLASS_WIZARD, {{GENDER_MALE, CLASSID_WIZARD_MALE}, {GENDER_FEMALE, CLASSID_WIZARD_FEMALE}}},
	{CLASS_DARK_ELF, {{GENDER_MALE, CLASSID_DARK_ELF_MALE}, {GENDER_FEMALE, CLASSID_DARK_ELF_FEMALE}}},
	{CLASS_DRAGON_KNIGHT, {{GENDER_MALE, CLASSID_DRAGON_KNIGHT_MALE}, {GENDER_FEMALE, CLASSID_DRAGON_KNIGHT_FEMALE}}},
	{CLASS_ILLUSIONIST, {{GENDER_MALE, CLASSID_ILLUSIONIST_MALE}, {GENDER_FEMALE, CLASSID_ILLUSIONIST_FEMALE}}},
};

uint16 Player::GetGfxId(Classes classes, Gender gender)
{
	return CLASS_GFX_IDS.at(classes).at(gender);
}

Classes Player::GetClasses(uint16 gfxId)
{
    switch (gfxId)
    {
        case CLASSID_PRINCE:
        case CLASSID_PRINCESS:
            return CLASS_PRINCE;
        case CLASSID_KNIGHT_MALE:
        case CLASSID_KNIGHT_FEMALE:
            return CLASS_KNIGHT;
        case CLASSID_ELF_MALE:
        case CLASSID_ELF_FEMALE:
            return CLASS_ELF;
        case CLASSID_WIZARD_MALE:
        case CLASSID_WIZARD_FEMALE:
            return CLASS_WIZARD;
        case CLASSID_DARK_ELF_MALE:
        case CLASSID_DARK_ELF_FEMALE:
            return CLASS_DARK_ELF;
        case CLASSID_DRAGON_KNIGHT_MALE:
        case CLASSID_DRAGON_KNIGHT_FEMALE:
            return CLASS_DRAGON_KNIGHT;
        case CLASSID_ILLUSIONIST_MALE:
        case CLASSID_ILLUSIONIST_FEMALE:
            return CLASS_ILLUSIONIST;
        default:
            return CLASS_NONE;
    }
}

Player::Player(WorldSession* session) : Unit(), m_session(session), m_camera(this)
{
	m_valuesCount = PLAYER_END;

	for (uint8 i = 0; i < MAX_STATS; ++i)
	{
		m_baseStat[i] = 0;
		m_createStat[i] = 0;
	}
	for (uint8 i = 0; i < MAX_ATTACK; ++i)
	{
		m_baseAttackDamage[i] = 0;
		m_createAttackDamage[i] = 0;
		m_baseAttackHit[i] = 0;
		m_createAttackHit[i] = 0;
	}
	m_createMagicHit = 0;
	m_createMagicDamage = 0;
	m_createMagicCritical = 0;
	m_baseArmor = 0;
	m_createArmor = 0;
	m_baseMaxMana = 0;
	m_baseMaxHealth = 0;
	m_createHealthGrowth = 0;
	m_createManaGrowth = 0;
	m_totalManaRegen = 0;
	m_manaRegen = 0;
	m_createManaRegen = 0;
	m_totalHealthRegen = 0;
	m_healthRegen = 0;
	m_createHealthRegen = 0;
	m_baseDefense = 0;
	m_createDefense = 0;
	m_baseSp = 0;
	m_baseDodge = 0;
	m_createDodge = 0;
	m_createStrengthWeightReduction = 0;
	m_createStaminaWeightReduction = 0;
	m_createMagicCostReduction = 0;
	m_damageReduction = 0;
	m_weightReduction = 0;

	m_bag = new PcInventory(this);
}

Player::~Player()
{
	if (m_bag != nullptr)
	{
		// todo: 是否要单独保存DB还是在玩家保存的时候顺便保存
		delete m_bag;
	}
}

void Player::CleanupsBeforeDelete()
{
	Unit::CleanupsBeforeDelete();
}

void Player::AddToWorld()
{
	if (!IsInWorld())
	{
		sObjectAccessor.AddObject(this);
	}

	Unit::AddToWorld();
}

void Player::RemoveFromWorld()
{
	if (IsInWorld())
	{
		GetCamera().ResetView();
		sObjectAccessor.RemoveObject(this);
	}

	Unit::RemoveFromWorld();
}

void Player::Update(uint32 update_diff, uint32 p_time)
{
    if (!IsInWorld())
	{
    	return;
	}

	// SetCanDelayTeleport(true);
    // Unit::Update(update_diff, p_time);
	// SetCanDelayTeleport(false);

	/*if (m_deathState == JUST_DIED)
	{
		KillPlayer();
	}*/
}

void Player::KillPlayer()
{
	SetDeathState(CORPSE);
	UpdateObjectVisibility();
}

void Player::SendDirectMessage(WorldPacket* data)
{
	GetSession()->SendPacket(data);
}

PcInventory* Player::GetBag()
{
	return m_bag;
}

WorldSession* Player::GetSession() const
{
	return m_session;
}

bool Player::IsGameMaster() const
{
	return m_session->GetSecurity() == SEC_ADMINISTRATOR;
}

bool Player::IsFriendlyTo(const Unit* unit) const
{
	return true;
}

bool Player::IsHostileTo(const Unit* unit) const
{
	return false;
}

int32 Player::GetMaxWeight() const
{
	int8 stength = GetStat(STAT_STRENGTH);
	int8 stamina = GetStat(STAT_STAMINA);

	// 放大100倍，避免浮点运算
	int32 max_weight = 150 * (60 * stength + 40 * stamina + 100);

	// 放大100倍，避免浮点运算
	int32 weight_reduction_by_armor = GetWeightReduction();
	int32 create_weight_reduction = 4 * (m_createStrengthWeightReduction + m_createStaminaWeightReduction);
	int32 weight_reduction = 100 + weight_reduction_by_armor + create_weight_reduction;

	// 这边算放大了100 * 100倍
	max_weight *= weight_reduction;
	max_weight *= sConfig.GetIntDefault("L1J.RateWeightLimit", 500);
	return max_weight;
}

template<class T>
inline void BeforeVisibilityDestroy(T* /*t*/, Player* /*p*/)
{
}

template<>
inline void BeforeVisibilityDestroy<Creature>(Creature* t, Player* p)
{
}

Camera& Player::GetCamera()
{
	return m_camera;
}

GridReference<Player>& Player::GetGridRef()
{
	return m_gridRef;
}

MapReference& Player::GetMapRef()
{
	return m_mapRef;
}

void Player::UpdateVisibilityOf(const WorldObject* viewPoint, WorldObject* target)
{
	if (HaveAtClient(target))
	{
		if (!target->IsVisibleForInState(this, viewPoint, true))
		{
			ObjectGuid t_guid = target->GetObjectGuid();
			if (target->GetTypeId() == TYPEID_UNIT)
			{
				BeforeVisibilityDestroy<Creature>((Creature*)target, this);
				// target->DestroyForPlayer(this, !target->IsInWorld() && ((Creature*)target)->IsDead());
			}
			else
			{
				// target->DestroyForPlayer(this);
			}
			m_clientGUIDs.erase(t_guid);

			DEBUG_FILTER_LOG(LOG_FILTER_VISIBILITY_CHANGES, "%s out of range for player %u. Distance = %f", t_guid.GetString().c_str(), GetGUIDLow(), GetTileDistance(target));
		}
	}
	else
	{
		if (target->IsVisibleForInState(this, viewPoint, false))
		{
			target->SendCreateUpdateToPlayer(this);
			if (target->GetTypeId() != TYPEID_GAMEOBJECT)
			{
				m_clientGUIDs.insert(target->GetObjectGuid());
			}

			DEBUG_FILTER_LOG(LOG_FILTER_VISIBILITY_CHANGES, "Object %u (Type: %u) is visible now for player %u. Distance = %f", target->GetGUIDLow(), target->GetTypeId(), GetGUIDLow(), GetTileDistance(target));
		}
	}
}

void Player::RelocateToHomebind(uint32 mapId, CoordUnit x, CoordUnit y)
{
	SetMap(sMapMgr.CreateMap(mapId, this));
	Relocate(x, y);
}

bool Player::HaveAtClient(const WorldObject* u)
{
	return u == this || m_clientGUIDs.find(u->GetObjectGuid()) != m_clientGUIDs.end();
}

/*********************************************************/
/***                   LOAD SYSTEM                     ***/
/*********************************************************/

bool Player::LoadFromDB(SqlQueryHolder* holder)
{
    //                   0      1          2               3      4          5            6    7
    // SELECT account_name, objid, char_name, Metempsychosis, level, HighLevel, AccessLevel, Exp,
    //     8      9     10     11  12   13   14   15   16   17     18      19     20   21
    // MaxHp, CurHp, MaxMp, CurMp, Ac, Str, Dex, Con, Wis, Cha, Intel, Status, Class, Sex,
    //   22       23    24    25     26    27      28     29      30        31        32           33
    // Type, Heading, LocX, LocY, MapID, Food, Lawful, Title, ClanID, Clanname, ClanRank, BonusStatus,
    //           34       35       36             37      38         39            40          41            42
    // ElixirStatus, ElfAttr, PKcount, PkCountForElf, ExpRes, PartnerID, OnlineStatus, HomeTownID, Contribution,
    //  43        44      45     46                     47                            48                          49            50           51           52
    // Pay, HellTime, Banned, Karma, UNIX_TIMESTAMP(LastPk), UNIX_TIMESTAMP(LastPkForElf), UNIX_TIMESTAMP(DeleteTime), OriginalStr, OriginalDex, OriginalCon,
    //          53           54           55                         56        57        58     59     60                       61
    // OriginalInt, OriginalWis, OriginalCha, UNIX_TIMESTAMP(LastActive), AinZone, AinPoint, Honor, Kills, UNIX_TIMESTAMP(birthday) FROM characters WHERE char_name = '%s'
    QueryResult *result = holder->GetResult(PLAYER_LOGIN_QUERY_LOADFROM);

    if (!result)
    {
        sLog.outError("%s not found in table `characters`, can't load. ", m_name.c_str());
        return false;
    }

    // 玩家名称合法性校验
    if (ObjectMgr::CheckPlayerName(m_name) != CHAR_NAME_SUCCESS ||
        (GetSession()->GetSecurity() == SEC_PLAYER && sObjectMgr.IsReservedName(m_name)))
    {
        delete result;
        sLog.outError("account (%s)'s character has security issues", m_name.c_str());
        return false;
    }

    Field* fields = result->Fetch();

    std::string accid = fields[0].GetCppString();
    if (accid != GetSession()->GetAccountId())
    {
        sLog.outError("%s loading from wrong account (is: %s, should be: %s)", m_name.c_str(), GetSession()->GetAccountId().c_str(), accid.c_str());
        delete result;
        return false;
    }
    uint32 id = fields[1].GetUInt32();
    Object::_Create(id, 0, HIGHGUID_PLAYER);
    SetGuidValue(OBJECT_FIELD_GUID, GetObjectGuid());
    m_name = fields[2].GetCppString();
    // todo: 转生次数 3
	SetUInt32Value(UNIT_FIELD_LEVEL, fields[4].GetUInt8());
	SetUInt32Value(PLAYER_FIELD_HIGH_LEVEL, fields[5].GetUInt8());
    // todo: 角色权限 AccessLevel 6
	SetUInt32Value(PLAYER_FIELD_XP, fields[7].GetUInt32());
	ModifyBaseMaxHealth(fields[8].GetInt32());
    SetHealth(std::max(1, fields[9].GetInt32()), true);
    ModifyBaseMaxMana(fields[10].GetInt32());
	SetMana(std::max(0, fields[11].GetInt32()), true);
    // ignore: Ac 12
    ModifyBaseStat(STAT_STRENGTH, fields[13].GetInt32());
	ModifyBaseStat(STAT_AGILITY, fields[14].GetInt32());
	ModifyBaseStat(STAT_STAMINA, fields[15].GetInt32());
	ModifyBaseStat(STAT_SPIRIT, fields[16].GetInt32());
	ModifyBaseStat(STAT_CHARM, fields[17].GetInt32());
	ModifyBaseStat(STAT_INTELLECT, fields[18].GetInt32());
	m_deathState = ALIVE;
    // ignore: Status 19 此处是设置角色当前挥舞的武器外型，用于更新人物外观，在后面玩家背包加载时会根据装备武器自动设置
    SetUInt32Value(UNIT_FIELD_GFX_ID, fields[20].GetUInt32());
    SetUInt32Value(PLAYER_FIELD_GFX_ID, fields[20].GetUInt32());
	SetByteValue(UNIT_FIELD_BYTES, 0, RACE_NONE);                                // 种族
	SetByteValue(UNIT_FIELD_BYTES, 1, GetClasses(fields[20].GetUInt32())); // 职业
	SetByteValue(UNIT_FIELD_BYTES, 2, fields[21].GetUInt8());                    // 性别
    // ignore: Type 22
    Relocate(fields[24].GetInt32(), fields[25].GetInt32(), Towards(fields[23].GetUInt8()));
    SetMap(sMapMgr.CreateMap(fields[26].GetUInt32(), this));
    SetFood(fields[27].GetUInt8());
    SetLawful(fields[28].GetInt16());
    m_title = fields[29].GetCppString();
    // todo: 血盟 ClanID 30 Clanname 31 ClanRank 32
    SetByteValue(PLAYER_FIELD_STATUS, 0, fields[33].GetUInt8());
	SetByteValue(PLAYER_FIELD_STATUS, 1, fields[34].GetUInt8());
    // todo: 精灵派系 ElfAttr 35
    SetInt16Value(PLAYER_FIELD_PK_COUNT, 0, fields[36].GetInt16());
	SetInt16Value(PLAYER_FIELD_PK_COUNT, 1, fields[37].GetInt16());
	// todo: 教堂经验恢复 ExpRes 38 结婚系统 PartnerID 39
	SetByteValue(PLAYER_FIELD_STATUS, 3, fields[40].GetUInt8());
    // todo: 村庄贡献系统 HomeTownID 41 Contribution 42
    // todo: Pay 43
    // todo: 地狱 HellTime 44
	SetByteValue(PLAYER_FIELD_STATUS, 3, fields[45].GetUInt8());
    // todo: 友好度系统 Karma 46
    SetUInt32Value(PLAYER_FIELD_LAST_PK, fields[47].GetUInt32());
	SetUInt32Value(PLAYER_FIELD_ELF_LAST_PK, fields[48].GetUInt32());
	SetUInt32Value(PLAYER_FIELD_DELETE, fields[49].GetUInt32());
	m_createStat[STAT_STRENGTH] = fields[50].GetInt32();
	m_createStat[STAT_AGILITY] = fields[51].GetInt32();
	m_createStat[STAT_STAMINA] = fields[52].GetInt32();
	m_createStat[STAT_SPIRIT] = fields[53].GetInt32();
	m_createStat[STAT_CHARM] = fields[54].GetInt32();
	m_createStat[STAT_INTELLECT] = fields[55].GetInt32();
    // todo: 殷海萨的祝福 LastActive 56 AinZone 57 AinPoint 58
    // todo: 竞技场 Honor 59 Kills 60
	SetUInt32Value(PLAYER_FIELD_BIRTHDAY, fields[61].GetUInt32());
	delete result;

	Refresh();

    m_moveSpeed  = MOVE_NORMAL;
    m_braveSpeed = false;

	SetTargetGuid(ObjectGuid());
	SetOwnerGuid(ObjectGuid());
	SetCreatorGuid(ObjectGuid());

    return true;
}

void Player::LoadSpells(QueryResult* result)
{
	if (!result)
	{
		sLog.outError("%s not found in table `character_skills`, can't load. ", m_name.c_str());
		return;
	}

	uint32 count = 0;
	// 1 - 10 法师职业法术 11 - 12 骑士职业法术 13 - 14 黑暗精灵职业法术 15 王族职业法术 17 - 22 精灵职业法术 23 - 25 龙骑士职业法术 26 - 28 幻术师职业法术
	std::vector<uint8> skills(28, 0);
	//          0         1          2               3
	// "SELECT id, skill_id, is_active, activetimeleft FROM character_skills WHERE char_obj_id = '%u'",
	do{
		Field *fields = result->Fetch();
		const SpellEntry* proto = sObjectMgr.GetSpellEntry(fields[1].GetUInt32());
		skills[proto->Rank - 1] |= 1 << proto->Number;
		count++;
	} while (result->NextRow());
	delete result;

	if (count)
	{
		S_AddSkill pkt(skills);
		SendDirectMessage(&pkt);
	}
}

void Player::SaveToDB()
{
}