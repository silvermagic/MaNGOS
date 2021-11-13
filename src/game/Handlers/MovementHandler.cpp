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

#include "Entities/Player.h"
#include "Packets/S_MoveChar.hpp"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"

void WorldSession::HandleMovementOpcodes(WorldPacket& recvPacket)
{
    CoordUnit x, y;
	uint8 towards;

    recvPacket >> x;
    recvPacket >> y;
    recvPacket >> towards;

    // todo: �����ж�

    // todo: �����ж�

    // todo: ֹͣڤ�롢�������� RemoveAurasWithInterruptFlags

    // ���֮ǰ��λ������ͨ��
    m_player->GetMap()->MarkPassable(m_player->GetPositionX(), m_player->GetPositionY(), true);

    switch (Towards(towards))
    {
    case SOUTH_EAST: // �������� ��ǰ��
    	++x; --y;
    	break;
    case SOUTH:      // �������� ��ǰ��
    	--y;
    	break;
    case NORTH_EAST: // ���Ϸ��� �Һ�
    	++x; ++y;
    	break;
    case EAST:       // �������� ��
    	++x;
    	break;
    case NORTH_WEST: // ���Ϸ��� ���
    	--x; ++y;
    	break;
    case NORTH:      // ���Ϸ��� ��
    	++y;
    	break;
    case SOUTH_WEST: // �������� ��ǰ��
    	--x; --y;
    	break;
    case WEST:      // �������� ��
    	--y;
    	break;
    }

    // todo: �����������

    Map* pMap = m_player->GetMap();
    // �ƶ���ң�����������
	pMap->PlayerRelocation(m_player, x, y, Towards(towards));
	// �㲥�ƶ���
	S_MoveChar pkt(m_player);
	pMap->MessageBroadcast(m_player, &pkt);

    // todo: ������

    m_player->GetMap()->MarkPassable(x, y, false);
}

