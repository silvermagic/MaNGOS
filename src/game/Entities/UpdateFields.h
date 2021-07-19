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

#ifndef _UPDATEFIELDS_AUTO_H
#define _UPDATEFIELDS_AUTO_H

// Auto generated for version 4, 3, 4, 15595

enum EObjectFields
{
    OBJECT_FIELD_GUID                                = 0x0,
    OBJECT_FIELD_DATA                                = 0x2,
    OBJECT_FIELD_TYPE                                = 0x4,
    OBJECT_FIELD_ENTRY                               = 0x5,
    OBJECT_FIELD_PADDING                             = 0x7,
    OBJECT_END                                       = 0x8
};

enum EUnitFields
{
	UNIT_FIELD_BYTES                                 = OBJECT_END + 0x0,  // Size:1 种族、职业、性别、空
	UNIT_FIELD_GFX_ID                                = OBJECT_END + 0x1,  // Size:1 角色变形使用的GFX图片编号
	UNIT_FIELD_LEVEL                                 = OBJECT_END + 0x2,  // Size:1 等级
	UNIT_FIELD_ARMOR                                 = OBJECT_END + 0x3,  // Size:1 物理抗性(防御)
	UNIT_FIELD_DEFENSE_GENERAL                       = OBJECT_END + 0x4,  // Size:1 魔法抗性
	UNIT_FIELD_DEFENSE_WIND                          = OBJECT_END + 0x5,  // Size:1 风魔法抗性
	UNIT_FIELD_DEFENSE_WATER                         = OBJECT_END + 0x6,  // Size:1 水魔法抗性
	UNIT_FIELD_DEFENSE_FIRE                          = OBJECT_END + 0x7,  // Size:1 火魔法抗性
	UNIT_FIELD_DEFENSE_EARTH                         = OBJECT_END + 0x8,  // Size:1 地魔法抗性
	UNIT_FIELD_STRENGTH                              = OBJECT_END + 0x9,  // Size:1 力量
	UNIT_FIELD_AGILITY                               = OBJECT_END + 0xA,  // Size:1 敏捷
	UNIT_FIELD_STAMINA                               = OBJECT_END + 0xB,  // Size:1 体质
	UNIT_FIELD_SPIRIT                                = OBJECT_END + 0xC,  // Size:1 精神
	UNIT_FIELD_CHARM                                 = OBJECT_END + 0xD,  // Size:1 魅力
	UNIT_FIELD_INTELLECT                             = OBJECT_END + 0xE,  // Size:1 智力
	UNIT_FIELD_BASE_STRENGTH                         = OBJECT_END + 0xF,  // Size:1 初始力量
	UNIT_FIELD_BASE_AGILITY                          = OBJECT_END + 0x10, // Size:1 初始敏捷
	UNIT_FIELD_BASE_STAMINA                          = OBJECT_END + 0x11, // Size:1 初始体质
	UNIT_FIELD_BASE_SPIRIT                           = OBJECT_END + 0x12, // Size:1 初始精神
	UNIT_FIELD_BASE_CHARM                            = OBJECT_END + 0x13, // Size:1 初始魅力
	UNIT_FIELD_BASE_INTELLECT                        = OBJECT_END + 0x14, // Size:1 初始智力
	UNIT_FIELD_FLAGS                                 = OBJECT_END + 0x15, // Size:1 标志
	UNIT_FIELD_HEALTH                                = OBJECT_END + 0x16, // Size:1 当前体力值
	UNIT_FIELD_MANA                                  = OBJECT_END + 0x17, // Size:1 当前魔力值
	UNIT_FIELD_MAXHEALTH                             = OBJECT_END + 0x18, // Size:1 体力上限
	UNIT_FIELD_MAXMANA                               = OBJECT_END + 0x19, // Size:1 魔力上限
	UNIT_FIELD_SUMMON                                = OBJECT_END + 0x1A, // Size:1 召唤的宠物
	UNIT_FIELD_SUMMONEDBY                            = OBJECT_END + 0x1C, // Size:1 召唤者
	UNIT_FIELD_CREATEDBY                             = OBJECT_END + 0x1D, // Size:1 创造者
	UNIT_FIELD_TARGET                                = OBJECT_END + 0x1E, // Size:1 目标
	UNIT_FIELD_LAWFUL                                = OBJECT_END + 0x1F, // Size:1 正义值
	UNIT_FIELD_STATUS                                = OBJECT_END + 0x20, // Size:1 饱食度、空、空、空
    UNIT_END                                         = OBJECT_END + 0x21
};

enum EItemFields
{
	ITEM_FIELD_OWNER                                 = OBJECT_END + 0x0,  // Size:2 道具拥有者
	ITEM_FIELD_STACK_COUNT                           = OBJECT_END + 0x2,  // Size:1 道具当前堆叠数目
	ITEM_FIELD_FLAGS                                 = OBJECT_END + 0x3,  // Size:1 道具标志
	ITEM_FIELD_ENCHANT_LEVEL                         = OBJECT_END + 0x4,  // Size:1 普通强化等级
	ITEM_FIELD_DURABILITY                            = OBJECT_END + 0x5,  // Size:1 道具损坏程度
	ITEM_FIELD_CHARGE_COUNT                          = OBJECT_END + 0x6,  // Size:1 道具剩余使用次数
	ITEM_FIELD_DURATION                              = OBJECT_END + 0x7,  // Size:1 道具剩余使用时间
	ITEM_FIELD_LAST_USED                             = OBJECT_END + 0x8,  // Size:1 道具最近使用时间
	ITEM_FIELD_ATTR_ENCHANT_KIND                     = OBJECT_END + 0x9,  // Size:1 属性强化类型
	ITEM_FIELD_ATTR_ENCHANT_LEVEL                    = OBJECT_END + 0xA,  // Size:1 属性强化等级
	ITTEM_FIELD_DEFENSE_WIND                         = OBJECT_END + 0xB,  // Size:1 风属性抗性
	ITTEM_FIELD_DEFENSE_WATER                        = OBJECT_END + 0xC,  // Size:1 水属性抗性
	ITTEM_FIELD_DEFENSE_FIRE                         = OBJECT_END + 0xD,  // Size:1 火属性抗性
	ITTEM_FIELD_DEFENSE_EARTH                        = OBJECT_END + 0xE,  // Size:1 地属性抗性
	ITEM_FIELD_SPELL_POWER                           = OBJECT_END + 0xF,  // Size:1 法术强度
	ITEM_FIELD_HEALTH                                = OBJECT_END + 0x10, // Size:1 体力值
	ITEM_FIELD_MANA                                  = OBJECT_END + 0x11, // Size:1 魔力值
	ITEM_FIELD_HEALTH_REGEN                          = OBJECT_END + 0x12, // Size:1 体力回复
	ITEM_FIELD_MANA_REGEN                            = OBJECT_END + 0x13, // Size:1 魔力回复
	ITEM_FIELD_BYTES                                 = OBJECT_END + 0x14, // Size:1 (普通/祝福/诅咒)、空、空、空
	ITEM_FIELD_STRENGTH                              = OBJECT_END + 0x15, // Size:1 力量
	ITEM_FIELD_AGILITY                               = OBJECT_END + 0x16, // Size:1 敏捷
	ITEM_FIELD_STAMINA                               = OBJECT_END + 0x17, // Size:1 体质
	ITEM_FIELD_SPIRIT                                = OBJECT_END + 0x18, // Size:1 精神
	ITEM_FIELD_CHARM                                 = OBJECT_END + 0x19, // Size:1 魅力
	ITEM_FIELD_INTELLECT                             = OBJECT_END + 0x1A, // Size:1 智力
    ITEM_END                                         = OBJECT_END + 0x1B
};

enum EPlayerFields
{
	PLAYER_FIELD_GFX_ID                              = UNIT_END + 0x0,  // Size:1 玩家角色默认GFX图片编号
	PLAYER_FIELD_HIGH_LEVEL                          = UNIT_END + 0x1,  // Size:1 历史最高等级
	PLAYER_FIELD_XP                                  = UNIT_END + 0x2,  // Size:1 经验值
	PLAYER_FIELD_LAST_PK                             = UNIT_END + 0x3,  // Size:1 最后一次PK时间(大陆)
	PLAYER_FIELD_ELF_LAST_PK                         = UNIT_END + 0x4,  // Size:1 最后一次PK时间(妖精森林)
	PLAYER_FIELD_PK_COUNT                            = UNIT_END + 0x5,  // Size:1 PK次数(大陆)、PK次数(妖精森林)
	PLAYER_FIELD_DELETE                              = UNIT_END + 0x6,  // Size:1 角色删除时间
	PLAYER_FIELD_BIRTHDAY                            = UNIT_END + 0x7,  // Size:1 角色创建日期
	PLAYER_FIELD_STATUS                              = UNIT_END + 0x8,  // Size:2 50级以后升级奖励的属性点数、万能药水奖励、在线状态、禁用状态
    PLAYER_END                                       = UNIT_END + 0xA
};

enum EContainerFields
{
	CONTAINER_FIELD_NUM_SLOTS                        = ITEM_END + 0x0,
	CONTAINER_FIELD_SLOTS                            = ITEM_END + 0x1, // 180个槽位
    CONTAINER_END                                    = ITEM_END + 0x169
};

enum EGameObjectFields
{
    GAMEOBJECT_END                                   = OBJECT_END + 0x2
};

enum EDynamicObjectFields
{
    DYNAMICOBJECT_END                                = OBJECT_END + 0x0
};

enum ECorpseFields
{
    CORPSE_END                                       = OBJECT_END + 0x0
};

enum EAreaTriggerFields
{
    AREATRIGGER_END                                  = OBJECT_END + 0x0
};

#endif
