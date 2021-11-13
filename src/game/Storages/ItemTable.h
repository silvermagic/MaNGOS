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
	uint32 SpellId;        // �����������
	uint32 SpellTrigger;   // ����ģʽ ItemSpelltriggerType
	int32  SpellCooldown;  // ��ȴʱ��
};

#define MAX_ITEM_PROTO_SPELLS  1

struct ItemEntry
{
	uint32 ItemId;                // ����ģ����
	ItemClass Class;              // �������� ItemClass
	uint32 SubClass;              // ���������� ItemSubclassWeapon ItemSubclassArmor ItemSubclassGeneric

	std::string name;             // ��������
	uint32 description;           // ����������Ϣ���
	uint32 weight;                // ��������
	uint32 flags;                 // ��־ ItemEntryFlags
	uint32 allowable;             // ְҵʹ������
	uint8 use_type;               // ʹ������(�����ڷ��͸��ͻ��ˣ���������ʹ��Class + SubClass�Ѿ�֪����ô���������)
	uint8 bless;                  // ����ף������ 0-��ͨ 1-ף�� 2-����
	ItemBondingType bonding;      // ���߰�����
	ItemMaterialType material;    // ���߲���
	InventoryType inventory_type; // ���λ��

	std::string identified;       // ����ʱ�������Ʊ��
	std::string unidentified;     // δ����ʱ�������Ʊ��

	uint32 gfx_in_backpack;       // ���߷����ڱ���/�ֿ����õ�GFXͼƬ���
	uint32 gfx_on_ground;         // ���߷����ڵ������õ�GFXͼƬ���

	uint32 min_use_level;         // �������ʹ�õȼ�
	uint32 max_use_level;         // �������ʹ�õȼ�

	uint32 spell_id;              // ��������Ч��
	uint32 spell_trigger;         // ��������Ч���������� ItemSpelltriggerType
	uint32 spell_cooldown;        // ����������ȴʱ��

	union {
		// ��װ���ĵ��ߵ�����
		struct {
			uint32 safenchant;                   // ����ֵ
			uint32 max_use_time;                 // �������ʹ��ʱ��
			int32 melee_hit_chance;              // ����/���ͽ�ս����
			int32 melee_damage_done;             // ����/���ͽ�ս�˺�
			int32 ranged_hit_chance;             // ����/����Զ������
			int32 ranged_damage_done;            // ����/����Զ���˺�
			int32 strength;                      // ����/������������
			int32 agility;                       // ����/������������
			int32 stamina;                       // ����/������������
			int32 spirit;                        // ����/������������
			int32 charm;                         // ����/������������
			int32 intellect;                     // ����/������������
			int32 health;                        // ����/������������
			int32 mana;                          // ����/����ħ������
			int32 health_regen;                  // ����/���������ظ�
			int32 mana_regen;                    // ����/����ħ���ظ�
			int32 spell_power;                   // ����/���ͷ���ǿ��
			int32 defense_magic;                 // ����/���ͷ�������

			union {
				// ��������
				struct {
					int32 armor;                 // ����
					int32 item_set;              // ��װ
					int32 damage_reduction;      // �˺�����
					int32 weight_reduction;      // ���ؼ���
					int32 defense_wind;          // ˮ���Է���
					int32 defense_water;         // ˮ���Է���
					int32 defense_fire;          // ˮ���Է���
					int32 defense_earth;         // ˮ���Է���
					int32 resist_stun;           // ѣ�ο���
					int32 resist_stone;          // ʯ������
					int32 resist_sleep;          // ˯�߿���
					int32 resist_freeze;         // ���Ό��
					int32 resist_sustain;        // ֧�ſ���
					int32 resist_blind;          // ��ä����
				};

				// ��������
				struct {
					struct {
						int32 damage_tiny;       // С���˺�
						int32 damage_large;      // ����˺�
					};                           // �����˺� WeaponDamageType
					uint32 double_damage_chance; // ����������(˫��)/�ػ���(צ)
					uint32 attack_range;         // ����������Χ
					int32 spell_damage_done;     // ����/���ͷ����˺�
				};
			};
		};

		// ����װ���ĵ��ߵ�����
		struct {
			struct {
				int32 ammo_damage_tiny;  // С���˺�
				int32 ammo_damage_large; // ����˺�
			};
			int32 food;                  // ��ʳ��
			CoordUnit x, y;              // ����ˢ�µص�
			uint32 map;                  // ����ˢ�µ�ͼ
			uint32 max_charge_count;     // �������ʹ�ô���
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
