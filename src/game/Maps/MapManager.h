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

#ifndef MANGOS_MAPMANAGER_H
#define MANGOS_MAPMANAGER_H

#include <boost/thread/recursive_mutex.hpp>
#include "Platform/Define.h"
#include "Utilities/Singleton.h"
#include "Map.h"

struct MANGOS_DLL_DECL MapID
{
    explicit MapID(uint32 id) : nMapId(id), nInstanceId(0) {}
    MapID(uint32 id, uint32 instid) : nMapId(id), nInstanceId(instid) {}

    bool operator<(const MapID& val) const
    {
        if (nMapId == val.nMapId)
            return nInstanceId < val.nInstanceId;

        return nMapId < val.nMapId;
    }

    bool operator==(const MapID& val) const { return nMapId == val.nMapId && nInstanceId == val.nInstanceId; }

    uint32 nMapId;
    uint32 nInstanceId;
};

class MANGOS_DLL_DECL MapManager : public MaNGOS::Singleton<MapManager, MaNGOS::ClassLevelLockable<MapManager, boost::recursive_mutex> >
{
	friend class MaNGOS::OperatorNew<MapManager>;
protected:
	MapManager();
	~MapManager();
	MapManager(const MapManager&);
	MapManager& operator=(const MapManager&);

	typedef boost::recursive_mutex LOCK_TYPE;
	typedef boost::lock_guard<LOCK_TYPE> LOCK_TYPE_GUARD;
	typedef MaNGOS::ClassLevelLockable<MapManager, boost::recursive_mutex>::Lock Guard;
	typedef std::map<MapID, Map*> MapMapType;

public:
    Map* CreateMap(uint32, const WorldObject* obj);
    Map* FindMap(uint32 mapid, uint32 instanceId = 0) const;

    void Update(uint32);

	void LoadTerrains();

    void RemoveAllObjectsInRemoveList();

    const MapMapType& Maps() const { return m_maps; }

protected:
    Map* CreateInstance(uint32 id, Player* player);

protected:
    MapMapType m_maps;
    IntervalTimer m_timer;
};

#define sMapMgr MapManager::Instance()

#endif
