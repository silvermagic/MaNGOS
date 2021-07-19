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

#ifndef MANGOS_GRIDIMPL_H
#define MANGOS_GRIDIMPL_H

#include "Cell.h"
#include "Map.h"

namespace MaNGOS
{
	inline GridArea ComputeGridArea(uint32 mapId, CoordUnit x, CoordUnit y, int8 radius)
	{
		const GridMapInfo& info = GRID_MAP_INFOS[mapId];
		MANGOS_ASSERT(info.ok);

		if (radius == 0)
		{
			GridUnit gridx = GridUnit((float(x) - info.x) / SIZE_OF_GRID);
			GridUnit gridy = GridUnit((float(y) - info.y) / SIZE_OF_GRID);
			if (gridx >= info.width)
			{
				gridx = info.width - 1;
			}
			if (gridy >= info.height)
			{
				gridy = info.height - 1;
			}

			GridPair gridlow(gridx, gridy);
			GridPair gridhigh(gridx, gridy);
			return GridArea(gridlow, gridhigh);
		}
		else
		{
			if ((x - radius) < info.x)
			{
				x = info.x + radius;
			}
			if ((y - radius) < info.y)
			{
				y = info.y + radius;
			}
			GridPair gridlow(GridUnit((float(x - radius) - info.x) / SIZE_OF_GRID), GridUnit((float(y - radius) - info.y) / SIZE_OF_GRID));
			if (gridlow.gridx >= info.width)
			{
				gridlow.gridx = info.width - 1;
			}
			if (gridlow.gridy >= info.height)
			{
				gridlow.gridy = info.height - 1;
			}

			GridPair gridhigh(GridUnit((float(x + radius) - info.x) / SIZE_OF_GRID), GridUnit((float(y + radius) - info.y) / SIZE_OF_GRID));
			if (gridhigh.gridx >= info.width)
			{
				gridhigh.gridx = info.width - 1;
			}
			if (gridhigh.gridy >= info.height)
			{
				gridhigh.gridy = info.height - 1;
			}

			return GridArea(gridlow, gridhigh);
		}
	}

	template<class T, class CONTAINER>
	inline void Visit(TypeContainerVisitor<T, CONTAINER>& visitor, Map* pMap, CoordUnit x, CoordUnit y, int8 radius)
	{
		MANGOS_ASSERT(pMap);

		GridPair gridPair = pMap->ComputeGridPair(x, y);
		if (radius <= 0)
		{
			pMap->Visit(gridPair, visitor);
			return;
		}

		// 限制搜索范围
		if (radius > 2 * SIZE_OF_GRID)
		{
			radius = 2 * SIZE_OF_GRID;
		}

		// 计算搜索区域
		GridArea area = ComputeGridArea(pMap->GetId(), x, y, radius);
		if (!area)
		{
			pMap->Visit(gridPair, visitor);
			return;
		}

		// 遍历搜索区域内的网格
		for (GridUnit x = area.gridlow.gridx; x <= area.gridhigh.gridx; ++x)
		{
			for (GridUnit y = area.gridlow.gridy; y <= area.gridhigh.gridy; ++y)
			{
				pMap->Visit(x, y, visitor);
			}
		}
	}

	template<class T, class CONTAINER>
	inline void Visit(TypeContainerVisitor<T, CONTAINER>& visitor, Map* pMap, const WorldObject* obj, int8 radius)
	{
		MANGOS_ASSERT(obj);
		Visit(visitor, pMap, obj->GetPositionX(), obj->GetPositionY(), radius);
	}

	template<class T>
	inline void VisitGridObjects(const WorldObject* obj, T& visitor, int8 radius)
	{
		MANGOS_ASSERT(obj);
		TypeContainerVisitor<T, GridTypeMapContainer > gnotifier(visitor);
		Visit(gnotifier, obj->GetMap(), obj, radius);
	}

	template<class T>
	inline void VisitWorldObjects(const WorldObject* obj, T& visitor, int8 radius)
	{
		MANGOS_ASSERT(obj);
		TypeContainerVisitor<T, WorldTypeMapContainer > wnotifier(visitor);
		Visit(wnotifier, obj->GetMap(), obj, radius);
	}

	template<class T>
	inline void VisitAllObjects(const WorldObject* obj, T& visitor, int8 radius)
	{
		MANGOS_ASSERT(obj);
		TypeContainerVisitor<T, GridTypeMapContainer > gnotifier(visitor);
		TypeContainerVisitor<T, WorldTypeMapContainer > wnotifier(visitor);
		Visit(gnotifier, obj->GetMap(), obj, radius);
		Visit(wnotifier, obj->GetMap(), obj, radius);
	}

	template<class T>
	inline void VisitGridObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius)
	{
		TypeContainerVisitor<T, GridTypeMapContainer > gnotifier(visitor);
		Visit(gnotifier, pMap, x, y, radius);
	}

	template<class T>
	inline void VisitWorldObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius)
	{
		TypeContainerVisitor<T, WorldTypeMapContainer > wnotifier(visitor);
		Visit(wnotifier, pMap, x, y, radius);
	}

	template<class T>
	inline void VisitAllObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius)
	{
		TypeContainerVisitor<T, GridTypeMapContainer > gnotifier(visitor);
		TypeContainerVisitor<T, WorldTypeMapContainer > wnotifier(visitor);
		Visit(gnotifier, pMap, x, y, radius);
		Visit(wnotifier, pMap, x, y, radius);
	}
}

#endif //MANGOS_GRIDIMPL_H