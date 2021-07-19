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

#include "Database/DatabaseEnv.h"
#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Util.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS] =
{
	&Spell::EffectManaDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
	&Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
	&Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
};

void Spell::EffectManaDrain(SpellEffectIndex eff)
{
}

void Spell::EffectHealthLeech(SpellEffectIndex eff)
{
}

void Spell::EffectHeal(SpellEffectIndex eff)
{
	if (m_unitTarget && m_unitTarget->IsAlive() && m_damage >= 0)
	{
		Unit* caster = GetAffectiveCaster();
		if (!caster)
		{
			return;
		}

		// 获取智力带来的治疗加成
		int32 bonus = caster->GetSpellBonus();
		if (bonus > 10)
		{
			bonus = 10;
		}
		int32 dice = m_proto->EffectBaseDice[eff] + bonus;
		// 治疗量公式就是掷N次骰子
		for (int i = 0; i < dice; ++i)
		{
			m_healing += irand(1, m_proto->EffectBasePoints[eff]);
		}
		// 计算正义值带来的治疗加成
		if (caster->GetLawful())
		{
			m_healing = int32(m_healing * (1.0 + float(caster->GetLawful()) / (MAX_LAWFUL + 1)));
		}

		// 计算光环带来的治疗加成或减免
		m_healing = m_caster->SpellHealingBonusDone(m_unitTarget, m_proto, m_healing, HEAL);
		m_healing = m_unitTarget->SpellHealingBonusTaken(m_caster, m_proto, m_healing, HEAL);
	}
}