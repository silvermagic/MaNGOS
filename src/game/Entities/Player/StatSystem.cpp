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
#include "Entities/CalcStat.h"
#include "Entities/Player.h"
#include "Packets/S_MPUpdate.hpp"
#include "Packets/S_HPUpdate.hpp"
#include "SQLStorages.h"

void Player::Refresh()
{
	UpdateLevel();

	// 命中、伤害
	UpdateBaseAttackHit();
	UpdateBaseAttackDamage();
	UpdateCreateAttackHit();
	UpdateCreateAttackDamage();
	UpdateCreateMagicHit();
	UpdateCreateMagicDamage();
	UpdateCreateMagicCritical();

	// 防御
	UpdateBaseArmor();
	UpdateCreateArmor();

	// 血量成长、魔量成长
	UpdateCreateManaGrowth();
	UpdateCreateHealthGrowth();

	// 体力回复量、魔力回复量
	UpdateCreateManaRegen();
	UpdateCreateHealthRegen();

	// 魔法防御、魔法攻击
	UpdateBaseDefense();
	UpdateCreateDefense();
	UpdateBaseSpellPower();

	// 闪避
	UpdateBaseDodge();
	UpdateCreateDodge();

	// 重量减免、魔法消耗减免
	UpdateCreateWeightReduction();
	UpdateCreateMagicCostReduction();
}

void Player::UpdateLevel()
{
	SetUInt32Value(UNIT_FIELD_LEVEL, boost::algorithm::clamp(CalcLevel(GetUInt32Value(PLAYER_FIELD_XP)), 1, 127));
	// todo: 体力回复
}

void Player::SetMana(int16 value, bool direct)
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
	S_MPUpdate pkt(m_mana, m_maxMana);
	m_session->SendPacket(&pkt);
}

void Player::SetHealth(int16 value, bool direct)
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
	S_HPUpdate pkt(m_health, m_maxHealth);
	m_session->SendPacket(&pkt);
}

int8 Player::GetCreateStat(Stats stat)
{
	return m_createStat[stat];
}

int8 Player::GetBaseStat(Stats stat)
{
	return m_baseStat[stat];
}

void Player::ModifyBaseStat(Stats stat, int32 diff)
{
	if (diff == 0)
	{
		return;
	}
	int8 value = boost::algorithm::clamp(m_baseStat[stat] + diff, 1, 127);
	ModifyStat(stat, value - m_baseStat[stat]);
	m_baseStat[stat] = value;
}

void Player::UpdateBaseAttackHit()
{
	int8 value = 0;
	switch (GetClass()) {
		case CLASS_PRINCE:
			value = int8(GetLevel() / 5);
			break;
		case CLASS_KNIGHT:
			value = int8(GetLevel() / 3);
			break;
		case CLASS_ELF:
			value = int8(GetLevel() / 5);
			break;
		case CLASS_WIZARD:
			break;
		case CLASS_DARK_ELF:
			value = int8(GetLevel() / 3);
			break;
		case CLASS_DRAGON_KNIGHT:
			value = int8(GetLevel() / 3);
			break;
		case CLASS_ILLUSIONIST:
			value = int8(GetLevel() / 5);
			break;
		default:
			value = 0;
	}
	m_baseAttackHit[MELEE_ATTACK] = value;
	m_baseAttackHit[RANGED_ATTACK] = value;
}

void Player::UpdateBaseAttackDamage()
{
	int8 dmg = 0;
	if (GetClass() == CLASS_KNIGHT || GetClass() == CLASS_DARK_ELF || GetClass() == CLASS_DRAGON_KNIGHT)
	{
		dmg = int8(GetLevel() / 10);
	}
	ModifyAttackDamage(MELEE_ATTACK, dmg - m_baseAttackDamage[MELEE_ATTACK]);
	m_baseAttackDamage[MELEE_ATTACK] = dmg;

	dmg = 0;
	if (GetClass() == CLASS_ELF)
	{
		dmg = int8(GetLevel() / 10);
	}
	ModifyAttackDamage(RANGED_ATTACK, dmg - m_baseAttackDamage[RANGED_ATTACK]);
	m_baseAttackDamage[RANGED_ATTACK] = dmg;
}

void Player::UpdateCreateAttackHit()
{
	int8 value = 0;
	int8 strength = m_createStat[STAT_STRENGTH];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (15 <= strength && strength <= 17) {
				value += 1;
			} else if (18 <= strength) {
				value += 2;
			}
			break;
		case CLASS_KNIGHT:
			if (strength == 18 || strength == 19) {
				value += 2;
			} else if (strength == 20) {
				value += 4;
			}
			break;
		case CLASS_ELF:
			if (strength == 12 || strength == 13) {
				value += 1;
			} else if (14 <= strength) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (strength == 10 || strength == 11) {
				value += 1;
			} else if (12 <= strength) {
				value += 2;
			}
			break;
		case CLASS_DARK_ELF:
			if (14 <= strength && strength <= 17) {
				value += 1;
			} else if (strength == 18) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (15 <= strength && strength <= 17) {
				value += 1;
			} else if (18 <= strength) {
				value += 3;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (strength == 13 || strength == 14) {
				value += 1;
			} else if (15 <= strength) {
				value += 2;
			}
			break;
		default:
			value += 0;
	}
	m_createAttackHit[MELEE_ATTACK] = value;

	value = 0;
	int8 agility = m_createStat[STAT_AGILITY];
	switch (GetClass()) {
		case CLASS_PRINCE:
			break;
		case CLASS_KNIGHT:
			break;
		case CLASS_ELF:
			if (13 <= agility && agility <= 15) {
				value += 2;
			} else if (16 <= agility) {
				value += 3;
			}
			break;
		case CLASS_WIZARD:
			break;
		case CLASS_DARK_ELF:
			if (agility == 17) {
				value += 1;
			} else if (agility == 18) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			break;
		case CLASS_ILLUSIONIST:
			break;
		default:
			value += 0;
	}
	m_createAttackHit[RANGED_ATTACK] = value;
}


void Player::UpdateCreateAttackDamage()
{
	int8 value = 0;
	int8 strength = m_createStat[STAT_STRENGTH];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (15 <= strength && strength <= 17) {
				value += 1;
			} else if (18 <= strength) {
				value += 2;
			}
			break;
		case CLASS_KNIGHT:
			if (strength == 18 || strength == 19) {
				value += 2;
			} else if (strength == 20) {
				value += 4;
			}
			break;
		case CLASS_ELF:
			if (strength == 12 || strength == 13) {
				value += 1;
			} else if (14 <= strength) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (strength == 10 || strength == 11) {
				value += 1;
			} else if (12 <= strength) {
				value += 2;
			}
			break;
		case CLASS_DARK_ELF:
			if (14 <= strength && strength <= 17) {
				value += 1;
			} else if (strength == 18) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (15 <= strength && strength <= 17) {
				value += 1;
			} else if (18 <= strength) {
				value += 3;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (strength == 13 || strength == 14) {
				value += 1;
			} else if (15 <= strength) {
				value += 2;
			}
			break;
		default:
			value += 0;
	}
	m_createAttackDamage[MELEE_ATTACK] = value;

	value = 0;
	int32 agility = m_createStat[STAT_AGILITY];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (13 <= agility) {
				value += 1;
			}
			break;
		case CLASS_KNIGHT:
			break;
		case CLASS_ELF:
			if (14 <= agility && agility <= 16) {
				value += 2;
			} else if (17 <= agility) {
				value += 3;
			}
			break;
		case CLASS_WIZARD:
			break;
		case CLASS_DARK_ELF:
			if (agility == 18) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			break;
		case CLASS_ILLUSIONIST:
			break;
		default:
			value += 0;
	}
	m_createAttackDamage[RANGED_ATTACK] = value;
}

void Player::UpdateCreateMagicHit()
{
	int8 value = 0;
	int8 intellect = m_createStat[STAT_INTELLECT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (intellect == 12 || intellect == 13) {
				value += 1;
			} else if (14 <= intellect) {
				value += 2;
			}
			break;
		case CLASS_KNIGHT:
			if (intellect == 10 || intellect == 11) {
				value += 1;
			} else if (intellect == 12) {
				value += 2;
			}
			break;
		case CLASS_ELF:
			if (intellect == 13 || intellect == 14) {
				value += 1;
			} else if (15 <= intellect) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (intellect >= 14) {
				value += 1;
			}
			break;
		case CLASS_DARK_ELF:
			if (intellect == 12 || intellect == 13) {
				value += 1;
			} else if (14 <= intellect) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (intellect == 12 || intellect == 13) {
				value += 2;
			} else if (intellect == 14 || intellect == 15) {
				value += 3;
			} else if (16 <= intellect) {
				value += 4;
			}
		case CLASS_ILLUSIONIST:
			if (13 <= intellect) {
				value += 1;
			}
		default:
			break;
	}
	m_createMagicHit = value;
}

void Player::UpdateCreateMagicDamage()
{
	int8 value = 0;
	int8 intellect = m_createStat[STAT_INTELLECT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			break;
		case CLASS_KNIGHT:
			break;
		case CLASS_ELF:
			break;
		case CLASS_WIZARD:
			if (13 <= intellect) {
				value += 1;
			}
			break;
		case CLASS_DARK_ELF:
			break;
		case CLASS_DRAGON_KNIGHT:
			if (intellect == 13 || intellect == 14) {
				value += 1;
			} else if (intellect == 15 || intellect == 16) {
				value += 2;
			} else if (intellect == 17) {
				value += 3;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (intellect == 16) {
				value += 1;
			} else if (intellect == 17) {
				value += 2;
			}
			break;
		default:
			break;
	}
	m_createMagicDamage = value;
}

void Player::UpdateCreateMagicCritical()
{
	int8 value = 0;
	int8 intellect = m_createStat[STAT_INTELLECT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			break;
		case CLASS_KNIGHT:
			break;
		case CLASS_ELF:
			if (intellect != 14 && intellect != 15) {
				if (16 <= intellect) {
					value += 4;
				}
			} else {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (intellect == 15) {
				value += 2;
			} else if (intellect == 16) {
				value += 4;
			} else if (intellect == 17) {
				value += 6;
			} else if (intellect == 18) {
				value += 8;
			}
			break;
		case CLASS_DARK_ELF:
			break;
		case CLASS_DRAGON_KNIGHT:
			break;
		case CLASS_ILLUSIONIST:
			break;
		default:
			break;
	}
	m_createMagicCritical = value;
}

void Player::UpdateBaseArmor()
{
	int ac = MaNGOS::CalcStatArmor(GetLevel(), m_baseStat[STAT_AGILITY]);
	ModifyArmor(ac - m_baseArmor);
	m_baseArmor = ac;
}

void Player::UpdateCreateArmor()
{
	int8 value = 0;
	int8 agility = m_createStat[STAT_AGILITY];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (12 <= agility && agility <= 14) {
				value += 1;
			} else if (agility != 15 && agility != 16) {
				if (17 <= agility) {
					value += 3;
				}
			} else {
				value += 2;
			}
			break;
		case CLASS_KNIGHT:
			if (agility != 13 && agility != 14) {
				if (15 <= agility) {
					value += 3;
				}
			} else {
				value += 1;
			}
			break;
		case CLASS_ELF:
			if (15 <= agility && agility <= 17) {
				value += 1;
			} else if (18 == agility) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (agility != 8 && agility != 9) {
				if (10 <= agility) {
					value += 2;
				}
			} else {
				value += 1;
			}
			break;
		case CLASS_DARK_ELF:
			if (17 <= agility) {
				value += 1;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (agility != 12 && agility != 13) {
				if (14 <= agility) {
					value += 2;
				}
			} else {
				value += 1;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (agility != 11 && agility != 12) {
				if (13 <= agility) {
					value += 2;
				}
			} else {
				value += 1;
			}
			break;
		default:
			value += 0;
	}
	m_createArmor = value;

	ModifyArmor(0 - m_createArmor);
}

void Player::ModifyBaseMaxMana(int32 diff)
{
	if (diff == 0)
	{
		return;
	}
	int16 value = boost::algorithm::clamp(m_baseMaxMana + diff, 0, 32767);
	ModifyMaxMana(value - m_baseMaxMana);
	m_baseMaxMana = value;
}

void Player::ModifyBaseMaxHealth(int32 diff)
{
	if (diff == 0)
	{
		return;
	}
	int16 value = boost::algorithm::clamp(m_baseMaxHealth + diff, 0, 32767);
	ModifyMaxHealth(value - m_baseMaxHealth);
	m_baseMaxHealth = value;
}

void Player::UpdateCreateManaGrowth()
{
	int8 value = 0;
	int8 spirit = m_createStat[STAT_SPIRIT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (16 <= spirit) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_KNIGHT:
			value += 0;
			break;
		case CLASS_ELF:
			if (14 <= spirit && spirit <= 16) {
				value += 1;
			} else if (17 <= spirit) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_WIZARD:
			if (13 <= spirit && spirit <= 16) {
				value += 1;
			} else if (17 <= spirit) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_DARK_ELF:
			if (12 <= spirit) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
		case CLASS_ILLUSIONIST:
			if (13 <= spirit && spirit <= 15) {
				value += 1;
			} else if (16 <= spirit) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		default:
			value += 0;
	}
	m_createManaGrowth = value;
}

void Player::UpdateCreateHealthGrowth()
{
	int8 value = 0;
	int8 stamina = m_createStat[STAT_STAMINA];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (stamina == 12 || stamina == 13) {
				value += 1;
			} else if (stamina == 14 || stamina == 15) {
				value += 2;
			} else if (16 <= stamina) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_KNIGHT:
			if (stamina == 15 || stamina == 16) {
				value += 1;
			} else if (17 <= stamina) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_ELF:
			if (15 <= stamina && stamina <= 17) {
				value += 1;
			} else if (stamina == 18) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_WIZARD:
			if (stamina == 14 || stamina == 15) {
				value += 1;
			} else if (16 <= stamina) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_DARK_ELF:
			if (stamina == 10 || stamina == 11) {
				value += 1;
			} else if (12 <= stamina) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (stamina == 15 || stamina == 16) {
				value += 1;
			} else if (17 <= stamina) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (stamina == 13 || stamina == 14) {
				value += 1;
			} else if (15 <= stamina) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		default:
			value += 0;
	}

	m_createHealthGrowth = value;
}

int8 Player::GetManaRegen() const
{
	return m_manaRegen;
}

void Player::ModifyManaRegen(int32 diff)
{
	if (diff == 0)
	{
		return;
	}
	m_totalManaRegen += diff;
	m_manaRegen = std::max(0, m_totalManaRegen);
}

int8 Player::GetHealthRegen() const
{
	return m_healthRegen;
}

void Player::ModifyHealthRegen(int32 diff)
{
	if (diff == 0)
	{
		return;
	}
	m_totalHealthRegen += diff;
	m_healthRegen = std::max(0, m_totalHealthRegen);
}

void Player::UpdateCreateManaRegen()
{
	int8 value = 0;
	int8 spirit = m_createStat[STAT_SPIRIT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (spirit == 13 || spirit == 14) {
				value += 1;
			} else if (15 <= spirit) {
				value += 2;
			}
			break;
		case CLASS_KNIGHT:
			if (spirit == 11 || spirit == 12) {
				value += 1;
			} else if (spirit == 13) {
				value += 2;
			}
			break;
		case CLASS_ELF:
			if (15 <= spirit && spirit <= 17) {
				value += 1;
			} else if (spirit == 18) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (spirit == 14 || spirit == 15) {
				value += 1;
			} else if (spirit == 16 || spirit == 17) {
				value += 2;
			} else if (spirit == 18) {
				value += 3;
			}
			break;
		case CLASS_DARK_ELF:
			if (13 <= spirit) {
				value += 1;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (spirit == 15 || spirit == 16) {
				value += 1;
			} else if (17 <= spirit) {
				value += 2;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (14 <= spirit && spirit <= 16) {
				value += 1;
			} else if (17 <= spirit) {
				value += 2;
			}
			break;
		default:
			value += 0;
	}
	m_createManaRegen = value;
}

void Player::UpdateCreateHealthRegen()
{
	int8 value = 0;
	int8 stamina = m_createStat[STAT_STAMINA];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (stamina == 13 || stamina == 14) {
				value += 1;
			} else if (stamina == 15 || stamina == 16) {
				value += 2;
			} else if (stamina == 17) {
				value += 3;
			} else if (stamina == 18) {
				value += 4;
			}
			break;
		case CLASS_KNIGHT:
			if (stamina == 16 || stamina == 17) {
				value += 2;
			} else if (stamina == 18) {
				value += 4;
			}
			break;
		case CLASS_ELF:
			if (stamina == 14 || stamina == 15) {
				value += 1;
			} else if (stamina == 16) {
				value += 2;
			} else if (17 <= stamina) {
				value += 3;
			}
			break;
		case CLASS_WIZARD:
			if (stamina == 17) {
				value += 1;
			} else if (stamina == 18) {
				value += 2;
			}
			break;
		case CLASS_DARK_ELF:
			if (stamina == 11 || stamina == 12) {
				value += 1;
			} else if (13 <= stamina) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (stamina == 16 || stamina == 17) {
				value += 1;
			} else if (stamina == 18) {
				value += 3;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (stamina == 14 || stamina == 15) {
				value += 1;
			} else if (16 <= stamina) {
				value += 2;
			}
			break;
		default:
			value += 0;
	}
	m_createHealthRegen = value;
}

int16 Player::GetBaseDefense() const
{
	return m_baseDefense;
}

int16 Player::GetBaseSpellPower() const
{
	return m_baseSp;
}

void Player::UpdateBaseDefense()
{
	int16 value = 0;
	switch (GetClass()) {
		case CLASS_PRINCE:
			value += 10;
			break;
		case CLASS_KNIGHT:
			break;
		case CLASS_ELF:
			value += 25;
			break;
		case CLASS_WIZARD:
			value += 15;
			break;
		case CLASS_DARK_ELF:
			value += 10;
			break;
		case CLASS_DRAGON_KNIGHT:
			value += 18;
			break;
		case CLASS_ILLUSIONIST:
			value += 20;
			break;
		default:
			value += 0;
	}

	int8 spirit = GetStat(STAT_SPIRIT);
	value += MaNGOS::CalcStatMagicDefense(spirit);
	value += GetLevel() / 2;
	ModifyDefense(SPELL_SCHOOL_NORMAL, value - m_baseDefense);
	m_baseDefense = value;
}

void Player::UpdateCreateDefense()
{
	int16 value = 0;
	int8 spirit = m_createStat[STAT_SPIRIT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (spirit != 12 && spirit != 13) {
				if (15 <= spirit) {
					value += 2;
				}
			} else {
				value += 1;
			}
			break;
		case CLASS_KNIGHT:
			if (spirit != 10 && spirit != 11) {
				if (12 <= spirit) {
					value += 2;
				}
			} else {
				value += 1;
			}
			break;
		case CLASS_ELF:
			if (13 <= spirit && spirit <= 15) {
				value += 1;
			} else if (16 <= spirit) {
				value += 2;
			}
			break;
		case CLASS_WIZARD:
			if (15 <= spirit) {
				value += 1;
			}
			break;
		case CLASS_DARK_ELF:
			if (11 <= spirit && spirit <= 13) {
				value += 1;
			} else if (spirit == 14) {
				value += 2;
			} else if (spirit == 15) {
				value += 3;
			} else if (16 <= spirit) {
				value += 4;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (14 <= spirit) {
				value += 1;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (15 <= spirit && spirit <= 17) {
				value += 2;
			} else if (spirit == 18) {
				value += 4;
			}
			break;
		default:
			value += 0;
	}
	m_createDefense = value;
}

void Player::UpdateBaseDodge()
{
	int16 value = 0;
	switch (GetClass()) {
		case CLASS_PRINCE:
			value += GetLevel() / 8;
			break;
		case CLASS_KNIGHT:
			value += GetLevel() / 4;
			break;
		case CLASS_ELF:
			value += GetLevel() / 8;
			break;
		case CLASS_WIZARD:
			value += GetLevel() / 10;
			break;
		case CLASS_DARK_ELF:
			value += GetLevel() / 6;
			break;
		case CLASS_DRAGON_KNIGHT:
			value += GetLevel() / 7;
			break;
		case CLASS_ILLUSIONIST:
			value += GetLevel() / 9;
			break;
		default:
			value += 0;
	}

	value += (GetStat(STAT_AGILITY) - 8) / 2;
	ModifyDodge(value - m_baseDodge);
	m_baseDodge = value;
}

void Player::UpdateCreateDodge()
{
	int16 value = 0;
	int8 agility = m_createStat[STAT_AGILITY];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (agility == 14 || agility == 15) {
				value += 1;
			} else if (agility == 16 || agility == 17) {
				value += 2;
			} else if (agility == 18) {
				value += 3;
			}
			break;
		case CLASS_KNIGHT:
			if (agility == 14 || agility == 15) {
				value += 1;
			} else if (agility == 16) {
				value += 3;
			}
			break;
		case CLASS_ELF:
			break;
		case CLASS_WIZARD:
			if (agility == 9 || agility == 10) {
				value += 1;
			} else if (agility == 11) {
				value += 2;
			}
			break;
		case CLASS_DARK_ELF:
			if (16 <= agility) {
				value += 2;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (agility == 13 || agility == 14) {
				value += 1;
			} else if (15 <= agility) {
				value += 2;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (agility == 12 || agility == 13) {
				value += 1;
			} else if (14 <= agility) {
				value += 2;
			}
			break;
		default:
			value += 0;
	}
	m_createDodge = value;

	ModifyDodge(m_createDodge);
}

void Player::UpdateCreateWeightReduction()
{
	int8 value = 0;
	int8 strength = m_createStat[STAT_STRENGTH];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (14 <= strength && strength <= 16) {
				value += 1;
			} else if (17 <= strength && strength <= 19) {
				value += 2;
			} else if (strength == 20) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_KNIGHT:
			value += 0;
			break;
		case CLASS_ELF:
			if (16 <= strength) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_WIZARD:
			if (9 <= strength) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_DARK_ELF:
			if (13 <= strength && strength <= 15) {
				value += 2;
			} else if (16 <= strength) {
				value += 3;
			} else {
				value += 0;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			if (16 <= strength) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_ILLUSIONIST:
			if (strength == 18) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		default:
			value += 0;
	}
	m_createStrengthWeightReduction = value;

	value = 0;
	int8 stamina = m_createStat[STAT_STAMINA];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (11 <= stamina) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_KNIGHT:
			if (15 <= stamina) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_ELF:
			if (15 <= stamina) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_WIZARD:
			if (strength == 13 || strength == 14) {
				value += 1;
			} else if (15 <= strength) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_DARK_ELF:
			if (9 <= stamina) {
				value += 1;
			} else {
				value += 0;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			value += 0;
			break;
		case CLASS_ILLUSIONIST:
			if (stamina == 17) {
				value += 1;
			} else if (stamina == 18) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		default:
			value += 0;
	}
	m_createStaminaWeightReduction = value;
}

void Player::UpdateCreateMagicCostReduction()
{
	int8 value = 0;
	int8 intellect = m_createStat[STAT_INTELLECT];
	switch (GetClass()) {
		case CLASS_PRINCE:
			if (intellect == 11 || intellect == 12) {
				value += 1;
			} else if (13 <= intellect) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_KNIGHT:
			if (intellect == 9 || intellect == 10) {
				value += 1;
			} else if (11 <= intellect) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_ELF:
			value += 0;
			break;
		case CLASS_WIZARD:
			value += 0;
			break;
		case CLASS_DARK_ELF:
			if (intellect == 13 || intellect == 14) {
				value += 1;
			} else if (15 <= intellect) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		case CLASS_DRAGON_KNIGHT:
			value += 0;
			break;
		case CLASS_ILLUSIONIST:
			if (intellect == 14) {
				value += 1;
			} else if (15 <= intellect) {
				value += 2;
			} else {
				value += 0;
			}
			break;
		default:
			value += 0;
	}
	m_createMagicCostReduction = value;
}

int8 Player::GetDamageReduction() const
{
	return m_damageReduction;
}

void Player::ModifyDamageReduction(int8 diff)
{
	m_damageReduction += diff;
}

int8 Player::GetWeightReduction() const
{
	return m_weightReduction;
}

void Player::ModifyWeightReduction(int8 diff)
{
	m_weightReduction += diff;
}