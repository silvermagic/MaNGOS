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
		case SOUTH:      // 正北方向 正前方
			++y;
			break;
		case SOUTH_EAST: // 东北方向 右前方
			--x; ++y;
			break;
		case EAST:       // 正东方向 右
			--x;
			break;
		case NORTH_EAST: // 东南方向 右后方
			--x; --y;
			break;
		case NORTH:      // 正南方向 后方
			--y;
			break;
		case NORTH_WEST: // 西南方向 左后方
			++x; --y;
			break;
		case WEST:       // 正西方向 左
			++x;
			break;
		case SOUTH_WEST: // 西北方向 左前方
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