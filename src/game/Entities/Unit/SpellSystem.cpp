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

#include "Entities/DynamicObject.h"
#include "Entities/GameObject.h"
#include "Entities/Unit.h"
#include "Items/Item.h"
#include "Maps/Map.h"
#include "Spells/Spell.h"
#include "Spells/SpellAuras.h"
#include "Spells/SpellMgr.h"
#include "ObjectMgr.h"

void Unit::UpdateSpells(uint32 diff)
{
	// SpellAuraHolder::Update���ܻ�ɾ���⻷�����ߣ�Ϊ����Ӱ�����������Ҫm_spellAuraHoldersUpdateIterator
	for (m_spellAuraHoldersUpdateIterator = m_spellAuraHolders.begin(); m_spellAuraHoldersUpdateIterator != m_spellAuraHolders.end();)
	{
		SpellAuraHolder *holder = m_spellAuraHoldersUpdateIterator->second;
		++m_spellAuraHoldersUpdateIterator;
		holder->Update(diff);
	}

	// ɾ���Ѿ����ڵĹ⻷
	for (SpellAuraHolderMap::iterator iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		SpellAuraHolder* holder = iter->second;

		// �־ù⻷�ͱ����⻷�������
		if (!(holder->IsPermanent() || holder->IsPassive()) && holder->GetAuraDuration() == 0)
		{
			RemoveSpellAuraHolder(holder, AURA_REMOVE_BY_EXPIRE);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::CleanupDeletedAuras()
{
	for (SpellAuraHolderList::const_iterator iter = m_deletedHolders.begin(); iter != m_deletedHolders.end(); ++iter)
	{
		delete *iter;
	}
	m_deletedHolders.clear();

	for (AuraList::const_iterator iter = m_deletedAuras.begin(); iter != m_deletedAuras.end(); ++iter)
	{
		delete *iter;
	}
	m_deletedAuras.clear();
}

void Unit::CastSpell(Unit* Victim, const SpellEntry* proto, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, const SpellEntry* triggeredBy)
{
	if (!proto)
	{
		if (triggeredByAura)
		{
			sLog.outError("CastSpell: unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetHolder()->GetSpellProto()->Id, triggeredByAura->GetEffIndex());
		}
		else
		{
			sLog.outError("CastSpell: unknown spell by caster: %s", GetGuidStr().c_str());
		}
		return;
	}

	if (castItem)
	{
		DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", proto->Id);
	}

	if (triggeredByAura)
	{
		if (!originalCaster)
		{
			originalCaster = triggeredByAura->GetHolder()->GetCasterGuid();
		}

		triggeredBy = triggeredByAura->GetHolder()->GetSpellProto();
	}

	Spell* spell = new Spell(this, proto, triggered, originalCaster, triggeredBy);

	SpellCastTargets targets;
	targets.SetUnitTarget(Victim);

	/*if (proto->Targets & TARGET_FLAG_DEST_LOCATION)
	{
		targets.SetDestination(Victim->GetPositionX(), Victim->GetPositionY());
	}

	if (proto->Targets & TARGET_FLAG_SOURCE_LOCATION)
	{
		if (WorldObject* caster = spell->GetCastingObject())
		{
			targets.SetSource(caster->GetPositionX(), caster->GetPositionY());
		}
	}*/

	spell->m_castItem = castItem;
	spell->Prepare(&targets, triggeredByAura);
}

void Unit::CastSpell(Unit* Victim, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, const SpellEntry* triggeredBy)
{
	const SpellEntry* proto = sObjectMgr.GetSpellEntry(spellId);

	if (!proto)
	{
		if (triggeredByAura)
		{
			sLog.outError("CastSpell: unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetHolder()->GetSpellProto()->Id, triggeredByAura->GetEffIndex());
		}
		else
		{
			sLog.outError("CastSpell: unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
		}
		return;
	}

	CastSpell(Victim, proto, triggered, castItem, triggeredByAura, originalCaster, triggeredBy);
}

void Unit::CastSpell(CoordUnit x, CoordUnit y, const SpellEntry* proto, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, const SpellEntry* triggeredBy)
{
	if (!proto)
	{
		if (triggeredByAura)
		{
			sLog.outError("CastSpell(x,y,z): unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetHolder()->GetSpellProto()->Id, triggeredByAura->GetEffIndex());
		}
		else
		{
			sLog.outError("CastSpell(x,y,z): unknown spell by caster: %s", GetGuidStr().c_str());
		}
		return;
	}

	if (castItem)
	{
		DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", proto->Id);
	}

	if (triggeredByAura)
	{
		if (!originalCaster)
		{
			originalCaster = triggeredByAura->GetHolder()->GetCasterGuid();
		}

		triggeredBy = triggeredByAura->GetHolder()->GetSpellProto();
	}

	Spell* spell = new Spell(this, proto, triggered, originalCaster, triggeredBy);

	SpellCastTargets targets;

	/*if (proto->Targets & TARGET_FLAG_DEST_LOCATION)
	{
		targets.SetDestination(x, y);
	}
	if (proto->Targets & TARGET_FLAG_SOURCE_LOCATION)
	{
		targets.SetSource(x, y);
	}*/

	spell->m_castItem = castItem;
	spell->Prepare(&targets, triggeredByAura);
}

void Unit::CastSpell(CoordUnit x, CoordUnit y, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, const SpellEntry* triggeredBy)
{
	const SpellEntry *proto = sObjectMgr.GetSpellEntry(spellId);

	if (!proto)
	{
		if (triggeredByAura)
		{
			sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetHolder()->GetSpellProto()->Id, triggeredByAura->GetEffIndex());
		}
		else
		{
			sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
		}
		return;
	}

	CastSpell(x, y, proto, triggered, castItem, triggeredByAura, originalCaster, triggeredBy);
}

SpellMissInfo Unit::SpellHitResult(Unit* pVictim, const SpellEntry* proto, bool canReflect)
{
	// ���淨���϶�������
	if (IsPositiveSpell(proto))
    {
        return SPELL_MISS_NONE;
    }

	// todo: ���ߴ���

	if (canReflect)
    {
		// todo: ���䴦��
    }

	switch (proto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
            return SPELL_MISS_NONE;
        case SPELL_DAMAGE_CLASS_MAGIC:
            return MagicSpellHitResult(pVictim, proto);
        case SPELL_DAMAGE_CLASS_MELEE:
        case SPELL_DAMAGE_CLASS_RANGED:
            return MeleeSpellHitResult(pVictim, proto);
    }

	return SPELL_MISS_NONE;
}

SpellMissInfo Unit::MeleeSpellHitResult(Unit* pVictim, const SpellEntry *proto)
{
	if (!pVictim->IsAlive())
	{
		return SPELL_MISS_NONE;
	}

	return SPELL_MISS_NONE;
}

SpellMissInfo Unit::MagicSpellHitResult(Unit* pVictim, const SpellEntry *proto)
{
    if (!pVictim->IsAlive())
    {
        return SPELL_MISS_NONE;
    }

    return SPELL_MISS_NONE;
}

void Unit::AddAuraToModifyList(Aura* aura)
{
	if (aura->GetModifier()->m_auraname < TOTAL_AURAS)
	{
		m_modifyAuras[aura->GetModifier()->m_auraname].push_back(aura);
	}
}

bool Unit::AddSpellAuraHolder(SpellAuraHolder* holder)
{
	const SpellEntry* proto = holder->GetSpellProto();
	if (!IsAlive() && !IsDeathPersistentSpell(proto) && (GetTypeId() != TYPEID_PLAYER || !((Player*)this)->GetSession()->PlayerLoading()))
	{
		delete holder;
		return false;
	}

	if (holder->GetTarget() != this)
	{
		sLog.outError("Holder (spell %u) add to spell aura holder list of %s (lowguid: %u) but spell aura holder target is %s (lowguid: %u)",
			holder->GetId(), (GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), GetGUIDLow(),
			(holder->GetTarget()->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), holder->GetTarget()->GetGUIDLow());
		delete holder;
		return false;
	}

	// ͬ����Ч�����Ǵ���
	auto bounds = GetSpellAuraHolderBounds(proto->Id);
	for (auto iter = bounds.first; iter != bounds.second; ++iter)
	{
		SpellAuraHolder* foundHolder = iter->second;
		// ���η�����ʩ������ͬ
		if (foundHolder->GetCasterGuid() == holder->GetCasterGuid())
		{
			if (proto->GetStackAmount())
			{
				// ����foundHolder�Ķѵ�����
				delete holder;
				return false;
			}

			// ��ͬ����Ч��ͬʱ���������һ��
			RemoveSpellAuraHolder(foundHolder, AURA_REMOVE_BY_STACK);
			break;
		}

		bool stop = false;
		for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		{
			// ����Ч��û��ͬʱ��Ч�����������Ǵ���
			if (!foundHolder->m_auras[i] || !holder->m_auras[i])
				continue;

			AuraType type = AuraType(proto->EffectApplyAuraName[i]);
			switch (type)
			{
				case SPELL_AURA_DUMMY:
				case SPELL_AURA_PERIODIC_DAMAGE:
					break;
				default:
					RemoveSpellAuraHolder(foundHolder, AURA_REMOVE_BY_STACK);
					stop = true;
					break;
			}

			if (stop)
			{
				break;
			}
		}
	}

	// ��ӹ⻷������
	holder->Add();
	m_spellAuraHolders.insert(SpellAuraHolderMap::value_type (holder->GetId(), holder));
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (Aura* aura = holder->GetAuraByEffectIndex(SpellEffectIndex(i)))
		{
			AddAuraToModifyList(aura);
		}
	}

	// Ӧ�÷���Ч��
	holder->ApplyAuraModifiers(true, true);
	DEBUG_LOG("Holder of spell %u now is in use", holder->GetId());

	if (holder->IsDeleted())
	{
		return false;
	}

	return true;
}

void Unit::DelaySpellAuraHolder(uint32 spellId, int32 delaytime, ObjectGuid casterGuid)
{
	SpellAuraHolderBounds bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second; ++iter)
	{
		SpellAuraHolder* holder = iter->second;
		if (casterGuid != holder->GetCasterGuid())
		{
			continue;
		}

		if (holder->GetAuraDuration() < delaytime)
		{
			holder->SetAuraDuration(0);
		}
		else
		{
			holder->SetAuraDuration(holder->GetAuraDuration() - delaytime);
		}

		holder->SendAuraUpdate(false);

		DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u partially interrupted on %s, new duration: %u ms", spellId, GetGuidStr().c_str(), holder->GetAuraDuration());
	}
}

void Unit::RemoveAura(Aura* aura, AuraRemoveMode mode)
{
	if (aura->GetModifier()->m_auraname < TOTAL_AURAS)
	{
		m_modifyAuras[aura->GetModifier()->m_auraname].remove(aura);
	}

	// ���ù⻷�Ƴ�ԭ��
	aura->SetRemoveMode(mode);

	DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Aura %u now is remove mode %d", aura->GetModifier()->m_auraname, mode);

	// ɾ���⻷Ч��ǰ�����뽫�⻷�ӹ⻷���������Ƴ�
	aura->GetHolder()->RemoveAura(aura->GetEffIndex());

	// ɾ���⻷Ч��
	aura->ApplyModifier(false, true);

	// ����⻷���ڱ�ʹ�ã����ӳ�ɾ��
	if (aura->IsInUse())
	{
		m_deletedAuras.push_back(aura);
	}
	else
	{
		delete aura;
	}
}

void Unit::RemoveAura(uint32 spellId, SpellEffectIndex eff, Aura* except)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second;)
	{
		Aura *aura = iter->second->m_auras[eff];
		if (aura && aura != except)
		{
			RemoveSingleAuraFromSpellAuraHolder(iter->second, eff);
			bounds = GetSpellAuraHolderBounds(spellId);
			iter = bounds.first;
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveSpellAuraHolder(SpellAuraHolder* holder, AuraRemoveMode mode)
{
	const SpellEntry *proto = holder->GetSpellProto();
	Unit* caster = holder->GetCaster();

	// ˵�����ε�������Unit::UpdateSpells����SpellAuraHolder::Update�Ĺ����з�����
	if (m_spellAuraHoldersUpdateIterator != m_spellAuraHolders.end() && m_spellAuraHoldersUpdateIterator->second == holder)
	{
		++m_spellAuraHoldersUpdateIterator;
	}

	// ����ӹ⻷�������б����Ƴ�
	SpellAuraHolderBounds bounds = GetSpellAuraHolderBounds(holder->GetId());
	for (SpellAuraHolderMap::iterator iter = bounds.first; iter != bounds.second; ++iter)
	{
		if (iter->second == holder)
		{
			m_spellAuraHolders.erase(iter);
			break;
		}
	}

	// �����Ƴ�ԭ��
	holder->SetRemoveMode(mode);

	// ɾ�����еĹ⻷
	for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (Aura* aura = holder->m_auras[i])
		{
			RemoveAura(aura, mode);
		}
	}

	// ����⻷�����������е���Դ��Ϊɾ��������׼��������ɾ�������ķ�����
	holder->Remove();

	// ������������ط���Ҫ���ʹ⻷�����ߵ����ݣ����ӳ�ɾ��
	if (holder->IsInUse())
	{
		holder->SetDeleted();
		m_deletedHolders.push_back(holder);
	}
	else
	{
		delete holder;
	}
}

void Unit::RemoveSingleAuraFromSpellAuraHolder(SpellAuraHolder* holder, SpellEffectIndex eff, AuraRemoveMode mode)
{
	Aura* aura = holder->GetAuraByEffectIndex(eff);
	if (!aura)
	{
		return;
	}

	if (aura->IsLastAuraOnHolder())
	{
		RemoveSpellAuraHolder(holder, mode);
	}
	else
	{
		RemoveAura(aura, mode);
	}
}

void Unit::RemoveSingleAuraFromSpellAuraHolder(uint32 spellId, SpellEffectIndex eff, ObjectGuid casterGuid, AuraRemoveMode mode)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second;)
	{
		Aura* aura = iter->second->m_auras[eff];
		if (aura && aura->GetHolder()->GetCasterGuid() == casterGuid)
		{
			RemoveSingleAuraFromSpellAuraHolder(iter->second, eff, mode);
			bounds = GetSpellAuraHolderBounds(spellId);
			iter = bounds.first;
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasDueToSpell(uint32 spellId, SpellAuraHolder* except, AuraRemoveMode mode)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second;)
	{
		if (iter->second != except)
		{
			RemoveSpellAuraHolder(iter->second, mode);
			bounds = GetSpellAuraHolderBounds(spellId);
			iter = bounds.first;
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second;)
	{
		if (iter->second->GetCastItemGuid() != castItem->GetObjectGuid())
		{
			RemoveSpellAuraHolder(iter->second);
			bounds = GetSpellAuraHolderBounds(spellId);
			iter = bounds.first;
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasByCasterSpell(uint32 spellId, ObjectGuid casterGuid)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second;)
	{
		if (iter->second->GetCasterGuid() == casterGuid)
		{
			RemoveSpellAuraHolder(iter->second);
			bounds = GetSpellAuraHolderBounds(spellId);
			iter = bounds.first;
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasAtMechanicImmunity(uint32 mechMask, uint32 exceptSpellId, bool non_positive)
{
	for (auto iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		const SpellEntry* proto = iter->second->GetSpellProto();
		if (proto->Id == exceptSpellId)
		{
			++iter;
		}
		else if (non_positive && iter->second->IsPositive())
		{
			++iter;
		}
		else if (iter->second->HasMechanicMask(mechMask))
		{
			RemoveAurasDueToSpell(proto->Id);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveSpellsCausingAura(AuraType auraType)
{
	for (auto iter = m_modifyAuras[auraType].begin(); iter != m_modifyAuras[auraType].end();)
	{
		RemoveAurasDueToSpell((*iter)->GetHolder()->GetId());
		iter = m_modifyAuras[auraType].begin();
	}
}

void Unit::RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except)
{
	for (auto iter = m_modifyAuras[auraType].begin(); iter != m_modifyAuras[auraType].end();)
	{
		if ((*iter)->GetHolder() == except)
		{
			++iter;
			continue;
		}

		RemoveAurasDueToSpell((*iter)->GetHolder()->GetId());
		iter = m_modifyAuras[auraType].begin();
	}
}

void Unit::RemoveSpellsCausingAura(AuraType auraType, ObjectGuid casterGuid)
{
	for (auto iter = m_modifyAuras[auraType].begin(); iter != m_modifyAuras[auraType].end();)
	{
		if ((*iter)->GetHolder()->GetCasterGuid() == casterGuid)
		{
			RemoveAurasDueToSpell((*iter)->GetHolder()->GetId());
			iter = m_modifyAuras[auraType].begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasWithInterruptFlags(uint32 flags)
{
	for (auto iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		if (iter->second->GetSpellProto()->AuraInterruptFlags & flags)
		{
			RemoveSpellAuraHolder(iter->second);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasWithAttribute(uint32 flags)
{
	for (auto iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		if (iter->second->GetSpellProto()->HasAttribute((SpellAttributes)flags))
		{
			RemoveSpellAuraHolder(iter->second);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAurasWithDispelType(DispelType type, ObjectGuid casterGuid)
{
	uint32 mask = GetDispellMask(type);
	for (auto iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		if (((1 << iter->second->GetSpellProto()->Dispel) & mask) && (!casterGuid || casterGuid == iter->second->GetCasterGuid()))
		{
			RemoveSpellAuraHolder(iter->second);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

void Unit::RemoveAllAuras(AuraRemoveMode mode)
{
	while (!m_spellAuraHolders.empty())
	{
		auto iter = m_spellAuraHolders.begin();
		RemoveSpellAuraHolder(iter->second, mode);
	}
}

void Unit::RemoveAllAurasOnDeath()
{
	for (auto iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
	{
		if (!iter->second->IsPassive() && !iter->second->IsDeathPersistent())
		{
			RemoveSpellAuraHolder(iter->second, AURA_REMOVE_BY_DEATH);
			iter = m_spellAuraHolders.begin();
		}
		else
		{
			++iter;
		}
	}
}

uint32 Unit::GetCooldown() const
{
	return m_cooldown;
}

void Unit::ResetCooldown(uint32 gcd)
{
	m_cooldown = gcd;
}

SpellAuraHolder* Unit::GetVisibleAura(uint8 slot) const
{
	auto iter = m_visibleAuras.find(slot);
	return (iter != m_visibleAuras.end()) ? iter->second : nullptr;
}

void Unit::SetVisibleAura(uint8 slot, SpellAuraHolder* holder)
{
	if (!holder)
	{
		m_visibleAuras.erase(slot);
	}
	else
	{
		m_visibleAuras[slot] = holder;
	}
}

SpellAuraHolder* Unit::GetSpellAuraHolder(uint32 spellId) const
{
	auto iter = m_spellAuraHolders.find(spellId);
	return (iter != m_spellAuraHolders.end()) ? iter->second : nullptr;
}

SpellAuraHolder* Unit::GetSpellAuraHolder(uint32 spellId, ObjectGuid casterGUID) const
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second; ++iter)
		if (iter->second->GetCasterGuid() == casterGUID)
			return iter->second;
	return nullptr;
}

Aura* Unit::GetAura(uint32 spellId, SpellEffectIndex eff)
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	if (bounds.first != bounds.second)
	{
		return bounds.first->second->GetAuraByEffectIndex(eff);
	}

	return nullptr;
}

Aura* Unit::GetAura(AuraType type, SpellFamily family, ObjectGuid casterGuid)
{
	return nullptr;
}

bool Unit::HasAura(AuraType auraType) const
{
	return !GetAurasByType(auraType).empty();
}

bool Unit::HasAura(uint32 spellId, SpellEffectIndex eff) const
{
	auto bounds = GetSpellAuraHolderBounds(spellId);
	for (auto iter = bounds.first; iter != bounds.second; ++iter)
	{
		if (iter->second->GetAuraByEffectIndex(eff))
		{
			return true;
		}
	}

	return false;
}

bool Unit::HasAura(uint32 spellId) const
{
	return m_spellAuraHolders.find(spellId) != m_spellAuraHolders.end();
}

bool Unit::HasSpell(uint32 spellId) const
{
	return false;
}

bool Unit::IsBrave() const
{
	if (GetClass() == CLASS_ELF)
	{
		return HasAura(STATUS_ELFBRAVE);
	}
	else
	{
		return HasAura(STATUS_BRAVE);
	}
}

bool Unit::IsFastMovable() const
{
	return HasAura(HOLY_WALK) || HasAura(MOVING_ACCELERATION) || HasAura(WIND_WALK) || HasAura(STATUS_RIBRAVE);
}

bool Unit::IsFrozen() const
{
	return false;
}

bool Unit::IsInvisble() const
{
	return HasAura(SPELL_AURA_MOD_INVISIBILITY);
}

bool Unit::IsPolymorphed() const
{
	return false;
}

uint32 Unit::SpellHealingBonusDone(Unit* pVictim, const SpellEntry* proto, int32 healamount, DamageEffectType damagetype, uint32 stack)
{
	return healamount;
}

uint32 Unit::SpellHealingBonusTaken(Unit* pCaster, const SpellEntry* proto, int32 healamount, DamageEffectType damagetype, uint32 stack)
{
	const AuraList& auras = GetAurasByType(SPELL_AURA_MOD_HEALING_PCT);
	float coeffs = 1.0f;
	for (auto iter = auras.begin(); iter != auras.end(); ++iter)
	{
		coeffs *= (100.0f + (*iter)->GetModifier()->m_amount) / 100.0f;
	}
	float heal = healamount * coeffs;
	return heal < 0 ? 0 : uint32(heal);
}

DynamicObject* Unit::GetDynObject(uint32 spellId, SpellEffectIndex effIndex)
{
	for (auto iter = m_dynObjGUIDs.begin(); iter != m_dynObjGUIDs.end();)
	{
		DynamicObject* dynObj = GetMap()->GetDynamicObject(*iter);
		if (!dynObj)
		{
			iter = m_dynObjGUIDs.erase(iter);
			continue;
		}

		if (dynObj->GetSpellId() == spellId && dynObj->GetEffIndex() == effIndex)
		{
			return dynObj;
		}

		++iter;
	}

	return nullptr;
}

DynamicObject* Unit::GetDynObject(uint32 spellId)
{
	for (auto iter = m_dynObjGUIDs.begin(); iter != m_dynObjGUIDs.end();)
	{
		DynamicObject* dynObj = GetMap()->GetDynamicObject(*iter);
		if (!dynObj)
		{
			iter = m_dynObjGUIDs.erase(iter);
			continue;
		}

		if (dynObj->GetSpellId() == spellId)
		{
			return dynObj;
		}

		++iter;
	}

	return nullptr;
}

void Unit::AddDynObject(DynamicObject* dynObj)
{
	m_dynObjGUIDs.push_back(dynObj->GetObjectGuid());
}

void Unit::RemoveDynObject(uint32 spellId)
{
	if (m_dynObjGUIDs.empty())
		return;

	for (auto iter = m_dynObjGUIDs.begin(); iter != m_dynObjGUIDs.end();)
	{
		DynamicObject* dynObj = GetMap()->GetDynamicObject(*iter);
		if (!dynObj)
		{
			iter = m_dynObjGUIDs.erase(iter);
			continue;
		}

		if (dynObj->GetSpellId() == spellId)
		{
			dynObj->Delete();
			iter = m_dynObjGUIDs.erase(iter);
			continue;
		}

		++iter;
	}

}

void Unit::RemoveDynObjectWithGUID(ObjectGuid guid)
{
	m_dynObjGUIDs.remove(guid);
}

void Unit::RemoveAllDynObjects()
{
	while (!m_dynObjGUIDs.empty())
	{
		if (DynamicObject* dynObj = GetMap()->GetDynamicObject(*m_dynObjGUIDs.begin()))
		{
			dynObj->Delete();
		}
		m_dynObjGUIDs.erase(m_dynObjGUIDs.begin());
	}
}

GameObject* Unit::GetGameObject(uint32 spellId) const
{
	for (auto iter = m_gameObj.begin(); iter != m_gameObj.end(); ++iter)
	{
		if ((*iter)->GetSpellId() == spellId)
		{
			return *iter;
		}
	}

	WildGameObjectMap::const_iterator find = m_wildGameObjs.find(spellId);
	if (find != m_wildGameObjs.end())
	{
		return GetMap()->GetGameObject(find->second);
	}

	return nullptr;
}

void Unit::AddGameObject(GameObject* gameObj)
{
	MANGOS_ASSERT(gameObj && !gameObj->GetOwnerGuid());
	m_gameObj.push_back(gameObj);
	gameObj->SetOwnerGuid(GetObjectGuid());
}

void Unit::AddWildGameObject(GameObject* gameObj)
{
	MANGOS_ASSERT(gameObj && gameObj->GetOwnerGuid().IsEmpty());
	m_wildGameObjs[gameObj->GetSpellId()] = gameObj->GetObjectGuid();

	for (auto iter = m_wildGameObjs.begin(); iter != m_wildGameObjs.end();)
	{
		GameObject* pGo = GetMap()->GetGameObject(iter->second);
		if (pGo)
		{
			++iter;
		}
		else
		{
			m_wildGameObjs.erase(iter++);
		}
	}
}

void Unit::RemoveGameObject(GameObject* gameObj, bool del)
{
	MANGOS_ASSERT(gameObj && gameObj->GetOwnerGuid() == GetObjectGuid());

	gameObj->SetOwnerGuid(ObjectGuid());

	// ���GameObject����
	if (uint32 spellId = gameObj->GetSpellId())
	{
		RemoveAurasDueToSpell(spellId);
	}

	m_gameObj.remove(gameObj);

	if (del)
	{
		gameObj->Delete();
	}
}

void Unit::RemoveGameObject(uint32 spellId, bool del)
{
	if (m_gameObj.empty())
	{
		return;
	}

	for (auto iter = m_gameObj.begin(), next = iter; iter != m_gameObj.end(); iter = next)
	{
		next = iter;
		if (spellId == 0 || (*iter)->GetSpellId() == spellId)
		{
			(*iter)->SetOwnerGuid(ObjectGuid());
			if (del)
			{
				(*iter)->Delete();
			}
			next = m_gameObj.erase(iter);
		}
		else
		{
			++next;
		}
	}
}

void Unit::RemoveAllGameObjects()
{
	// ɾ������
	for (auto iter = m_gameObj.begin(); iter != m_gameObj.end();)
	{
		(*iter)->SetOwnerGuid(ObjectGuid());
		(*iter)->Delete();
		iter = m_gameObj.erase(iter);
	}

	// ��ɾ�����ã���ɾ������
	m_wildGameObjs.clear();
}
