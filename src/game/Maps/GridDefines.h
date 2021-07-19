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

#ifndef MANGOS_GRIDDEFINES_H
#define MANGOS_GRIDDEFINES_H

#include <cmath>
#include "GridSystem/Grid.h"
#include "GridSystem/GridReference.h"
#include "GridSystem/GridRefManager.h"
#include "Common.h"

class Corpse;
class Creature;
class DynamicObject;
class GameObject;
class WorldObject;
class Pet;
class Player;
class Camera;

#define SIZE_OF_SCREEN          15 // 屏幕大小
#define SIZE_OF_GRID            16 // 天堂所有地图的长宽的最大公约数

#define MIN_MAP_UPDATE_DELAY    50

typedef TYPELIST_4(Player, Creature/*pets*/, Corpse/*resurrectable*/, Camera)           AllWorldObjectTypes;
typedef TYPELIST_4(GameObject, Creature/*except pets*/, DynamicObject, Corpse/*Bones*/) AllGridObjectTypes;
typedef TYPELIST_4(Creature, Pet, GameObject, DynamicObject)                            AllMapStoredObjectTypes;

typedef GridRefManager<Camera>          CameraMapType;
typedef GridRefManager<Corpse>          CorpseMapType;
typedef GridRefManager<Creature>        CreatureMapType;
typedef GridRefManager<DynamicObject>   DynamicObjectMapType;
typedef GridRefManager<GameObject>      GameObjectMapType;
typedef GridRefManager<Player>          PlayerMapType;

typedef Grid<Player, AllWorldObjectTypes, AllGridObjectTypes> GridType;

typedef TypeMapContainer<AllGridObjectTypes> GridTypeMapContainer;
typedef TypeMapContainer<AllWorldObjectTypes> WorldTypeMapContainer;

typedef uint8 GridUnit;

// 网格地图信息
struct GridMapInfo
{
	CoordUnit x, y;
	GridUnit width, height;
	bool ok;
};
extern std::map<uint32, GridMapInfo> GRID_MAP_INFOS;

// 瓦片地图信息
struct TileMapInfo
{
	CoordUnit x, y;
	CoordUnit width, height;
	std::vector<uint8> tiles;
	bool ok;
};
extern std::map<uint32, TileMapInfo> TILE_MAP_INFOS;

struct MANGOS_DLL_DECL GridPair
{
	GridPair(GridUnit x = 0, GridUnit y = 0) : gridx(x), gridy(y) {}
	GridPair(const GridPair& obj) : gridx(obj.gridx), gridy(obj.gridy) {}

	bool operator==(const GridPair& obj) const { return (obj.gridx == gridx && obj.gridy == gridy); }
	bool operator!=(const GridPair& obj) const { return !operator==(obj); }
	GridPair& operator=(const GridPair& obj) { gridx = obj.gridx; gridy = obj.gridy; return *this; }
	void operator<<(uint8 offset) { gridx > offset ? gridx -= offset : gridx = 0; }
	void operator>>(uint8 offset) { gridx += offset; }
	void operator-=(uint8 offset) { gridy > offset ? gridy -= offset : gridy = 0; }
	void operator+=(uint8 offset) { gridy += offset; }

	GridUnit gridx, gridy;
};

struct MANGOS_DLL_DECL GridArea
{
	GridArea() {}
	GridArea(GridPair& low, GridPair& high) : gridlow(low), gridhigh(high) {}

	bool operator!() const { return gridlow == gridhigh; }
	void ResizeBorders(const GridPair& low, const GridPair& high) { gridlow = low; gridhigh = high; }

	GridPair gridlow, gridhigh;
};

#endif //MANGOS_GRIDDEFINES_H
