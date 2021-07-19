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

#ifndef MANGOS_S_ACTIVESPELLS_HPP
#define MANGOS_S_ACTIVESPELLS_HPP

#include "Opcodes.h"
#include "WorldPacket.h"

class S_ActiveSpells : public WorldPacket
{
public:
	S_ActiveSpells() : WorldPacket(S_OPCODE_ACTIVESPELLS, 2, false)
	{
		*this << uint8(0x14);
		*this << uint8(0x69);
	}
};

#endif //MANGOS_S_ACTIVESPELLS_HPP