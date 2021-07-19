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

#include <stdarg.h>
#include <algorithm>
#include <boost/algorithm/clamp.hpp>
#include "Spells/Spell.h"
#include "Spells/SpellAuras.h"
#include "Util.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "SQLStorages.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Creature.h"
#include "ObjectGuid.h"
#include "Pet.h"
#include "Player.h"
#include "Unit.h"

Unit::Unit()
{
    m_objectType |= TYPEMASK_UNIT;
    m_objectTypeId = TYPEID_UNIT;

	m_state = 0;
	m_deathState = ALIVE;

	m_spellAuraHolders.clear();
	m_spellAuraHoldersUpdateIterator = m_spellAuraHolders.end();
	m_deletedAuras.clear();
	m_deletedHolders.clear();
	for (uint8 i = 0; i < TOTAL_AURAS; ++i)
	{
		m_modifyAuras[i].clear();
	}
	m_visibleAuras.clear();
	m_cooldown = 0;

	for (uint8 i = 0; i < MAX_STATS; ++i)
	{
		m_totalStat[i] = 0;
		m_stat[i] = 0;
	}
	for (uint8 i = 0; i < MAX_ATTACK; ++i)
	{
		m_totalAttackDamage[i] = 0;
		m_attackDamage[i] = 0;
		m_totalAttackHit[i] = 0;
		m_attackHit[i] = 0;
	}
	m_totalAc = 0;
	m_ac = 0;
	m_mana = 0;
	m_health = 0;
	m_totalMaxMana = 0;
	m_maxMana = 0;
	m_totalMaxHealth = 0;
	m_maxHealth = 0;
	for (uint8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
	{
		m_totalDefense[i] = 0;
		m_defense[i] = 0;
	}
	m_totalSp = 0;
	m_sp = 0;
	m_totalDodge = 0;
	m_dodge = 0;
	for (uint8 i = 0; i < MAX_STORED_MECHANICS; ++i)
	{
		m_totalResist[i] = 0;
		m_resist[i] = 0;
	}
}

Unit::~Unit()
{
	MANGOS_ASSERT(m_gameObj.size() == 0);
	MANGOS_ASSERT(m_dynObjGUIDs.size() == 0);
	MANGOS_ASSERT(m_deletedAuras.size() == 0);
	MANGOS_ASSERT(m_deletedHolders.size() == 0);
}

void Unit::Update(uint32 update_diff, uint32 p_time)
{
    if (!IsInWorld())
	{
    	return;
	}

    // m_Events.Update(update_diff);
	// UpdateSpells(update_diff);
	// CleanupDeletedAuras();
}

void Unit::AddToWorld()
{
	Object::AddToWorld();
	// todo: ÆôÓÃAI
}

void Unit::RemoveFromWorld()
{
	if (IsInWorld())
	{
		// todo: É¾³ýÕÙ»½
		RemoveAllGameObjects();
		RemoveAllDynObjects();
		CleanupDeletedAuras();
		GetViewPoint().Event_RemovedFromWorld();
	}

	Object::RemoveFromWorld();
}

void Unit::CleanupsBeforeDelete()
{
	if (m_uint32Values)
	{
		m_Events.KillAllEvents(false);
		// todo: Í£Ö¹Õ½¶·
		RemoveAllAuras(AURA_REMOVE_BY_DELETE);
	}
	WorldObject::CleanupsBeforeDelete();
}

void Unit::OnRelocated()
{
}

Unit* Unit::GetOwner() const
{
	if (ObjectGuid ownerid = GetOwnerGuid())
	{
		return ObjectAccessor::GetUnit(*this, ownerid);
	}
	return nullptr;
}

const ObjectGuid& Unit::GetCreatorGuid() const
{
	return GetGuidValue(UNIT_FIELD_CREATEDBY);
}

const ObjectGuid& Unit::GetOwnerGuid() const
{
	return  GetGuidValue(UNIT_FIELD_SUMMONEDBY);
}

const ObjectGuid& Unit::GetPetGuid() const
{
	return GetGuidValue(UNIT_FIELD_SUMMON);
}

const ObjectGuid& Unit::GetTargetGuid() const
{
	return GetGuidValue(UNIT_FIELD_TARGET);
}

void Unit::SetCreatorGuid(ObjectGuid creator)
{
	SetGuidValue(UNIT_FIELD_CREATEDBY, creator);
}

void Unit::SetOwnerGuid(ObjectGuid owner)
{
	SetGuidValue(UNIT_FIELD_SUMMONEDBY, owner);
}

void Unit::SetPetGuid(ObjectGuid pet)
{
	SetGuidValue(UNIT_FIELD_SUMMON, pet);
}

void Unit::SetTargetGuid(ObjectGuid targetGuid)
{
	SetGuidValue(UNIT_FIELD_TARGET, targetGuid);
}

bool Unit::GetBrave() const
{
    return m_braveSpeed;
}

Classes Unit::GetClass() const
{
    return Classes(GetByteValue(UNIT_FIELD_BYTES, 1));
}

uint32 Unit::GetClassMask() const
{
    return 1 << (GetClass() - 1);
}

DeathState Unit::GetDeathState() const
{
    return m_deathState;
}

uint8 Unit::GetFood() const
{
	return GetByteValue(UNIT_FIELD_STATUS, 0);
}

uint8 Unit::GetGender() const
{
    return GetByteValue(UNIT_FIELD_BYTES, 2);
}

uint8 Unit::GetLevel() const
{
    return boost::algorithm::clamp(GetUInt32Value(UNIT_FIELD_LEVEL), 1, 127);
}

std::string Unit::GetName() const
{
    return m_name;
}

std::string Unit::GetTitle() const
{
    return m_title;
}

UnitMoveType Unit::GetSpeed() const
{
    return m_moveSpeed;
}

void Unit::AddUnitState(uint32 flag)
{
    m_state |= flag;
}

void Unit::ClearUnitState(uint32 flag)
{
    m_state &= ~flag;
}

void Unit::SetSpeed(UnitMoveType type)
{
    m_moveSpeed = type;
}

void Unit::SetBrave(bool enabled)
{
    m_braveSpeed = enabled;
}

void Unit::SetDeathState(DeathState state)
{
    if (state != ALIVE && state != JUST_ALIVED)
    {
    }

    if (state == JUST_DIED)
    {
    }
    else if (state == JUST_ALIVED)
    {
    }

    if (m_deathState != ALIVE && state == ALIVE)
    {
    }
    m_deathState = state;
}

void Unit::SetFood(uint8 value)
{
	SetByteValue(UNIT_FIELD_STATUS, 0, value);
}

void Unit::SetRoot(bool enabled)
{
	// todo: Êø¸¿Ð§¹û
}

bool Unit::CanFreeMove() const
{
    return true;
}

bool Unit::HasUnitState(uint32 flag) const
{
    return (m_state & flag);
}

bool Unit::IsAlive() const
{
    return (m_deathState == ALIVE);
}

bool Unit::IsDead() const
{
    return (m_deathState == DEAD || m_deathState == CORPSE);
}

bool Unit::IsTargetableForAttack(bool inverseAlive) const
{
    if (GetTypeId() == TYPEID_PLAYER && ((Player*)this)->IsGameMaster())
    {
        return false;
    }

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
    {
        return false;
    }

    if (IsAlive() == inverseAlive)
    {
        return false;
    }

    return IsInWorld() && !HasUnitState(UNIT_STAT_DIED);
}

bool Unit::IsPassiveToHostile() const
{
    return HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
}

bool Unit::IsVisibleForOrDetect(const Unit* u, const WorldObject* viewPoint, bool detect, bool inVisibleList) const
{
	return true;
}

bool Unit::IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const
{
	return IsVisibleForOrDetect(player, viewPoint, false, inVisibleList);
}