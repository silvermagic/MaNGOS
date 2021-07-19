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

#ifndef _SPELLMGR_H
#define _SPELLMGR_H

#include <map>
#include "Common.h"
#include "Utilities/UnorderedMapSet.h"
#include "SharedDefines.h"
#include "SQLStructure.h"

class Player;
class Spell;
class Unit;

bool IsDeathPersistentSpell(const SpellEntry *proto);
bool IsPassiveSpell(const SpellEntry *proto);
bool IsPositiveSpell(const SpellEntry *proto);
bool IsSingleTargetSpell(const SpellEntry *proto);

bool IsSpellAppliesAura(const SpellEntry *proto, uint32 effectMask = ((1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)));
bool IsSpellBreakStealth(const SpellEntry *proto);
bool IsSpellHaveEffect(const SpellEntry *proto, SpellEffects effect);

bool IsAreaAuraEffect(uint32 effect);
bool IsAuraApplyEffect(const SpellEntry *proto, SpellEffectIndex eff);
bool IsPositiveEffect(const SpellEntry *proto, SpellEffectIndex eff);

bool IsPositiveTarget(uint32 target);

uint32 GetAllSpellMechanicMask(const SpellEntry *proto);
uint32 GetDispellMask(DispelType dispel);

class SpellMgr
{
public:
    SpellMgr();
    ~SpellMgr();

public:
    static SpellMgr& Instance();
};

#define sSpellMgr SpellMgr::Instance()
#endif
