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

#ifndef MANGOS_SPELLS_H_
#define MANGOS_SPELLS_H_

#include "Common.h"
#include "SharedDefines.h"

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct SpellEntry
{
	/**
	 * 基本
	 */
	uint32    Id;                                      // 法术编号
	uint32    School;                                  // 法术属性 enum SpellSchools
	uint32    Category;                                // 法术类别 诅咒等
	uint32    Dispel;                                  // 法术驱散类型 enum DispelType
	uint32    Mechanic;                                // 法术机制 enum MechanicsType
	uint32    Duration;                                // 持续时间
	int32     Range;                                   // 施法距离
	uint32    RecoveryTime;                            // 冷却时间
	int32     PowerCost[MAX_POWERS];                   // 能量消耗量
	uint32    DmgClass;                                // 伤害类型 enum SpellDmgClass
	/**
	 * 效果
	 */
	uint32    Targets;                                 // 法术目标 enum SpellCastTargetFlags
	uint32    TargetCreatureType;                      // 法术目标生物类型
	uint32    ProcFlags;                               // 触发法术条件 enum ProcFlags
	int32     ProcChance;                              // 触发法术几率
	int32     ProcCharges;                             // 触发法术次数
	uint32    Effect[MAX_EFFECT_INDEX];                // 法术效果 enum SpellEffects
	uint32    EffectItemType[MAX_EFFECT_INDEX];        // 法术道具编号
	int32     EffectBasePoints[MAX_EFFECT_INDEX];      // 基础点数
	int32     EffectBaseDice[MAX_EFFECT_INDEX];        // 浮动点数
	uint32    EffectImplicitTarget[MAX_EFFECT_INDEX];  // 法术效果目标 enum Targets
	int32     EffectRadius[MAX_EFFECT_INDEX];          // 法术效果范围
	uint32    EffectApplyAuraName[MAX_EFFECT_INDEX];   // 法术效果产生的光环编号 enum AuraType
	int32     EffectMiscValue[MAX_EFFECT_INDEX];       // 光环参数
	uint32    EffectTriggerSpell[MAX_EFFECT_INDEX];    // 法术效果触发的新法术编号
	/**
	 * 物品
	 */
	uint32    Reagent[MAX_SPELL_REAGENTS];              // 消耗物品的item_id
	int32     ReagentCount[MAX_SPELL_REAGENTS];         // 消耗物品个数
	int32     EquippedItemClass;                        // 需要装备的物品类型 enum ItemClass
	int32     EquippedItemSubClassMask;                 // 需要装备的物品子类型 enum ItemSubclassWeapon | enum ItemSubclassArmor
	int32     EquippedItemInventoryTypeMask;            // 需要装备的部位
	/**
	 * 标识
	 */
	uint32    AuraInterruptFlags;                       // 光环中断标志
	uint32    Attributes;                               // 法术属性
	/**
	 * 消息
	 */
	uint32    ActionID;                                 //
	uint32    CastGFXID[2];                             // 施法动作
	uint32    MessageID[3];                             // 0 - 施法成功 1 - 施法取消 2 - 施法失败
	/**
	 * 法术等级
	 */
	uint32    SpellFamilyName;                         // 职业法术类型 enum SpellFamily
	uint32    Rank;                                    // 法术等级 1 - 10 法师职业法术 11 - 12 骑士职业法术 13 - 14 黑暗精灵职业法术 15 王族职业法术 17 - 22 精灵职业法术 23 - 25 龙骑士职业法术 26 - 28 幻术师职业法术
	uint32    Number;                                  // 法术等级内编号 0 - 7 (每个法术等级包含8个法术)

	inline bool HasAttribute(SpellAttributes attribute) const { return Attributes & attribute; }
	inline SpellSchools GetSpellSchool() const { return SpellSchools(School); }
	inline SpellDmgClass GetDmgClass() const { return SpellDmgClass(DmgClass); }
	inline uint32 GetStackAmount() const { return 0;}   // 天堂目前没有法术可堆叠
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

SpellFamily GetSpellFamily(uint8 rank);

#endif //MANGOS_SPELLS_H_
