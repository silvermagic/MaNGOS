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
#ifndef MANGOS_GAMECLOCK_H
#define MANGOS_GAMECLOCK_H

#include <chrono>
#include "Common.h"

namespace GameClock
{
	/// 服务器启动时间
	time_t GetStartTime();

	/// 服务器当前时间单位秒
	time_t GetGameTime();

	/// 自服务启动已经经过的毫秒数
	uint32 GetGameTimeMS();

	/// 当前系统时钟时间点
	std::chrono::system_clock::time_point GetGameTimeSystemPoint();

	/// 当前稳定时钟的时间点
	std::chrono::steady_clock::time_point GetGameTimeSteadyPoint();

	/// 服务器启动后，游戏世界的持续时间
	uint32 GetUptime();

	/// 服务器当前日期
	const tm* GetDateAndTime();

	/// 更新游戏时间
	void UpdateGameTimers();

	/// 游戏世界当前时间单位秒，游戏世界时间的时间比真实世界时间快6倍，所以它的时间不是从1970年01月01日0:00:00开始算的，
	/// 而一般由开服时间开始计算，这边暂时使用2021年01月01日0:00:00
	time_t GetGameWorldTime();
}

#endif //MANGOS_GAMECLOCK_H
