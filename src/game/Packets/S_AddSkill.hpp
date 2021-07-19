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

#ifndef MANGOS_S_ADDSKILL_HPP
#define MANGOS_S_ADDSKILL_HPP

#include <vector>
#include "Opcodes.h"
#include "WorldPacket.h"

class S_AddSkill : public WorldPacket
{
public:
	S_AddSkill(std::vector<uint8> skills) : WorldPacket(S_OPCODE_ADDSKILL, 37)
	{
		// 法术职业法术 5 - 8 、 9 - 10
		int32 wizard[2] = {skills[4] + skills[5] + skills[6] + skills[7], skills[8] + skills[9]};
		if (wizard[0] > 0 && wizard[1] == 0)
		{
			*this << uint8(50);
		}
		else if (wizard[1] > 0)
		{
			*this << uint8(100);
		}
		else
		{
			*this << uint8(32);
		}

		for (auto skill : skills)
		{
			*this << uint8(skill);
		}
		*this << uint32(0);
		*this << uint32(0);
	}
};

#endif //MANGOS_S_ADDSKILL_HPP
