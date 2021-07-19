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

#ifndef MANGOS_CELL_H
#define MANGOS_CELL_H

#include "GridSystem/TypeContainer.h"
#include "GridSystem/TypeContainerVisitor.h"
#include "GridDefines.h"

class Map;
class WorldObject;

namespace MaNGOS
{
	GridArea ComputeGridArea(uint32 mapId, CoordUnit x, CoordUnit y, int8 radius);

	template<class T, class CONTAINER> void Visit(TypeContainerVisitor<T, CONTAINER>& visitor, Map* pMap, CoordUnit x, CoordUnit y, int8 radius);
	template<class T, class CONTAINER> void Visit(TypeContainerVisitor<T, CONTAINER>& visitor, Map* pMap, const WorldObject* obj, int8 radius);

	template<class T> void VisitGridObjects(const WorldObject* obj, T& visitor, int8 radius);
	template<class T> void VisitWorldObjects(const WorldObject* obj, T& visitor, int8 radius);
	template<class T> void VisitAllObjects(const WorldObject* obj, T& visitor, int8 radius);

	template<class T> void VisitGridObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius);
	template<class T> void VisitWorldObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius);
	template<class T> void VisitAllObjects(CoordUnit x, CoordUnit y, Map* pMap, T& visitor, int8 radius);
}


#endif //MANGOS_CELL_H
