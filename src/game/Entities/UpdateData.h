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

#ifndef __UPDATEDATA_H
#define __UPDATEDATA_H

#include "Utilities/ByteBuffer.h"
#include "ObjectGuid.h"

class WorldPacket;

enum ObjectUpdateType
{
    UPDATETYPE_VALUES               = 0,
};

enum ObjectUpdateFlags
{
    UPDATEFLAG_NONE                 = 0x0000,
    UPDATEFLAG_SELF                 = 0x0001,
};

class UpdateData
{
public:
    UpdateData(uint16 mapId);

    void AddOutOfRangeGUID(GuidSet& guids);
    void AddOutOfRangeGUID(ObjectGuid const& guid);
    void AddUpdateBlock(const ByteBuffer& block);
    bool BuildPacket(WorldPacket* packet);
    bool HasData() { return m_blockCount > 0 || !m_outOfRangeGUIDs.empty(); }
    void Clear();

    GuidSet const& GetOutOfRangeGUIDs() const { return m_outOfRangeGUIDs; }

    void SetMapId(uint16 mapId) { m_map = mapId; }

protected:
    uint16 m_map;
    uint32 m_blockCount;
    GuidSet m_outOfRangeGUIDs;
    ByteBuffer m_data;

    void Compress(void* dst, uint32* dst_size, void* src, int src_size);
};
#endif
