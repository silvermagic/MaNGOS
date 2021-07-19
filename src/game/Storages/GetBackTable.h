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

#ifndef MANGOS_GETBACK_H_
#define MANGOS_GETBACK_H_

#include "Common.h"
#include "SharedDefines.h"

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct GetBackEntry // 玩家使用回城卷后的重定位坐标
{
	uint32 start_x;              // 子区域范围
	uint32 end_x;
	uint32 start_y;
	uint32 end_y;
	uint32 areaID;               // 子区域标识(例如在大陆MAP_ID=4，不同地方使用回城回到的地方也不同，所以需要划分成子区域)
	uint32 coords[3][2];         // 随机返回坐标，随机选取三个中的一个
	uint32 mapID;                // 地图标识
	uint32 homeTownID;           // 村庄标识
	uint32 elfHomeTownID;        // 妖精村庄(妖精森林)
	uint32 darkElfHomeTownID;    // 黑暗精灵村庄(沉默洞穴)
	bool isScrollEscape;         // 是否允许使用回家卷
	// DBString note;

	bool IsSpecifyArea() const { return start_x != 0 && end_x != 0 && start_y != 0 && end_y != 0; }
	void GetBackPosition(uint8 i, CoordUnit &x, CoordUnit &y) const { i %=3; x = coords[i][0]; y = coords[i][1]; }
};

struct GetBackRestartEntry // 玩家重新登入后的重定位坐标
{
	uint32 ID;        // 子区域标识
	// DBString note;
	uint32 coords[2]; // 返回坐标
	uint32 mapID;     // 地图标识
};

struct TownEntry
{
	uint32 ID; // 村庄ID
	// DBString name; // 村庄名称
	uint32 village_head; // 村长ID
	// DBString village_head_name; // 村长名称
	int32 tax_rate; // 税率
	int32 tax_rate_reserved;
	int32 sales_money; // 村庄销售额
	int32 sales_money_yesterday;
	int32 town_tax; // 税收
	int32 town_fix_tax;
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif //MANGOS_GETBACK_H_
