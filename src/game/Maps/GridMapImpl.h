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

#ifndef MANGOS_GRIDMAPIMPL_H
#define MANGOS_GRIDMAPIMPL_H

#include "GridMap.h"

template<class T>
inline void GridMap::Remove(T* obj, bool remove)
{
	GridPair gridPair = ComputeGridPair(obj->GetPositionX(), obj->GetPositionY());
	if (!IsGridPosValid(gridPair))
	{
		sLog.outError("GridMap::Remove: Object (GUID: %u TypeId:%u) have invalid coordinates X:%f Y:%f grid [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), gridPair.gridx, gridPair.gridy);
		return;
	}

	DEBUG_LOG("Remove object (GUID: %u TypeId:%u) from grid[%u,%u]", obj->GetGUIDLow(), obj->GetTypeId(), gridPair.gridx, gridPair.gridy);

	if (obj->isActiveObject())
	{
		RemoveFromActive(obj);
	}

	if (remove)
	{
		obj->CleanupsBeforeDelete();
	}
	else
	{
		obj->RemoveFromWorld();
	}

	UpdateObjectVisibility(obj, gridPair);
	RemoveFromGrid(obj, gridPair);

	obj->ResetMap();
	if (remove)
	{
		delete obj;
	}
}

template<class T>
inline void GridMap::AddToGrid(T* obj, const GridPair& gridPair)
{
	m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].template AddGridObject<T>(obj);
}

template<>
inline void GridMap::AddToGrid(Player* obj, const GridPair& gridPair)
{
	m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].AddWorldObject(obj);
}

template<>
inline void GridMap::AddToGrid(Corpse* obj, const GridPair& gridPair)
{
	if (obj->GetType() != CORPSE_BONES)
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].AddWorldObject(obj);
	}
	else
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].AddGridObject(obj);
	}
}

template<>
inline void GridMap::AddToGrid(Creature* obj, const GridPair& gridPair)
{
	if (obj->IsPet())
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].AddWorldObject<Creature>(obj);
		obj->SetCurrentGrid(gridPair);
	}
	else
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].AddGridObject<Creature>(obj);
		obj->SetCurrentGrid(gridPair);
	}
}

template<class T>
inline void GridMap::RemoveFromGrid(T* obj, const GridPair& gridPair)
{
	m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].template RemoveGridObject<T>(obj);
}

template<>
inline void GridMap::RemoveFromGrid(Player* obj, const GridPair& gridPair)
{
	m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].RemoveWorldObject(obj);
}

template<>
inline void GridMap::RemoveFromGrid(Corpse* obj, const GridPair& gridPair)
{
	if (obj->GetType() != CORPSE_BONES)
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].RemoveWorldObject(obj);
	}
	else
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].RemoveGridObject(obj);
	}
}

template<>
inline void GridMap::RemoveFromGrid(Creature* obj, const GridPair& gridPair)
{
	if (obj->IsPet())
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].RemoveWorldObject<Creature>(obj);
	}
	else
	{
		m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].RemoveGridObject<Creature>(obj);
	}
}

#endif //MANGOS_GRIDMAPIMPL_H
