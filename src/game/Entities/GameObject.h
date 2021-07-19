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

#ifndef MANGOSSERVER_GAMEOBJECT_H
#define MANGOSSERVER_GAMEOBJECT_H

#include "Object.h"

class MANGOS_DLL_SPEC GameObject : public WorldObject
{
public:
    explicit GameObject() {}

	void SetOwnerGuid(ObjectGuid /* ownerGuid */) {}
	const ObjectGuid& GetOwnerGuid() const { return GetGuidValue(0); }
	Unit* GetOwner() const { return m_owner; }
	uint32 GetSpellId() const { return m_spellId;}
	void Delete() {}
    GridReference<GameObject>& GetGridRef() { return m_gridRef; }

protected:
	Unit* m_owner;
	uint32 m_spellId;
    GridReference<GameObject> m_gridRef;
};
#endif
