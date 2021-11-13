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

#ifndef MANGOS_PCINVENTORY_H
#define MANGOS_PCINVENTORY_H

#include "Inventory.h"

struct ItemEntry;

class Player;

class PcInventory : public Inventory
{
public:
	PcInventory(Player* owner);

	void Update(uint32 diff) override;

	uint8 GetWeight240() const; 			// ���ؼ���
	WeaponShapeType GetWeaponShape() const; // ��ȡ��ǰ����������������

	/**
	 * Ӧ��/ȡ�����߷���Ч��
	 *
	 * @param item
	 * @param apply
	 */
	void ApplyAuraEffect(Item* item, bool apply);

	/**
	 * ������ʹ�����ƣ��Ե��ߵ�ְҵʹ�����ơ��ȼ�ʹ�����ƣ�����direct_actionΪtrueʱ��ʹ����ҵ�״̬���м��
	 *
	 * @param pItem ����
	 * @param direct_action false��ʾ��ǰ��⴦�����ݿ���ؽ׶Σ�����ֱ�Ӷ����״̬���м�⣬�˻�������ļ򻯰汾
	 * @return �����
	 */
	InventoryResult CanUseItem(Item* pItem, bool direct_action = true) const;
	/**
	 * ������ʹ�����ƣ�����ļ򻯰汾�����Ե��ߵ�ְҵʹ�����ƺ͵ȼ�ʹ�����ƽ��м��
	 *
	 * @param proto ����ģ��
	 * @return �����
	 */
	InventoryResult CanUseItem(const ItemEntry* proto) const;

	/**
	 * �Ƿ���װ�����ߣ������װ���滻���������CanUnequipItem����ܷ���������װ��
	 *
	 * @param pItem         ��װ���ĵ���
	 * @param dest          ���ڱ����������װ����λ��
	 * @param direct_action false��ʾ��ǰ��⴦�����ݿ���ؽ׶Σ�����ֱ�Ӷ����״̬���м��
	 * @return �Ƿ���װ������
	 */
	InventoryResult CanEquipItem(Item* pItem, uint8& dest, bool direct_action = true) const;
	/**
	 * �Ƿ������µ���
	 *
	 * @param pItem �����µĵ���
	 * @param direct_action false��ʾ��ǰ��⴦�����ݿ���ؽ׶Σ�����ֱ�Ӷ����״̬���м��
	 * @return �Ƿ������µ���
	 */
	InventoryResult CanUnequipItem(Item* pItem, bool direct_action = true) const;

	/**
	 * ����װ�����ߣ��ڼ������ݿ⣬����ʼ����Ҷ����ʱ��ʹ�ã�������װ�������е��ߵļ�⣬
	 * ��Ϊ��ʱ��϶��ǿյģ���ʱ���൱����ҿ�����һ�����Ŀ��ٴ���װ��
	 *
	 * @param slot   ����װ����λ��
	 * @param pItem ��װ���ĵ���
	 */
	void QuickEquipItem(uint8 slot, Item* pItem);

	/**
	 * װ�����ߣ�����֮ǰ�ȵ���CanEquipItem����Ƿ�����
	 *
	 * @param slot  ����װ����λ��
	 * @param pItem ��װ���ĵ���
	 */
	void EquipItem(uint8 slot, Item* pItem);

	/**
	 * ���µ��ߣ�����֮ǰ�ȵ���CanUnequipItem����Ƿ�����
	 *
	 * @param pItem �����µĵ���
	 */
	void UnequipItem(Item* pItem);

	/**
	 * �Ƿ�����ʹ��˫������
	 *
	 * @return true��ʾ����ʹ��
	 */
	bool IsTwoHandUsed() const;

	/**
	 * ��д����
	 */

	InventoryResult CanStoreItem(uint32 item, int32 count) const override;
	void LoadFromDB(QueryResult* result, uint32 timediff) override;
	void SaveToDB(Item* pItem, bool insert) override;
	void DeleteFromDB(Item* pItem) override;

protected:
	/**
	 * ���µ���ʹ��ʱ��
	 *
	 * @param diff �����ϴθ����Ѿ�������ʱ��
	 */
	void UpdateItemDuration(uint32 diff);
	void AddItemDurations(Item* pItem);           // ���õ���ʹ�ü�ʱ
	void RemoveItemDurations(Item* pItem);        // ֹͣ����ʹ�ü�ʱ

	/**
	 * ���µ��߷���Ч��ʱ��
	 *
	 * @param diff �����ϴθ����Ѿ�������ʱ��
	 */
	void UpdateAuraEffectDuration(uint32 diff);
	void AddAuraEffectDurations(Item* pItem);     // ���õ���ʹ�ü�ʱ
	void RemoveAuraEffectDurations(Item* pItem);  // ֹͣ����ʹ�ü�ʱ

	/**
	 * Ӧ��/ȡ�����ߴ��������Լӳ�
	 *
	 * @param pItem ����ʵ��
	 * @param apply true��ʾӦ�����Լӳ�
	 */
	void ApplyItemMods(Item* pItem, bool apply);

	/**
	 * Ӧ��װ�����ߴ����ķ���(���������������ħ��ͷ�������Զ�����ħ��)
	 *
	 * @param item        ����
	 * @param apply       ����/����
	 */
	void ApplyItemEquipSpell(Item* item, bool apply);

	/**
	 * ���ݵ��߿�����ͻ�ȡ��Ӧ��װ����λ��
	 *
	 * @param invType  �������
	 * @param slots    �����ҵ���װ����λ��
	 * @param subClass ����������
	 * @return �Ƿ�ɹ��ҵ���װ����λ��
	 */
	bool GetSlotsForInventoryType(uint8 invType, uint8* slots, uint32 subClass) const;

	/**
	 * �Ƴ����ߵķ���Ч����ֹͣ����ʹ�ü�ʱ�����ӵ����б����Ƴ���������������ݿ����Ƴ�
	 *
	 * @param pItem ���Ƴ�����
	 */
	void MoveItemFromInventory(Item* pItem);

	/**
	 *
	 * @param weight
	 * @return
	 */
	uint8 ComputeWeight240(int32 weight) const;

protected:
	Player* m_owner;

	Item* m_slots[EQUIPMENT_SLOT_COUNT];

	typedef std::list<Item*> ItemDurationList;
	ItemDurationList m_itemDuration;

	typedef std::list<Item*> ItemAuraEffectDurationList;
	ItemAuraEffectDurationList m_itemAuraEffectDuration;
};

#endif //MANGOS_PCINVENTORY_H
