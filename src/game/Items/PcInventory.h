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

	uint8 GetWeight240() const; 			// 负重计算
	WeaponShapeType GetWeaponShape() const; // 获取当前挥舞的武器外观造型

	/**
	 * 应用/取消道具法术效果
	 *
	 * @param item
	 * @param apply
	 */
	void ApplyAuraEffect(Item* item, bool apply);

	/**
	 * 检查道具使用限制，对道具的职业使用限制、等级使用限制，并在direct_action为true时对使用玩家的状态进行检测
	 *
	 * @param pItem 道具
	 * @param direct_action false表示当前检测处于数据库加载阶段，不能直接对玩家状态进行检测，退化成下面的简化版本
	 * @return 检测结果
	 */
	InventoryResult CanUseItem(Item* pItem, bool direct_action = true) const;
	/**
	 * 检查道具使用限制，上面的简化版本，仅对道具的职业使用限制和等级使用限制进行检测
	 *
	 * @param proto 道具模板
	 * @return 检测结果
	 */
	InventoryResult CanUseItem(const ItemEntry* proto) const;

	/**
	 * 是否能装备道具，如果是装备替换，还会调用CanUnequipItem检测能否脱下现有装备
	 *
	 * @param pItem         待装备的道具
	 * @param dest          用于保存道具最终装备的位置
	 * @param direct_action false表示当前检测处于数据库加载阶段，不能直接对玩家状态进行检测
	 * @return 是否能装备道具
	 */
	InventoryResult CanEquipItem(Item* pItem, uint8& dest, bool direct_action = true) const;
	/**
	 * 是否能脱下道具
	 *
	 * @param pItem 待脱下的道具
	 * @param direct_action false表示当前检测处于数据库加载阶段，不能直接对玩家状态进行检测
	 * @return 是否能脱下道具
	 */
	InventoryResult CanUnequipItem(Item* pItem, bool direct_action = true) const;

	/**
	 * 快速装备道具，在加载数据库，并初始化玩家对象的时候使用，跳过对装备槽现有道具的检测，
	 * 因为这时候肯定是空的，这时候相当于玩家空身，正一件件的快速穿上装备
	 *
	 * @param slot   道具装备的位置
	 * @param pItem 待装备的道具
	 */
	void QuickEquipItem(uint8 slot, Item* pItem);

	/**
	 * 装备道具，调用之前先调用CanEquipItem检测是否允许
	 *
	 * @param slot  道具装备的位置
	 * @param pItem 待装备的道具
	 */
	void EquipItem(uint8 slot, Item* pItem);

	/**
	 * 脱下道具，调用之前先调用CanUnequipItem检测是否允许
	 *
	 * @param pItem 待脱下的道具
	 */
	void UnequipItem(Item* pItem);

	/**
	 * 是否正在使用双手武器
	 *
	 * @return true表示正在使用
	 */
	bool IsTwoHandUsed() const;

	/**
	 * 重写操作
	 */

	InventoryResult CanStoreItem(uint32 item, int32 count) const override;
	void LoadFromDB(QueryResult* result, uint32 timediff) override;
	void SaveToDB(Item* pItem, bool insert) override;
	void DeleteFromDB(Item* pItem) override;

protected:
	/**
	 * 更新道具使用时长
	 *
	 * @param diff 距离上次更新已经经过的时间
	 */
	void UpdateItemDuration(uint32 diff);
	void AddItemDurations(Item* pItem);           // 启用道具使用计时
	void RemoveItemDurations(Item* pItem);        // 停止道具使用计时

	/**
	 * 更新道具法术效果时长
	 *
	 * @param diff 距离上次更新已经经过的时间
	 */
	void UpdateAuraEffectDuration(uint32 diff);
	void AddAuraEffectDurations(Item* pItem);     // 启用道具使用计时
	void RemoveAuraEffectDurations(Item* pItem);  // 停止道具使用计时

	/**
	 * 应用/取消道具带来的属性加成
	 *
	 * @param pItem 道具实例
	 * @param apply true表示应用属性加成
	 */
	void ApplyItemMods(Item* pItem, bool apply);

	/**
	 * 应用装备道具触发的法术(例如天堂佩戴力量魔法头盔，会自动新增魔法)
	 *
	 * @param item        道具
	 * @param apply       穿上/脱下
	 */
	void ApplyItemEquipSpell(Item* item, bool apply);

	/**
	 * 根据道具库存类型获取对应的装备槽位置
	 *
	 * @param invType  库存类型
	 * @param slots    保存找到的装备槽位置
	 * @param subClass 道具子类型
	 * @return 是否成功找到了装备槽位置
	 */
	bool GetSlotsForInventoryType(uint8 invType, uint8* slots, uint32 subClass) const;

	/**
	 * 移除道具的法术效果、停止道具使用计时，并从道具列表中移除，但不将其从数据库中移除
	 *
	 * @param pItem 待移除道具
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
