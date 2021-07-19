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

#include "SQLStorages.h"
#include "SQLStructure.h"

std::map<uint32, SpellEntry> SPELL_PROTOS;

static const std::map<uint8, uint8> SCHOOL_TYPES = {
	{0  /* 无属性 */, SPELL_SCHOOL_NORMAL},
	{1  /* 地系魔法 */, SPELL_SCHOOL_EARTH},
	{2  /* 火系魔法 */, SPELL_SCHOOL_FIRE},
	{3  /**/, SPELL_SCHOOL_NORMAL},
	{4  /* 水系魔法 */, SPELL_SCHOOL_WATER},
	{5  /**/, SPELL_SCHOOL_NORMAL},
	{6  /**/, SPELL_SCHOOL_NORMAL},
	{8  /* 风系魔法 */, SPELL_SCHOOL_WIND},
	{12  /* 风系魔法 */, SPELL_SCHOOL_NORMAL},
	{16 /* 射线系魔法 */, SPELL_SCHOOL_NORMAL},
};

static const std::map<uint8, uint32> CAST_TARGET_FLAGS = {
	{0  /* 施法目标是自身 */, TARGET_FLAG_SELF},
	{1  /* 施法目标是玩家 */, TARGET_FLAG_UNIT},
	{2  /* 施法目标是NPC */, TARGET_FLAG_UNIT},
	{3  /*  */, TARGET_FLAG_UNIT},
	{4  /* 施法目标是血盟成员 */, TARGET_FLAG_UNIT},
	{8  /* 施法目标是队伍成员 */, TARGET_FLAG_UNIT},
	{12  /* 施法目标是自身或血盟成员 */, TARGET_FLAG_UNIT},
	{16 /* 施法目标是宠物 */, TARGET_FLAG_UNIT},
	{20  /* 施法目标是血盟成员 */, TARGET_FLAG_UNIT},
	{32 /* 施法目标是位置 */, TARGET_FLAG_SOURCE_LOCATION | TARGET_FLAG_DEST_LOCATION},
};

static const std::map<uint8, SpellFamily> FAMILY_TYPES = {
	{1, SPELL_FAMILY_WIZARD},
	{2, SPELL_FAMILY_WIZARD},
	{3, SPELL_FAMILY_WIZARD},
	{4, SPELL_FAMILY_WIZARD},
	{5, SPELL_FAMILY_WIZARD},
	{6, SPELL_FAMILY_WIZARD},
	{7, SPELL_FAMILY_WIZARD},
	{8, SPELL_FAMILY_WIZARD},
	{9, SPELL_FAMILY_WIZARD},
	{10, SPELL_FAMILY_WIZARD},
};

void LoadSpells()
{
	QueryResult* result = WorldDatabase.Query(
		//                 0     1            2             3          4          5              6
		"SELECT skill_id, name, skill_level, skill_number, mpConsume, hpConsume, itemConsumeId, " \
		//              7           8             9      10         11            12
		"itemConsumeCount, reuseDelay, buffDuration, target, target_to, damage_value, " \
		//        13                 14                 15                16    17    18
		"damage_dice, damage_dice_count, probability_value, probability_dice, attr, type, " \
		//   19      20    21       22  23      24         25       26        27
		"lawful, ranged, area, through, id, nameid, action_id, castgfx, castgfx2, " \
		//            28             29            30
		"sysmsgID_happen, sysmsgID_stop, sysmsgID_fail FROM skills");

	if (!result)
	{
		BarGoLink bar(1);
		bar.step();
		sLog.outString();
		sLog.outErrorDb(">> Loaded 0 spell. DB table `skills` is empty.");
		return;
	}

	uint32 count = 0;
	BarGoLink bar(result->GetRowCount());
	do
	{
		Field* fields = result->Fetch();
		bar.step();

		uint32 guid = fields[0].GetUInt64();
		SpellEntry &proto = SPELL_PROTOS[guid];
		proto.Rank = fields[2].GetUInt8();
		proto.Number = fields[3].GetUInt8();
		proto.SpellFamilyName = GetSpellFamily(proto.Rank);
		proto.PowerCost[POWER_MANA] = fields[4].GetInt32();
		proto.PowerCost[POWER_HEALTH] = fields[5].GetInt32();
		proto.Reagent[0] = fields[6].GetUInt32();
		proto.ReagentCount[0] = fields[7].GetInt32();
		proto.RecoveryTime = fields[8].GetUInt32();
		proto.Duration = fields[9].GetUInt32();
		// todo: 10 target -> attach buff none 放到标志位里面？
		proto.Targets = CAST_TARGET_FLAGS.at(fields[11].GetUInt8());
		proto.School = SCHOOL_TYPES.at(fields[17].GetUInt8());
		proto.Category = SpellCategories(fields[18].GetUInt8());
		proto.PowerCost[POWER_LAWFUL] = fields[19].GetInt32();
		proto.Range = fields[20].GetInt32();
		// proto.DmgClass;

		/*
		proto.TargetCreatureType;
		proto.ProcFlags;
		proto.ProcChance;
		proto.ProcCharges;
		proto.Effect[MAX_EFFECT_INDEX] ;
		proto.EffectItemType[MAX_EFFECT_INDEX];
		proto.EffectBasePoints[MAX_EFFECT_INDEX];
		proto.EffectBaseDice[MAX_EFFECT_INDEX];
		proto.EffectImplicitTarget[MAX_EFFECT_INDEX];
		proto.EffectRadius[MAX_EFFECT_INDEX];
		proto.EffectApplyAuraName[MAX_EFFECT_INDEX];
		proto.EffectMiscValue[MAX_EFFECT_INDEX];
		proto.EffectTriggerSpell[MAX_EFFECT_INDEX];

		proto.EquippedItemClass;
		proto.EquippedItemSubClassMask;
		proto.EquippedItemInventoryTypeMask;

		proto.AuraInterruptFlags;
		proto.Attributes;*/

		proto.ActionID = fields[25].GetUInt32();
		proto.CastGFXID[0] = fields[26].GetUInt32();
		proto.CastGFXID[1] = fields[27].GetUInt32();
		proto.MessageID[0] = fields[28].GetUInt32();
		proto.MessageID[1] = fields[29].GetUInt32();
		proto.MessageID[2] = fields[30].GetUInt32();

		// 先在代码中写死，后期固化到数据库表，使用新表spells替代skills
		switch (guid)
		{
			case TINY_HEAL:
				proto.Effect[0] = SPELL_EFFECT_HEAL;
				proto.EffectBasePoints[0] = fields[12].GetInt32();
				proto.EffectBaseDice[0] = fields[13].GetInt32();
				break;
			default:
				proto.Mechanic = MECHANIC_NONE;
				proto.Dispel = DISPEL_MAGIC;
				proto.EffectRadius[0] = fields[21].GetInt32();
				break;
		}

		++count;
	} while (result->NextRow());

	delete result;

	sLog.outString();
	sLog.outString(">> Loaded " SIZEFMTD " spells", count);
}

SpellFamily GetSpellFamily(uint8 rank)
{
	switch (rank)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			return SPELL_FAMILY_WIZARD;
		case 11:
		case 12:
			return SPELL_FAMILY_KNIGHT;
		case 13:
		case 14:
			return SPELL_FAMILY_DARK_ELF;
		case 15:
		case 16:
			return SPELL_FAMILY_PRINCE;
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			return SPELL_FAMILY_ELF;
		case 23:
		case 24:
		case 25:
			return SPELL_FAMILY_DRAGON_KNIGHT;
		case 26:
		case 27:
		case 28:
			return SPELL_FAMILY_ILLUSIONIST;
	}

	return SPELL_FAMILY_NONE;
}

