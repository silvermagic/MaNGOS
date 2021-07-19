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

    // todo: 传送判断

    // todo: 加速判断

    // todo: 停止冥想、绝对屏障 RemoveAurasWithInterruptFlags

    // 玩家之前的位置允许通过
    m_player->GetMap()->MarkPassable(m_player->GetPositionX(), m_player->GetPositionY(), true);

    switch (Towards(towards))
    {
    case SOUTH_EAST: // 东北方向 右前方
    	++x; --y;
    	break;
    case SOUTH:      // 正北方向 正前方
    	--y;
    	break;
    case NORTH_EAST: // 东南方向 右后方
    	++x; ++y;
    	break;
    case EAST:       // 正东方向 右
    	++x;
    	break;
    case NORTH_WEST: // 西南方向 左后方
    	--x; ++y;
    	break;
    case NORTH:      // 正南方向 后方
    	++y;
    	break;
    case SOUTH_WEST: // 西北方向 左前方
    	--x; --y;
    	break;
    case WEST:      // 正西方向 左
    	--y;
    	break;
    }

    // todo: 地牢随机传送

    Map* pMap = m_player->GetMap();
    // 移动玩家，并更新网格
	pMap->PlayerRelocation(m_player, x, y, Towards(towards));
	// 广播移动包
	S_MoveChar pkt(m_player);
	pMap->MessageBroadcast(m_player, &pkt);

    // todo: 陷阱检测

    m_player->GetMap()->MarkPassable(x, y, false);
}

