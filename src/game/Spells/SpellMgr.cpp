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

#include "SharedDefines.h"
#include "SpellMgr.h"

bool IsDeathPersistentSpell(const SpellEntry *proto)
{
    return proto->HasAttribute(SPELL_ATTR_DEATH_PERSISTENT);
}

bool IsPassiveSpell(const SpellEntry *proto)
{
    return proto->HasAttribute(SPELL_ATTR_PASSIVE);
}

bool IsPositiveSpell(const SpellEntry *proto)
{
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (proto->Effect[SpellEffectIndex(i)] && !IsPositiveEffect(proto, SpellEffectIndex(i)))
		{
			return false;
		}
	}
    return true;
}

bool IsSingleTargetSpell(const SpellEntry *proto)
{
	return true;
}

bool IsSpellAppliesAura(const SpellEntry *proto, uint32 effectMask)
{
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (effectMask & (1 << i))
        {
            if (IsAuraApplyEffect(proto, SpellEffectIndex(i)))
            {
                return true;
            }
        }
    }

    return false;
}

bool IsSpellBreakStealth(const SpellEntry *proto)
{
    return !proto->HasAttribute(SPELL_ATTR_NOT_BREAK_STEALTH);
}

bool IsSpellHaveEffect(const SpellEntry *proto, SpellEffects effect)
{
    for (uint8 i = 0; i <= MAX_EFFECT_INDEX; ++i)
    {
        if (proto->Effect[i] == effect)
        {
            return true;
        }
    }

    return false;
}

bool IsAreaAuraEffect(uint32 effect)
{
	return false;
}

bool IsAuraApplyEffect(const SpellEntry *proto, SpellEffectIndex eff)
{
    switch (proto->Effect[eff])
    {
        case SPELL_EFFECT_APPLY_AURA:
        case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
            return true;
    }
    return false;
}

bool IsPositiveEffect(const SpellEntry *proto, SpellEffectIndex eff)
{
    return true;
}

bool IsPositiveTarget(uint32 target)
{
	return true;
}

uint32 GetAllSpellMechanicMask(const SpellEntry *proto)
{
	uint32 mask = 1 << (proto->Mechanic - 1);
	return mask;
}

uint32 GetDispellMask(DispelType dispel)
{
    if (dispel == DISPEL_ALL)
        return DISPEL_ALL_MASK;
    else
        return (1 << dispel);
}