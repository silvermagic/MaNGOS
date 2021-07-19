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

#ifndef MANGOS_OBJECTACCESSOR_H
#define MANGOS_OBJECTACCESSOR_H

#include <set>
#include <list>
#include <boost/thread/shared_mutex.hpp>
#include "Common.h"
#include "Platform/Define.h"
#include "Utilities/Singleton.h"
#include "Utilities/ThreadingModel.h"
#include "Utilities/UnorderedMapSet.h"
#include "Entities/Corpse.h"
#include "Entities/Object.h"
#include "Entities/Player.h"
#include "Entities/UpdateData.h"
#include "Maps/GridDefines.h"

class Unit;
class WorldObject;
class Map;

template <class T>
class HashMapHolder
{
public:
    typedef UNORDERED_MAP<ObjectGuid, T*> MapType;
    typedef boost::shared_mutex LockType;
    typedef boost::shared_lock<LockType> ReadGuard;
    typedef boost::unique_lock<LockType> WriteGuard;

    static void Insert(T* o)
    {
        WriteGuard guard(m_lock);
        m_objectMap[o->GetObjectGuid()] = o;
    }

    static void Remove(T* o)
    {
        WriteGuard guard(m_lock);
        m_objectMap.erase(o->GetObjectGuid());
    }

    static T* Find(ObjectGuid guid)
    {
        ReadGuard guard(m_lock);
        auto iter = m_objectMap.find(guid);
        return (iter != m_objectMap.end()) ? iter->second : nullptr;
    }

    static MapType& GetContainer() { return m_objectMap; }

    static LockType& GetLock() { return m_lock; }

private:
    HashMapHolder() {}

    static LockType m_lock;
    static MapType  m_objectMap;
};

class MANGOS_DLL_DECL ObjectAccessor
{
    friend class MaNGOS::OperatorNew<ObjectAccessor>;
protected:
    ObjectAccessor();
    ~ObjectAccessor();
    ObjectAccessor(const ObjectAccessor&) = delete;
    ObjectAccessor& operator=(const ObjectAccessor&) = delete;

public:
    typedef UNORDERED_MAP<ObjectGuid, Corpse*> Player2CorpsesMapType;

    // Search player at any map in world and other objects at same map with `obj`
    // Note: recommended use Map::GetUnit version if player also expected at same map only

    ///- 找到
    static Unit* GetUnit(const WorldObject& obj, ObjectGuid guid);

    // Player access
    static Player* FindPlayer(ObjectGuid guid, bool inWorld = true);// if need player at specific map better use Map::GetPlayer
    static Player* FindPlayerByName(const char* name);
    static void KickPlayer(ObjectGuid guid);

    HashMapHolder<Player>::MapType& GetPlayers()
    {
        return HashMapHolder<Player>::GetContainer();
    }

    void SaveAllPlayers();

    // Corpse access
    Corpse* GetCorpseForPlayerGUID(ObjectGuid guid);
    static Corpse* GetCorpseInMap(ObjectGuid guid, uint32 mapid);
    void RemoveCorpse(Corpse* corpse);
    void AddCorpse(Corpse* corpse);
    void AddCorpsesToGrid(GridPair const& gridpair, GridType& grid, Map* map);
    Corpse* ConvertCorpseForPlayer(ObjectGuid player_guid, bool insignia = false);
    void RemoveOldCorpses();

    /// 管理游戏世界中的玩家和尸体对象，方便使用GUID进行检索，仅允许在Player/Corpse的AddToWorld/RemoveFromWorld方法中使用
    void AddObject(Corpse* object) { HashMapHolder<Corpse>::Insert(object); }
    void AddObject(Player* object) { HashMapHolder<Player>::Insert(object); }
    void RemoveObject(Corpse* object) { HashMapHolder<Corpse>::Remove(object); }
    void RemoveObject(Player* object) { HashMapHolder<Player>::Remove(object); }

protected:
    Player2CorpsesMapType   m_player2corpse;

    typedef boost::mutex LockType;
    typedef MaNGOS::GeneralLock<LockType> Guard;
    LockType m_playerGuard;
    LockType m_corpseGuard;
};

#define sObjectAccessor MaNGOS::Singleton<ObjectAccessor, MaNGOS::ClassLevelLockable<ObjectAccessor, boost::mutex> >::Instance()

#endif
