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

#include <cmath>
#include "Utilities/SingletonImpl.h"
#include "ObjectAccessor.h"

#define CLASS_LOCK MaNGOS::ClassLevelLockable<ObjectAccessor, boost::mutex>
INSTANTIATE_SINGLETON_2(ObjectAccessor, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(ObjectAccessor, boost::mutex);

ObjectAccessor::ObjectAccessor() {}
ObjectAccessor::~ObjectAccessor() {}

Unit* ObjectAccessor::GetUnit(WorldObject const& u, ObjectGuid guid)
{
    return NULL;
}

Corpse* ObjectAccessor::GetCorpseInMap(ObjectGuid guid, uint32 mapid)
{
    return NULL;
}

Player* ObjectAccessor::FindPlayer(ObjectGuid guid, bool inWorld /*= true*/)
{
    return NULL;
}

Player* ObjectAccessor::FindPlayerByName(const char* name)
{
    return NULL;
}

void ObjectAccessor::SaveAllPlayers()
{
}

void ObjectAccessor::KickPlayer(ObjectGuid guid)
{
}

Corpse* ObjectAccessor::GetCorpseForPlayerGUID(ObjectGuid guid)
{
    return NULL;
}

void ObjectAccessor::RemoveCorpse(Corpse* corpse)
{
}

void ObjectAccessor::AddCorpse(Corpse* corpse)
{
}

void ObjectAccessor::AddCorpsesToGrid(GridPair const& gridpair, GridType& grid, Map* map)
{
}

Corpse* ObjectAccessor::ConvertCorpseForPlayer(ObjectGuid player_guid, bool insignia)
{
    return NULL;
}

void ObjectAccessor::RemoveOldCorpses()
{
}

/// Define the static member of HashMapHolder

template <class T> typename HashMapHolder<T>::MapType HashMapHolder<T>::m_objectMap;
template <class T> typename HashMapHolder<T>::LockType HashMapHolder<T>::m_lock;

/// Global definitions for the hashmap storage

template class HashMapHolder<Player>;
template class HashMapHolder<Corpse>;
