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
	UNIT_FIELD_BYTES                                 = OBJECT_END + 0x0,  // Size:1 ���塢ְҵ���Ա𡢿�
	UNIT_FIELD_GFX_ID                                = OBJECT_END + 0x1,  // Size:1 ��ɫ����ʹ�õ�GFXͼƬ���
	UNIT_FIELD_LEVEL                                 = OBJECT_END + 0x2,  // Size:1 �ȼ�
	UNIT_FIELD_ARMOR                                 = OBJECT_END + 0x3,  // Size:1 ������(����)
	UNIT_FIELD_DEFENSE_GENERAL                       = OBJECT_END + 0x4,  // Size:1 ħ������
	UNIT_FIELD_DEFENSE_WIND                          = OBJECT_END + 0x5,  // Size:1 ��ħ������
	UNIT_FIELD_DEFENSE_WATER                         = OBJECT_END + 0x6,  // Size:1 ˮħ������
	UNIT_FIELD_DEFENSE_FIRE                          = OBJECT_END + 0x7,  // Size:1 ��ħ������
	UNIT_FIELD_DEFENSE_EARTH                         = OBJECT_END + 0x8,  // Size:1 ��ħ������
	UNIT_FIELD_STRENGTH                              = OBJECT_END + 0x9,  // Size:1 ����
	UNIT_FIELD_AGILITY                               = OBJECT_END + 0xA,  // Size:1 ����
	UNIT_FIELD_STAMINA                               = OBJECT_END + 0xB,  // Size:1 ����
	UNIT_FIELD_SPIRIT                                = OBJECT_END + 0xC,  // Size:1 ����
	UNIT_FIELD_CHARM                                 = OBJECT_END + 0xD,  // Size:1 ����
	UNIT_FIELD_INTELLECT                             = OBJECT_END + 0xE,  // Size:1 ����
	UNIT_FIELD_BASE_STRENGTH                         = OBJECT_END + 0xF,  // Size:1 ��ʼ����
	UNIT_FIELD_BASE_AGILITY                          = OBJECT_END + 0x10, // Size:1 ��ʼ����
	UNIT_FIELD_BASE_STAMINA                          = OBJECT_END + 0x11, // Size:1 ��ʼ����
	UNIT_FIELD_BASE_SPIRIT                           = OBJECT_END + 0x12, // Size:1 ��ʼ����
	UNIT_FIELD_BASE_CHARM                            = OBJECT_END + 0x13, // Size:1 ��ʼ����
	UNIT_FIELD_BASE_INTELLECT                        = OBJECT_END + 0x14, // Size:1 ��ʼ����
	UNIT_FIELD_FLAGS                                 = OBJECT_END + 0x15, // Size:1 ��־
	UNIT_FIELD_HEALTH                                = OBJECT_END + 0x16, // Size:1 ��ǰ����ֵ
	UNIT_FIELD_MANA                                  = OBJECT_END + 0x17, // Size:1 ��ǰħ��ֵ
	UNIT_FIELD_MAXHEALTH                             = OBJECT_END + 0x18, // Size:1 ��������
	UNIT_FIELD_MAXMANA                               = OBJECT_END + 0x19, // Size:1 ħ������
	UNIT_FIELD_SUMMON                                = OBJECT_END + 0x1A, // Size:1 �ٻ��ĳ���
	UNIT_FIELD_SUMMONEDBY                            = OBJECT_END + 0x1C, // Size:1 �ٻ���
	UNIT_FIELD_CREATEDBY                             = OBJECT_END + 0x1D, // Size:1 ������
	UNIT_FIELD_TARGET                                = OBJECT_END + 0x1E, // Size:1 Ŀ��
	UNIT_FIELD_LAWFUL                                = OBJECT_END + 0x1F, // Size:1 ����ֵ
	UNIT_FIELD_STATUS                                = OBJECT_END + 0x20, // Size:1 ��ʳ�ȡ��ա��ա���
    UNIT_END                                         = OBJECT_END + 0x21
};

enum EItemFields
{
	ITEM_FIELD_OWNER                                 = OBJECT_END + 0x0,  // Size:2 ����ӵ����
	ITEM_FIELD_STACK_COUNT                           = OBJECT_END + 0x2,  // Size:1 ���ߵ�ǰ�ѵ���Ŀ
	ITEM_FIELD_FLAGS                                 = OBJECT_END + 0x3,  // Size:1 ���߱�־
	ITEM_FIELD_ENCHANT_LEVEL                         = OBJECT_END + 0x4,  // Size:1 ��ͨǿ���ȼ�
	ITEM_FIELD_DURABILITY                            = OBJECT_END + 0x5,  // Size:1 �����𻵳̶�
	ITEM_FIELD_CHARGE_COUNT                          = OBJECT_END + 0x6,  // Size:1 ����ʣ��ʹ�ô���
	ITEM_FIELD_DURATION                              = OBJECT_END + 0x7,  // Size:1 ����ʣ��ʹ��ʱ��
	ITEM_FIELD_LAST_USED                             = OBJECT_END + 0x8,  // Size:1 �������ʹ��ʱ��
	ITEM_FIELD_ATTR_ENCHANT_KIND                     = OBJECT_END + 0x9,  // Size:1 ����ǿ������
	ITEM_FIELD_ATTR_ENCHANT_LEVEL                    = OBJECT_END + 0xA,  // Size:1 ����ǿ���ȼ�
	ITTEM_FIELD_DEFENSE_WIND                         = OBJECT_END + 0xB,  // Size:1 �����Կ���
	ITTEM_FIELD_DEFENSE_WATER                        = OBJECT_END + 0xC,  // Size:1 ˮ���Կ���
	ITTEM_FIELD_DEFENSE_FIRE                         = OBJECT_END + 0xD,  // Size:1 �����Կ���
	ITTEM_FIELD_DEFENSE_EARTH                        = OBJECT_END + 0xE,  // Size:1 �����Կ���
	ITEM_FIELD_SPELL_POWER                           = OBJECT_END + 0xF,  // Size:1 ����ǿ��
	ITEM_FIELD_HEALTH                                = OBJECT_END + 0x10, // Size:1 ����ֵ
	ITEM_FIELD_MANA                                  = OBJECT_END + 0x11, // Size:1 ħ��ֵ
	ITEM_FIELD_HEALTH_REGEN                          = OBJECT_END + 0x12, // Size:1 �����ظ�
	ITEM_FIELD_MANA_REGEN                            = OBJECT_END + 0x13, // Size:1 ħ���ظ�
	ITEM_FIELD_BYTES                                 = OBJECT_END + 0x14, // Size:1 (��ͨ/ף��/����)���ա��ա���
	ITEM_FIELD_STRENGTH                              = OBJECT_END + 0x15, // Size:1 ����
	ITEM_FIELD_AGILITY                               = OBJECT_END + 0x16, // Size:1 ����
	ITEM_FIELD_STAMINA                               = OBJECT_END + 0x17, // Size:1 ����
	ITEM_FIELD_SPIRIT                                = OBJECT_END + 0x18, // Size:1 ����
	ITEM_FIELD_CHARM                                 = OBJECT_END + 0x19, // Size:1 ����
	ITEM_FIELD_INTELLECT                             = OBJECT_END + 0x1A, // Size:1 ����
    ITEM_END                                         = OBJECT_END + 0x1B
};

enum EPlayerFields
{
	PLAYER_FIELD_GFX_ID                              = UNIT_END + 0x0,  // Size:1 ��ҽ�ɫĬ��GFXͼƬ���
	PLAYER_FIELD_HIGH_LEVEL                          = UNIT_END + 0x1,  // Size:1 ��ʷ��ߵȼ�
	PLAYER_FIELD_XP                                  = UNIT_END + 0x2,  // Size:1 ����ֵ
	PLAYER_FIELD_LAST_PK                             = UNIT_END + 0x3,  // Size:1 ���һ��PKʱ��(��½)
	PLAYER_FIELD_ELF_LAST_PK                         = UNIT_END + 0x4,  // Size:1 ���һ��PKʱ��(����ɭ��)
	PLAYER_FIELD_PK_COUNT                            = UNIT_END + 0x5,  // Size:1 PK����(��½)��PK����(����ɭ��)
	PLAYER_FIELD_DELETE                              = UNIT_END + 0x6,  // Size:1 ��ɫɾ��ʱ��
	PLAYER_FIELD_BIRTHDAY                            = UNIT_END + 0x7,  // Size:1 ��ɫ��������
	PLAYER_FIELD_STATUS                              = UNIT_END + 0x8,  // Size:2 50���Ժ��������������Ե���������ҩˮ����������״̬������״̬
    PLAYER_END                                       = UNIT_END + 0xA
};

enum EContainerFields
{
	CONTAINER_FIELD_NUM_SLOTS                        = ITEM_END + 0x0,
	CONTAINER_FIELD_SLOTS                            = ITEM_END + 0x1, // 180����λ
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
