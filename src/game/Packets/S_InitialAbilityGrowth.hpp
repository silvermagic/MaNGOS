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

#ifndef MANGOS_S_INITIALABILITYGROWTH_HPP
#define MANGOS_S_INITIALABILITYGROWTH_HPP

#include "Entities/Player.h"
#include "Opcodes.h"
#include "SharedDefines.h"
#include "WorldPacket.h"

class S_InitialAbilityGrowth : public WorldPacket
{
	const std::map<Classes, std::map<Stats, uint8>> PLAYER_ORIGINAL_STATS = {
		{CLASS_PRINCE, {{STAT_STRENGTH, 13}, {STAT_AGILITY, 10}, {STAT_STAMINA, 10}, {STAT_SPIRIT, 11}, {STAT_CHARM, 13}, {STAT_INTELLECT, 10}}},
		{CLASS_KNIGHT, {{STAT_STRENGTH, 16}, {STAT_AGILITY, 12}, {STAT_STAMINA, 14}, {STAT_SPIRIT, 9}, {STAT_CHARM, 12}, {STAT_INTELLECT, 8}}},
		{CLASS_ELF, {{STAT_STRENGTH, 11}, {STAT_AGILITY, 12}, {STAT_STAMINA, 12}, {STAT_SPIRIT, 12}, {STAT_CHARM, 9}, {STAT_INTELLECT, 12}}},
		{CLASS_WIZARD, {{STAT_STRENGTH, 8}, {STAT_AGILITY, 7}, {STAT_STAMINA, 12}, {STAT_SPIRIT, 12}, {STAT_CHARM, 8}, {STAT_INTELLECT, 12}}},
		{CLASS_DARK_ELF, {{STAT_STRENGTH, 12}, {STAT_AGILITY, 15}, {STAT_STAMINA, 8}, {STAT_SPIRIT, 10}, {STAT_CHARM, 9}, {STAT_INTELLECT, 11}}},
		{CLASS_DRAGON_KNIGHT, {{STAT_STRENGTH, 13}, {STAT_AGILITY, 11}, {STAT_STAMINA, 14}, {STAT_SPIRIT, 12}, {STAT_CHARM, 8}, {STAT_INTELLECT, 11}}},
		{CLASS_ILLUSIONIST, {{STAT_STRENGTH, 11}, {STAT_AGILITY, 10}, {STAT_STAMINA, 12}, {STAT_SPIRIT, 12}, {STAT_CHARM, 8}, {STAT_INTELLECT, 12}}},
	};
public:
	S_InitialAbilityGrowth(Player* player) : WorldPacket(S_OPCODE_INITABILITYGROWTH, 5)
	{
		int32 str = player->GetCreateStat(STAT_STRENGTH) - PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_STRENGTH);
		int32 dex = player->GetCreateStat(STAT_AGILITY) -  PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_AGILITY);
		int32 con = player->GetCreateStat(STAT_STAMINA) -  PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_STAMINA);
		int32 wis = player->GetCreateStat(STAT_SPIRIT) -  PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_SPIRIT);
		int32 intel = player->GetCreateStat(STAT_INTELLECT) -  PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_INTELLECT);
		int32 cha = player->GetCreateStat(STAT_CHARM) -  PLAYER_ORIGINAL_STATS.at(player->GetClass()).at(STAT_CHARM);

		*this << uint8(4);
		*this << uint8(intel * 16 + str);
		*this << uint8(dex * 16 + wis);
		*this << uint8(cha * 16 + con);
		*this << uint8(0);
	}
};

#endif //MANGOS_S_INITIALABILITYGROWTH_HPP

