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

#ifndef MANGOS_S_SERVERMESSAGE_HPP
#define MANGOS_S_SERVERMESSAGE_HPP

#include "Opcodes.h"
#include "WorldPacket.h"

enum ServerMessage
{
	SELECTED         = 0, // 被选择了
	NOTHING_HAPPENED = 1, // 没有任何事情发生
	CANNOT_BE_USED   = 74, // 无法使用
	CANNOT_USE_HERE  = 563, // 你无法在这个地方使用
};

class S_ServerMessage : public WorldPacket
{
public:
	S_ServerMessage(uint16 type) : WorldPacket(S_OPCODE_SERVERMSG, 3)
	{
		*this << uint16(type);
		*this << uint8(0);
	}

	S_ServerMessage(uint16 type, std::string msg) : WorldPacket(S_OPCODE_SERVERMSG, 3 + msg.size())
	{
		*this << uint16(type);
		*this << uint8(1);
		*this << msg;
	}
};

#endif //MANGOS_S_SERVERMESSAGE_HPP
