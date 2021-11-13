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

#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Entities/Player.h"
#include "Packets/S_SPMR.hpp"
#include "ObjectMgr.h"
#include "SQLStructure.h"
#include "Item.h"
#include "PcInventory.h"

PcInventory::PcInventory(Player* owner) : m_owner(owner)
{
	for (uint8 i = 0; i < EQUIPMENT_SLOT_COUNT; ++i)
	{
		m_slots[i] = nullptr;
	}
}

void PcInventory::Update(uint32 diff)
{
}

uint8 PcInventory::GetWeight240() const
{
	return ComputeWeight240(GetWeight());
}

WeaponShapeType PcInventory::GetWeaponShape() const
{
	Item* pItem = m_slots[EQUIPMENT_SLOT_MAINHAND];
	if (pItem)
	{
		switch (pItem->GetProto()->SubClass)
		{
			case ITEM_SUBCLASS_WEAPON_SWORD:
				return WEAPON_SHAPE_SWORD;
			case ITEM_SUBCLASS_WEAPON_DAGGER:
				return WEAPON_SHAPE_DAGGER;
			case ITEM_SUBCLASS_WEAPON_SWORD2:
				return WEAPON_SHAPE_SWORD2;
			case ITEM_SUBCLASS_WEAPON_BOW:
				return WEAPON_SHAPE_BOW;
			case ITEM_SUBCLASS_WEAPON_SPEAR2:
				return WEAPON_SHAPE_SPEAR2;
			case ITEM_SUBCLASS_WEAPON_AXE:
				return WEAPON_SHAPE_AXE;
			case ITEM_SUBCLASS_WEAPON_STAFF:
				return WEAPON_SHAPE_STAFF;
			// case ITEM_SUBCLASS_WEAPON_STING:
			//	 return WEAPON_SHAPE_STING;
			case ITEM_SUBCLASS_WEAPON_ARROW:
				return WEAPON_SHAPE_ARROW;
			case ITEM_SUBCLASS_WEAPON_GAUNTLET:
				return WEAPON_SHAPE_GAUNTLET;
			case ITEM_SUBCLASS_WEAPON_CLAW:
				return WEAPON_SHAPE_CLAW;
			case ITEM_SUBCLASS_WEAPON_EDORYU:
				return WEAPON_SHAPE_EDORYU;
			case ITEM_SUBCLASS_WEAPON_CROSSBOW:
				return WEAPON_SHAPE_CROSSBOW;
			case ITEM_SUBCLASS_WEAPON_SPEAR:
				return WEAPON_SHAPE_SPEAR;
			case ITEM_SUBCLASS_WEAPON_AXE2:
				return WEAPON_SHAPE_AXE2;
			case ITEM_SUBCLASS_WEAPON_STAFF2:
				return WEAPON_SHAPE_STAFF2;
			case ITEM_SUBCLASS_WEAPON_KIRINGKU:
				return WEAPON_SHAPE_KIRINGKU;
			case ITEM_SUBCLASS_WEAPON_CHAINSWORD:
				return WEAPON_SHAPE_CHAINSWORD;
		}
	}

	return WEAPON_SHAPE_NONE;
}

void PcInventory::UpdateItemDuration(uint32 diff)
{
}

void PcInventory::AddItemDurations(Item* pItem)
{
}

void PcInventory::RemoveItemDurations(Item* pItem)
{
}

void PcInventory::UpdateAuraEffectDuration(uint32 diff)
{
}

void PcInventory::ApplyAuraEffect(Item* item, bool apply)
{
}

void PcInventory::AddAuraEffectDurations(Item* pItem)
{
}

void PcInventory::RemoveAuraEffectDurations(Item* pItem)
{
}

InventoryResult PcInventory::CanUseItem(Item* pItem, bool direct_action) const
{
	if (pItem)
	{
		DEBUG_LOG("STORAGE: CanUseItem item = %u", pItem->GetEntry());

		// direct_actionΪtrueһ���ʾ��ǰ���������ݿ�����ڼ䣬��������ҵ�״̬���
		if (direct_action)
		{
			if (!m_owner->IsAlive())
			{
				return EQUIP_ERR_YOU_ARE_DEAD;
			}

			// todo: �Ƿ���ڱ��ι⻷Ч����������Ч����װ���Ƿ���ʹ������
		}

		const ItemEntry* proto = pItem->GetProto();
		if (proto)
		{
			// �����ߵ�ʹ������
			InventoryResult msg = CanUseItem(proto);
			if (msg != EQUIP_ERR_OK)
			{
				return msg;
			}

			return EQUIP_ERR_OK;
		}
	}

	return EQUIP_ERR_ITEM_NOT_FOUND;
}

InventoryResult PcInventory::CanUseItem(const ItemEntry* proto) const
{
	if (proto)
	{
		if ((proto->allowable & m_owner->GetClassMask()) == 0)
		{
			return EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM;
		}

		if (proto->min_use_level != 0 && m_owner->GetLevel() < proto->min_use_level)
		{
			return EQUIP_ERR_CANT_EQUIP_LEVEL;
		}

		if (proto->max_use_level != 0 && proto->max_use_level < m_owner->GetLevel())
		{
			return EQUIP_ERR_CANT_EQUIP_LEVEL;
		}

		return EQUIP_ERR_OK;
	}

	return EQUIP_ERR_ITEM_NOT_FOUND;
}

InventoryResult PcInventory::CanEquipItem(Item* pItem, uint8& dest, bool direct_action) const
{
	if (pItem)
	{
		DEBUG_LOG("PcInventory: CanEquipItem item = %u, count = %u", pItem->GetEntry(), pItem->GetCount());

		const ItemEntry* proto = pItem->GetProto();
		if (proto)
		{
			// ���ʹ�õ��ߵ����״̬
			if (direct_action)
			{
				// ����ҵǳ�ʱ��ֹװ������
				if (m_owner->GetSession()->PlayerLogout())
				{
					return EQUIP_ERR_YOU_ARE_LOGOUT;
				}
			}

			// ��ȡ����װ��λ�ã�����ʹ�ÿ���λ��
			uint8 slots[4];
			if (!GetSlotsForInventoryType(proto->inventory_type, slots, proto->SubClass))
			{
				return EQUIP_ERR_CANT_BE_EQUIPPED;
			}
			uint8 eslot = slots[0];
			for (uint8 i = 0; i < 4; ++i)
			{
				if (slots[i] != NULL_SLOT && !m_slots[slots[i]])
				{
					eslot = slots[i];
					break;
				}
			}

			// ���װ�������Ƿ��Ѿ��е��ߣ�����еĻ��Ƿ���ȡ��
			InventoryResult msg = CanUnequipItem(m_slots[eslot]);
			if (msg != EQUIP_ERR_OK)
			{
				return msg;
			}

			// ������ʹ������
			msg = CanUseItem(pItem, direct_action);
			if (msg != EQUIP_ERR_OK)
			{
				return msg;
			}

			// ����װ����ͻ���
			uint32 type = proto->inventory_type;
			if (eslot == EQUIPMENT_SLOT_OFFHAND)
			{
				if (type == INVTYPE_WEAPON)
				{
					// ���˿�սʿ�⣬����ְҵ����˫������
					return EQUIP_ERR_CANT_DUAL_WIELD;
				}
				else if (type == INVTYPE_SHIELD)
				{
					// ���ƺͱۼײ���ͬʱ���
					if (m_slots[EQUIPMENT_SLOT_SHOULDER])
					{
						return EQUIP_ERR_CANT_DUAL_BLOCK;
					}
				}

				if (m_slots[EQUIPMENT_SLOT_MAINHAND] && m_slots[EQUIPMENT_SLOT_MAINHAND]->GetProto()->IsTwoHandWeapon())
				{
					return EQUIP_ERR_CANT_EQUIP_WITH_TWOHANDED;
				}
			}
			else if (eslot == EQUIPMENT_SLOT_SHOULDER)
			{
				// ���ƺͱۼײ���ͬʱ���
				if (m_slots[EQUIPMENT_SLOT_OFFHAND] && m_slots[EQUIPMENT_SLOT_OFFHAND]->GetProto()->inventory_type == INVTYPE_SHIELD)
				{
					return EQUIP_ERR_CANT_DUAL_BLOCK;
				}
			}

			dest = eslot;
			return EQUIP_ERR_OK;
		}
	}

	return EQUIP_ERR_ITEM_NOT_FOUND;
}

InventoryResult PcInventory::CanUnequipItem(Item* pItem, bool direct_action) const
{
	if (!pItem || !pItem->IsEquipped() || pItem->GetSlot() >= EQUIPMENT_SLOT_END)
	{
		return EQUIP_ERR_OK;
	}

	DEBUG_LOG("STORAGE: CanUnequipItem slot = %u, item = %u, count = %u", pItem->GetSlot(), pItem->GetEntry(), pItem->GetCount());

	// װ�������䣬�޷�����
	if (pItem->GetBless() == 2)
	{
		return EQUIP_ERR_CANT_TAKE_OFF;
	}

	return EQUIP_ERR_OK;
}

void PcInventory::QuickEquipItem(uint8 slot, Item* pItem)
{
	if (pItem)
	{
		// װ������
		pItem->SetEquipped(true);

		// ������ߴ���ʹ��ʱ�����ƣ���ʼ��ʱ
		AddItemDurations(pItem);

		// �����߷��õ�װ���ۣ�����������۸ı䣬���͸��°�
		m_slots[slot] = pItem;
		pItem->SetSlot(slot);
		if (IsInWorld())
		{
			// todo: ����
		}

		// Ӧ�õ��ߴ��������Խ���
		ApplyItemMods(pItem, true);

		// todo: ��װ����
	}
}

void PcInventory::EquipItem(uint8 slot, Item* pItem)
{
	if (pItem && !pItem->IsEquipped())
	{
		// todo: ��Ͼ������Ϲ⻷

		// ������ߴ���ʹ��ʱ�����ƣ���ʼ��ʱ
		AddItemDurations(pItem);

		// ȡ�����е���
		if (m_slots[slot])
		{
			UnequipItem(pItem);
		}

		// װ������
		pItem->SetEquipped(true);

		// �����߷��õ�װ���ۣ�����������۸ı䣬���͸��°�
		m_slots[slot] = pItem;
		pItem->SetSlot(slot);
		if (IsInWorld())
		{
			// todo: ����
		}

		// Ӧ�õ��ߴ��������Խ���
		ApplyItemMods(pItem, true);

		// todo: ��װ����
	}
}

void PcInventory::UnequipItem(Item* pItem)
{
	if (pItem && pItem->IsEquipped())
	{
		// todo: ��Ͼ������Ϲ⻷

		// ������ߴ���ʹ��ʱ�����ƣ�ֹͣ��ʱ
		RemoveItemDurations(pItem);

		// ���µ���
		pItem->SetEquipped(false);

		// �����߷��õ�װ���ۣ�����������۸ı䣬���͸��°�
		uint8 eslot = pItem->GetSlot();
		if (eslot < EQUIPMENT_SLOT_END)
		{
			m_slots[eslot] = nullptr;
		}
		if (IsInWorld())
		{
			// todo: ����
		}

		// ȡ�����ߴ��������Խ���
		ApplyItemMods(pItem, false);

		// todo: ��װ����
	}
}

bool PcInventory::IsTwoHandUsed() const
{
	return m_slots[EQUIPMENT_SLOT_MAINHAND] && m_slots[EQUIPMENT_SLOT_MAINHAND]->GetProto()->IsTwoHandWeapon();
}

InventoryResult PcInventory::CanStoreItem(uint32 item, int32 count) const
{
	// todo: ���������ж�
	return EQUIP_ERR_OK;
}

void PcInventory::LoadFromDB(QueryResult* result, uint32 timediff)
{
	//         0        1          2      3            4           5      6           7
	// SELECT id, item_id, item_name, count, is_equipped, enchantlvl, is_id, durability,
	//            8               9                         10     11                 12                  13      14       15       16      17
	// charge_count, remaining_time, UNIX_TIMESTAMP(last_used), bless, attr_enchant_kind, attr_enchant_level, firemr, watermr, earthmr, windmr,
	//    18     19     20   21   22      23      24      25      26      27      28             29        30                         31
	// addsp, addhp, addmp, hpr, mpr, addstr, adddex, addint, addcon, addwis, addcha, Records_count, addLucky, UNIX_TIMESTAMP(DeleteDate) FROM character_items

	if (result)
	{
		std::list<Item*> errors;

		do
		{
			Field* fields = result->Fetch();
			uint32 guidLow = fields[0].GetUInt32();
			uint32 item_id = fields[1].GetUInt32();
			const ItemEntry* proto = sObjectMgr.GetItemEntry(item_id);
			if (!proto)
			{
				CharacterDatabase.PExecute("DELETE FROM character_items WHERE id = '%u'", guidLow);
				sLog.outError("PcInventory::LoadFromDB: Player %s has an unknown item (id: #%u) in inventory, deleted.",
					fields[2].GetCppString().c_str(),
					item_id);
				continue;
			}

			Item* pItem = new Item;
			pItem->Create(guidLow, item_id);
			pItem->SetName(fields[2].GetCppString());
			pItem->SetCount(fields[3].GetInt32());
			pItem->SetEquipped(false);
			pItem->SetEnchantLevel(fields[5].GetInt32());
			pItem->SetIdentified(fields[6].GetBool());
			pItem->SetDurability(fields[7].GetInt32());
			pItem->SetChargeCount(fields[8].GetInt32());
			pItem->SetDuration(fields[9].GetUInt32());
			// todo: ���ʹ��ʱ��
			pItem->SetBless(fields[11].GetUInt8());
			pItem->SetAttrEnchantKind(ItemAttrEnchantType(fields[12].GetUInt8()));
			pItem->SetAttrEnchantLevel(fields[13].GetInt32());
			pItem->SetDefense(SPELL_SCHOOL_FIRE, fields[14].GetInt32());
			pItem->SetDefense(SPELL_SCHOOL_WATER, fields[15].GetInt32());
			pItem->SetDefense(SPELL_SCHOOL_EARTH, fields[16].GetInt32());
			pItem->SetDefense(SPELL_SCHOOL_WIND, fields[17].GetInt32());
			pItem->SetSpellPower(fields[18].GetInt32());
			pItem->SetMaxHealth(fields[19].GetInt32());
			pItem->SetMaxMana(fields[20].GetInt32());
			pItem->SetHealthRegen(fields[21].GetInt32());
			pItem->SetManaRegen(fields[22].GetInt32());
			pItem->SetStat(STAT_STRENGTH, fields[23].GetInt32());
			pItem->SetStat(STAT_AGILITY, fields[24].GetInt32());
			pItem->SetStat(STAT_INTELLECT, fields[25].GetInt32());
			pItem->SetStat(STAT_STAMINA, fields[26].GetInt32());
			pItem->SetStat(STAT_SPIRIT, fields[27].GetInt32());
			pItem->SetStat(STAT_CHARM, fields[28].GetInt32());

			// ��ӵ�����
			m_items[guidLow] = pItem;

			// ����װ���ж�
			if (fields[4].GetBool())
			{
				uint8 eslot;
				InventoryResult msg = CanEquipItem(pItem, eslot, false);
				if (msg != EQUIP_ERR_OK)
				{
					continue;
				}
				QuickEquipItem(eslot, pItem);
			}
		} while (result->NextRow());
		delete result;
	}
}

void PcInventory::SaveToDB(Item* pItem, bool insert)
{
	uint32 guid = pItem->GetGUIDLow();

	if (insert)
	{
		static SqlStatementID delItem;
		static SqlStatementID insItem;

		SqlStatement stmt = CharacterDatabase.CreateStatement(delItem, "DELETE FROM character_items where id = ?");
		stmt.PExecute(guid);

		stmt = CharacterDatabase.CreateStatement(insItem,
			"INSERT INTO character_items "
   			// 0        1        2          3      4            5           6      7           8
 			"(id, item_id, char_id, item_name, count, is_equipped, enchantlvl, is_id, durability, "
		    //          9              10         11     12                 13                  14      15       16       17      18
			"charge_count, remaining_time, last_used, bless, attr_enchant_kind, attr_enchant_level, firemr, watermr, earthmr, windmr, "
   			//  19     20     21   22   23      24      25      26      27      28      29
			"addsp, addhp, addmp, hpr, mpr, addstr, adddex, addint, addcon, addwis, addcha) VALUES ("
   			// 0  1  2  3  4  5  6  7  8  9 10     11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
			"  ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW(), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		stmt.addUInt32(guid);                                  // 0
		stmt.addUInt32(pItem->GetItemId());                    // 1
		stmt.addUInt32(pItem->GetOwner()->GetGUIDLow());       // 2
		stmt.addString(pItem->GetName());                      // 3
		stmt.addUInt32(pItem->GetCount());                     // 4
		stmt.addBool(pItem->IsEquipped());                     // 5
		stmt.addInt32(pItem->GetEnchantLevel());               // 6
		stmt.addBool(pItem->IsIdentified());                   // 7
		stmt.addInt32(pItem->GetDurability());                 // 8
		stmt.addInt32(pItem->GetChargeCount());                // 9
		stmt.addUInt32(pItem->GetDuration());                  // 10
		// todo: ���ʹ��ʱ��
		stmt.addInt8(pItem->GetBless());                       // 12
		stmt.addInt32(pItem->GetAttrEnchantKind());            // 13
		stmt.addInt32(pItem->GetAttrEnchantLevel());           // 14
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_FIRE));   // 15
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_WATER));  // 16
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_EARTH));  // 17
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_WIND));   // 18
		stmt.addInt32(pItem->GetSpellPower());                 // 19
		stmt.addInt32(pItem->GetMaxHealth());                  // 20
		stmt.addInt32(pItem->GetMaxMana());                    // 21
		stmt.addInt32(pItem->GetHealthegen());                 // 22
		stmt.addInt32(pItem->GetManaRegen());                  // 23
		stmt.addInt32(pItem->GetStat(STAT_STRENGTH));          // 24
		stmt.addInt32(pItem->GetStat(STAT_AGILITY));           // 25
		stmt.addInt32(pItem->GetStat(STAT_INTELLECT));         // 26
		stmt.addInt32(pItem->GetStat(STAT_STAMINA));           // 27
		stmt.addInt32(pItem->GetStat(STAT_SPIRIT));            // 28
		stmt.addInt32(pItem->GetStat(STAT_CHARM));             // 29
		stmt.Execute();
	}
	else
	{
		static SqlStatementID updItem;
		SqlStatement stmt = CharacterDatabase.CreateStatement(updItem,
			"UPDATE character_items SET "
   			//         0          1                2               3          4               5
			"char_id = ?, count = ?, is_equipped = ?, enchantlvl = ?, is_id = ?, durability = ?, "
   			//              6                   7          8                      9                      10          11           12           13          14
			"charge_count = ?, remaining_time = ?, bless = ?, attr_enchant_kind = ?, attr_enchant_level = ?, firemr = ?, watermr = ?, earthmr = ?, windmr = ?, "
   			//      15         16         17       18       19          20          21          22          23          24          25
			"addsp = ?, addhp = ?, addmp = ?, hpr = ?, mpr = ?, addstr = ?, adddex = ?, addint = ?, addcon = ?, addwis = ?, addcha = ? WHERE id = ?");
		stmt.addUInt32(pItem->GetOwner()->GetGUIDLow());      // 0
		stmt.addUInt32(pItem->GetCount());                    // 1
		stmt.addBool(pItem->IsEquipped());                    // 2
		stmt.addInt32(pItem->GetEnchantLevel());              // 3
		stmt.addBool(pItem->IsIdentified());                  // 4
		stmt.addInt32(pItem->GetDurability());                // 5
		stmt.addInt32(pItem->GetChargeCount());               // 6
		stmt.addUInt32(pItem->GetDuration());                 // 7
		stmt.addBool(pItem->GetBless());                      // 8
		stmt.addInt32(pItem->GetAttrEnchantKind());           // 9
		stmt.addInt32(pItem->GetAttrEnchantLevel());          // 10
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_FIRE));  // 11
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_WATER)); // 12
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_EARTH)); // 13
		stmt.addInt32(pItem->GetDefense(SPELL_SCHOOL_WIND));  // 14
		stmt.addInt32(pItem->GetSpellPower());                // 15
		stmt.addInt32(pItem->GetMaxHealth());                 // 16
		stmt.addInt32(pItem->GetMaxMana());                   // 17
		stmt.addInt32(pItem->GetHealthegen());                // 18
		stmt.addInt32(pItem->GetManaRegen());                 // 19
		stmt.addInt32(pItem->GetStat(STAT_STRENGTH));         // 20
		stmt.addInt32(pItem->GetStat(STAT_AGILITY));          // 21
		stmt.addInt32(pItem->GetStat(STAT_INTELLECT));        // 22
		stmt.addInt32(pItem->GetStat(STAT_STAMINA));          // 23
		stmt.addInt32(pItem->GetStat(STAT_SPIRIT));           // 24
		stmt.addInt32(pItem->GetStat(STAT_CHARM));            // 25
		stmt.addUInt32(guid);
		stmt.Execute();
	}
}

void PcInventory::DeleteFromDB(Item* pItem)
{
	static SqlStatementID delItem;

	SqlStatement stmt = CharacterDatabase.CreateStatement(delItem, "DELETE FROM character_items WHERE id = ?");
	stmt.PExecute(pItem->GetGUIDLow());
}

void PcInventory::ApplyItemMods(Item* pItem, bool apply)
{
	// Ӧ�õ���ģ������Լӳ�
	const ItemEntry* proto = pItem->GetProto();
	m_owner->ModifyMaxHealth(apply ? proto->health : -proto->health);
	m_owner->ModifyMaxMana(apply ? proto->mana : -proto->mana);
	m_owner->ModifyStat(STAT_STRENGTH, apply ? proto->strength : -proto->strength);
	m_owner->ModifyStat(STAT_STAMINA, apply ? proto->stamina : -proto->stamina);
	m_owner->ModifyStat(STAT_AGILITY, apply ? proto->agility : -proto->agility);
	m_owner->ModifyStat(STAT_INTELLECT, apply ? proto->intellect : -proto->intellect);
	m_owner->ModifyStat(STAT_SPIRIT, apply ? proto->spirit : -proto->spirit);
	if (proto->spirit != 0)
	{
		m_owner->UpdateBaseDefense();
	}
	m_owner->ModifyStat(STAT_CHARM, apply ? proto->charm : -proto->charm);
	// ���������ģ����������Լӳ�
	if (proto->Class == ITEM_CLASS_ARMOR)
	{
		int32 ac = proto->armor;
		switch (proto->SubClass)
		{
			case ITEM_SUBCLASS_ARMOR_ARMOUR:
			{
				// ����Ƿ�������׻���Ч��
				ac -= pItem->GetAuraEffect(ITEM_AURA_EFFECT_TYPE_AC);
			}
			case ITEM_SUBCLASS_ARMOR_HELMET:
			case ITEM_SUBCLASS_ARMOR_SHIRT:
			case ITEM_SUBCLASS_ARMOR_CLOAK:
			case ITEM_SUBCLASS_ARMOR_GLOVE:
			case ITEM_SUBCLASS_ARMOR_BOOTS:
			case ITEM_SUBCLASS_ARMOR_SHIELD:
			case ITEM_SUBCLASS_ARMOR_GARDEBRAS:
				ac -= pItem->GetEnchantLevel();
				break;
			default:
				break;
		}
		m_owner->ModifyArmor(apply ? ac : -ac);
		m_owner->ModifyDamageReduction(apply ? proto->damage_reduction : -proto->damage_reduction);
		m_owner->ModifyWeightReduction(apply ? proto->weight_reduction : -proto->weight_reduction);
		m_owner->ModifyAttackHit(MELEE_ATTACK, apply ? proto->melee_hit_chance : -proto->melee_hit_chance);
		m_owner->ModifyAttackHit(RANGED_ATTACK, apply ? proto->ranged_hit_chance : -proto->ranged_hit_chance);
		m_owner->ModifyAttackDamage(MELEE_ATTACK, apply ? proto->melee_damage_done : -proto->melee_damage_done);
		m_owner->ModifyAttackDamage(RANGED_ATTACK, apply ? proto->ranged_damage_done : -proto->ranged_damage_done);
		m_owner->ModifyDefense(SPELL_SCHOOL_EARTH, apply ? proto->defense_earth + pItem->GetDefense(SPELL_SCHOOL_EARTH) : -proto->defense_earth - pItem->GetDefense(SPELL_SCHOOL_EARTH));
		m_owner->ModifyDefense(SPELL_SCHOOL_WIND, apply ? proto->defense_wind + pItem->GetDefense(SPELL_SCHOOL_WIND) : -proto->defense_wind - pItem->GetDefense(SPELL_SCHOOL_WIND));
		m_owner->ModifyDefense(SPELL_SCHOOL_WATER, apply ? proto->defense_water + pItem->GetDefense(SPELL_SCHOOL_WATER) : -proto->defense_water - pItem->GetDefense(SPELL_SCHOOL_WATER));
		m_owner->ModifyDefense(SPELL_SCHOOL_FIRE, apply ? proto->defense_fire + pItem->GetDefense(SPELL_SCHOOL_FIRE) : -proto->defense_fire - pItem->GetDefense(SPELL_SCHOOL_FIRE));
		m_owner->ModifyResist(MECHANIC_STUN, apply ? proto->resist_stun : -proto->resist_stun);
		m_owner->ModifyResist(MECHANIC_STONE, apply ? proto->resist_stone : -proto->resist_stone);
		m_owner->ModifyResist(MECHANIC_SLEEP, apply ? proto->resist_sleep : -proto->resist_sleep);
		m_owner->ModifyResist(MECHANIC_FREEZE, apply ? proto->resist_freeze : -proto->resist_freeze);
		m_owner->ModifyResist(MECHANIC_SUSTAIN, apply ? proto->resist_sustain : -proto->resist_sustain);
		m_owner->ModifyResist(MECHANIC_BLIND, apply ? proto->resist_blind : -proto->resist_blind);
	}

	// Ӧ�õ���ʵ�������Լӳ�
	m_owner->ModifyMaxHealth(apply ? pItem->GetMaxHealth() : -pItem->GetMaxHealth());
	m_owner->ModifyMaxMana(apply ? pItem->GetMaxMana() : -pItem->GetMaxMana());
	int32 mdef = pItem->GetDefense(SPELL_SCHOOL_NORMAL);
	if (mdef != 0)
	{
		m_owner->ModifyDefense(SPELL_SCHOOL_NORMAL, apply ? mdef : -mdef);
		S_SPMR pkt(m_owner);
		m_owner->GetSession()->SendPacket(&pkt);
	}

	ApplyItemEquipSpell(pItem, true);
}

void PcInventory::ApplyItemEquipSpell(Item* item, bool apply)
{
	// todo: ����ͷ����װ��ʱ��������ħ��Ч��
}

bool PcInventory::GetSlotsForInventoryType(uint8 invType, uint8* slots, uint32 subClass) const
{
	slots[0] = NULL_SLOT;
	slots[1] = NULL_SLOT;
	slots[2] = NULL_SLOT;
	slots[3] = NULL_SLOT;

	switch (invType)
	{
	case INVTYPE_HEAD:
		slots[0] = EQUIPMENT_SLOT_HEAD;
		break;
	case INVTYPE_NECK:
		slots[0] = EQUIPMENT_SLOT_NECK;
		break;
	case INVTYPE_SHOULDERS:
		slots[0] = EQUIPMENT_SLOT_SHOULDER;
		break;
	case INVTYPE_BODY:
		slots[0] = EQUIPMENT_SLOT_BODY;
		break;
	case INVTYPE_CHEST:
		slots[0] = EQUIPMENT_SLOT_CHEST;
		break;
	case INVTYPE_WAIST:
		slots[0] = EQUIPMENT_SLOT_WAIST;
		break;
	case INVTYPE_FEET:
		slots[0] = EQUIPMENT_SLOT_FEET;
		break;
	case INVTYPE_HANDS:
		slots[0] = EQUIPMENT_SLOT_HANDS;
		break;
	case INVTYPE_FINGER:
		slots[0] = EQUIPMENT_SLOT_FINGER1;
		slots[1] = EQUIPMENT_SLOT_FINGER2;
		// todo: ������������Ӳ�λ�������
		// slots[2] = EQUIPMENT_SLOT_FINGER3;
		break;
	case INVTYPE_WEAPON:
	case INVTYPE_RANGED:
		slots[0] = EQUIPMENT_SLOT_MAINHAND;
		// todo: ��սʿְҵ����˫��
		break;
	case INVTYPE_SHIELD:
		slots[0] = EQUIPMENT_SLOT_OFFHAND;
		break;
	case INVTYPE_CLOAK:
		slots[0] = EQUIPMENT_SLOT_BACK;
		break;
	default:
		return false;
	}

	return true;
}

void PcInventory::MoveItemFromInventory(Item* pItem)
{
	// todo: �Ƴ����ߵķ���Ч����ֹͣ����ʹ�ü�ʱ
	m_items.erase(pItem->GetGUIDLow());
}

uint8 PcInventory::ComputeWeight240(int32 weight) const
{
	uint8 weight240 = 0;
	if (sConfig.GetIntDefault("L1J.RateWeightLimit", 500) != 0)
	{
		weight *= 10000;
		int32 max_weight = m_owner->GetMaxWeight();
		if (weight > max_weight)
		{
			weight240 = 240;
		}
		else
		{
			weight240 = uint8((weight * 100 / max_weight) * 240 / 100);
		}
	}
	return weight240;
}