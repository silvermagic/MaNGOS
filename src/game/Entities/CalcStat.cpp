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

#include "CalcStat.h"

namespace MaNGOS
{
	int8 CalcStatArmor(uint8 lvl, int8 baseDex)
	{
		int8 ac = 10;
		if (baseDex <= 9)
		{
			ac -= lvl / 8;
		}
		else if (baseDex >= 10 && baseDex <= 12)
		{
			ac -= lvl / 7;
		}
		else if (baseDex >= 13 && baseDex <= 15)
		{
			ac -= lvl / 6;
		}
		else if (baseDex >= 16 && baseDex <= 17)
		{
			ac -= lvl / 5;
		}
		else if (baseDex >= 18)
		{
			ac -= lvl / 4;
		}
		return ac;
	}

	int16 CalcStatMagicDefense(int8 spirit)
	{
		int8 value = 0;
		if (spirit <= 14)
		{
			value += 0;
		}
		else if (15 <= spirit && spirit <= 16)
		{
			value += 3;
		}
		else if (spirit == 17)
		{
			value += 6;
		}
		else if (spirit == 18)
		{
			value += 10;
		}
		else if (spirit == 19)
		{
			value += 15;
		}
		else if (spirit == 20)
		{
			value += 21;
		}
		else if (spirit == 21)
		{
			value += 28;
		}
		else if (spirit == 22)
		{
			value += 37;
		}
		else if (spirit == 23)
		{
			value += 47;
		}
		else if (spirit == 24)
		{
			value += 50;
		}
		else
		{
			value += 50;
		}
		return value;
	}
}