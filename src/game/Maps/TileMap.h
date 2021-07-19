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

#ifndef MANGOS_TILEMAP_H
#define MANGOS_TILEMAP_H

#include <bitset>
#include <list>
#include "Platform/Define.h"
#include "Utilities/Singleton.h"
#include "Entities/Object.h"

struct MapEntry;

class MANGOS_DLL_SPEC TileMap
{
public:
	TileMap(uint32 mapId);

	/**
	 * 碰撞信息
	 */

	uint16 GetTile(CoordUnit x, CoordUnit y);    // 获取世界坐标对应瓦片信息
	uint16 GetRawTile(CoordUnit x, CoordUnit y); // 获取世界坐标对应瓦片原始信息(地图原始信息，不包含站在瓦片上的玩家和生物)

	bool IsPassable(CoordUnit x, CoordUnit y);                       // 计算世界坐标四周是否允许通过
	bool IsPassable(CoordUnit x, CoordUnit y, Towards towards);      // 计算世界坐标指定朝向是否允许通过
	bool IsArrowPassable(CoordUnit x, CoordUnit y);                  // 计算世界坐标四周是否允许箭矢通过
	bool IsArrowPassable(CoordUnit x, CoordUnit y, Towards towards); // 计算世界坐标指定朝向是否允许箭矢通过
	void MarkPassable(CoordUnit x, CoordUnit y, bool ok);            // 设置世界坐标是否允许通过

	bool IsSafetyZone(CoordUnit x, CoordUnit y);  // 判断世界坐标处是否为安全区域
	bool IsCombatZone(CoordUnit x, CoordUnit y);  // 判断世界坐标处是否为战斗区域
	bool IsNormalZone(CoordUnit x, CoordUnit y);  // 判断世界坐标处是否为一般区域
	bool IsFishingZone(CoordUnit x, CoordUnit y); // 判断世界坐标处是否为垂钓区域
	bool IsExistDoor(CoordUnit x, CoordUnit y);   // 判断世界坐标处是否存在门

	bool IsInLineOfSight(CoordUnit srcX, CoordUnit srcY, CoordUnit destX, CoordUnit destY); // 判断两者之间是否存在视线遮挡(例如在蚂蚁洞穴里面中间是否被隔开了)

	/**
	 * 帮助函数
	 */

	uint32 GetId(void) const;     // 地图编号
	std::string GetName() const;  // 地图名称
	bool Instanceable() const;    // 判断地图是否能多次实例化(副本地图)
	bool IsEscapable() const;     // 判断地图是否允许回城
	bool IsMarkable() const;      // 判断地图是否允许记忆坐标
	bool IsPainwand() const;      // 判断地图是否允许复活
	bool IsPenalty() const;       // 判断地图死亡是否受到惩罚
	bool IsRecallPets() const;    // 判断地图是否允许召唤宠物
	bool IsResurrection() const;  // 判断地图是否允许复活
	bool IsTakePets() const;      // 判断地图是否允许携带宠物
	bool IsTeleportable() const;  // 判断地图是否允许随机传送
	bool IsUnderWater() const;    // 判断地图是否位于水下
	bool IsUsableItem() const;    // 判断地图是否允许使用道具
	bool IsUsableSkill() const;   // 判断地图是否允许使用魔法

	bool IsCoordPosValid(CoordUnit x, CoordUnit y) const; // 判断坐标是否合法

protected:
	/**
	 * 获取瓦片信息
	 */
	uint32 GetTileID(CoordUnit x, CoordUnit y) const;    // 计算瓦片索引

	void   tile(CoordUnit x, CoordUnit y, uint16 value); // 设置世界坐标对应瓦片信息
	uint16 tile(CoordUnit x, CoordUnit y);               // 获取世界坐标对应瓦片信息
	uint16 rawTile(CoordUnit x, CoordUnit y);            // 获取世界坐标对应瓦片原始信息(瓦片地图原始信息，不包含玩家和生物)

protected:
	const MapEntry* m_proto;     // 地图信息
	TileMapInfo m_tminfo;        // 地图瓦片信息
};



#endif
