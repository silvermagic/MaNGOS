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


#include <time.h>
#include "Utilities/Timer.h"
#include "Util.h"
#include "GameClock.h"

const time_t BASE_TIME_IN_MILLIS_REAL = 1057233600000L;

namespace GameClock
{
	const time_t StartTime = time(nullptr);

	time_t GameTime = 0;
	uint32 GameMSTime = 0;

	std::chrono::system_clock::time_point GameTimeSystemPoint = std::chrono::system_clock::time_point::min();
	std::chrono::steady_clock::time_point GameTimeSteadyPoint = std::chrono::steady_clock::time_point::min();

	tm DateTime;

	time_t GameWorldTime = 0;

	time_t GetStartTime()
	{
		return StartTime;
	}

	time_t GetGameTime()
	{
		return GameTime;
	}

	uint32 GetGameTimeMS()
	{
		return GameMSTime;
	}

	std::chrono::system_clock::time_point GetGameTimeSystemPoint()
	{
		return GameTimeSystemPoint;
	}

	std::chrono::steady_clock::time_point GetGameTimeSteadyPoint()
	{
		return GameTimeSteadyPoint;
	}

	uint32 GetUptime()
	{
		return uint32(GameTime - StartTime);
	}

	tm const* GetDateAndTime()
	{
		return &DateTime;
	}

	void UpdateGameTimers()
	{
		GameTime = time(nullptr);
		GameMSTime = WorldTimer::getMSTime();
		GameWorldTime = (GameTime - BASE_TIME_IN_MILLIS_REAL) * 6 / 1000;
		GameWorldTime -= (GameWorldTime % 3);
		GameTimeSystemPoint = std::chrono::system_clock::now();
		GameTimeSteadyPoint = std::chrono::steady_clock::now();
#if PLATFORM == PLATFORM_WINDOWS
		localtime_s(&DateTime, &GameTime);
#else
		localtime_r(&GameTime, &DateTime);
#endif
	}

	time_t GetGameWorldTime()
	{
		return GameWorldTime;
	}
}



