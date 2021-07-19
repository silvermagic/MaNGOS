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

#include "SQLStorages.h"
#include "SQLStructure.h"

const uint8 MAX_LEVEL = 99;
const char ExpFmt[] = "iii";
SQLStorage sExpStore(ExpFmt, "level", "exp");

uint32 CalcExp(uint8 level)
{
	MANGOS_ASSERT(level <= (MAX_LEVEL + 1));

	const ExpEntry *expInfo = sExpStore.LookupEntry<ExpEntry>(level - 1);
	return expInfo->exp;
}

uint32 CalcLevelUpExp(uint8 level)
{
	return CalcExp(level + 1) - CalcExp(level);
}

uint8 CalcLevel(uint32 exp)
{
	uint8 level = 1;
	for (; level < sExpStore.GetMaxEntry(); ++level)
	{
		const ExpEntry *expInfo = sExpStore.LookupEntry<ExpEntry>(level);
		if (exp < expInfo->exp)
		{
			break;
		}
	}

	return std::min(level, MAX_LEVEL);
}

uint8 CalcExpPercentage(uint8 level, uint32 exp)
{
	return uint8(100.0 * ((double)(exp - CalcExp(level)) / (double)CalcLevelUpExp(level)));
}