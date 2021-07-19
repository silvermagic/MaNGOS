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

#ifndef MANGOS_S_INVLIST_HPP
#define MANGOS_S_INVLIST_HPP

#include "Config/Config.h"
#include "Items/Item.h"
#include "Util.h"
#include "Opcodes.h"
#include "WorldPacket.h"

class S_InvList : public WorldPacket
{
public:
	S_InvList(std::vector<Item*> &&pItems) : WorldPacket(S_OPCODE_INVLIST, 0)
	{
		*this << uint8(pItems.size());
		for (auto& pItem : pItems)
		{
			*this << uint32(pItem->GetGUIDLow());
			*this << uint8(pItem->GetProto()->use_type);
			*this << uint8(0);
			*this << uint16(pItem->GetProto()->gfx_in_backpack);
			*this << uint8(pItem->GetBless());
			*this << uint32(pItem->GetCount());
			*this << uint8(pItem->IsIdentified());
			*this << Utf8toLocale(pItem->GetName(true), LocaleConstant(sConfig.GetIntDefault("L1J.ClientLanguage", 5)));
			if (pItem->IsIdentified())
			{
				ByteBuffer buffer;
				pItem->GetStatusBytes(buffer);
				*this << uint8(buffer.size());
				this->append(buffer.contents(), buffer.size());
			}
			else
			{
				*this << uint8(0);
			}
		}
	}
};

#endif //MANGOS_S_INVLIST_HPP

