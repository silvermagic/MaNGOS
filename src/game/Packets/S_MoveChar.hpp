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

#ifndef MANGOS_S_MOVECHAR_HPP
#define MANGOS_S_MOVECHAR_HPP

#include "Entities/Player.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldPacket.h"

class S_MoveChar : public WorldPacket
{
public:
	S_MoveChar(Player* player) : WorldPacket(S_OPCODE_MOVEOBJECT, 14)
	{
		CoordUnit x, y;
		player->GetPosition(x, y);
		Towards toward = player->GetOrientation();
		switch (toward)
		{
		case SOUTH:      // �������� ��ǰ��
			++y;
			break;
		case SOUTH_EAST: // �������� ��ǰ��
			--x; ++y;
			break;
		case EAST:       // �������� ��
			--x;
			break;
		case NORTH_EAST: // ���Ϸ��� �Һ�
			--x; --y;
			break;
		case NORTH:      // ���Ϸ��� ��
			--y;
			break;
		case NORTH_WEST: // ���Ϸ��� ���
			++x; --y;
			break;
		case WEST:       // �������� ��
			++x;
			break;
		case SOUTH_WEST: // �������� ��ǰ��
			++x; ++y;
			break;
		}

		*this << player->GetGUIDLow();
		*this << uint16(x);
		*this << uint16(y);
		*this << uint8(toward);
		*this << uint8(0x81);
		*this << uint32(0);
	}
};

#endif //MANGOS_S_MOVECHAR_HPP