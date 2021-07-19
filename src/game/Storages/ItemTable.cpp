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

std::map<uint32, ItemEntry> ITEM_PROTOS;

static const std::map<std::string, ItemSubclassWeapon> WEAPON_TYPES = {
	{"sword",         ITEM_SUBCLASS_WEAPON_SWORD},
	{"dagger",        ITEM_SUBCLASS_WEAPON_DAGGER},
	{"tohandsword",   ITEM_SUBCLASS_WEAPON_SWORD2},
	{"bow",           ITEM_SUBCLASS_WEAPON_BOW},
	{"spear",         ITEM_SUBCLASS_WEAPON_SPEAR2},
	{"blunt",         ITEM_SUBCLASS_WEAPON_AXE},
	{"staff",         ITEM_SUBCLASS_WEAPON_STAFF},
	{"throwingknife", ITEM_SUBCLASS_WEAPON_STING},
	{"arrow",         ITEM_SUBCLASS_WEAPON_ARROW},
	{"gauntlet",      ITEM_SUBCLASS_WEAPON_GAUNTLET},
	{"claw",          ITEM_SUBCLASS_WEAPON_CLAW},
	{"edoryu",        ITEM_SUBCLASS_WEAPON_EDORYU},
	{"singlebow",     ITEM_SUBCLASS_WEAPON_CROSSBOW},
	{"singlespear",   ITEM_SUBCLASS_WEAPON_SPEAR},
	{"tohandblunt",   ITEM_SUBCLASS_WEAPON_AXE2},
	{"tohandstaff",   ITEM_SUBCLASS_WEAPON_STAFF2},
	{"kiringku",      ITEM_SUBCLASS_WEAPON_KIRINGKU},
	{"chainsword",    ITEM_SUBCLASS_WEAPON_CHAINSWORD}
};

static const std::map<std::string, ItemSubclassArmor> ARMOR_TYPES = {
	{"none",    ITEM_SUBCLASS_ARMOR},
	{"helm",    ITEM_SUBCLASS_ARMOR_HELMET},
	{"armor",   ITEM_SUBCLASS_ARMOR_ARMOUR},
	{"T",       ITEM_SUBCLASS_ARMOR_SHIRT},
	{"cloak",   ITEM_SUBCLASS_ARMOR_CLOAK},
	{"glove",   ITEM_SUBCLASS_ARMOR_GLOVE},
	{"boots",   ITEM_SUBCLASS_ARMOR_BOOTS},
	{"shield",  ITEM_SUBCLASS_ARMOR_SHIELD},
	{"amulet",  ITEM_SUBCLASS_ARMOR_NECKLACE},
	{"ring",    ITEM_SUBCLASS_ARMOR_RING},
	{"belt",    ITEM_SUBCLASS_ARMOR_BELT},
	{"ring2",   ITEM_SUBCLASS_ARMOR_RING},
	{"earring", ITEM_SUBCLASS_ARMOR_EARRING},
	{"guarder", ITEM_SUBCLASS_ARMOR_GARDEBRAS}
};

static const std::map<std::string, ItemSubclassGeneric> GENERIC_TYPES = {
	{"arrow",          ITEM_SUBCLASS_GENERIC_ARROW},
	{"wand",           ITEM_SUBCLASS_GENERIC_WAND},
	{"light",          ITEM_SUBCLASS_GENERIC_LIGHT},
	{"gem",            ITEM_SUBCLASS_GENERIC_GEM},
	{"totem",          ITEM_SUBCLASS_GENERIC_TOTEM},
	{"firecracker",    ITEM_SUBCLASS_GENERIC_FIRECRACKER},
	{"potion",         ITEM_SUBCLASS_GENERIC_POTION},
	{"food",           ITEM_SUBCLASS_GENERIC_FOOD},
	{"scroll",         ITEM_SUBCLASS_GENERIC_SCROLL},
	{"questitem",      ITEM_SUBCLASS_GENERIC_QUESTITEM},
	{"spellbook",      ITEM_SUBCLASS_GENERIC_SPELLBOOK},
	{"petitem",        ITEM_SUBCLASS_GENERIC_PETITEM},
	{"other",          ITEM_SUBCLASS_GENERIC_OTHER},
	{"material",       ITEM_SUBCLASS_GENERIC_MATERIAL},
	{"event",          ITEM_SUBCLASS_GENERIC_EVENT},
	{"sting",          ITEM_SUBCLASS_GENERIC_STING},
	{"treasure_box",   ITEM_SUBCLASS_GENERIC_TREASURE_BOX},
	{"magic_doll",     ITEM_SUBCLASS_GENERIC_MAGIC_DOLL},
	{"scrollshop",     ITEM_SUBCLASS_GENERIC_SCROLLSHOP},
	{"TeleportScroll", ITEM_SUBCLASS_GENERIC_TELEPORTSCROLL}
};

static const std::map<std::string, ItemMaterialType> MATERIAL_TYPES = {
	{"none",         ITEM_MATERIAL_NONE},
	{"liquid",       ITEM_MATERIAL_LIQUID},
	{"web",          ITEM_MATERIAL_WEB},
	{"vegetation",   ITEM_MATERIAL_VEGETATION},
	{"animalmatter", ITEM_MATERIAL_ANIMALMATTER},
	{"paper",        ITEM_MATERIAL_PAPER},
	{"cloth",        ITEM_MATERIAL_CLOTH},
	{"leather",      ITEM_MATERIAL_LEATHER},
	{"wood",         ITEM_MATERIAL_WOOD},
	{"bone",         ITEM_MATERIAL_BONE},
	{"dragonscale",  ITEM_MATERIAL_DRAGONSCALE},
	{"iron",         ITEM_MATERIAL_IRON},
	{"steel",        ITEM_MATERIAL_STEEL},
	{"copper",       ITEM_MATERIAL_COPPER},
	{"silver",       ITEM_MATERIAL_SILVER},
	{"gold",         ITEM_MATERIAL_GOLD},
	{"platinum",     ITEM_MATERIAL_PLATINUM},
	{"mithril",      ITEM_MATERIAL_MITHRIL},
	{"blackmithril", ITEM_MATERIAL_BLACKMITHRIL},
	{"glass",        ITEM_MATERIAL_GLASS},
	{"gemstone",     ITEM_MATERIAL_GEMSTONE},
	{"mineral",      ITEM_MATERIAL_MINERAL},
	{"oriharukon",   ITEM_MATERIAL_ORIHARUKON}
};

static const std::map<std::string, uint8> USE_TYPES = {
	{"none", -1},
	{"normal", 0},
	{"weapon", 1},
	{"armor", 2},
	{"spell_long", 5},
	{"ntele", 6},
	{"identify", 7},
	{"res", 8},
	{"letter", 12},
	{"letter_w", 13},
	{"choice", 14},
	{"instrument", 15},
	{"sosc", 16},
	{"spell_short", 17},
	{"T", 18},
	{"cloak", 19},
	{"glove", 20},
	{"boots", 21},
	{"helm", 22},
	{"ring", 23},
	{"amulet", 24},
	{"shield", 25},
	{"guarder", 25},
	{"dai", 26},
	{"zel", 27},
	{"blank", 28},
	{"btele", 29},
	{"spell_buff", 30},
	{"ccard", 31},
	{"ccard_w", 32},
	{"vcard", 33},
	{"vcard_w", 34},
	{"wcard", 35},
	{"wcard_w", 36},
	{"belt", 37},
	{"earring", 40},
	{"fishing_rod", 42},
	{"del", 46}
};

void LoadItemsFromArmor()
{
	uint32 count = 0;

	// 加载防具道具
	QueryResult *result = WorldDatabase.Query(
		//                0     1                     2                   3     4         5       6
		"SELECT item_id, name, unidentified_name_id, identified_name_id, type, material, weight, " \
		//    7       8            9  10          11         12          13        14       15           16
		"invgfx, grdgfx, itemdesc_id, ac, safenchant, use_royal, use_knight, use_mage, use_elf, use_darkelf, " \
		//             17               18       19       20       21       22       23       24      25      26
		"use_dragonknight, use_illusionist, add_str, add_con, add_dex, add_int, add_wis, add_cha, add_hp, add_mp, " \
		//    27       28      29       30       31     32          33                34                35
		"add_hpr, add_mpr, add_sp, min_lvl, max_lvl, m_def, haste_item, damage_reduction, weight_reduction, " \
		//         36            37                38                39     40     41           42            43
		"hit_modifier, dmg_modifier, bow_hit_modifier, bow_dmg_modifier, bless, trade, cant_delete, max_use_time, " \
		//          44            45            46             47           48            49            50
		"defense_water, defense_wind, defense_fire, defense_earth, regist_stun, regist_stone, regist_sleep, " \
		//          51              52            53
		"regist_freeze, regist_sustain, regist_blind FROM armor");

	if (!result) {
		BarGoLink bar(1);
		bar.step();
		sLog.outString();
		sLog.outErrorDb(">> Loaded 0 armor. DB table `armor` is empty.");
		return;
	}

	BarGoLink bar(result->GetRowCount());
	do {
		Field *fields = result->Fetch();
		bar.step();

		uint32 guid = fields[0].GetUInt64();
		ItemEntry &proto = ITEM_PROTOS[guid];
		proto.Class = ITEM_CLASS_ARMOR;
		proto.name = fields[1].GetCppString();
		proto.unidentified = fields[2].GetCppString();
		proto.identified = fields[3].GetCppString();
		proto.SubClass = ARMOR_TYPES.at(fields[4].GetCppString());
		proto.use_type = USE_TYPES.at(fields[4].GetCppString());
		proto.material = MATERIAL_TYPES.at(fields[5].GetCppString());
		proto.weight = fields[6].GetUInt32();
		proto.gfx_in_backpack = fields[7].GetUInt32();
		proto.gfx_on_ground = fields[8].GetUInt32();
		proto.description = fields[9].GetUInt32();
		proto.armor = fields[10].GetInt32();
		proto.safenchant = fields[11].GetUInt32();
		proto.allowable |= fields[12].GetBool() ? 1 << (CLASS_PRINCE - 1) : 0;
		proto.allowable |= fields[13].GetBool() ? 1 << (CLASS_KNIGHT - 1) : 0;
		proto.allowable |= fields[14].GetBool() ? 1 << (CLASS_WIZARD - 1) : 0;
		proto.allowable |= fields[15].GetBool() ? 1 << (CLASS_ELF - 1) : 0;
		proto.allowable |= fields[16].GetBool() ? 1 << (CLASS_DARK_ELF - 1) : 0;
		proto.allowable |= fields[17].GetBool() ? 1 << (CLASS_DRAGON_KNIGHT - 1) : 0;
		proto.allowable |= fields[18].GetBool() ? 1 << (CLASS_ILLUSIONIST - 1) : 0;
		proto.strength = fields[19].GetInt32();
		proto.stamina = fields[20].GetInt32();
		proto.agility = fields[21].GetInt32();
		proto.intellect = fields[22].GetInt32();
		proto.spirit = fields[23].GetInt32();
		proto.charm = fields[24].GetInt32();
		proto.health = fields[25].GetInt32();
		proto.mana = fields[26].GetInt32();
		proto.health_regen = fields[27].GetInt32();
		proto.mana_regen = fields[28].GetInt32();
		proto.spell_power = fields[29].GetInt32();
		proto.min_use_level = fields[30].GetUInt32();
		proto.max_use_level = fields[31].GetUInt32();
		proto.defense_magic = fields[32].GetInt32();
		proto.flags |= fields[33].GetBool() ? ITEM_FLAG_HASTE : 0;
		proto.damage_reduction = fields[34].GetInt32();
		proto.weight_reduction = fields[35].GetInt32();
		proto.melee_hit_chance = fields[36].GetInt32();
		proto.melee_damage_done = fields[37].GetInt32();
		proto.ranged_hit_chance = fields[38].GetInt32();
		proto.ranged_damage_done = fields[39].GetInt32();
		proto.bless = fields[40].GetUInt8();
		proto.flags |= fields[41].GetBool() ? 0 : ITEM_FLAG_CANT_TRADE;
		proto.flags |= fields[42].GetBool() ? ITEM_FLAG_CANT_DELETE : 0;
		proto.max_use_time = fields[43].GetUInt32();
		proto.defense_water = fields[44].GetInt32();
		proto.defense_wind = fields[45].GetInt32();
		proto.defense_fire = fields[46].GetInt32();
		proto.defense_earth = fields[47].GetInt32();
		proto.resist_stun = fields[48].GetInt32();
		proto.resist_stone = fields[49].GetInt32();
		proto.resist_sleep = fields[50].GetInt32();
		proto.resist_freeze = fields[51].GetInt32();
		proto.resist_sustain = fields[52].GetInt32();
		proto.resist_blind = fields[48].GetInt32();
		switch (proto.SubClass)
		{
			case ITEM_SUBCLASS_ARMOR_HELMET:         // 头盔
				proto.inventory_type = INVTYPE_HEAD;
				break;
			case ITEM_SUBCLASS_ARMOR_ARMOUR:         // 盔甲
				proto.inventory_type = INVTYPE_BODY;
				break;
			case ITEM_SUBCLASS_ARMOR_SHIRT:          // T恤
				proto.inventory_type = INVTYPE_CHEST;
				break;
			case ITEM_SUBCLASS_ARMOR_CLOAK:          // 斗篷
				proto.inventory_type = INVTYPE_CLOAK;
				break;
			case ITEM_SUBCLASS_ARMOR_GLOVE:          // 手套
				proto.inventory_type = INVTYPE_HANDS;
				break;
			case ITEM_SUBCLASS_ARMOR_BOOTS:          // 靴子
				proto.inventory_type = INVTYPE_FEET;
				break;
			case ITEM_SUBCLASS_ARMOR_SHIELD:         // 盾牌
				proto.inventory_type = INVTYPE_SHIELD;
				break;
			case ITEM_SUBCLASS_ARMOR_NECKLACE:       // 项链
				proto.inventory_type = INVTYPE_NECK;
				break;
			case ITEM_SUBCLASS_ARMOR_RING :          // 戒子
				proto.inventory_type = INVTYPE_FINGER;
				break;
			case ITEM_SUBCLASS_ARMOR_BELT:           // 腰带
				proto.inventory_type = INVTYPE_WAIST;
				break;
			case ITEM_SUBCLASS_ARMOR_EARRING:        // 耳环
				proto.inventory_type = INVTYPE_EAR;
				break;
			case ITEM_SUBCLASS_ARMOR_GARDEBRAS:      // 臂甲
				proto.inventory_type = INVTYPE_SHOULDERS;
				break;
			default:
				proto.inventory_type = INVTYPE_NON_EQUIP;
				break;
		}

		++count;
	} while (result->NextRow());

	delete result;

	sLog.outString();
	sLog.outString(">> Loaded " SIZEFMTD " armors", count);
}

void LoadItemsFromOther()
{
	uint32 count = 0;

	// 加载其他道具
	QueryResult *result = WorldDatabase.Query(
		//                0     1                     2                   3          4         5
		"SELECT item_id, name, unidentified_name_id, identified_name_id, item_type, use_type, " \
		//      6       7       8       9           10         11                12         13         14
		"material, weight, invgfx, grdgfx, itemdesc_id, stackable, max_charge_count, dmg_small, dmg_large, " \
		//    15       16    17    18     19     20     21           22        23        24          25
		"min_lvl, max_lvl, locx, locy, mapid, bless, trade, cant_delete, can_seal, delay_id, delay_time, " \
		//         26           27            28         29        30          31
		"delay_effect, food_volume, save_at_once, DropBoard, DropItem, delete_day FROM etcitem");

	if (!result) {
		BarGoLink bar(1);
		bar.step();
		sLog.outString();
		sLog.outErrorDb(">> Loaded 0 item. DB table `item` is empty.");
		return;
	}

	BarGoLink bar(result->GetRowCount());
	do {
		Field *fields = result->Fetch();
		bar.step();

		uint8 stats = 0;
		uint32 guid = fields[0].GetUInt64();
		ItemEntry &proto = ITEM_PROTOS[guid];
		proto.Class = ITEM_CLASS_GENERIC;
		proto.name = fields[1].GetCppString();
		proto.unidentified = fields[2].GetCppString();
		proto.identified = fields[3].GetCppString();
		proto.SubClass = GENERIC_TYPES.at(fields[4].GetCppString());
		proto.use_type = USE_TYPES.at(fields[5].GetCppString());
		if (proto.use_type != -1)
		{
			proto.flags |= ITEM_FLAG_USABLE;
		}
		proto.material = MATERIAL_TYPES.at(fields[6].GetCppString());
		proto.weight = fields[7].GetUInt32();
		proto.gfx_in_backpack = fields[8].GetUInt32();
		proto.gfx_on_ground = fields[9].GetUInt32();
		proto.description = fields[10].GetUInt32();
		proto.flags |= fields[11].GetBool() ? ITEM_FLAG_STACKABLE : 0;
		proto.max_charge_count = fields[12].GetUInt32();
		proto.ammo_damage_tiny = fields[13].GetUInt32();
		proto.ammo_damage_large = fields[14].GetUInt32();
		proto.min_use_level = fields[15].GetUInt32();
		proto.max_use_level = fields[16].GetUInt32();
		proto.x = fields[17].GetUInt16();
		proto.y = fields[18].GetUInt16();
		proto.map = fields[19].GetUInt32();
		proto.bless = fields[20].GetUInt8();
		proto.flags |= fields[21].GetBool() ? 0 : ITEM_FLAG_CANT_TRADE;
		proto.flags |= fields[22].GetBool() ? ITEM_FLAG_CANT_DELETE : 0;
		proto.flags |= fields[23].GetBool() ? 0 : ITEM_FLAG_CANT_SEAL;
		// 法术效果 24 25 26
		proto.food = fields[27].GetUInt32();
		// 丢地上和丢怪物身上统一处理
		proto.flags |= fields[28].GetBool() ? 0 : ITEM_FLAG_CANT_DROP;
		// 删除时间 30
		proto.inventory_type = INVTYPE_NON_EQUIP;

		++count;
	} while (result->NextRow());

	delete result;

	sLog.outString();
	sLog.outString(">> Loaded " SIZEFMTD " items", count);

}

void LoadItemsFromWeapon()
{
	uint32 count = 0;

	// 加载武器道具
	QueryResult *result = WorldDatabase.Query(
		//                0     1                     2                   3     4         5
		"SELECT item_id, name, unidentified_name_id, identified_name_id, type, material, " \
		//    6       7       8            9          10        11      12          13         14
		"weight, invgfx, grdgfx, itemdesc_id, dmg_small, dmg_large, `range`, safenchant, use_royal, " \
		//       15        16       17           18                19               20           21
		"use_knight, use_mage, use_elf, use_darkelf, use_dragonknight, use_illusionist, hitmodifier, "
		//        22       23       24       25       26       27       28      29      30       31       32
		"dmgmodifier, add_str, add_con, add_dex, add_int, add_wis, add_cha, add_hp, add_mp, add_hpr, add_mpr, " \
		//   33     34          35                 36                37        38       39       40     41
		"add_sp, m_def, haste_item, double_dmg_chance, magicdmgmodifier, canbedmg, min_lvl, max_lvl, bless, " \
		//  42           43            44
		"trade, cant_delete, max_use_time FROM weapon");

	if (!result) {
		BarGoLink bar(1);
		bar.step();
		sLog.outString();
		sLog.outErrorDb(">> Loaded 0 weapon. DB table `weapon` is empty.");
		return;
	}

	BarGoLink bar(result->GetRowCount());
	do {
		Field *fields = result->Fetch();
		bar.step();

		uint32 guid = fields[0].GetUInt64();
		ItemEntry &proto = ITEM_PROTOS[guid];
		proto.Class = ITEM_CLASS_WEAPON;
		proto.name = fields[1].GetCppString();
		proto.unidentified = fields[2].GetCppString();
		proto.identified = fields[3].GetCppString();
		proto.SubClass = WEAPON_TYPES.at(fields[4].GetCppString());
		proto.use_type = USE_TYPES.at("weapon");
		proto.material = MATERIAL_TYPES.at(fields[5].GetCppString());
		proto.weight = fields[6].GetUInt32();
		proto.gfx_in_backpack = fields[7].GetUInt32();
		proto.gfx_on_ground = fields[8].GetUInt32();
		proto.description = fields[9].GetUInt32();
		proto.damage_tiny = fields[10].GetUInt32();
		proto.damage_large = fields[11].GetUInt32();
		proto.attack_range = fields[12].GetUInt32();
		proto.safenchant = fields[13].GetUInt32();
		proto.allowable |= fields[14].GetBool() ? 1 << (CLASS_PRINCE - 1) : 0;
		proto.allowable |= fields[15].GetBool() ? 1 << (CLASS_KNIGHT - 1) : 0;
		proto.allowable |= fields[16].GetBool() ? 1 << (CLASS_WIZARD - 1) : 0;
		proto.allowable |= fields[17].GetBool() ? 1 << (CLASS_ELF - 1) : 0;
		proto.allowable |= fields[18].GetBool() ? 1 << (CLASS_DARK_ELF - 1) : 0;
		proto.allowable |= fields[19].GetBool() ? 1 << (CLASS_DRAGON_KNIGHT - 1) : 0;
		proto.allowable |= fields[20].GetBool() ? 1 << (CLASS_ILLUSIONIST - 1) : 0;
		switch (proto.SubClass)
		{
			case ITEM_SUBCLASS_WEAPON_BOW:                       // 弓(双手武器)
				proto.flags |= ITEM_FLAG_TWO_HANDED_WEAPON;
			case ITEM_SUBCLASS_WEAPON_GAUNTLET:                  // 手甲(黑妖扔飞刀的单手武器)
			case ITEM_SUBCLASS_WEAPON_CROSSBOW:                  // 弩(单手武器)
				proto.ranged_hit_chance = fields[21].GetInt32();
				proto.ranged_damage_done = fields[22].GetInt32();
				proto.inventory_type = INVTYPE_RANGED;
				break;
			case ITEM_SUBCLASS_WEAPON_SWORD2:                    // 双手剑
			case ITEM_SUBCLASS_WEAPON_SPEAR2:                    // 矛(双手武器)
			case ITEM_SUBCLASS_WEAPON_CLAW:                      // 钢爪(双手武器)
			case ITEM_SUBCLASS_WEAPON_EDORYU:                    // 双刀(双手武器)
			case ITEM_SUBCLASS_WEAPON_AXE2:                      // 双手斧/锤
			case ITEM_SUBCLASS_WEAPON_STAFF2:                     // 魔杖(双手武器)
			case ITEM_SUBCLASS_WEAPON_CHAINSWORD:                // 锁链剑(双手武器)
				proto.melee_hit_chance = fields[21].GetInt32();
				proto.melee_damage_done = fields[22].GetInt32();
				proto.flags |= ITEM_FLAG_TWO_HANDED_WEAPON;
				break;
			case ITEM_SUBCLASS_WEAPON_SWORD:                     // 单手剑
			case ITEM_SUBCLASS_WEAPON_DAGGER:                    // 匕首(单手武器)
			case ITEM_SUBCLASS_WEAPON_AXE:                       // 单手斧/锤
			case ITEM_SUBCLASS_WEAPON_STAFF:                     // 魔杖(单手武器)
			case ITEM_SUBCLASS_WEAPON_SPEAR:                     // 矛(单手武器)
			case ITEM_SUBCLASS_WEAPON_KIRINGKU:                  // 奇古兽(单手武器)
				proto.melee_hit_chance = fields[21].GetInt32();
				proto.melee_damage_done = fields[22].GetInt32();
				proto.inventory_type = INVTYPE_WEAPON;
				break;
			default:
				proto.inventory_type = INVTYPE_NON_EQUIP;
				break;
		}
		proto.strength = fields[23].GetInt32();
		proto.stamina = fields[24].GetInt32();
		proto.agility = fields[25].GetInt32();
		proto.intellect = fields[26].GetInt32();
		proto.spirit = fields[27].GetInt32();
		proto.charm = fields[28].GetInt32();
		proto.health = fields[29].GetInt32();
		proto.mana = fields[30].GetInt32();
		proto.health_regen = fields[31].GetInt32();
		proto.mana_regen = fields[32].GetInt32();
		proto.spell_power = fields[33].GetInt32();
		proto.defense_magic = fields[34].GetInt32();
		proto.flags |= fields[35].GetBool() ? ITEM_FLAG_HASTE : 0;
		proto.double_damage_chance = fields[36].GetUInt32();
		proto.spell_damage_done = fields[37].GetInt32();
		proto.flags |= fields[38].GetBool() ? 0 : ITEM_FLAG_CANT_DAMAGE;
		proto.min_use_level = fields[39].GetUInt32();
		proto.max_use_level = fields[40].GetUInt32();
		proto.bless = fields[41].GetUInt8();

		++count;
	} while (result->NextRow());

	delete result;

	sLog.outString();
	sLog.outString(">> Loaded " SIZEFMTD " weapons", count);
}



