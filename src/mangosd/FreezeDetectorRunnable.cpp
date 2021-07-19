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

/// \addtogroup mangosd
/// @{
/// \file

#include "Common.h"
#include "Log.h"
#include "Util.h"
#include "Config/Config.h"
#include "World.h"
#include "FreezeDetectorRunnable.h"

FreezeDetectorRunnable::FreezeDetectorRunnable(uint32 t) : _delaytime(t)
{
}

/// %Thread start
void FreezeDetectorRunnable::operator()()
{
    if (!_delaytime)
        return;
    sLog.outString("Starting up anti-freeze thread (%u seconds max stuck time)...", _delaytime / 1000);
    m_loops = 0;
    w_loops = 0;
    m_lastchange = 0;
    w_lastchange = 0;
    while (!World::IsStopped())
    {
        boost::this_thread::sleep_for(boost::chrono::microseconds{1000});

        uint32 curtime = WorldTimer::getMSTime();
        // DEBUG_LOG("anti-freeze: time=%u, counters=[%u; %u]",curtime,Master::m_masterLoopCounter,World::m_worldLoopCounter);

        // normal work
        if (w_loops != World::m_worldLoopCounter)
        {
            w_lastchange = curtime;
            w_loops = World::m_worldLoopCounter;
        }
            // possible freeze
        else if (WorldTimer::getMSTimeDiff(w_lastchange, curtime) > _delaytime)
        {
            sLog.outError("World Thread hangs, kicking out server!");
            *((uint32 volatile*)NULL) = 0;          // bang crash
        }
    }
    sLog.outString("Anti-freeze thread exiting without problems.");
}
