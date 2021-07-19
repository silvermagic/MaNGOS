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
#include "ObjectMgr.h"
#include "SQLStorages.h"
#include "Item.h"
#include "Inventory.h"

bool ItemComparator(const Item* pItemA, const Item* pItemB)
{
	return pItemA->GetEnchantLevel() - pItemB->GetEnchantLevel();
}

Inventory::Inventory() : m_type(WAREHOUSE_TYPE_NONE)
{
}

Inventory::~Inventory()
{
}

void Inventory::Update(uint32 diff)
{
}

void Inventory::Clear()
{
}

WarehouseType Inventory::GetType() const
{
	return m_type;
}

int32 Inventory::GetWeight() const
{
	int32 weight = 0;
	for (auto& slot : m_items)
	{
		weight += slot.second->GetWeight();
	}
	return weight;
}

const ObjectGuid& Inventory::GetOwnerGuid() const
{
	return GetGuidValue(ITEM_FIELD_OWNER);
}

void Inventory::SetOwnerGuid(ObjectGuid guid)
{
	SetGuidValue(ITEM_FIELD_OWNER, guid);
}

InventoryResult Inventory::CanStoreItem(Item* pItem) const
{
	if (!pItem)
	{
		return EQUIP_ERR_NONE;
	}

	return CanStoreItem(pItem->GetProto()->ItemId, pItem->GetCount());
}

InventoryResult Inventory::CanStoreItem(uint32 item, int32 count) const
{
	if (count <= 0)
	{
		return EQUIP_ERR_NONE;
	}

	const ItemEntry* proto = sObjectMgr.GetItemEntry(item);
	if (!proto)
	{
		return EQUIP_ERR_NONE;
	}

	static int32 MAX_NPC_ITEM = sConfig.GetIntDefault("L1J.MaxNpcItem", 80);
	if (m_items.size() > MAX_NPC_ITEM || (m_items.size() == MAX_NPC_ITEM || (!proto->IsStackable() || !CheckItemCount(item))))
	{
		return EQUIP_ERR_BAG_FULL;
	}

	static int32 RATE_WEIGHT_LIMIT_PET = sConfig.GetIntDefault("L1J.RateWeightLimitforPet", 50);
	int32 weight = GetWeight() + proto->weight * count / 1000 + 1;
	if (weight < 0 || proto->weight < 0 || weight > (MAX_WEIGHT * RATE_WEIGHT_LIMIT_PET))
	{
		return EQUIP_ERR_OVER_WEIGHT;
	}

	int32 amount = GetItemCount(item);
	if ((amount + count) > MAX_AMOUNT)
	{
		return EQUIP_ERR_CANT_CARRY_MORE_OF_THIS;
	}

	return EQUIP_ERR_OK;
}

bool Inventory::CheckItemCount(uint32 item, int32 count) const
{
	if (count == 0)
	{
		return true;
	}

	const ItemEntry* proto = sObjectMgr.GetItemEntry(item);
	if (proto)
	{
		if (proto->IsStackable())
		{
			Item* pItem = GetItem(item);
			if (pItem && pItem->GetCount() >= count)
			{
				return true;
			}
		}
		else
		{
			std::vector<Item*> pItems = GetItems(item);
			if (pItems.size() >= count)
			{
				return true;
			}
		}
	}

	return false;
}

bool Inventory::CheckEnchantItemCount(uint32 item, int32 enchant, int32 count) const
{
	int32 amount = 0;
	for (auto& kv : m_items)
	{
		Item* pItem = kv.second;
		if (pItem->IsEquipped())
		{
			continue;
		}

		if (pItem->GetItemId() == item && pItem->GetEnchantLevel() == enchant)
		{
			if (++amount == count)
			{
				return true;
			}
		}
	}
	return false;
}

bool Inventory::CostItem(uint32 item, int32 count)
{
	if (count <= 0)
	{
		return false;
	}

	const ItemEntry* proto = sObjectMgr.GetItemEntry(item);
	if (!proto)
	{
		return false;
	}

	if (proto->IsStackable())
	{
		Item* pItem = GetItem(item);
		if (pItem && pItem->GetCount() >= count)
		{
			RemoveItem(pItem, count);
			return true;
		}
	}
	else
	{
		std::vector<Item*> pItems = GetItems(item);
		if (pItems.size() == count)
		{
			for (std::size_t i = 0; i < pItems.size(); ++i)
			{
				RemoveItem(pItems[i]);
			}
			return true;
		}
		else if (pItems.size() > count)
		{
			std::sort(pItems.begin(), pItems.end(), ItemComparator);
			for (std::size_t i = 0; i < count; ++i)
			{
				RemoveItem(pItems[i]);
			}
			return true;
		}
	}

	return false;
}

bool Inventory::CostEnchantItem(uint32 item, int32 enchant, int32 count)
{
	std::vector<Item*> pItems = GetEnchantItems(item, enchant);
	if (pItems.size() >= count)
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			RemoveItem(pItems[i]);
		}
		return true;
	}

	return false;
}

int32 Inventory::RemoveItem(Item* pItem)
{
	if (!pItem)
	{
		return 0;
	}

	return RemoveItem(pItem, pItem->GetCount());
}

int32 Inventory::RemoveItem(uint32 guid)
{
	Item* pItem = GetItem(guid);
	if (!pItem)
	{
		return 0;
	}

	return RemoveItem(pItem, pItem->GetCount());
}

int32 Inventory::RemoveItem(Item* pItem, int32 count)
{
	if (!pItem)
	{
		return 0;
	}

	int32 amount = pItem->GetCount();
	if (amount <= count)
	{
		MoveItemFromInventory(pItem);
		DeleteFromDB(pItem);
		delete pItem;
		return amount;
	}
	else
	{
		amount -= count;
		pItem->SetCount(amount);
		SaveToDB(pItem);
		return count;
	}
}

Item* Inventory::StoreItem(Item* pItem)
{
	pItem->SetOwnerGuid(GetOwnerGuid());

	if (pItem->IsStackable())
	{
		Item* pSrcItem = GetItem(pItem->GetItemId());
		if (pSrcItem)
		{
			pSrcItem->SetCount(pSrcItem->GetCount() + pItem->GetCount());
			SaveToDB(pSrcItem);
			delete pItem;
			return pSrcItem;
		}
	}

	MoveItemToInventory(pItem);
	SaveToDB(pItem);
	return pItem;
}

Item* Inventory::StoreNewItem(Item* pItem)
{
	const ItemEntry* proto = pItem->GetProto();
	// todo: 使用次数 蜡烛使用时间
	return StoreItem(pItem);
}

Item* Inventory::StoreNewItem(uint32 item, int32 count)
{
	const ItemEntry* proto = sObjectMgr.GetItemEntry(item);
	if (!proto || count <= 0)
	{
		return nullptr;
	}

	if (proto->IsStackable())
	{
		Item* pNewItem = Item::CreateItem(item, count);
		if (!pNewItem)
		{
			return nullptr;
		}

		return StoreNewItem(pNewItem);
	}
	else
	{
		Item* pLastItem = nullptr;
		for (int32 i = 0; i < count; ++i)
		{
			Item* pNewItem = Item::CreateItem(item, 1);
			pLastItem = StoreNewItem(pNewItem);
		}
		return pLastItem;
	}
}

Item* Inventory::TradeItem(Item* pItem, int32 SplitedAmount, Inventory* pTo)
{
	if (!pItem || pItem->GetCount() <= 0 || SplitedAmount <= 0 || pItem->IsEquipped())
	{
		return nullptr;
	}

	InventoryResult msg = CanStoreItem(pItem->GetItemId(), SplitedAmount);
	if (msg != EQUIP_ERR_OK)
	{
		return nullptr;
	}

	Item* pTargetItem = nullptr;
	if (pItem->GetCount() <= SplitedAmount)
	{
		CharacterDatabase.BeginTransaction();
		MoveItemFromInventory(pItem);
		DeleteFromDB(pItem);
		pTargetItem = pTo->StoreItem(pItem);
		CharacterDatabase.CommitTransaction();
	}
	else
	{
		Item* pNewItem = pItem->CloneItem(SplitedAmount);
		if (!pNewItem)
		{
			return nullptr;
		}
		CharacterDatabase.BeginTransaction();
		pItem->SetCount(pItem->GetCount() - SplitedAmount);
		SaveToDB(pItem);
		pTargetItem = pTo->StoreItem(pNewItem);
		CharacterDatabase.CommitTransaction();
	}

	return pTargetItem;
}

Item* Inventory::TradeItem(ObjectGuid guid, int32 SplitedAmount, Inventory* pTo)
{
	Item* pItem = GetItem(guid);
	return TradeItem(pItem, SplitedAmount, pTo);
}

Item* Inventory::GetItem(uint32 item) const
{
	for (auto& kv : m_items)
	{
		Item* pItem = kv.second;
		const ItemEntry* proto = pItem->GetProto();
		if (proto && proto->ItemId == item)
		{
			return pItem;
		}
	}

	return nullptr;
}

Item* Inventory::GetItem(ObjectGuid guid) const
{
	for (auto& kv : m_items)
	{
		Item* pItem = kv.second;
		if (pItem->GetObjectGuid() == guid)
		{
			return pItem;
		}
	}

	return nullptr;
}

std::vector<Item*> Inventory::GetItems() const
{
	std::vector<Item*> pItems;
	for (auto& kv : m_items)
	{
		pItems.push_back(kv.second);
	}

	return std::move(pItems);
}

std::vector<Item*> Inventory::GetItems(uint32 item, bool skip_equipped) const
{
	std::vector<Item*> pItems;
	for (auto& kv : m_items)
	{
		Item* pItem = kv.second;
		if (pItem->GetItemId() == item)
		{
			if (skip_equipped && pItem->IsEquipped())
			{
				continue;
			}
			pItems.push_back(pItem);
		}
	}

	return std::move(pItems);
}

std::vector<Item*> Inventory::GetEnchantItems(uint32 item, int32 enchant, bool skip_equipped) const
{
	std::vector<Item*> pItems;
	for (auto& kv : m_items)
	{
		Item* pItem = kv.second;
		if (pItem->GetItemId() == item && pItem->GetEnchantLevel() == enchant)
		{
			if (skip_equipped && pItem->IsEquipped())
			{
				continue;
			}
			pItems.push_back(pItem);
		}
	}

	return std::move(pItems);
}

int32 Inventory::GetItemCount(uint32 item) const
{
	const ItemEntry* proto = sObjectMgr.GetItemEntry(item);
	if (proto)
	{
		if (proto->IsStackable())
		{
			Item* pItem = GetItem(item);
			if (pItem)
			{
				return pItem->GetCount();
			}
		}
		else
		{
			std::vector<Item*> items = GetItems(item);
			return items.size();
		}
	}

	return 0;
}

void Inventory::LoadFromDB(QueryResult* result, uint32 timediff)
{
}

void Inventory::SaveToDB(Item* pItem, bool insert)
{
}

void Inventory::DeleteFromDB(Item* pItem)
{
}

void Inventory::MoveItemFromInventory(Item* pItem)
{
	m_items.erase(pItem->GetGUIDLow());
}

void Inventory::MoveItemToInventory(Item* pItem)
{
	m_items[pItem->GetGUIDLow()] = pItem;
}

