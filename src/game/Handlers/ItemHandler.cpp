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
#include "Items/Item.h"
#include "Items/PcInventory.h"
#include "Packets/S_ServerMessage.hpp"
#include "WorldSession.h"

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
	uint32 itemId;
	recvPacket >> itemId;

	if (m_player->IsDead())
	{
		return;
	}

	if (!m_player->GetMap()->IsUsableItem())
	{
		S_ServerMessage pkt(CANNOT_USE_HERE);
		m_player->SendDirectMessage(&pkt);
		return;
	}

	Item *pItem = m_player->GetBag()->GetItem(itemId);
	if (!pItem)
	{
		return;
	}

	switch (pItem->GetProto()->Class)
	{
		case ITEM_CLASS_WEAPON:
		case ITEM_CLASS_ARMOR:
			return HandleEquipItem(pItem);
		default:
		{
			if (pItem->GetProto()->IsUsable())
			{
				S_ServerMessage pkt(CANNOT_BE_USED, pItem->GetName(false));
				m_player->SendDirectMessage(&pkt);
				return;
			}

			// m_player->CastItemUseSpell(pItem, targets, cast_count, glyphIndex);
			break;
		}
	}
}

void WorldSession::HandleEquipItem(Item *pItem)
{

}
