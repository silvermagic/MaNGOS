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

#ifndef MANGOS_INVENTORY_H
#define MANGOS_INVENTORY_H

#include "Entities/Object.h"
#include "SharedDefines.h"

class Item;
class QueryResult;

class Inventory : public Object
{
	friend class Item;
public:
	Inventory();
	virtual ~Inventory();

	virtual void Update(uint32 diff);

	void Clear();                           // ������
	WarehouseType GetType() const;          // ��ȡ��������
	virtual int32 GetWeight() const;        // ͳ�Ʊ����ڵ���������
	const ObjectGuid& GetOwnerGuid() const; // ��ȡ�����Ĺ�����
	void SetOwnerGuid(ObjectGuid guid);     // ���ñ����Ĺ�����

	/**
	 * ����Ƿ��ܷ����µ��ߣ���Ҫ����Ƿ񳬹������������ơ��Ƿ񳬹�������������(����ұ�������)���Լ��Ƿ񳬹�������������
	 *
	 * @param pItem �µ���
	 * @return �����
	 */
	InventoryResult CanStoreItem(Item* pItem) const;
	virtual InventoryResult CanStoreItem(uint32 item, int32 count) const;

	/**
	 * ��鱳���Ƿ����㹻�����ĵ���
	 *
	 * @param item  ����ģ����
	 * @param count ��������
	 * @return �����
	 */
	bool CheckItemCount(uint32 item, int32 count = 1) const;
	/**
	 * ��鱳���Ƿ����㹻������ǿ������(����װ������)
	 *
	 * @param item    ����ģ����
	 * @param enchant ����ǿ���ȼ�
	 * @param count   ��������
	 * @return �����
	 */
	bool CheckEnchantItemCount(uint32 item, int32 enchant, int32 count) const;

	/**
	 * ���ĵ��ߣ��������������������²ŵ���DestroyItemִ��ɾ�������򷵻�false��ʹ�ó���: ��ҩ����ʸ������ʩ�����ϵ�
	 *
	 * @param item  ����ģ����
	 * @param count ��������
	 * @return �����Ƿ�ɹ�
	 */
	bool CostItem(uint32 item, int32 count);
	/**
	 * ����ָ���ȼ��ĵ��ߣ��������������������²ŵ���DestroyItemִ��ɾ�������򷵻�false��ʹ�ó���: ��װ��
	 *
	 * @param item    ����ģ����
	 * @param enchant ����ǿ���ȼ�
	 * @param count   ��������
	 * @return �����Ƿ�ɹ�
	 */
	bool CostEnchantItem(uint32 item, int32 enchant, int32 count);

	/**
	 * ɾ������ʵ����ͬʱ�����ݿ���ɾ����ʹ�ó���: �������ɵ��ӵ��ߵ��������ɵ��ӵ������һ�����õ�
	 *
	 * @param pItem  ����ʵ��
	 * @return ʵ��ɾ��������
	 */
	int32 RemoveItem(Item* pItem);
	int32 RemoveItem(uint32 guid);
	/**
	 * �ӵ�����ɾ��ָ��������ͬʱ�������ݿ⣬ʹ�ó���: ʹ�õ���(�����ʸ)�����ɵ��ӵ��ߵ�������ʱ��������ȫ������
	 *
	 * @param pItem ����ʵ��
	 * @param count ɾ������
	 * @return ʵ��ɾ��������
	 */
	int32 RemoveItem(Item* pItem, int32 count);

	/**
	 * �������(�������Ѿ����ڵģ�����Ҫ�Ե���ʵ�����Խ��������ʼ��)��ͬʱ�������ݿ�(�����߱��뱣֤��ʱ�ĵ��߽��������ڴ���)��ʹ�ó���: ���ס��ֿ��ȡ
	 *
	 * @param pItem
	 * @return
	 */
	Item* StoreItem(Item* pItem);
	/**
	 * �����µ���(������ȫ�µģ���Ҫ���ݵ���ģ��Ե���ʵ�����Խ��������ʼ��)��ͬʱ�������ݿ�(�����߱��뱣֤��ʱ�ĵ��߽��������ڴ���)��ʹ�ó���: ������䡢����GM���ߴ���ָ��
	 *
	 * @param pItem  �µ���
	 * @return ����ʵ��(�����������ѵ��������ʸ���򷵻صĶ���һ���Ǵ���ĵ���)
	 */
	Item* StoreNewItem(Item* pItem);
	Item* StoreNewItem(uint32 item, int32 count);

	/**
	 * ������ĵ��߽��׵������ֿ⣬��Ҫ�����ߴ�ԭ���ݿ�ɾ�����������µ����ݿ⣬ʹ�ó���: �ֿ��ȡ������
	 *
	 * @param pItem         �����׵���
	 * @param SplitedAmount ���߽�������
	 * @param pTo           ��Ҫ����Ĳֿ�
	 * @return ����ֿ�ĵ���ʵ��(���ֻ���ײ��ֵ��ߣ���ô���صĽ��ǿ�¡�ĵ���)
	 */
	Item* TradeItem(Item* pItem, int32 SplitedAmount, Inventory* pTo);
	Item* TradeItem(ObjectGuid guid, int32 SplitedAmount, Inventory* pTo);

	/**
	 * �ڱ�������Ѱ��ָ��ģ���ŵĵ���
	 *
	 * @param item ����ģ����
	 * @return ����ʵ��(����Ϊ�գ�������ڶ�����ص�һ���ҵ���)
	 */
	Item* GetItem(uint32 item) const;
	Item* GetItem(ObjectGuid guid) const; // ���ݵ���GUID����
	std::vector<Item*> GetItems() const;
	std::vector<Item*> GetItems(uint32 item, bool skip_equipped = true) const;
	std::vector<Item*> GetEnchantItems(uint32 item, int32 enchant, bool skip_equipped = true) const;

	/**
	 * ͳ�Ƶ�������
	 *
	 * @param item ����ģ����
	 * @return ��������
	 */
	int32 GetItemCount(uint32 item) const;

	/**
	 * ���ݿ����
	 */

	virtual void LoadFromDB(QueryResult* result, uint32 timediff);
	virtual void SaveToDB(Item* pItem, bool insert = false);
	virtual void DeleteFromDB(Item* pItem);

protected:
	/**
	 * �����ߴӲֿ��б��Ƴ������Ὣ���ߴ����ݿ���ɾ����PC������Ҫ��д
	 *
	 * @param pItem ���Ƴ�����
	 */
	virtual void MoveItemFromInventory(Item* pItem);
	/**
	 * ��������ӵ��ֿ��б����Ὣ����ͬ�������ݿ�
	 *
	 * @param pItem ���Ƴ�����
	 */
	virtual void MoveItemToInventory(Item* pItem);

protected:
	WarehouseType m_type;
	std::map<uint32, Item*> m_items;
};

#endif //MANGOS_INVENTORY_H
