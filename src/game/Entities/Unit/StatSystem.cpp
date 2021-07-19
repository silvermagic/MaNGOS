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

#include <boost/algorithm/clamp.hpp>
#include "Entities/Unit.h"
#include "Spells/SpellAuras.h"

int8 Unit::GetStat(Stats stat) const
{
	return m_stat[stat];
}

void Unit::ModifyStat(Stats stat, int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalStat[stat] += diff;
	m_stat[stat] = boost::algorithm::clamp(m_totalStat[stat], 1, 127);
}

int8 Unit::GetAttackHit(WeaponAttackType att) const
{
	return m_attackHit[att];
}

int8 Unit::GetAttackDamage(WeaponAttackType att) const
{
	return m_attackDamage[att];
}

void Unit::ModifyAttackHit(WeaponAttackType att, int32 diff)
{	if (diff == 0)
	{
		return;
	}

	m_totalAttackHit[att] += diff;
	m_attackHit[att] = boost::algorithm::clamp(m_totalAttackHit[att], -128, 127);
}

void Unit::ModifyAttackDamage(WeaponAttackType att, int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalAttackDamage[att] += diff;
	m_attackDamage[att] = boost::algorithm::clamp(m_totalAttackDamage[att], -128, 127);
}

int8 Unit::GetArmor() const
{
	return m_ac;
}

void Unit::ModifyArmor(int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalAc += diff;
	m_ac = boost::algorithm::clamp(m_totalAc, -128, 127);
}

int16 Unit::GetMana() const
{
	return m_mana;
}

int16 Unit::GetMaxMana() const
{
	return m_maxMana;
}

void Unit::ModifyMaxMana(int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalMaxMana += diff;
	m_maxMana = boost::algorithm::clamp(m_totalMaxMana, 0, MAX_MANA);
	SetMana(std::min(m_mana, m_maxMana));
}

int16 Unit::GetHealth() const
{
	return m_health;
}

int16 Unit::GetMaxHealth() const
{
	return m_maxHealth;
}

void Unit::ModifyMaxHealth(int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalMaxHealth += diff;
	m_maxHealth = boost::algorithm::clamp(m_totalMaxHealth, 1, MAX_HEALTH);
	SetHealth(std::min(m_health, m_maxHealth));
}

int16 Unit::GetLawful() const
{
	return GetInt32Value(UNIT_FIELD_LAWFUL);
}

void Unit::SetMana(int16 value, bool direct)
{
	if (direct)
	{
		m_mana = value;
		return;
	}

	if (m_mana == value)
	{
		return;
	}

	m_mana = std::min(value, m_maxMana);
}

void Unit::SetHealth(int16 value, bool direct)
{
	if (direct)
	{
		m_health = value;
		return;
	}

	if (m_health == value)
	{
		return;
	}

	m_health = std::min(value, m_maxHealth);
}

void Unit::SetLawful(int16 value)
{
	SetInt32Value(UNIT_FIELD_LAWFUL, boost::algorithm::clamp(value, -MAX_LAWFUL, MAX_LAWFUL));
}

int16 Unit::GetDefense(SpellSchools school) const
{
	return m_defense[school];
}

void Unit::ModifyDefense(SpellSchools school, int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalDefense[school] += diff;
	switch (school)
	{
		case SPELL_SCHOOL_NORMAL:
			m_defense[school] = std::max(0, m_totalDefense[school]);
			break;
		case SPELL_SCHOOL_WIND:
		case SPELL_SCHOOL_WATER:
		case SPELL_SCHOOL_FIRE:
		case SPELL_SCHOOL_EARTH:
			m_defense[school] = boost::algorithm::clamp(m_totalDefense[school], -128, 127);
			break;
	}
}

int16 Unit::GetSpellPower() const
{
	return m_sp;
}

void Unit::ModifySpellPower(int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalSp += diff;
	m_sp = m_totalSp;
}

void Unit::UpdateBaseSpellPower()
{
	int32 value = GetSpellLevel() + GetSpellBonus();
	ModifySpellPower(value - m_baseSp);
	m_baseSp = value;
}

int32 Unit::GetSpellLevel() const
{
	return GetLevel() / 4;
}

int32 Unit::GetSpellBonus() const
{
	int8 intellect = GetStat(STAT_INTELLECT);
	if (intellect <= 5)
	{
		return -2;
	}
	else if (intellect <= 8)
	{
		return -1;
	}
	else if (intellect <= 11)
	{
		return 0;
	}
	else if (intellect <= 14)
	{
		return 1;
	}
	else if (intellect <= 17)
	{
		return 2;
	}
	else if (intellect <= 24)
	{
		return intellect - 15;
	}
	else if (intellect <= 35)
	{
		return 10;
	}
	else if (intellect <= 42)
	{
		return 11;
	}
	else if (intellect <= 49)
	{
		return 12;
	}
	else if (intellect <= 50)
	{
		return 13;
	}
	else
	{
		return intellect - 25;
	}
}

int8 Unit::GetDodge() const
{
	return m_dodge;
}

void Unit::ModifyDodge(int32 diff)
{
	if (diff == 0)
	{
		return;
	}

	m_totalDodge += diff;
	m_dodge = std::max(0, m_totalDodge);
}

int8 Unit::GetResist(MechanicsType type) const
{
	if (type == MECHANIC_NONE)
	{
		return 0;
	}
	return m_resist[type - 1];
}

void Unit::ModifyResist(MechanicsType type, int32 diff)
{
	if (diff == 0 || type == MECHANIC_NONE)
	{
		return;
	}

	m_totalResist[type - 1] += diff;
	m_resist[type - 1] = boost::algorithm::clamp(m_totalResist[type - 1], -128, 127);
}

int16 Unit::GetPower(uint8 power)
{
	switch (power)
	{
		case POWER_MANA:
			return GetMana();
		case POWER_HEALTH:
			return GetHealth();
		case POWER_LAWFUL:
			return GetLawful();
		default:
			return 0;
	}
}

void Unit::ModifyPower(uint8 power, int16 value)
{
	switch (power)
	{
		case POWER_MANA:
			SetMana(m_mana + value);
			break;
		case POWER_HEALTH:
			SetHealth(m_health + value);
			break;
		case POWER_LAWFUL:
			SetLawful(GetLawful() + value);
			break;
	}
}

float Unit::GetTotalAuraMultiplier(AuraType auratype) const
{
	float multiplier = 1.0f;

	auto auras = GetAurasByType(auratype);
	for (auto iter = auras.begin(); iter != auras.end(); ++iter)
	{
		multiplier *= (100.0f + (*iter)->GetModifier()->m_amount) / 100.0f;
	}

	return multiplier;
}