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

#ifndef MANGOS_S_OWNCHARSTATUS_HPP
#define MANGOS_S_OWNCHARSTATUS_HPP

#include "Entities/Player.h"
#include "GameClock.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldPacket.h"

class S_OwnCharStatus : public WorldPacket
{
public:
	S_OwnCharStatus(Player* player) : WorldPacket(S_OPCODE_OWNCHARSTATUS, 36, false)
	{
		time_t now = GameClock::GetGameWorldTime();
		*this << uint32(player->GetGUIDLow());
		*this << uint8(boost::algorithm::clamp(player->GetLevel(), 1, 127));
		*this << uint32(player->GetUInt32Value(PLAYER_FIELD_XP));
		*this << uint8(player->GetStat(STAT_STRENGTH));
		*this << uint8(player->GetStat(STAT_INTELLECT));
		*this << uint8(player->GetStat(STAT_SPIRIT));
		*this << uint8(player->GetStat(STAT_AGILITY));
		*this << uint8(player->GetStat(STAT_STAMINA));
		*this << uint8(player->GetStat(STAT_CHARM));
		*this << uint16(player->GetHealth());
		*this << uint16(player->GetMaxHealth());
		*this << uint16(player->GetMana());
		*this << uint16(player->GetMaxMana());
		*this << uint8(player->GetArmor());
		*this << uint32(now - (now % 300));
		*this << uint8(player->GetFood());
		*this << uint8(player->GetBag()->GetWeight240());
		*this << uint16(player->GetLawful());
		*this << uint8(player->GetDefense(SPELL_SCHOOL_FIRE));
		*this << uint8(player->GetDefense(SPELL_SCHOOL_WATER));
		*this << uint8(player->GetDefense(SPELL_SCHOOL_WIND));
		*this << uint8(player->GetDefense(SPELL_SCHOOL_EARTH));
	}
};

#endif //MANGOS_S_OWNCHARSTATUS_HPP
