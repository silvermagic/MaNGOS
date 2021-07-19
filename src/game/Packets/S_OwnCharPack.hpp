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

#ifndef MANGOS_S_OWNCHARPACK_HPP
#define MANGOS_S_OWNCHARPACK_HPP

#include "Entities/Player.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldPacket.h"

class S_OwnCharPack : public WorldPacket
{
    enum CHAR_STATUS
    {
        POISON          = 0x01,
        INVISIBLE       = 0x02,
        PC              = 0x04,
        FREEZE          = 0x08,
        BRAVE           = 0x10,
        ELFBRAVE        = 0x20,
        FASTMOVABLE     = 0x40,
        GHOST           = 0x80,
    };
public:
	S_OwnCharPack(Player* player) : WorldPacket(S_OPCODE_CHARPACK, 0, false) //player->GetName().size() + player->GetTitle().size() + 32, false)
	{
	    uint8 status = PC;
	    if (player->IsInvisble())
        {
	        status |= INVISIBLE;
        }
	    if (player->IsBrave())
        {
            status |= BRAVE;
            if (player->GetClass() == CLASS_ELF)
            {
                status |= ELFBRAVE;
            }
        }
		if (player->IsFastMovable())
        {
            status |= FASTMOVABLE;
        }
		// todo: Áé»ê×´Ì¬

		*this << uint16(player->GetPositionX());
		*this << uint16(player->GetPositionY());
		*this << uint32(player->GetGUIDLow());
		if (player->IsDead())
		{
			*this << uint16(player->GetUInt32Value(PLAYER_FIELD_GFX_ID));
			*this << uint8(0);
		}
		else
		{
			*this << uint16(player->GetUInt32Value(UNIT_FIELD_GFX_ID));
			*this << uint8(player->GetBag()->GetWeaponShape());
		}
		*this << uint8(player->GetOrientation());
		*this << uint8(0); // todo: ÊÓÒ°
		*this << uint8(player->GetSpeed());
		*this << uint32(player->GetUInt32Value(PLAYER_FIELD_XP));
		*this << int16(player->GetLawful());
		*this << player->GetName();
		*this << player->GetTitle();
		*this << uint8(status); // todo: ×´Ì¬
		*this << uint32(0); // todo: ÑªÃË±àºÅ
		*this << ""; // todo: ÑªÃËÃû³Æ
		*this << uint8(0);
        *this << uint8(0);
		*this << uint8(0xFF); // todo: ¶ÓÎé
		*this << uint8(0);
		*this << uint8(0);
		*this << uint8(0);
		*this << uint8(0xFF);
		*this << uint8(0xFF);
	}
};

#endif //MANGOS_S_OWNCHARPACK_HPP
