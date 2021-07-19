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

#ifndef MANGOS_SHAREDDEFINES_H
#define MANGOS_SHAREDDEFINES_H

#include <cassert>
#include "Platform/Define.h"
#include "MagicNumber.h"

enum Gender
{
    GENDER_MALE                        = 0,
    GENDER_FEMALE                      = 1,
    GENDER_NONE                        = 2
};

#define MAX_GENDER                       3

enum Races
{
    RACE_NONE              = 0,
};

#define MAX_RACES         1

enum Classes : uint8
{
	CLASS_NONE          = 0,
    CLASS_PRINCE        = 1,
    CLASS_KNIGHT        = 2,
    CLASS_ELF           = 3,
    CLASS_WIZARD        = 4,
    CLASS_DARK_ELF      = 5,
    CLASS_DRAGON_KNIGHT = 6,
    CLASS_ILLUSIONIST   = 7
};

// max+1 for unit class
#define MAX_CLASSES       9

#define CLASSMASK_ALL_PLAYABLE \
    ((1<<(CLASS_PRINCE-1))|(1<<(CLASS_KNIGHT-1))|(1<<(CLASS_ELF-1))| \
    (1<<(CLASS_WIZARD-1))  |(1<<(CLASS_DARK_ELF-1)) |(1<<(CLASS_DRAGON_KNIGHT-1))| \
    (1<<(CLASS_ILLUSIONIST-1))

enum Stats
{
    STAT_STRENGTH                      = 0,
    STAT_AGILITY                       = 1,
    STAT_STAMINA                       = 2,
    STAT_SPIRIT                        = 3,
    STAT_CHARM                         = 4,
    STAT_INTELLECT                     = 5
};

#define MAX_STATS                        6

enum Powers
{
    POWER_MANA                          = 0,
    POWER_HEALTH                        = 1,
    POWER_LAWFUL                        = 2
};

#define MAX_POWERS                        3

/*********************************************************/
/***                     技能相关定义                    ***/
/*********************************************************/

#define MAX_AURAS 32                                        // 光环槽上限(玩家客户端右上角显示的技能图标)

/**
 * 法术属性
 */
enum SpellSchools
{
    SPELL_SCHOOL_NORMAL                 = 0, // 一般法术
    SPELL_SCHOOL_WIND                   = 1, // 风属性法术
    SPELL_SCHOOL_WATER                  = 2, // 水属性法术
    SPELL_SCHOOL_FIRE                   = 3, // 火属性法术
    SPELL_SCHOOL_EARTH                  = 4  // 地属性法术
};

#define MAX_SPELL_SCHOOL                  5

/**
 * 法术类别
 */
enum SpellCategories : uint8
{
	SPELLCATEGORY_PROBABILITY = 0x01,
	SPELLCATEGORY_CHANGE      = 0x02,
	SPELLCATEGORY_CURSE       = 0x04,
	SPELLCATEGORY_DEATH       = 0x08,
	SPELLCATEGORY_HEAL        = 0x10,
	SPELLCATEGORY_RESTORE     = 0x20,
	SPELLCATEGORY_ATTACK      = 0x40,
	SPELLCATEGORY_OTHER       = 0x80,
};

/**
 * 法术机制类型，一些法术除了带来伤害或者治疗，还会给Unit附加一些特殊状态，我们将这些额外效果称为法术机制
 */
enum MechanicsType
{
    MECHANIC_NONE             = 0,
    MECHANIC_STUN             = 1, // 眩晕
    MECHANIC_STONE            = 2, // 石化
    MECHANIC_SLEEP            = 3, // 睡眠
    MECHANIC_FREEZE           = 4, // 冰冻
    MECHANIC_SUSTAIN          = 5, // 支撑 ?
    MECHANIC_BLIND            = 6 // 致盲
};

#define MAX_STORED_MECHANICS    6

/**
 * 法术驱散类型
 */
enum DispelType
{
    DISPEL_NONE         = 0, // 表示当前法术不能被驱散
    DISPEL_MAGIC        = 1, // 魔法，表示当前法术能被魔法相消术(一种能驱散所有魔法效果的法术)驱散
    DISPEL_CURSE        = 2, // 诅咒，表示当前法术能被解除诅咒类法术/魔法相消术驱散
    DISPEL_POISON       = 3, // 中毒，表示当前法术能被解毒类法术/魔法相消术驱散
    DISPEL_INVISIBILITY = 4, // 隐身，表示当前法术能被显形类法术/魔法相消术驱散
    DISPEL_ALL          = 5
};

#define DISPEL_ALL_MASK ( (1<<DISPEL_MAGIC) | (1<<DISPEL_CURSE) | (1<<DISPEL_POISON) )

/**
 * 法术免疫类型
 */
enum ImmunityType
{
    IMMUNITY_EFFECT                = 0,                     // 免疫特定法术效果(enum SpellEffects)
    IMMUNITY_STATE                 = 1,                     // 免疫特定光环效果(enum AuraType)，如果一个法术存在多个光环效果，其他光环效果不受影响
    IMMUNITY_SCHOOL                = 2,                     // 免疫法术(enum SpellSchoolMask)
    IMMUNITY_DAMAGE                = 3,                     // 免疫伤害(enum SpellSchoolMask)
    IMMUNITY_DISPEL                = 4,                     // 免疫特定法术驱散(enum DispelType)
    IMMUNITY_MECHANIC              = 5                      // 免疫特定法术机制(enum MechanicsType)
};

#define MAX_IMMUNITY           5

/**
 * 一个法术最多产生三个法术效果
 */
enum SpellEffectIndex : uint8
{
    EFFECT_INDEX_0     = 0,
    EFFECT_INDEX_1     = 1,
    EFFECT_INDEX_2     = 2
};

#define MAX_EFFECT_INDEX 3

/**
 * 法术效果类型
 */
enum SpellEffects
{
    SPELL_EFFECT_NONE                      = 0,
    SPELL_EFFECT_SCHOOL_DAMAGE             = 1,  // 属性伤害
    SPELL_EFFECT_DUMMY                     = 2,  // 需要特殊处理
    SPELL_EFFECT_APPLY_AURA                = 3,  // 法术效果为产生一个光环(enum AuraType)
	SPELL_EFFECT_MANA_DRAIN                = 8,  // 吸取魔力
	SPELL_EFFECT_HEALTH_LEECH              = 9,  // 吸取体力
	SPELL_EFFECT_HEAL                      = 10, // 治疗

    SPELL_EFFECT_PERSISTENT_AREA_AURA      = 27, // 持有区域光环

	SPELL_EFFECT_SUMMON                    = 28, // 召唤

    SPELL_EFFECT_APPLY_AREA_AURA_PARTY     = 35, // 范围光环

	SPELL_EFFECT_LEARN_SPELL               = 36, // 学习技能

	SPELL_EFFECT_DISPEL                    = 38, // 驱散

	SPELL_EFFECT_SUMMON_PET                = 56, // 召唤宠物

	SPELL_EFFECT_TRIGGER_SPELL             = 64, // 触发技能

	SPELL_EFFECT_SUMMON_PLAYER             = 85, // 召唤玩家

	SPELL_EFFECT_RESURRECT                 = 113, // 复活

	SPELL_EFFECT_APPLY_AREA_AURA_FRIEND    = 128, // 给队友施加BUFF
	SPELL_EFFECT_APPLY_AREA_AURA_ENEMY     = 129, // 给敌人施加BUFF

    TOTAL_SPELL_EFFECTS                    = 4
};

/**
 * 光环类型
 */
enum AuraType
{
    SPELL_AURA_NONE                        = 0,
    SPELL_AURA_DUMMY                       = 1,   // 需要编写特定代码处理的光环
    SPELL_AURA_PERIODIC_DAMAGE             = 2,   // 周期触发伤害
    SPELL_AURA_PERIODIC_TRIGGER_SPELL      = 3,   // 周期触发技能
	SPELL_AURA_MOD_INVISIBILITY            = 4,   // 隐形

	SPELL_AURA_MOD_HEALING_PCT             = 118, // 修正自身作为被治疗者所受到的额外治疗效果(百分比) 例如水之元气会造成治疗量翻倍，而污浊之水会造成治疗减半
	SPELL_AURA_MOD_HEALING_DONE_PERCENT    = 136, // 修正自身作为治疗施法者所带来的额外治疗效果(百分比)
    TOTAL_AURAS = 5
};

/**
 * 区域光环类型
 */
enum AreaAuraType
{
    AREA_AURA_PARTY,
    AREA_AURA_RAID,
    AREA_AURA_FRIEND,
    AREA_AURA_ENEMY,
    AREA_AURA_PET,
    AREA_AURA_OWNER
};

// 玩家施法目标，例如燃烧的火球需要指定一个NPC，初级治疗需要指定一个玩家或NPC
enum SpellCastTargetFlags
{
	TARGET_FLAG_SELF            = 0x00000000,
	TARGET_FLAG_UNIT            = 0x00000001,
	TARGET_FLAG_SOURCE_LOCATION = 0x00000002,
	TARGET_FLAG_DEST_LOCATION   = 0x00000004,
};

// 法术效果目标，例如燃烧的火球施法时只指定了一个NPC目标(施法目标)，但是最终目标NPC周围4格范围内的所有NPC(法术效果目标)都会受到火球伤害
enum Targets
{
	TARGET_NONE                        = 0,
	TARGET_SELF                        = 1,
};

enum SpellMissInfo
{
    SPELL_MISS_NONE                    = 0,
    SPELL_MISS_MISS                    = 1,
    SPELL_MISS_RESIST                  = 2,
    SPELL_MISS_IMMUNE                  = 3,
    SPELL_MISS_REFLECT                 = 4
};

enum SpellHitType
{
};

enum SpellDmgClass
{
    SPELL_DAMAGE_CLASS_NONE     = 0,
    SPELL_DAMAGE_CLASS_MAGIC    = 1,
    SPELL_DAMAGE_CLASS_MELEE    = 2,
    SPELL_DAMAGE_CLASS_RANGED   = 3
};

enum DamageEffectType
{
	DIRECT_DAMAGE           = 0,                            // used for normal weapon damage (not for class abilities or spells)
	SPELL_DIRECT_DAMAGE     = 1,                            // spell/class abilities damage
	DOT                     = 2,
	HEAL                    = 3,
	NODAMAGE                = 4,                            // used also in case when damage applied to health but not applied to spell channelInterruptFlags/etc
	SELF_DAMAGE             = 5
};

enum AuraFlags
{
    AFLAG_NONE                  = 0x00
};

enum AuraRemoveMode : uint8
{
    AURA_REMOVE_BY_DEFAULT,
    AURA_REMOVE_BY_STACK,                                   // at replace by similar aura
    AURA_REMOVE_BY_CANCEL,
    AURA_REMOVE_BY_DISPEL,
    AURA_REMOVE_BY_DEATH,
    AURA_REMOVE_BY_DELETE,                                  // use for speedup and prevent unexpected effects at player logout/pet unsummon (must be used _only_ after save), delete.
    AURA_REMOVE_BY_EXPIRE,                                  // at duration end
    AURA_REMOVE_BY_TRACKING,                                // aura is removed because of a conflicting tracked aura
};

enum SpellCastResult
{
    SPELL_FAILED_SUCCESS = 0,
	SPELL_FAILED_BAD_TARGETS                    = 0x0A, // 错误的法术目标
	SPELL_FAILED_DONT_REPORT                    = 0x17, // 忽略失败结果(一般是触发类法术在技能冷却时间内多次触发导致的失败)
	SPELL_FAILED_EQUIPPED_ITEM_CLASS            = 0x19, // 没有正确的装备道具
	SPELL_FAILED_ITEM_GONE                      = 0x26, // 丢失法术道具(交易中或丢弃)
	SPELL_FAILED_ITEM_NOT_READY                 = 0x28, // 施法道具有问题或者施法材料数量不够
	SPELL_FAILED_NOT_READY                      = 0x3C, // 技能CD
	SPELL_FAILED_NO_CHARGES_REMAIN              = 0x44, // 法术道具使用次数已用尽
	SPELL_FAILED_NO_POWER                       = 0x4D, // 法力不够
	SPELL_FAILED_OUT_OF_RANGE                   = 0x59, // 超出施法距离
    SPELL_CAST_OK = 255
};

// Used in addition to SPELL_FAILED_CUSTOM_ERROR
enum SpellCastResultCustom
{
    // Postfix _NONE will not display the text in client
    SPELL_FAILED_CUSTOM_ERROR_1         = 1,                // "Something bad happened, and we want to display a custom message!"
};

// ***********************************
// Spell Attributes definitions
// ***********************************

enum SpellAttributes
{
    SPELL_ATTR_PASSIVE                         = 0x00000001, // 0 被动法术
	SPELL_ATTR_NEGATIVE                        = 0x00000002, // 1 负面法术
    SPELL_ATTR_NOT_BREAK_STEALTH               = 0x00000004, // 2 施放法术不会解除隐身状态
    SPELL_ATTR_DEATH_PERSISTENT                = 0x00000008, // 3 死亡后法术效果依然存在(召唤、迷魅)
};

enum SpellFamily
{
    SPELL_FAMILY_NONE          = 0, // BOSS法术
    SPELL_FAMILY_PRINCE        = 1, // 王族职业法术
    SPELL_FAMILY_KNIGHT        = 2, // 骑士职业法术
    SPELL_FAMILY_ELF           = 3, // 精灵职业法术
    SPELL_FAMILY_WIZARD        = 4, // 法师职业法术
    SPELL_FAMILY_DARK_ELF      = 5, // 黑暗精灵职业法术
    SPELL_FAMILY_DRAGON_KNIGHT = 6, // 龙骑士职业法术
    SPELL_FAMILY_ILLUSIONIST   = 7  // 幻术师职业法术
};

enum AuraInterruptFlags
{
	AURA_INTERRUPT_FLAG_UNK0                        = 0x00000001,   // 0    removed when getting hit by a negative spell?
	AURA_INTERRUPT_FLAG_DAMAGE                      = 0x00000002,   // 1    removed by any damage
	AURA_INTERRUPT_FLAG_UNK2                        = 0x00000004,   // 2
	AURA_INTERRUPT_FLAG_MOVE                        = 0x00000008,   // 3    removed by any movement
	AURA_INTERRUPT_FLAG_TURNING                     = 0x00000010,   // 4    removed by any turning
	AURA_INTERRUPT_FLAG_ENTER_COMBAT                = 0x00000020,   // 5    removed by entering combat
	AURA_INTERRUPT_FLAG_NOT_MOUNTED                 = 0x00000040,   // 6    removed by unmounting
	AURA_INTERRUPT_FLAG_NOT_ABOVEWATER              = 0x00000080,   // 7    removed by entering water
	AURA_INTERRUPT_FLAG_NOT_UNDERWATER              = 0x00000100,   // 8    removed by leaving water
	AURA_INTERRUPT_FLAG_NOT_SHEATHED                = 0x00000200,   // 9    removed by unsheathing
	AURA_INTERRUPT_FLAG_UNK10                       = 0x00000400,   // 10
	AURA_INTERRUPT_FLAG_UNK11                       = 0x00000800,   // 11
	AURA_INTERRUPT_FLAG_MELEE_ATTACK                = 0x00001000,   // 12   removed by melee attacks
	AURA_INTERRUPT_FLAG_UNK13                       = 0x00002000,   // 13
	AURA_INTERRUPT_FLAG_UNK14                       = 0x00004000,   // 14
	AURA_INTERRUPT_FLAG_UNK15                       = 0x00008000,   // 15   removed by casting a spell?
	AURA_INTERRUPT_FLAG_UNK16                       = 0x00010000,   // 16
	AURA_INTERRUPT_FLAG_MOUNTING                    = 0x00020000,   // 17   removed by mounting
	AURA_INTERRUPT_FLAG_NOT_SEATED                  = 0x00040000,   // 18   removed by standing up (used by food and drink mostly and sleep/Fake Death like)
	AURA_INTERRUPT_FLAG_CHANGE_MAP                  = 0x00080000,   // 19   leaving map/getting teleported
	AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION    = 0x00100000,   // 20   removed by auras that make you invulnerable, or make other to loose selection on you
	AURA_INTERRUPT_FLAG_UNK21                       = 0x00200000,   // 21
	AURA_INTERRUPT_FLAG_UNK22                       = 0x00400000,   // 22
	AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT            = 0x00800000,   // 23   removed by entering pvp combat
	AURA_INTERRUPT_FLAG_DIRECT_DAMAGE               = 0x01000000    // 24   removed by any direct damage
};

enum ProcFlags
{
    PROC_FLAG_NONE                          = 0x00000000,

    PROC_FLAG_KILLED                        = 0x00000001,   // 00 Killed by aggressor
    PROC_FLAG_KILL                          = 0x00000002,   // 01 Kill target (in most cases need XP/Honor reward, see Unit::IsTriggeredAtSpellProcEvent for additinoal check)

    PROC_FLAG_SUCCESSFUL_MELEE_HIT          = 0x00000004,   // 02 Successful melee auto attack
    PROC_FLAG_TAKEN_MELEE_HIT               = 0x00000008,   // 03 Taken damage from melee auto attack hit

    PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT    = 0x00000010,   // 04 Successful attack by Spell that use melee weapon
    PROC_FLAG_TAKEN_MELEE_SPELL_HIT         = 0x00000020,   // 05 Taken damage by Spell that use melee weapon

    PROC_FLAG_SUCCESSFUL_RANGED_HIT         = 0x00000040,   // 06 Successful Ranged auto attack
    PROC_FLAG_TAKEN_RANGED_HIT              = 0x00000080,   // 07 Taken damage from ranged auto attack

    PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT   = 0x00000100,   // 08 Successful Ranged attack by Spell that use ranged weapon
    PROC_FLAG_TAKEN_RANGED_SPELL_HIT        = 0x00000200,   // 09 Taken damage by Spell that use ranged weapon

    PROC_FLAG_SUCCESSFUL_POSITIVE_AOE_HIT   = 0x00000400,   // 10 Successful AoE (not 100% shure unused)
    PROC_FLAG_TAKEN_POSITIVE_AOE            = 0x00000800,   // 11 Taken AoE      (not 100% shure unused)

    PROC_FLAG_SUCCESSFUL_AOE_SPELL_HIT      = 0x00001000,   // 12 Successful AoE damage spell hit (not 100% shure unused)
    PROC_FLAG_TAKEN_AOE_SPELL_HIT           = 0x00002000,   // 13 Taken AoE damage spell hit      (not 100% shure unused)

    PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL     = 0x00004000,   // 14 Successful cast positive spell (by default only on healing)
    PROC_FLAG_TAKEN_POSITIVE_SPELL          = 0x00008000,   // 15 Taken positive spell hit (by default only on healing)

    PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT = 0x00010000,   // 16 Successful negative spell cast (by default only on damage)
    PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT      = 0x00020000,   // 17 Taken negative spell (by default only on damage)

    PROC_FLAG_ON_DO_PERIODIC                = 0x00040000,   // 18 Successful do periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)
    PROC_FLAG_ON_TAKE_PERIODIC              = 0x00080000,   // 19 Taken spell periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)

    PROC_FLAG_TAKEN_ANY_DAMAGE              = 0x00100000,   // 20 Taken any damage
    PROC_FLAG_ON_TRAP_ACTIVATION            = 0x00200000,   // 21 On trap activation

    PROC_FLAG_TAKEN_OFFHAND_HIT             = 0x00400000,   // 22 Taken off-hand melee attacks(not used)
    PROC_FLAG_SUCCESSFUL_OFFHAND_HIT        = 0x00800000,   // 23 Successful off-hand melee attacks

    PROC_FLAG_ON_DEATH                      = 0x01000000,   // 24 On death
    PROC_FLAG_ON_JUMP                       = 0x02000000,   // 25 On jump

    PROC_FLAG_UNK26                         = 0x04000000,   // 26 not used
    PROC_FLAG_UNK27                         = 0x08000000,   // 27 not used

    PROC_FLAG_UNK28                         = 0x10000000,   // 28 not used
    PROC_FLAG_UNK29                         = 0x20000000,   // 29 not used

    PROC_FLAG_UNK30                         = 0x40000000,   // 30 not used
    PROC_FLAG_UNK31                         = 0x80000000,   // 31 not used
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_SUCCESSFUL_MELEE_HIT        | \
                                  PROC_FLAG_TAKEN_MELEE_HIT             | \
                                  PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT  | \
                                  PROC_FLAG_TAKEN_MELEE_SPELL_HIT       | \
                                  PROC_FLAG_SUCCESSFUL_RANGED_HIT       | \
                                  PROC_FLAG_TAKEN_RANGED_HIT            | \
                                  PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT | \
                                  PROC_FLAG_TAKEN_RANGED_SPELL_HIT)

#define NEGATIVE_TRIGGER_MASK (MELEE_BASED_TRIGGER_MASK                | \
                               PROC_FLAG_SUCCESSFUL_AOE_SPELL_HIT      | \
                               PROC_FLAG_TAKEN_AOE_SPELL_HIT           | \
                               PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT | \
                               PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT)

#define SPELL_CAST_TRIGGER_MASK (PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT    | \
                                 PROC_FLAG_SUCCESSFUL_RANGED_HIT         | \
                                 PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT   | \
                                 PROC_FLAG_SUCCESSFUL_POSITIVE_AOE_HIT   | \
                                 PROC_FLAG_SUCCESSFUL_AOE_SPELL_HIT      | \
                                 PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL     | \
                                 PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT)


// 光环状态，例如有些技能会在血量低于20%时发动
enum AuraState
{
    // (C) used in caster aura state     (T) used in target aura state
    // (c) used in caster aura state-not (t) used in target aura state-not
    AURA_STATE_DEFENSE                      = 1,            // C   |
    AURA_STATE_HEALTHLESS_20_PERCENT        = 2,            // CcT |
    AURA_STATE_BERSERKING                   = 3,            // C T |
    AURA_STATE_FROZEN                       = 4,            //  cT | frozen target
    AURA_STATE_JUDGEMENT                    = 5,            // C   |
    // AURA_STATE_UNKNOWN6                   = 6,           //     | not used
    AURA_STATE_HUNTER_PARRY                 = 7,            // C   |
    AURA_STATE_ROGUE_ATTACK_FROM_STEALTH    = 7,            // C   | FIX ME: not implemented yet!
    // AURA_STATE_UNKNOWN7                   = 7,           //  c  | random/focused bursts spells (?)
    // AURA_STATE_UNKNOWN8                   = 8,           //     | not used
    // AURA_STATE_UNKNOWN9                   = 9,           //     | not used
    AURA_STATE_WARRIOR_VICTORY_RUSH         = 10,           // C   | warrior victory rush
    // AURA_STATE_UNKNOWN11                  = 11,          //    t|
    AURA_STATE_FAERIE_FIRE                  = 12,           //  c t|
    AURA_STATE_HEALTHLESS_35_PERCENT        = 13,           // C T |
    AURA_STATE_CONFLAGRATE                  = 14,           //   T | per-caster
    AURA_STATE_SWIFTMEND                    = 15,           //   T |
    AURA_STATE_DEADLY_POISON                = 16,           //   T |
    AURA_STATE_ENRAGE                       = 17,           // C   |
    AURA_STATE_BLEEDING                     = 18,           // C  t|
    // AURA_STATE_UNKNOWN19                  = 19,          //     | not used
    // AURA_STATE_UNKNOWN20                  = 20,          //  c  | only (45317 Suicide)
    // AURA_STATE_UNKNOWN21                  = 21,          //     | not used
    // AURA_STATE_UNKNOWN22                  = 22,          // C   | not implemented yet (Requires Evasive Charges to use)
    AURA_STATE_HEALTH_ABOVE_75_PERCENT      = 23,           // C   |
};

enum ProcFlagsEx
{
    PROC_EX_NONE                = 0x0000000,                // If none can tigger on Hit/Crit only (passive spells MUST defined by SpellFamily flag)
    PROC_EX_NORMAL_HIT          = 0x0000001,                // If set only from normal hit (only damage spells)
    PROC_EX_CRITICAL_HIT        = 0x0000002,
    PROC_EX_MISS                = 0x0000004,
    PROC_EX_RESIST              = 0x0000008,
    PROC_EX_DODGE               = 0x0000010,
    PROC_EX_PARRY               = 0x0000020,
    PROC_EX_BLOCK               = 0x0000040,
    PROC_EX_EVADE               = 0x0000080,
    PROC_EX_IMMUNE              = 0x0000100,
    PROC_EX_DEFLECT             = 0x0000200,
    PROC_EX_ABSORB              = 0x0000400,
    PROC_EX_REFLECT             = 0x0000800,
    PROC_EX_INTERRUPT           = 0x0001000,                // Melee hit result can be Interrupt (not used)
    PROC_EX_FULL_BLOCK          = 0x0002000,                // block al attack damage
    PROC_EX_RESERVED2           = 0x0004000,
    PROC_EX_RESERVED3           = 0x0008000,
    PROC_EX_EX_TRIGGER_ALWAYS   = 0x0010000,                // If set trigger always ( no matter another flags) used for drop charges
    PROC_EX_EX_ONE_TIME_TRIGGER = 0x0020000,                // If set trigger always but only one time (not used)
    PROC_EX_PERIODIC_POSITIVE   = 0x0040000,                // For periodic heal
    PROC_EX_CAST_END            = 0x0080000                 // procs on end of cast
};

enum SpellTargets
{
    SPELL_TARGETS_HOSTILE,
    SPELL_TARGETS_NOT_FRIENDLY,
    SPELL_TARGETS_NOT_HOSTILE,
    SPELL_TARGETS_FRIENDLY,
    SPELL_TARGETS_AOE_DAMAGE,
    SPELL_TARGETS_ALL
};

enum DeathState
{
	ALIVE          = 0,                                     // show as alive
	JUST_DIED      = 1,                                     // temporary state at die, for creature auto converted to CORPSE, for player at next update call
	CORPSE         = 2,                                     // corpse state, for player this also meaning that player not leave corpse
	DEAD           = 3,                                     // for creature despawned state (corpse despawned), for player CORPSE/DEAD not clear way switches (FIXME), and use m_deathtimer > 0 check for real corpse state
	JUST_ALIVED    = 4,                                     // temporary state at resurrection, for creature auto converted to ALIVE, for player at next update call
};

enum VictimState
{
    VICTIMSTATE_UNAFFECTED     = 0,
    VICTIMSTATE_NORMAL         = 1,
    VICTIMSTATE_DODGE          = 2,
    VICTIMSTATE_INTERRUPT      = 3,
    VICTIMSTATE_EVADES         = 4,
    VICTIMSTATE_IS_IMMUNE      = 5,
    VICTIMSTATE_DEFLECTS       = 6
};

/*********************************************************/
/***                      Item                         ***/
/*********************************************************/

// 通过道具的库存类型很容易转换为装备槽位置，这样技能就能使用掩码的方式来记录所需套装(EquippedItemInventoryTypeMask)需要装备哪些位置
enum InventoryType
{
	INVTYPE_NON_EQUIP                           = 0,
	INVTYPE_HEAD                                = 1,  // 头部
	INVTYPE_NECK                                = 2,  // 颈部
	INVTYPE_EAR                                 = 3,  // 耳部
	INVTYPE_SHOULDERS                           = 4,  // 肩部
	INVTYPE_BODY                                = 5,  // 身体
	INVTYPE_CHEST                               = 6,  // 内衣
	INVTYPE_WAIST                               = 7,  // 腰部
	INVTYPE_FEET                                = 8,  // 脚部
	INVTYPE_HANDS                               = 9,  // 手部
	INVTYPE_FINGER                              = 10, // 指部
	INVTYPE_WEAPON                              = 11, // 武器(单手/双手)
	INVTYPE_SHIELD                              = 13, // 盾牌
	INVTYPE_RANGED                              = 14, // 弓(单手/双手)
	INVTYPE_CLOAK                               = 15  // 斗篷、披风
};

#define MAX_INVTYPE                               18

enum ItemClass
{
	ITEM_CLASS_GENERIC                          = 0, // 其他
	ITEM_CLASS_WEAPON                           = 1, // 武器
	ITEM_CLASS_ARMOR                            = 2  // 护甲
};

#define MAX_ITEM_CLASS                            3

enum ItemSubclassWeapon
{
	ITEM_SUBCLASS_WEAPON                        = 0,
	ITEM_SUBCLASS_WEAPON_SWORD                  = 1,  // 单手剑
	ITEM_SUBCLASS_WEAPON_DAGGER                 = 2,  // 匕首(单手武器)
	ITEM_SUBCLASS_WEAPON_SWORD2                 = 3,  // 双手剑
	ITEM_SUBCLASS_WEAPON_BOW                    = 4,  // 弓(双手武器)
	ITEM_SUBCLASS_WEAPON_SPEAR2                 = 5,  // 矛(双手武器)
	ITEM_SUBCLASS_WEAPON_AXE                    = 6,  // 单手斧/锤
	ITEM_SUBCLASS_WEAPON_STAFF                  = 7,  // 魔杖(单手武器)
	ITEM_SUBCLASS_WEAPON_STING					= 8,  // 飞刀
    ITEM_SUBCLASS_WEAPON_ARROW                  = 9,  // 箭矢
	ITEM_SUBCLASS_WEAPON_GAUNTLET               = 10, // 手甲(黑妖扔飞刀的单手武器)
	ITEM_SUBCLASS_WEAPON_CLAW                   = 11, // 钢爪(双手武器)
	ITEM_SUBCLASS_WEAPON_EDORYU                 = 12, // 双刀(双手武器)
	ITEM_SUBCLASS_WEAPON_CROSSBOW               = 13, // 弩(单手武器)
	ITEM_SUBCLASS_WEAPON_SPEAR                  = 14, // 矛(单手武器)
	ITEM_SUBCLASS_WEAPON_AXE2                   = 15, // 双手斧/锤
	ITEM_SUBCLASS_WEAPON_STAFF2					= 16, // 魔杖(双手武器)
	ITEM_SUBCLASS_WEAPON_KIRINGKU               = 17, // 奇古兽(单手武器)
	ITEM_SUBCLASS_WEAPON_CHAINSWORD             = 18  // 锁链剑(双手武器)
};

#define MAX_ITEM_SUBCLASS_WEAPON                  19

enum WeaponShapeType : uint8
{
	WEAPON_SHAPE_NONE		= 0,
	WEAPON_SHAPE_SWORD		= 4,
	WEAPON_SHAPE_DAGGER		= 46,
	WEAPON_SHAPE_SWORD2		= 50,
	WEAPON_SHAPE_BOW		= 20,
	WEAPON_SHAPE_AXE		= 11,
	WEAPON_SHAPE_SPEAR2		= 24,
	WEAPON_SHAPE_STAFF		= 40,
	// WEAPON_SHAPE_STING		= 2922,
	WEAPON_SHAPE_ARROW		= 66,
	WEAPON_SHAPE_GAUNTLET	= 62,
	WEAPON_SHAPE_CLAW		= 58,
	WEAPON_SHAPE_EDORYU		= 54,
	WEAPON_SHAPE_CROSSBOW	= 20,
	WEAPON_SHAPE_SPEAR		= 24,
	WEAPON_SHAPE_AXE2		= 11,
	WEAPON_SHAPE_STAFF2		= 40,
	WEAPON_SHAPE_KIRINGKU	= 58,
	WEAPON_SHAPE_CHAINSWORD	= 24
};

enum ItemSubclassArmor
{
	ITEM_SUBCLASS_ARMOR                         = 0,
	ITEM_SUBCLASS_ARMOR_HELMET                  = 1,  // 头盔
	ITEM_SUBCLASS_ARMOR_ARMOUR                  = 2,  // 盔甲
	ITEM_SUBCLASS_ARMOR_SHIRT                   = 3,  // T恤
	ITEM_SUBCLASS_ARMOR_CLOAK                   = 4,  // 斗篷
	ITEM_SUBCLASS_ARMOR_GLOVE                   = 5,  // 手套
	ITEM_SUBCLASS_ARMOR_BOOTS                   = 6,  // 靴子
	ITEM_SUBCLASS_ARMOR_SHIELD                  = 7,  // 盾牌
	ITEM_SUBCLASS_ARMOR_NECKLACE                = 8,  // 项链
	ITEM_SUBCLASS_ARMOR_RING                    = 9,  // 戒子
	ITEM_SUBCLASS_ARMOR_BELT                    = 10, // 腰带
	ITEM_SUBCLASS_ARMOR_RING2                   = 11, // 戒子
	ITEM_SUBCLASS_ARMOR_EARRING                 = 12, // 耳环
	ITEM_SUBCLASS_ARMOR_GARDEBRAS               = 13  // 臂甲
};

#define MAX_ITEM_SUBCLASS_ARMOR                   14

enum ItemSubclassGeneric
{
	ITEM_SUBCLASS_GENERIC_ARROW                 = 0,  // 箭
	ITEM_SUBCLASS_GENERIC_WAND                  = 1,  // 闪电魔杖
	ITEM_SUBCLASS_GENERIC_LIGHT                 = 2,  // 灯笼
	ITEM_SUBCLASS_GENERIC_GEM                   = 3,  // 宝石
	ITEM_SUBCLASS_GENERIC_TOTEM                 = 4,  // 图腾(天堂燃柳村任务道具)
	ITEM_SUBCLASS_GENERIC_FIRECRACKER           = 5,  // 烟花
	ITEM_SUBCLASS_GENERIC_POTION                = 6,  // 药水
	ITEM_SUBCLASS_GENERIC_FOOD                  = 7,  // 食物
	ITEM_SUBCLASS_GENERIC_SCROLL                = 8,  // 卷轴
	ITEM_SUBCLASS_GENERIC_QUESTITEM             = 9,  // 任务道具
	ITEM_SUBCLASS_GENERIC_SPELLBOOK             = 10, // 魔法书
	ITEM_SUBCLASS_GENERIC_PETITEM               = 11, // 宠物装备
	ITEM_SUBCLASS_GENERIC_OTHER                 = 12, // 杂项
	ITEM_SUBCLASS_GENERIC_MATERIAL              = 13, // 材料
	ITEM_SUBCLASS_GENERIC_EVENT                 = 14, // 卡片
	ITEM_SUBCLASS_GENERIC_STING                 = 15, // 飞刀
	ITEM_SUBCLASS_GENERIC_TREASURE_BOX          = 16, // 宝箱
	ITEM_SUBCLASS_GENERIC_MAGIC_DOLL            = 17, // 魔法娃娃
	ITEM_SUBCLASS_GENERIC_SCROLLSHOP            = 18, // 随身商店
	ITEM_SUBCLASS_GENERIC_TELEPORTSCROLL        = 19  // 传送卷轴
};

#define MAX_ITEM_SUBCLASS_GENERIC                 20

const uint32 MaxItemSubclassValues[MAX_ITEM_CLASS] = {
	MAX_ITEM_SUBCLASS_WEAPON,
	MAX_ITEM_SUBCLASS_ARMOR,
	MAX_ITEM_SUBCLASS_GENERIC
};

enum ItemEntryFlags
{
	ITEM_FLAG_CONJURED                        = 0x00000002, // 道具是通过SPELL_EFFECT_CREATE_ITEM法术效果创造出来的
    ITEM_FLAG_HASTE                           = 0x00000004, // 加速效果
	ITEM_FLAG_USABLE                          = 0x00000008, // 允许使用
	ITEM_FLAG_STACKABLE                       = 0x00000010, // 允许堆叠
	ITEM_FLAG_TWO_HANDED_WEAPON               = 0x00000020, // 双手武器
    ITEM_FLAG_CANT_SEAL                       = 0x00000080, // 不允许出售
    ITEM_FLAG_CANT_TRADE                      = 0x00000100, // 不允许交易
	ITEM_FLAG_CANT_DAMAGE                     = 0x00000200, // 不会损坏
	ITEM_FLAG_CANT_DELETE                     = 0x00000400, // 不允许删除
    ITEM_FLAG_CANT_DROP                       = 0x00000800, // 不允许丢弃
};

enum ItemSpelltriggerType
{
	ITEM_SPELLTRIGGER_ON_USE          = 0, // 使用后触发(魔法书)
	ITEM_SPELLTRIGGER_ON_EQUIP        = 1, // 装备后触发(力量魔法头盔)
	ITEM_SPELLTRIGGER_CHANCE_ON_HIT   = 2  // 击中后几率触发(克特之剑)
};

#define MAX_ITEM_SPELLTRIGGER           4

enum ItemBondingType
{
	NO_BIND                                     = 0, // 不绑定
	BIND_WHEN_PICKED_UP                         = 1, // 拾取绑定
	BIND_WHEN_EQUIPPED                          = 2, // 装备绑定
	BIND_WHEN_USE                               = 3, // 使用绑定
	BIND_QUEST_ITEM                             = 4  // 任务道具
};

#define MAX_BIND_TYPE                             5

enum ItemMaterialType
{
	ITEM_MATERIAL_NONE                          = 0,
	ITEM_MATERIAL_LIQUID                        = 1,  // 液体
	ITEM_MATERIAL_WEB                           = 2,  // 网(芮克妮的网)
	ITEM_MATERIAL_VEGETATION                    = 3,  // 植物类食物(香蕉)
	ITEM_MATERIAL_ANIMALMATTER                  = 4,  // 动物部分肉体(安塔瑞斯之眼)
	ITEM_MATERIAL_PAPER                         = 5,  // 纸
	ITEM_MATERIAL_CLOTH                         = 6,  // 布料
	ITEM_MATERIAL_LEATHER                       = 7,  // 皮毛(奇美拉之皮)
	ITEM_MATERIAL_WOOD                          = 8,  // 木材(闪电魔杖)
	ITEM_MATERIAL_BONE                          = 9,  // 骨头(安塔瑞斯之爪、蛇女之鳞)
	ITEM_MATERIAL_DRAGONSCALE                   = 10, // 龙鳞
	ITEM_MATERIAL_IRON                          = 11, // 铁
	ITEM_MATERIAL_STEEL                         = 12, // 钢
	ITEM_MATERIAL_COPPER                        = 13, // 铜
	ITEM_MATERIAL_SILVER                        = 14, // 银
	ITEM_MATERIAL_GOLD                          = 15, // 金
	ITEM_MATERIAL_PLATINUM                      = 16, // 白金
	ITEM_MATERIAL_MITHRIL                       = 17, // 米索莉
	ITEM_MATERIAL_BLACKMITHRIL                  = 18, // 黑色米索莉
	ITEM_MATERIAL_GLASS                         = 19, // 玻璃(红色药水、水晶球)
	ITEM_MATERIAL_GEMSTONE                      = 20, // 宝石
	ITEM_MATERIAL_MINERAL                       = 21, // 矿石(磨刀石、黑魔石)
	ITEM_MATERIAL_ORIHARUKON                    = 22  // 奥里哈鲁根
};

#define MAX_MATERIAL_TYPE                         23

enum ItemDynFlags
{
	ITEM_DYNFLAG_BINDED                       = 0x00000001, // 灵魂绑定
	ITEM_DYNFLAG_IDENTIFIED                   = 0x00000002, // 道具鉴定
	ITEM_DYNFLAG_EQUIPPED                     = 0x00000004  // 装备道具
};

enum ItemAttrEnchantType : uint8
{
	ITEM_ATTR_ENCHANT_TYPE_NONE             = 0,
	ITEM_ATTR_ENCHANT_TYPE_EARTH            = 1, // 地属性强化
	ITEM_ATTR_ENCHANT_TYPE_FIRE             = 2, // 火属性强化
	ITEM_ATTR_ENCHANT_TYPE_WATER            = 4, // 水属性强化
	ITEM_ATTR_ENCHANT_TYPE_WIND             = 8  // 风属性强化
};

/**
 * 天堂不像魔兽世界，没有背包这种类型的道具，所以就没有背包槽位(魔兽世界用来扩容背包大小的道具)，
 * 但是天堂也有类似魔兽世界银行这种仓库(即没有普通的背包道具，但是有银行这种特殊类型的背包道具)，
 * 其也是没有背包槽位的，所以这边将背包槽位全部去掉，仅保留普通的道具槽
 */
enum EquipmentSlots
{
	EQUIPMENT_SLOT_START        = 0,
	EQUIPMENT_SLOT_HEAD         = 0,  // 头部
	EQUIPMENT_SLOT_EAR          = 1,  // 耳部
	EQUIPMENT_SLOT_NECK         = 2,  // 颈部
	EQUIPMENT_SLOT_SHOULDER     = 3,  // 肩部
	EQUIPMENT_SLOT_CHEST        = 4,  // 胸部
	EQUIPMENT_SLOT_BODY         = 5,  // 身体
	EQUIPMENT_SLOT_WAIST        = 6,  // 腰部
	EQUIPMENT_SLOT_FEET         = 7,  // 脚部
	EQUIPMENT_SLOT_HANDS        = 8,  // 手部
	EQUIPMENT_SLOT_FINGER1      = 9,  // 指部
	EQUIPMENT_SLOT_FINGER2      = 10,
	EQUIPMENT_SLOT_FINGER3      = 11,
	EQUIPMENT_SLOT_BACK         = 12, // 背部
	EQUIPMENT_SLOT_MAINHAND     = 13, // 主手武器
	EQUIPMENT_SLOT_OFFHAND      = 14, // 副手武器/盾牌
	EQUIPMENT_SLOT_END          = 15,
	EQUIPMENT_SLOT_COUNT        = (EQUIPMENT_SLOT_END - EQUIPMENT_SLOT_START),
	NULL_SLOT                   = 0xFF
};

enum InventoryResult
{
	EQUIP_ERR_OK                                 = 0,
	EQUIP_ERR_NONE                               = 1,  // 参数错误
	EQUIP_ERR_ITEM_NOT_FOUND                     = 2,  // 道具或模板找不到
	EQUIP_ERR_BAG_FULL                           = 2,  // 仓库已满
	EQUIP_ERR_CANT_CARRY_MORE_OF_THIS            = 3,  // 无法再携带更多此类道具
	EQUIP_ERR_OVER_WEIGHT                        = 4,  // 超过负重限制
	EQUIP_ERR_CANT_TAKE_OFF                      = 5,  // 无法脱下装备(装备是受诅咒的)
	EQUIP_ERR_CANT_BE_EQUIPPED                   = 6,  // 无法装备，道具不是武器或者防具
	EQUIP_ERR_CANT_DUAL_BLOCK                    = 6,  // 盾牌和臂甲不能同时装备
	EQUIP_ERR_CANT_DUAL_WIELD                    = 7,  // 不能够同时使用两把武器
	EQUIP_ERR_CANT_EQUIP_LEVEL                   = 8,  // 不符合道具的等级限制
	EQUIP_ERR_CANT_EQUIP_WITH_TWOHANDED          = 9,  // 不能够在已经持有双手武器的情况下装备此道具，一般是盾牌
	EQUIP_ERR_YOU_ARE_DEAD                       = 11, // 你已经死亡，不能够切换装备
	EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM        = 12, // 不符合道具的职业限制
	EQUIP_ERR_YOU_ARE_LOGOUT                     = 13, // 玩家已经登出游戏(防止作弊)
};

enum WarehouseType
{
	WAREHOUSE_TYPE_NONE        = 0,
	WAREHOUSE_TYPE_DWARF       = 1,
	WAREHOUSE_TYPE_ELF         = 2,
	WAREHOUSE_TYPE_CLAN        = 3,
	WAREHOUSE_TYPE_GROUND      = 4
};

enum ItemAuraEffectType
{
	ITEM_AURA_EFFECT_TYPE_NONE		= 0,
	ITEM_AURA_EFFECT_TYPE_DAMAGE    = 1, // 提升/降低伤害
	ITEM_AURA_EFFECT_TYPE_HIT       = 2, // 提升/降低命中
	ITEM_AURA_EFFECT_TYPE_AC        = 3  // 提升/降低防御
};

#define MAX_ITEM_AURA_EFFECT 2

#define MAX_AMOUNT  2000000000
#define MAX_WEIGHT  1500

/*********************************************************/
/***                       Combat                      ***/
/*********************************************************/

enum WeaponAttackType
{
	MELEE_ATTACK   = 0,
	RANGED_ATTACK  = 1
};

#define MAX_ATTACK  2

/*********************************************************/
/***                      Spell                        ***/
/*********************************************************/

enum SpellState
{
	SPELL_STATE_PREPARING = 0,                              // 施法准备时期
	SPELL_STATE_CASTING   = 1,                              // 引导施法时期(读条)
	SPELL_STATE_FINISHED  = 2,                              // 施法成功或失败
	SPELL_STATE_DELAYED   = 3                               // 引导施法结束(读条结束)，法术命中目标前的飞行时期(例如魔兽的奥术飞弹)
};

/*********************************************************/
/***                      Unit                         ***/
/*********************************************************/

enum UnitFlags
{
    UNIT_FLAG_DISARMED              = 0x00000001, // 解除武装(主手)
    UNIT_FLAG_DISARM_OFFHAND        = 0x00000002, // 解除武装(副手)
    ITEM_FLAG_UNIQUE_EQUIPPED       = 0x00000004, //
    UNIT_FLAG_PASSIVE               = 0x00000008, // 不会主动攻击敌人
    UNIT_FLAG_NON_ATTACKABLE        = 0x00000010, // 不能被攻击
	UNIT_FLAG_IN_COMBAT             = 0x00080000, // 进入战斗状态
};

// 内部状态表示，被光环或路径生成器使用
enum UnitState
{
	// 持续状态
	UNIT_STAT_MELEE_ATTACKING = 0x00000001,
	UNIT_STAT_ATTACK_PLAYER   = 0x00000002, // 攻击玩家或玩家控制的单位
    UNIT_STAT_DIED            = 0x00000004,
	UNIT_STAT_STUNNED         = 0x00000008,

	UNIT_STAT_CAN_NOT_REACT   = UNIT_STAT_STUNNED | UNIT_STAT_DIED,

    UNIT_STAT_ALL_STATE       = 0xFFFFFFFF
};

// 一段加速
enum UnitMoveType : uint8
{
    MOVE_NORMAL         = 0,
    MOVE_RUN            = 1,
    MOVE_SLOW           = 2,
};

#define MAX_MOVE_TYPE     3

/*********************************************************/
/***                      LOGIN                        ***/
/*********************************************************/

enum ResponseCodes
{
	CHAR_NAME_SUCCESS                                      = 0x00,
	CHAR_NAME_FAILURE                                      = 0x01,
	CHAR_NAME_TOO_SHORT                                    = 0x02,
	CHAR_NAME_TOO_LONG                                     = 0x03,
	CHAR_NAME_INVALID_CHARACTER                            = 0x04,
	CHAR_NAME_MIXED_LANGUAGES                              = 0x05
};

/*********************************************************/
/***                      WORLD                        ***/
/*********************************************************/

enum WeatherState
{
	WEATHER_STATE_FINE              = 0,
	WEATHER_STATE_LIGHT_SNOW        = 1, // 小雪
	WEATHER_STATE_MEDIUM_SNOW       = 2, // 中雪
	WEATHER_STATE_HEAVY_SNOW        = 3  // 大雪
};

enum WeatherType : uint8
{
	WEATHER_TYPE_FINE       = 0, // 正常
	WEATHER_TYPE_SNOW       = 1  // 下雪
};

#define MAX_WEATHER_TYPE 2
#define WEATHER_SEASONS 4

#define MAX_MANA   32767
#define MAX_HEALTH 32767
#define MAX_LAWFUL 32767

#endif //MANGOS_SHAREDDEFINES_H
