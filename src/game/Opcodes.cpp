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

/** \file
    \ingroup u2w
*/

#include "Opcodes.h"
#include "WorldSession.h"

static void DefineOpcode(OpcodeType opcode, const char* name, SessionStatus status, PacketProcessing packetProcessing, void (WorldSession::*handler)(WorldPacket& recvPacket))
{
    opcodeTable[opcode].name = name;
    opcodeTable[opcode].status = status;
    opcodeTable[opcode].packetProcessing = packetProcessing;
    opcodeTable[opcode].handler = handler;
}

#define OPCODE( name, status, packetProcessing, handler ) DefineOpcode( name, #name, status, packetProcessing, handler )

/// Correspondence between opcodes and their names
OpcodeHandler opcodeTable[MAX_OPCODE_TABLE_SIZE];

void InitializeOpcodes()
{
    for(uint16 i = 0; i < MAX_OPCODE_TABLE_SIZE; ++i)
        DefineOpcode(i, "UNKNOWN", STATUS_UNHANDLED, PROCESS_INPLACE, &WorldSession::Handle_NULL);

	OPCODE(C_OPCODE_COMMONCLICK, STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleCommonClick);
	OPCODE(C_OPCODE_QUITGAME, STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL);
	OPCODE(C_OPCODE_MOVECHAR, STATUS_LOGGEDIN, PROCESS_THREADUNSAFE, &WorldSession::HandleMovementOpcodes);
	OPCODE(C_OPCODE_LOGINTOSERVER, STATUS_AUTHED, PROCESS_THREADUNSAFE, &WorldSession::HandleLoginToServer);

	/// 天堂和魔兽世界不同，天堂的服务端操作码和客户端的操作码是重叠的，所以这边不能设置
	// OPCODE(S_OPCODE_CHARPACK, STATUS_NEVER, PROCESS_INPLACE, &WorldSession::Handle_ServerSide);
};
