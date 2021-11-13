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
	 * ����
	 */
	uint32    Id;                                      // �������
	uint32    School;                                  // �������� enum SpellSchools
	uint32    Category;                                // ������� �����
	uint32    Dispel;                                  // ������ɢ���� enum DispelType
	uint32    Mechanic;                                // �������� enum MechanicsType
	uint32    Duration;                                // ����ʱ��
	int32     Range;                                   // ʩ������
	uint32    RecoveryTime;                            // ��ȴʱ��
	int32     PowerCost[MAX_POWERS];                   // ����������
	uint32    DmgClass;                                // �˺����� enum SpellDmgClass
	/**
	 * Ч��
	 */
	uint32    Targets;                                 // ����Ŀ�� enum SpellCastTargetFlags
	uint32    TargetCreatureType;                      // ����Ŀ����������
	uint32    ProcFlags;                               // ������������ enum ProcFlags
	int32     ProcChance;                              // ������������
	int32     ProcCharges;                             // ������������
	uint32    Effect[MAX_EFFECT_INDEX];                // ����Ч�� enum SpellEffects
	uint32    EffectItemType[MAX_EFFECT_INDEX];        // �������߱��
	int32     EffectBasePoints[MAX_EFFECT_INDEX];      // ��������
	int32     EffectBaseDice[MAX_EFFECT_INDEX];        // ��������
	uint32    EffectImplicitTarget[MAX_EFFECT_INDEX];  // ����Ч��Ŀ�� enum Targets
	int32     EffectRadius[MAX_EFFECT_INDEX];          // ����Ч����Χ
	uint32    EffectApplyAuraName[MAX_EFFECT_INDEX];   // ����Ч�������Ĺ⻷��� enum AuraType
	int32     EffectMiscValue[MAX_EFFECT_INDEX];       // �⻷����
	uint32    EffectTriggerSpell[MAX_EFFECT_INDEX];    // ����Ч���������·������
	/**
	 * ��Ʒ
	 */
	uint32    Reagent[MAX_SPELL_REAGENTS];              // ������Ʒ��item_id
	int32     ReagentCount[MAX_SPELL_REAGENTS];         // ������Ʒ����
	int32     EquippedItemClass;                        // ��Ҫװ������Ʒ���� enum ItemClass
	int32     EquippedItemSubClassMask;                 // ��Ҫװ������Ʒ������ enum ItemSubclassWeapon | enum ItemSubclassArmor
	int32     EquippedItemInventoryTypeMask;            // ��Ҫװ���Ĳ�λ
	/**
	 * ��ʶ
	 */
	uint32    AuraInterruptFlags;                       // �⻷�жϱ�־
	uint32    Attributes;                               // ��������
	/**
	 * ��Ϣ
	 */
	uint32    ActionID;                                 //
	uint32    CastGFXID[2];                             // ʩ������
	uint32    MessageID[3];                             // 0 - ʩ���ɹ� 1 - ʩ��ȡ�� 2 - ʩ��ʧ��
	/**
	 * �����ȼ�
	 */
	uint32    SpellFamilyName;                         // ְҵ�������� enum SpellFamily
	uint32    Rank;                                    // �����ȼ� 1 - 10 ��ʦְҵ���� 11 - 12 ��ʿְҵ���� 13 - 14 �ڰ�����ְҵ���� 15 ����ְҵ���� 17 - 22 ����ְҵ���� 23 - 25 ����ʿְҵ���� 26 - 28 ����ʦְҵ����
	uint32    Number;                                  // �����ȼ��ڱ�� 0 - 7 (ÿ�������ȼ�����8������)

	inline bool HasAttribute(SpellAttributes attribute) const { return Attributes & attribute; }
	inline SpellSchools GetSpellSchool() const { return SpellSchools(School); }
	inline SpellDmgClass GetDmgClass() const { return SpellDmgClass(DmgClass); }
	inline uint32 GetStackAmount() const { return 0;}   // ����Ŀǰû�з����ɶѵ�
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

SpellFamily GetSpellFamily(uint8 rank);

#endif //MANGOS_SPELLS_H_
