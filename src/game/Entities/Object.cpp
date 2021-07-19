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

#include <boost/algorithm/clamp.hpp>
#include "Maps/CellImpl.h"
#include "Maps/GridNotifiersImpl.h"
#include "Maps/Map.h"
#include "Log.h"
#include "Util.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldPacket.h"
#include "Object.h"
#include "ObjectGuid.h"
#include "UpdateData.h"
#include "UpdateMask.h"

Object::Object()
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = 0;
    m_uint32Values_mirror = 0;
    m_valuesCount       = 0;

    m_inWorld           = false;
    m_objectUpdated     = false;
}

Object::~Object()
{
    if (IsInWorld())
    {
        ///- Do NOT call RemoveFromWorld here, if the object is a player it will crash
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still in world!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    if (m_objectUpdated)
    {
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still have updated status!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    delete[] m_uint32Values;
    delete[] m_uint32Values_mirror;
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[ m_valuesCount ];
    memset(m_uint32Values, 0, m_valuesCount * sizeof(uint32));

    m_uint32Values_mirror = new uint32[ m_valuesCount ];
    memset(m_uint32Values_mirror, 0, m_valuesCount * sizeof(uint32));

    m_objectUpdated = false;
}

void Object::_Create(uint32 guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values)
        _InitValues();

    ObjectGuid guid = ObjectGuid(guidhigh, entry, guidlow);
    SetGuidValue(OBJECT_FIELD_GUID, guid);
    SetUInt32Value(OBJECT_FIELD_TYPE, m_objectType);
}

/*********************************************************/
/***                 GAME GRID FRAMEWORK               ***/
/*********************************************************/

void Object::AddToClientUpdateList()
{
    sLog.outError("Unexpected call of Object::AddToClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::RemoveFromClientUpdateList()
{
    sLog.outError("Unexpected call of Object::RemoveFromClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::MarkForClientUpdate()
{
    if (m_inWorld)
    {
        if (!m_objectUpdated)
        {
            AddToClientUpdateList();
            m_objectUpdated = true;
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    if (m_uint32Values)
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (m_uint32Values_mirror[index] != m_uint32Values[index])
                m_uint32Values_mirror[index] = m_uint32Values[index];
        }
    }

    if (m_objectUpdated)
    {
        if (remove)
            RemoveFromClientUpdateList();
        m_objectUpdated = false;
    }
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData* /*data*/, Player* /*target*/) const
{
    sLog.outError("Unexpected call of Object::BuildCreateUpdateBlockForPlayer for object (TypeId: %u)", GetTypeId());
    MANGOS_ASSERT(false);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData* /*data*/, Player* /*target*/) const
{
    sLog.outError("Unexpected call of Object::BuildValuesUpdateBlockForPlayer for object (TypeId: %u)", GetTypeId());
    MANGOS_ASSERT(false);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData* /*data*/) const
{
    sLog.outError("Unexpected call of Object::BuildOutOfRangeUpdateBlock for object (TypeId: %u)", GetTypeId());
    MANGOS_ASSERT(false);
}

void Object::SendForcedObjectUpdate()
{
    sLog.outError("Unexpected call of Object::SendForcedObjectUpdate for object (TypeId: %u)", GetTypeId());
    MANGOS_ASSERT(false);
}

void Object::SendCreateUpdateToPlayer(Player* /*player*/)
{
    sLog.outError("Unexpected call of Object::SendCreateUpdateToPlayer for object (TypeId: %u)", GetTypeId());
    MANGOS_ASSERT(false);
}

/*********************************************************/
/***              ATTRIBUTE STORAGE SYSTEM             ***/
/*********************************************************/

bool Object::LoadValues(const char* data)
{
    if (!m_uint32Values) _InitValues();

    Tokens tokens = StrSplit(data, " ");

    if (tokens.size() != m_valuesCount)
        return false;

    Tokens::iterator iter;
    int index;
    for (iter = tokens.begin(), index = 0; index < m_valuesCount; ++iter, ++index)
    {
        m_uint32Values[index] = atol((*iter).c_str());
    }

    return true;
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[ index ] != value)
    {
        m_int32Values[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[ index ] != value)
    {
        m_uint32Values[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetUInt64Value(uint16 index, const uint64& value)
{
    MANGOS_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*) & (m_uint32Values[ index ])) != value)
    {
        m_uint32Values[ index ] = *((uint32*)&value);
        m_uint32Values[ index + 1 ] = *(((uint32*)&value) + 1);
        MarkForClientUpdate();
    }
}

void Object::SetFloatValue(uint16 index, float value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[ index ] != value)
    {
        m_floatValues[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        sLog.outError("Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[ index ] >> (offset * 16)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 16));
        MarkForClientUpdate();
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;
        MarkForClientUpdate();
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;
        MarkForClientUpdate();
    }
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[ index ] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[ index ] |= uint32(uint32(newFlag) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(oldFlag) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::SetShortFlag(uint16 index, bool highpart, uint16 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (!(uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (highpart ? 16 : 0));
        MarkForClientUpdate();
    }
}

void Object::RemoveShortFlag(uint16 index, bool highpart, uint16 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (highpart ? 16 : 0));
        MarkForClientUpdate();
    }
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    sLog.outError("Attempt %s nonexistent value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // ASSERT must fail after function call
    return false;
}

bool Object::PrintEntryError(char const* descr) const
{
    sLog.outError("Object Type %u, Entry %u (lowguid %u) with invalid call for %s", GetTypeId(), GetEntry(), GetObjectGuid().GetCounter(), descr);

    // always false for continue assert fail
    return false;
}



//////////////////////////////////////////////////////////////////////////

WorldObject::WorldObject() : m_currMap(NULL), m_isActiveObject(false)
{
}

void WorldObject::AddObjectToRemoveList()
{
    GetMap()->AddObjectToRemoveList(this);
}

void WorldObject::UpdateObjectVisibility()
{
    GridPair gridPair = GetMap()->ComputeGridPair(GetPositionX(), GetPositionY());;
    GetMap()->UpdateObjectVisibility(this, gridPair);
}

void WorldObject::UpdateVisibilityAndView()
{
    GetViewPoint().Call_UpdateVisibilityForOwner();
    UpdateObjectVisibility();
    GetViewPoint().Event_ViewPointVisibilityChanged();
}

void WorldObject::CleanupsBeforeDelete()
{
    RemoveFromWorld();
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh, uint32 /*phaseMask*/)
{
    Object::_Create(guidlow, 0, guidhigh);
}

Creature* WorldObject::SummonCreature(uint32 /*id*/, CoordUnit /*x*/, CoordUnit /*y*/, Towards /*ang*/, TempSummonType /*spwtype*/, uint32 /*despwtime*/, bool /*asActiveObject*/)
{
    return NULL;
}

bool WorldObject::PrintCoordinatesError(CoordUnit x, CoordUnit y, char const* descr) const
{
    sLog.outError("%s with invalid %s coordinates: mapid = %uu, x = %d, y = %d", GetGuidStr().c_str(), descr, GetMapId(), x, y);
    return false;
}

/*********************************************************/
/***               WORLDOBJECT FEATURES                ***/
/*********************************************************/

uint32 WorldObject::GetMapId() const
{
    return m_currMap->GetId();
}

uint32 WorldObject::GetInstID() const
{
    return m_currMap->GetInstID();
}

void WorldObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    m_phaseMask = newPhaseMask;

    if (update && IsInWorld())
        UpdateVisibilityAndView();
}

void WorldObject::SetMap(Map* map)
{
    MANGOS_ASSERT(map);
    m_currMap = map;
}

void WorldObject::SetActiveObjectState(bool active)
{
    m_isActiveObject = active;
}

/*********************************************************/
/***                 COORDINATE SYSTEM                 ***/
/*********************************************************/

void WorldObject::SetOrientation(Towards orientation)
{
    m_position.o = orientation;
}

bool WorldObject::IsPositionValid() const
{
    MANGOS_ASSERT(m_currMap)

    return m_currMap->IsCoordPosValid(m_position.x, m_position.y);
}

void WorldObject::Relocate(CoordUnit x, CoordUnit y, Towards orientation)
{
    m_position.x = x;
    m_position.y = y;
    m_position.o = orientation;
}

void WorldObject::Relocate(CoordUnit x, CoordUnit y)
{
    m_position.x = x;
    m_position.y = y;
}

void WorldObject::GetNearPoint(CoordUnit& x, CoordUnit& y, Towards angle, int32 distance2d) const
{
    x = m_position.x;
    y = m_position.y;
    switch (angle)
    {
        case SOUTH:
            y -= distance2d;
            break;
        case SOUTH_EAST:
            x += distance2d;
            y -= distance2d;
            break;
        case EAST:
            x += distance2d;
            break;
        case NORTH_EAST:
            x += distance2d;
            y += distance2d;
            break;
        case NORTH:
            y += distance2d;
            break;
        case NORTH_WEST:
            x -= distance2d;
            y += distance2d;
            break;
        case WEST:
            x -= distance2d;
            break;
        case SOUTH_WEST:
            x -= distance2d;
            y -= distance2d;
            break;
    }
}

float WorldObject::GetLineDistance(const WorldObject* target) const
{
    return GetLineDistance(target->GetPositionX(), target->GetPositionY());
}

float WorldObject::GetLineDistance(CoordUnit x, CoordUnit y) const
{
    int32 dx = (int32)GetPositionX() - x;
    int32 dy = (int32)GetPositionY() - y;
    return sqrt((dx * dx) + (dy * dy));
}

int32 WorldObject::GetTileLineDistance(const WorldObject* target) const
{
    return GetTileLineDistance(target->GetPositionX(), target->GetPositionY());
}

int32 WorldObject::GetTileLineDistance(CoordUnit x, CoordUnit y) const
{
    int32 dx = (int32)GetPositionX() - x;
    int32 dy = (int32)GetPositionY() - y;
    return std::max(abs(dx), abs(dy));
}

int32 WorldObject::GetTileDistance(const WorldObject* target) const
{
    return GetTileDistance(target->GetPositionX(), target->GetPositionY());
}

int32 WorldObject::GetTileDistance(CoordUnit x, CoordUnit y) const
{
    int32 dx = (int32)GetPositionX() - x;
    int32 dy = (int32)GetPositionY() - y;
    return abs(dx) + abs(dy);
}

bool WorldObject::IsWithinDist(WorldObject const* target, int32 dist2compare)
{
    return IsWithinDist(target->GetPositionX(), target->GetPositionY(), dist2compare);
}

bool WorldObject::IsWithinDist(CoordUnit x, CoordUnit y, int32 dist2compare)
{
    if (dist2compare == -1) {
        return IsInScreen(x, y);
    } else if (dist2compare == 0) {
        return (GetPositionX() == x) && (GetPositionY() == y);
    } else if (GetTileLineDistance(x, y) <= dist2compare) {
        return true;
    }

    return false;
}

bool WorldObject::IsInScreen(const WorldObject* target) const
{
    return IsInScreen(GetPositionX(), GetPositionY());
}

bool WorldObject::IsInScreen(CoordUnit x, CoordUnit y) const
{
    int32 dist = GetTileDistance(x, y);
    if (dist > 17) {
        return false;
    } else if (dist <= 13) {
        return true;
    } else {
        dist = abs((int32)GetPositionX() - ((int32)x - 15));
        dist += abs((int32)GetPositionY() - ((int32)y - 15));
        if (17 <= dist && dist <= 43)
            return true;
    }
    return false;
}

bool WorldObject::IsInMap(const WorldObject* obj) const
{
	return IsInWorld() && obj->IsInWorld() && (GetMap() == obj->GetMap());
}

bool WorldObject::IsWithinLOS(CoordUnit ox, CoordUnit oy) const
{
	CoordUnit x, y;
	GetPosition(x, y);
	return GetMap()->IsInLineOfSight(x, y, ox, oy);
}

bool WorldObject::IsWithinLOSInMap(const WorldObject* obj) const
{
	if (!IsInMap(obj))
	{
		return false;
	}
	CoordUnit ox, oy;
	obj->GetPosition(ox, oy);
	return(IsWithinLOS(ox, oy));
}

/*********************************************************/
/***                   MESSAGE SYSTEM                  ***/
/*********************************************************/

void WorldObject::SendMessageToSet(WorldPacket* data, bool /*bToSelf*/)
{
    if (IsInWorld())
	{
    	GetMap()->MessageBroadcast(this, data);
	}
}

void WorldObject::SendMessageToSetInRange(WorldPacket* data, uint8 dist, bool /*bToSelf*/)
{
    if (IsInWorld())
	{
    	GetMap()->MessageDistBroadcast(this, data, dist);
	}
}

void WorldObject::SendMessageToSetExcept(WorldPacket* data, const Player* skipped_receiver)
{
    if (IsInWorld())
    {
        MaNGOS::MessageDelivererExcept notifier(*this, data, skipped_receiver);
        MaNGOS::VisitWorldObjects(this, notifier, SIZE_OF_SCREEN);
    }
}

