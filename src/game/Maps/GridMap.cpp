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

#include "Entities/Corpse.h"
#include "Entities/Creature.h"
#include "Entities/DynamicObject.h"
#include "Entities/GameObject.h"
#include "Entities/Pet.h"
#include "Entities/Player.h"
#include "ObjectAccessor.h"
#include "GridMapImpl.h"

GridMap::GridMap(uint32 mapId) : m_gminfo(GRID_MAP_INFOS[mapId])
{
	MANGOS_ASSERT(m_gminfo.ok);
	m_grids.resize(m_gminfo.width * m_gminfo.height);
	m_bits.resize(m_grids.size());
}

void GridMap::UpdateObjectVisibility(WorldObject* obj, const GridPair& gridPair)
{
	return;
}

Creature* GridMap::GetAnyTypeCreature(ObjectGuid guid)
{
	switch (guid.GetHigh())
	{
		case HIGHGUID_UNIT:
		case HIGHGUID_VEHICLE:
			return GetCreature(guid);
		case HIGHGUID_PET:
			return GetPet(guid);
		default:
			break;
	}

	return nullptr;
}

Corpse* GridMap::GetCorpse(ObjectGuid guid)
{
	// todo: 尸体查询
	return nullptr;
}

Creature* GridMap::GetCreature(ObjectGuid guid)
{
	return m_objectsStore.find<Creature>(guid, (Creature*)nullptr);
}

DynamicObject* GridMap::GetDynamicObject(ObjectGuid guid)
{
	return m_objectsStore.find<DynamicObject>(guid, (DynamicObject*)nullptr);
}

GameObject* GridMap::GetGameObject(ObjectGuid guid)
{
	return m_objectsStore.find<GameObject>(guid, (GameObject*)nullptr);
}

Pet* GridMap::GetPet(ObjectGuid guid)
{
	return m_objectsStore.find<Pet>(guid, (Pet*)nullptr);
}

Player* GridMap::GetPlayer(ObjectGuid guid)
{
	// 仅查找处于游戏世界当中的玩家对象
	Player* plr = ObjectAccessor::FindPlayer(guid);
	return plr && plr->GetMap() == this ? plr : nullptr;
}

Unit* GridMap::GetUnit(ObjectGuid guid)
{
	if (guid.IsPlayer())
		return GetPlayer(guid);

	return GetAnyTypeCreature(guid);
}

WorldObject* GridMap::GetWorldObject(ObjectGuid guid)
{
	switch (guid.GetHigh())
	{
		case HIGHGUID_PLAYER:
			return GetPlayer(guid);
		case HIGHGUID_GAMEOBJECT:
			return GetGameObject(guid);
		case HIGHGUID_UNIT:
		case HIGHGUID_VEHICLE:
			return GetCreature(guid);
		case HIGHGUID_PET:
			return GetPet(guid);
		case HIGHGUID_DYNAMICOBJECT:
			return GetDynamicObject(guid);
		case HIGHGUID_CORPSE:
			return GetCorpse(guid);
		case HIGHGUID_MO_TRANSPORT:
		case HIGHGUID_TRANSPORT:
		default:
			break;
	}

	return nullptr;
}

void GridMap::AddToActive(WorldObject* obj)
{
	m_activeNonPlayers.insert(obj);
}

void GridMap::RemoveFromActive(WorldObject* obj)
{
	// 如果Map::Update正在处理活动对象，且要删除的对象正好是下一个待处理的活动对象，那么需要手动调整迭代器
	if (m_activeNonPlayersIter != m_activeNonPlayers.end())
	{
		ActiveNonPlayers::iterator itr = m_activeNonPlayers.find(obj);
		if (itr == m_activeNonPlayersIter)
		{
			++m_activeNonPlayersIter;
		}
		m_activeNonPlayers.erase(itr);
	}
	else
	{
		m_activeNonPlayers.erase(obj);
	}
}

void GridMap::RemoveAllObjectsInRemoveList()
{
	if (i_objectsToRemove.empty())
	{
		return;
	}

	while (!i_objectsToRemove.empty())
	{
		WorldObject* obj = *i_objectsToRemove.begin();
		i_objectsToRemove.erase(i_objectsToRemove.begin());

		switch (obj->GetTypeId())
		{
			case TYPEID_CORPSE:
				Remove((Corpse*)obj, true);
				break;
			case TYPEID_DYNAMICOBJECT:
				Remove((DynamicObject*)obj, true);
				break;
			case TYPEID_GAMEOBJECT:
				Remove((GameObject*)obj, true);
				break;
			case TYPEID_UNIT:
				Remove((Creature*)obj, true);
				break;
			default:
				sLog.outError("Non-grid object (TypeId: %u) in grid object removing list, ignored.", obj->GetTypeId());
				break;
		}
	}
}

GridPair GridMap::ComputeGridPair(CoordUnit x, CoordUnit y) const
{
	return GridPair(GridUnit((float(x) - m_gminfo.x) / SIZE_OF_GRID), GridUnit((float(y) - m_gminfo.y) / SIZE_OF_GRID));
}

bool GridMap::IsGridPosValid(const GridPair& gridPair) const
{
	return gridPair.gridx < m_gminfo.width && gridPair.gridy < m_gminfo.height;
}

uint32 GridMap::GetGridID(GridUnit x, GridUnit y) const
{
	return x + y * m_gminfo.width;
}

bool GridMap::IsGridMarked(GridUnit x, GridUnit y) const
{
	return m_bits.test(GetGridID(x, y));
}

void GridMap::MarkGrid(GridUnit x, GridUnit y)
{
	m_bits.set(GetGridID(x, y));
}

void GridMap::ResetMarkedGrids()
{
	m_bits.reset();
}

