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

#ifndef MANGOS_ITEMS_H_
#define MANGOS_ITEMS_H_

#include "Common.h"
#include "SharedDefines.h"

struct ItemSpell
{
	uint32 SpellId;        // 触发法术编号
	uint32 SpellTrigger;   // 触发模式 ItemSpelltriggerType
	int32  SpellCooldown;  // 冷却时间
};

#define MAX_ITEM_PROTO_SPELLS  1

struct ItemEntry
{
	uint32 ItemId;                // 道具模板编号
	ItemClass Class;              // 道具类型 ItemClass
	uint32 SubClass;              // 道具子类型 ItemSubclassWeapon ItemSubclassArmor ItemSubclassGeneric

	std::string name;             // 道具名称
	uint32 description;           // 道具描述信息编号
	uint32 weight;                // 道具重量
	uint32 flags;                 // 标志 ItemEntryFlags
	uint32 allowable;             // 职业使用限制
	uint8 use_type;               // 使用类型(仅用于发送给客户端，服务端这边使用Class + SubClass已经知道怎么处理道具了)
	uint8 bless;                  // 道具祝福类型 0-普通 1-祝福 2-诅咒
	ItemBondingType bonding;      // 道具绑定类型
	ItemMaterialType material;    // 道具材质
	InventoryType inventory_type; // 佩戴位置

	std::string identified;       // 鉴定时道具名称编号
	std::string unidentified;     // 未鉴定时道具名称编号

	uint32 gfx_in_backpack;       // 道具放置在背包/仓库中用的GFX图片编号
	uint32 gfx_on_ground;         // 道具放置在地面上用的GFX图片编号

	uint32 min_use_level;         // 道具最低使用等级
	uint32 max_use_level;         // 道具最高使用等级

	uint32 spell_id;              // 法术道具效果
	uint32 spell_trigger;         // 法术道具效果触发条件 ItemSpelltriggerType
	uint32 spell_cooldown;        // 法术道具冷却时间

	union {
		// 可装备的道具的特性
		struct {
			uint32 safenchant;                   // 安定值
			uint32 max_use_time;                 // 道具最大使用时间
			int32 melee_hit_chance;              // 提升/降低近战命中
			int32 melee_damage_done;             // 提升/降低近战伤害
			int32 ranged_hit_chance;             // 提升/降低远程命中
			int32 ranged_damage_done;            // 提升/降低远程伤害
			int32 strength;                      // 提升/降低力量属性
			int32 agility;                       // 提升/降低力量属性
			int32 stamina;                       // 提升/降低力量属性
			int32 spirit;                        // 提升/降低力量属性
			int32 charm;                         // 提升/降低力量属性
			int32 intellect;                     // 提升/降低力量属性
			int32 health;                        // 提升/降低体力上限
			int32 mana;                          // 提升/降低魔力上限
			int32 health_regen;                  // 提升/降低体力回复
			int32 mana_regen;                    // 提升/降低魔力回复
			int32 spell_power;                   // 提升/降低法术强度
			int32 defense_magic;                 // 提升/降低法术防御

			union {
				// 防具特性
				struct {
					int32 armor;                 // 防御
					int32 item_set;              // 套装
					int32 damage_reduction;      // 伤害减免
					int32 weight_reduction;      // 负重减免
					int32 defense_wind;          // 水属性防御
					int32 defense_water;         // 水属性防御
					int32 defense_fire;          // 水属性防御
					int32 defense_earth;         // 水属性防御
					int32 resist_stun;           // 眩晕抗性
					int32 resist_stone;          // 石化抗性
					int32 resist_sleep;          // 睡眠抗性
					int32 resist_freeze;         // 冻结抗性
					int32 resist_sustain;        // 支撑抗性
					int32 resist_blind;          // 致盲抗性
				};

				// 武器特性
				struct {
					struct {
						int32 damage_tiny;       // 小怪伤害
						int32 damage_large;      // 大怪伤害
					};                           // 武器伤害 WeaponDamageType
					uint32 double_damage_chance; // 武器暴击率(双刀)/重击率(爪)
					uint32 attack_range;         // 武器攻击范围
					int32 spell_damage_done;     // 提升/降低法术伤害
				};
			};
		};

		// 不可装备的道具的特性
		struct {
			struct {
				int32 ammo_damage_tiny;  // 小怪伤害
				int32 ammo_damage_large; // 大怪伤害
			};
			int32 food;                  // 饱食度
			CoordUnit x, y;              // 道具刷新地点
			uint32 map;                  // 道具刷新地图
			uint32 max_charge_count;     // 道具最大使用次数
		};
	};

	bool CanBeDamage() const
	{
		return !(flags & ITEM_FLAG_CANT_DAMAGE);
	}

	bool CanBeSeal() const
	{
		return !(flags & ITEM_FLAG_CANT_SEAL);
	}

	bool IsStackable() const
	{
		return flags & ITEM_FLAG_STACKABLE;
	}

	bool IsHaste() const
	{
		return flags & ITEM_FLAG_HASTE;
	}

	bool IsTwoHandWeapon() const
	{
		return flags & ITEM_FLAG_TWO_HANDED_WEAPON;
	}

	bool IsUsable() const
	{
		return flags & ITEM_FLAG_USABLE;
	}
};

#endif //MANGOS_ITEMS_H_
