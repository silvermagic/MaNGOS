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

	void Clear();                           // 清理背包
	WarehouseType GetType() const;          // 获取背包类型
	virtual int32 GetWeight() const;        // 统计背包内道具总重量
	const ObjectGuid& GetOwnerGuid() const; // 获取背包的管理者
	void SetOwnerGuid(ObjectGuid guid);     // 设置背包的管理者

	/**
	 * 检查是否还能放入新道具，主要检查是否超过背包容量限制、是否超过背包重量限制(仅玩家背包类型)，以及是否超过道具数量限制
	 *
	 * @param pItem 新道具
	 * @return 检查结果
	 */
	InventoryResult CanStoreItem(Item* pItem) const;
	virtual InventoryResult CanStoreItem(uint32 item, int32 count) const;

	/**
	 * 检查背包是否有足够数量的道具
	 *
	 * @param item  道具模板编号
	 * @param count 道具数量
	 * @return 检查结果
	 */
	bool CheckItemCount(uint32 item, int32 count = 1) const;
	/**
	 * 检查背包是否有足够数量的强化道具(跳过装备道具)
	 *
	 * @param item    道具模板编号
	 * @param enchant 道具强化等级
	 * @param count   道具数量
	 * @return 检查结果
	 */
	bool CheckEnchantItemCount(uint32 item, int32 enchant, int32 count) const;

	/**
	 * 消耗道具，仅道具数量充足的情况下才调用DestroyItem执行删除，否则返回false，使用场景: 喝药、箭矢、消耗施法材料等
	 *
	 * @param item  道具模板编号
	 * @param count 消耗数量
	 * @return 消耗是否成功
	 */
	bool CostItem(uint32 item, int32 count);
	/**
	 * 消耗指定等级的道具，仅道具数量充足的情况下才调用DestroyItem执行删除，否则返回false，使用场景: 点装备
	 *
	 * @param item    道具模板编号
	 * @param enchant 道具强化等级
	 * @param count   消耗数量
	 * @return 消耗是否成功
	 */
	bool CostEnchantItem(uint32 item, int32 enchant, int32 count);

	/**
	 * 删除道具实例，同时从数据库中删除，使用场景: 丢弃不可叠加道具到垃圾箱或可叠加道具最后一个被用掉
	 *
	 * @param pItem  道具实例
	 * @return 实际删除的数量
	 */
	int32 RemoveItem(Item* pItem);
	int32 RemoveItem(uint32 guid);
	/**
	 * 从道具中删除指定数量，同时更新数据库，使用场景: 使用道具(例如箭矢)或丢弃可叠加道具到垃圾箱时，不输入全部数量
	 *
	 * @param pItem 道具实例
	 * @param count 删除数量
	 * @return 实际删除的数量
	 */
	int32 RemoveItem(Item* pItem, int32 count);

	/**
	 * 存入道具(道具是已经存在的，不需要对道具实例属性进行随机初始化)，同时插入数据库(调用者必须保证此时的道具仅存在于内存中)，使用场景: 交易、仓库存取
	 *
	 * @param pItem
	 * @return
	 */
	Item* StoreItem(Item* pItem);
	/**
	 * 存入新道具(道具是全新的，需要根据道具模板对道具实例属性进行随机初始化)，同时插入数据库(调用者必须保证此时的道具仅存在于内存中)，使用场景: 怪物掉落、购买、GM道具创建指令
	 *
	 * @param pItem  新道具
	 * @return 道具实例(如果道具允许堆叠，例如箭矢，则返回的对象不一定是存入的道具)
	 */
	Item* StoreNewItem(Item* pItem);
	Item* StoreNewItem(uint32 item, int32 count);

	/**
	 * 将自身的道具交易到其他仓库，需要将道具从原数据库删除，并插入新的数据库，使用场景: 仓库存取、交易
	 *
	 * @param pItem         待交易道具
	 * @param SplitedAmount 道具交易数量
	 * @param pTo           将要存入的仓库
	 * @return 存入仓库的道具实例(如果只交易部分道具，那么返回的将是克隆的道具)
	 */
	Item* TradeItem(Item* pItem, int32 SplitedAmount, Inventory* pTo);
	Item* TradeItem(ObjectGuid guid, int32 SplitedAmount, Inventory* pTo);

	/**
	 * 在背包里面寻找指定模板编号的道具
	 *
	 * @param item 道具模板编号
	 * @return 道具实例(可能为空，如果存在多个返回第一个找到的)
	 */
	Item* GetItem(uint32 item) const;
	Item* GetItem(ObjectGuid guid) const; // 根据道具GUID检索
	std::vector<Item*> GetItems() const;
	std::vector<Item*> GetItems(uint32 item, bool skip_equipped = true) const;
	std::vector<Item*> GetEnchantItems(uint32 item, int32 enchant, bool skip_equipped = true) const;

	/**
	 * 统计道具数量
	 *
	 * @param item 道具模板编号
	 * @return 道具数量
	 */
	int32 GetItemCount(uint32 item) const;

	/**
	 * 数据库操作
	 */

	virtual void LoadFromDB(QueryResult* result, uint32 timediff);
	virtual void SaveToDB(Item* pItem, bool insert = false);
	virtual void DeleteFromDB(Item* pItem);

protected:
	/**
	 * 将道具从仓库列表移除，不会将道具从数据库中删除，PC背包需要重写
	 *
	 * @param pItem 待移除道具
	 */
	virtual void MoveItemFromInventory(Item* pItem);
	/**
	 * 将道具添加到仓库列表，不会将道具同步到数据库
	 *
	 * @param pItem 待移除道具
	 */
	virtual void MoveItemToInventory(Item* pItem);

protected:
	WarehouseType m_type;
	std::map<uint32, Item*> m_items;
};

#endif //MANGOS_INVENTORY_H
