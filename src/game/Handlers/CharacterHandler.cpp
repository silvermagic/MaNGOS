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

#include <boost/algorithm/clamp.hpp>
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Entities/ObjectGuid.h"
#include "Entities/Player.h"
#include "Items/PcInventory.h"
#include "Maps/Map.h"
#include "Packets/S_ActiveSpells.hpp"
#include "Packets/S_CharTitle.hpp"
#include "Packets/S_InitialAbilityGrowth.hpp"
#include "Packets/S_InvList.hpp"
#include "Packets/S_Karma.hpp"
#include "Packets/S_LoginToGame.hpp"
#include "Packets/S_MapID.hpp"
#include "Packets/S_OwnCharStatus.hpp"
#include "Packets/S_OwnCharPack.hpp"
#include "Packets/S_SPMR.hpp"
#include "Packets/S_Weather.hpp"
#include "Common.h"
#include "Log.h"
#include "Util.h"
#include "AccountMgr.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "SQLStorages.h"
#include "SQLStructure.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

std::map<Classes, uint8> PLAYER_ORIGINAL_STATS_BONUS = {
	{CLASS_PRINCE, 8},
	{CLASS_KNIGHT, 4},
	{CLASS_ELF, 7},
	{CLASS_WIZARD, 16},
	{CLASS_DARK_ELF, 10},
	{CLASS_DRAGON_KNIGHT, 6},
	{CLASS_ILLUSIONIST, 10}
};

class LoginQueryHolder : public SqlQueryHolder
{
private:
	std::string m_accountId;
	ObjectGuid m_guid;
public:
	LoginQueryHolder(std::string accountId, ObjectGuid guid)
		: m_accountId(accountId), m_guid(guid)
	{
	}
	ObjectGuid GetGuid() const
	{
		return m_guid;
	}
	std::string GetAccountId() const
	{
		return m_accountId;
	}
	bool Initialize();
};

bool LoginQueryHolder::Initialize()
{
	SetSize(MAX_PLAYER_LOGIN_QUERY);

	bool res = true;
	res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADFROM,
		"SELECT account_name, objid, char_name, Metempsychosis, level, HighLevel, AccessLevel, Exp, "
		"MaxHp, CurHp, MaxMp, CurMp, Ac, Str, Dex, Con, Wis, Cha, Intel, Status, Class, Sex,"
		"Type, Heading, LocX, LocY, MapID, Food, Lawful, Title, ClanID, Clanname, ClanRank, BonusStatus,"
		"ElixirStatus, ElfAttr, PKcount, PkCountForElf, ExpRes, PartnerID, OnlineStatus, HomeTownID, Contribution,"
		"Pay, HellTime, Banned, Karma, UNIX_TIMESTAMP(LastPk), UNIX_TIMESTAMP(LastPkForElf), UNIX_TIMESTAMP(DeleteTime), OriginalStr, OriginalDex, OriginalCon,"
		"OriginalWis, OriginalCha, OriginalInt, UNIX_TIMESTAMP(LastActive), AinZone, AinPoint, Honor, Kills, UNIX_TIMESTAMP(birthday) FROM characters WHERE objid = '%u'",
		m_guid.GetCounter());
	res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADSPELLS,
		"SELECT id, skill_id, is_active, activetimeleft FROM character_skills WHERE char_obj_id = '%u'",
		m_guid.GetCounter());
	/*res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADAURAS,
		"SELECT skill_id, remaining_time, poly_id FROM characters_buff WHERE char_obj_id = '%u'",
		m_guid.GetCounter());
	res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADQUESTS,
		"SELECT quest_id, quest_step FROM character_quests WHERE char_id = '%u'",
		m_guid.GetCounter());
	res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADTELEPORT,
		"SELECT id, name, locx, locy, mapid FROM character_teleport WHERE char_id = '%u'",
		m_guid.GetCounter());*/
	res &= SetPQuery(PLAYER_LOGIN_QUERY_LOADINVENTORY,
		"SELECT id, item_id, item_name, count, is_equipped, enchantlvl, is_id, durability, "
		"charge_count, remaining_time, UNIX_TIMESTAMP(last_used), bless, attr_enchant_kind, attr_enchant_level, firemr, watermr, earthmr, windmr, "
		"addsp, addhp, addmp, hpr, mpr, addstr, adddex, addint, addcon, addwis, addcha, Records_count, addLucky, UNIX_TIMESTAMP(DeleteDate) FROM character_items WHERE char_id = '%u'",
		m_guid.GetCounter());
	return res;
}

class CharacterHandler
{
public:
	void HandleCharEnumCallback(QueryResult* result, std::string account)
	{
		WorldSession* session = sWorld.FindSession(account);
		if (!session)
		{
			delete result;
			return;
		}
		session->HandleCharEnum(result);
	}
	void HandlePlayerLoginCallback(QueryResult* /*dummy*/, SqlQueryHolder* holder)
	{
		if (!holder) return;
		WorldSession* session = sWorld.FindSession(((LoginQueryHolder*)holder)->GetAccountId());
		if (!session)
		{
			delete holder;
			return;
		}
		session->HandlePlayerLogin((LoginQueryHolder*)holder);
	}
} chrHandler;

void WorldSession::HandleCharEnum(QueryResult* result)
{
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			std::string char_name = fields[0].GetCppString();
			std::string clan_name = fields[1].GetCppString();
			uint8 lvl = fields[8].GetUInt8();
			if (sConfig.GetBoolDefault("L1J.CharacterConfigInServerSide", true))
			{
				if (lvl < 1)
				{
					lvl = 1;
				}
				else if (lvl > 127)
				{
					lvl = 127;
				}
			}
			else
			{
				lvl = 1;
			}
			WorldPacket packet(S_OPCODE_CHARLIST, char_name.size() + clan_name.size() + 21);
			packet << char_name;
			packet << clan_name;
			packet << fields[2].GetUInt8(); // 职业
			packet << fields[3].GetUInt8(); // 性别
			packet << fields[4].GetInt16(); // 正义
			packet << uint16(boost::algorithm::clamp(fields[5].GetInt16(), 1, 32767));
			packet << uint16(boost::algorithm::clamp(fields[6].GetInt16(), 0, 32767));
			packet << fields[7].GetUInt8();
			packet << lvl;
			packet << fields[9].GetUInt8();  // 力量
			packet << fields[10].GetUInt8(); // 敏捷
			packet << fields[11].GetUInt8(); // 体制
			packet << fields[12].GetUInt8(); // 精神
			packet << fields[13].GetUInt8(); // 魅力
			packet << fields[14].GetUInt8(); // 智力
			packet << fields[15].GetUInt8(); // 是否是管理员
			packet << date_from_iso_string(fields[16].GetUInt64()); // 生日
			SendPacket(&packet);
		} while (result->NextRow());
		delete result;
	}
}

void WorldSession::HandleCommonClick(WorldPacket& recvPacket)
{
	uint8 charcount = sAccountMgr.GetCharactersCount(GetAccountId());
    /* QueryResult* result = CharacterDatabase.PQuery("SELECT COUNT(objid) FROM characters WHERE account_name = '%s'", GetAccountId().c_str());
	if (result)
	{
		Field* fields = result->Fetch();
		charcount = fields[0].GetUInt8();
		delete result;
	}*/

	WorldPacket packet(S_OPCODE_CHARAMOUNT, 2);
	packet << charcount;
	uint8 slot = uint8(sConfig.GetIntDefault("L1J.DefaultCharacterSlot", 3) & 0xFF) + GetCharacterSlot();
	packet << slot;
	SendPacket(&packet);

	if (charcount)
	{
		CharacterDatabase.AsyncPQuery(&chrHandler,
			&CharacterHandler::HandleCharEnumCallback,
			GetAccountId(),
			"SELECT "
			"char_name, "                        // 0
			"Clanname, "                         // 1
			"Type, "                             // 2
			"Sex, "                              // 3
			"Lawful, "                           // 4
			"CurHp, "                            // 5
			"CurMp, "                            // 6
			"Ac, "                               // 7
			"level, "                            // 8
			"Str, "                              // 9
			"Dex, "                              // 10
			"Con, "                              // 11
			"Wis, "                              // 12
			"Cha, "                              // 13
			"Intel, "                            // 14
			"AccessLevel, "                      // 15
			"UNIX_TIMESTAMP(birthday) "          // 16
			"FROM characters "
			"WHERE account_name = '%s'", GetAccountId().c_str());
	}
}

void WorldSession::HandlePlayerLogin(LoginQueryHolder* holder)
{
	Player* pCurrChar = new Player(this);
	if (!pCurrChar->LoadFromDB(holder))
	{
		KickPlayer();
		delete pCurrChar;
		delete holder;
		m_playerLoading = false;
		return;
	}
	SetPlayer(pCurrChar);

	// 清理所有光环，玩家光环会在LoadAuras的时候设置
	m_player->RemoveAllAuras();

	// 更新角色在线状态
	m_player->SetByteValue(PLAYER_FIELD_STATUS, 3, true);
	static SqlStatementID updonline;
	SqlStatement stmt = CharacterDatabase.CreateStatement(updonline, "UPDATE characters SET OnlineStatus = 1 WHERE objid = ?");
	stmt.PExecute(m_player->GetGUIDLow());

	{
		S_InitialAbilityGrowth pkt(m_player);
		SendPacket(&pkt);
	}
	{
		S_LoginToGame pkt;
		SendPacket(&pkt);
	}

	// todo: 改成传送
	const GetBackRestartEntry* iter = sGetBackRestartStore.LookupEntry<GetBackRestartEntry>(m_player->GetMapId());
	if (iter != nullptr)
	{
		m_player->RelocateToHomebind(iter->mapID, iter->coords[0], iter->coords[1]);
	}
	if (sConfig.GetBoolDefault("L1J.GetBack", false))
	{
		GetBackPosition(m_player);
	}

	if (!m_player->GetMap()->Add(m_player))
	{
		// todo: 添加失败
	}
	/*{
		S_ActiveSpells pkt;
		SendPacket(&pkt);
	}*/
/*	{
		S_Karma pkt(_player);
		SendPacket(&pkt);
	}*/

	{
		S_OwnCharStatus pkt(m_player);
		SendPacket(&pkt);
	}
	{
		S_MapID pkt(m_player->GetMapId(), false);
		SendPacket(&pkt);
	}
    {
		S_SPMR pkt(m_player);
		SendPacket(&pkt);
	}
	{
		S_CharTitle pkt(m_player);
		SendPacket(&pkt);
	}
	{
		S_Weather pkt(sWorld.GetWeatherState());
		SendPacket(&pkt);
	}

	m_player->GetBag()->LoadFromDB(holder->GetResult(PLAYER_LOGIN_QUERY_LOADINVENTORY), 0);
	{
		S_OwnCharPack pkt(m_player);
		SendPacket(&pkt);
	}
	{
		S_InvList pkt(m_player->GetBag()->GetItems());
		SendPacket(&pkt);
	}

	m_player->LoadSpells(holder->GetResult(PLAYER_LOGIN_QUERY_LOADSPELLS));

	// 加载buff
	// LoadAuras(holder->GetResult(PLAYER_LOGIN_QUERY_LOADAURAS), 0);

	// todo: 道具排序发送到客户端
	// todo: 在线奖励系统
	// todo: 召唤系统
	// todo: 攻城时间系统
	// todo: 玩家上线通知
	// todo: 血盟系统
	// todo: 结婚系统
	// todo: 回血 回魔 自动更新系统
	// todo: 经验系统

	{
		S_OwnCharStatus pkt(m_player);
		SendPacket(&pkt);
	}

	// 地狱惩罚系统
}

void WorldSession::HandleLoginToServer(WorldPacket& recvPacket)
{
	if (PlayerLoading() || GetPlayer() != NULL)
	{
		sLog.outError("Player tryes to login again, AccountId = %d", GetAccountId().c_str());
		return;
	}

	std::string char_name;
	recvPacket >> char_name;
	ObjectGuid playerGuid = sObjectMgr.GetPlayerGuidByName(char_name);

	m_playerLoading = true;
	LoginQueryHolder* holder = new LoginQueryHolder(GetAccountId(), playerGuid);
	if (!holder->Initialize())
	{
		delete holder;
		m_playerLoading = false;
		return;
	}

	CharacterDatabase.DelayQueryHolder(&chrHandler, &CharacterHandler::HandlePlayerLoginCallback, holder);
}