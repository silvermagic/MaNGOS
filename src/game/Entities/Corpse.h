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

#ifndef MANGOSSERVER_CORPSE_H
#define MANGOSSERVER_CORPSE_H

#include "GridSystem/GridReference.h"
#include "Object.h"

enum CorpseType
{
    CORPSE_BONES             = 0,
    CORPSE_RESURRECTABLE_PVE = 1,
    CORPSE_RESURRECTABLE_PVP = 2
};
#define MAX_CORPSE_TYPE        3

class Corpse : public WorldObject
{
public:
    explicit Corpse(CorpseType type = CORPSE_BONES) : m_type(type) {}
    ~Corpse() {}

    ObjectGuid const& GetOwnerGuid() const { return GetGuidValue(0); }
    CorpseType GetType() const { return m_type; }
    GridReference<Corpse>& GetGridRef() { return m_gridRef; }

private:
    GridReference<Corpse> m_gridRef;

    CorpseType m_type;
};
#endif
