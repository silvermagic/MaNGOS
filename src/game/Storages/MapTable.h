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

#ifndef MANGOS_MAPS_H_
#define MANGOS_MAPS_H_

#include "Common.h"
#include "SharedDefines.h"

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

typedef char const* DBString;

struct MapEntry // 地图信息
{
	uint32 ID;
	DBString name;
	uint32 start_x;
	uint32 end_x;
	uint32 start_y;
	uint32 end_y;
	float monster_amount;    // 怪物数量
	float drop_rate;         // 掉率
	bool isUnderWater;       // 是否在水下
	bool isMarkable;         // 是否允许记录记忆坐标
	bool isTeleportable;     // 是否允许传送(随机顺飞)
	bool isEscapable;        // 是否允许回城
	bool isResurrection;     // 是否允许复活
	bool isPainwand;
	bool isPenalty;
	bool isTakePets;         // 是否允许携带宠物(狗)
	bool isRecallPets;       // 是否允许召唤宠物
	bool isUsableItem;       // 是否允许使用道具
	bool isUsableSkill;      // 是否允许使用技能
	bool isArena;

	bool Instanceable() const { return false; }
	CoordUnit GetHeight() const { return std::max(CoordUnit(0), CoordUnit(end_x - start_x + 1)); }
	CoordUnit GetWidth() const { return std::max(CoordUnit(0), CoordUnit(end_y - start_y + 1)); }

	bool IsInMap(CoordUnit x, CoordUnit y) const
	{
		if (ID == 4 && (x < 32520 || y < 32070 || (x < 33950 && y < 32190)))
			return false;

		return (start_x <= x && x <= end_x) && (start_y <= y && y <= end_y);
	}
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif //MANGOS_MAPS_H_
