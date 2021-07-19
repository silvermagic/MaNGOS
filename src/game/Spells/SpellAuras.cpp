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

#include "Utilities/Timer.h"
#include "Entities/DynamicObject.h"
#include "Entities/Player.h"
#include "Entities/Unit.h"
#include "Items/Item.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"

#define NULL_AURA_SLOT 0xFF

extern pAuraHandler AuraHandler[TOTAL_AURAS];

SpellAuraHolder::SpellAuraHolder(const SpellEntry* proto, Unit* target, WorldObject* caster, Item* castItem) :
	m_spellProto(proto), m_target(target), m_castItemGuid(castItem ? castItem->GetObjectGuid() : ObjectGuid()),
	m_auraSlot(MAX_AURAS), m_procCharges(0), m_removeMode(AURA_REMOVE_BY_DEFAULT), m_isPermanent(false),
	m_deleted(false),
	m_in_use(0)
{
	MANGOS_ASSERT(target);
	MANGOS_ASSERT(proto && proto == sObjectMgr.GetSpellEntry(proto->Id) && "`info` must be pointer to sSpellStore element");

	if (!caster)
	{
		m_casterGuid = target->GetObjectGuid();
	}
	else
	{
		MANGOS_ASSERT(caster->isType(TYPEMASK_UNIT))
		m_casterGuid = caster->GetObjectGuid();
	}

	m_applyTime = time(nullptr);
	m_isPassive = IsPassiveSpell(proto);
    m_isDeathPersist = IsDeathPersistentSpell(proto);
	m_procCharges = proto->ProcCharges;

	m_duration = m_maxDuration = proto->Duration;
	// 判断是否为永久型法术
	if (m_maxDuration == -1 || (m_isPassive && proto->Duration == 0))
	{
		m_isPermanent = true;
	}

	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		m_auras[i] = nullptr;
}

SpellAuraHolder::~SpellAuraHolder()
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		if (Aura* aura = m_auras[i])
		{
			delete aura;
		}
}

void SpellAuraHolder::Update(uint32 diff)
{
	SetInUse(true);
	UpdateHolder(diff);
	SetInUse(false);
}

void SpellAuraHolder::RefreshHolder()
{
	SetAuraDuration(GetAuraMaxDuration());
	SendAuraUpdate(false);
}

void SpellAuraHolder::Add()
{
	if (!GetId())
	{ // 找不到对应法术
		return;
	}
	if (!m_target)
	{ // 没有目标
		return;
	}

	// 尝试寻找一个空闲的光环槽
	uint8 slot = NULL_AURA_SLOT;
	if (m_target->GetVisibleAurasCount() < MAX_AURAS)
	{
		const Unit::VisibleAuraMap& visibleAuras = m_target->GetVisibleAuras();
		for (uint8 i = 0; i < MAX_AURAS; ++i)
		{
			Unit::VisibleAuraMap::const_iterator iter = visibleAuras.find(i);
			if (iter == visibleAuras.end())
			{
				slot = i;
				break;
			}
		}
	}

	Unit* caster = GetCaster();
	if (IsNeedVisibleSlot(caster))
	{
		SetAuraSlot(slot);
		if (slot < MAX_AURAS)
		{
			SetVisibleAura(false);
			SendAuraUpdate(false);
		}
	}
}

void SpellAuraHolder::Remove()
{
	// 移除所有由光环触发的法术，除了相同光环替换案例
	if (m_removeMode != AURA_REMOVE_BY_STACK)
	{
		CleanupTriggeredSpells();
	}

	Unit* caster = GetCaster();

	// 移除持久型法术创建的动态对象
	if (caster && IsPersistent())
	{
		if (DynamicObject* dynObj = caster->GetDynObject(GetId()))
		{
			dynObj->RemoveAffected(m_target);
		}
	}

	uint8 slot = GetAuraSlot();
	if (slot >= MAX_AURAS)
	{
		return;
	}

	if (!m_target->GetVisibleAura(slot))
	{
		return;
	}

	// 可见光环处理
	SetVisibleAura(true);
	if (m_removeMode != AURA_REMOVE_BY_DELETE)
	{
		SendAuraUpdate(true);
	}
}

void SpellAuraHolder::AddAura(Aura* aura, SpellEffectIndex index)
{
	m_auras[index] = aura;
}

void SpellAuraHolder::RemoveAura(SpellEffectIndex index)
{
	m_auras[index] = nullptr;
}

void SpellAuraHolder::ApplyAuraModifiers(bool apply, bool real)
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX && !IsDeleted(); ++i)
    {
        if (Aura* aura = GetAuraByEffectIndex(SpellEffectIndex(i)))
        {
            aura->ApplyModifier(apply, real);
        }
    }
}

void SpellAuraHolder::CleanupTriggeredSpells()
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (!m_spellProto->EffectApplyAuraName[i])
		{
			continue;
		}

		uint32 spellId = m_spellProto->EffectTriggerSpell[i];
		if (!spellId)
		{
			continue;
		}

		const SpellEntry* proto = sObjectMgr.GetSpellEntry(spellId);
		if (!proto)
		{
			continue;
		}

		if (proto->Duration != -1)
		{
			continue;
		}

		m_target->RemoveAurasDueToSpell(spellId);
	}
}

void SpellAuraHolder::SendAuraUpdate(bool remove) const
{
}

void SpellAuraHolder::SetDeleted()
{
	m_deleted = true;
}

void SpellAuraHolder::SetInUse(bool state)
{
	if (state)
	{
		++m_in_use;
	}
	else
	{
		if (m_in_use)
		{
			--m_in_use;
		}
	}
}

void SpellAuraHolder::SetLoadedState(const ObjectGuid& casterGUID, const ObjectGuid& itemGUID, uint32 charges,
	int32 maxduration, int32 duration)
{
	m_casterGuid = casterGUID;
	m_castItemGuid = itemGUID;
	m_procCharges = charges;
	SetAuraMaxDuration(maxduration);
	SetAuraDuration(duration);
}

void SpellAuraHolder::SetRemoveMode(AuraRemoveMode mode)
{
	m_removeMode = mode;
}

void SpellAuraHolder::SetVisibleAura(bool remove)
{
	m_target->SetVisibleAura(m_auraSlot, remove ? NULL : this);
}

bool SpellAuraHolder::IsInUse() const
{
	return m_in_use;
}

bool SpellAuraHolder::IsDeleted() const
{
	return m_deleted;
}

bool SpellAuraHolder::IsEmptyHolder() const
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		if (m_auras[i])
		{
			return false;
		}
	return true;
}

uint32 SpellAuraHolder::GetId() const
{
	return m_spellProto->Id;
}

const SpellEntry* SpellAuraHolder::GetSpellProto() const
{
	return m_spellProto;
}

Aura* SpellAuraHolder::GetAuraByEffectIndex(SpellEffectIndex index) const
{
	return m_auras[index];
}

const ObjectGuid& SpellAuraHolder::GetCasterGuid() const
{
	return m_casterGuid;
}

void SpellAuraHolder::SetCasterGuid(ObjectGuid guid)
{
	m_casterGuid = guid;
}

const ObjectGuid& SpellAuraHolder::GetCastItemGuid() const
{
	return m_castItemGuid;
}

Unit* SpellAuraHolder::GetCaster() const
{
	if (GetCasterGuid() == m_target->GetObjectGuid())
	{
		return m_target;
	}

	return ObjectAccessor::GetUnit(*m_target, m_casterGuid);
}

Unit* SpellAuraHolder::GetTarget() const
{
	return m_target;
}

void SpellAuraHolder::SetTarget(Unit* target)
{
	m_target = target;
}

time_t SpellAuraHolder::GetAuraApplyTime() const
{
	return m_applyTime;
}

int32 SpellAuraHolder::GetAuraMaxDuration() const
{
	return m_maxDuration;
}

void SpellAuraHolder::SetAuraMaxDuration(int32 duration)
{
	m_maxDuration = duration;
	if (duration > 0)
	{
		if (!(IsPassive() && GetSpellProto()->Duration == 0))
		{
			SetPermanent(false);
		}
	}
}

int32 SpellAuraHolder::GetAuraDuration() const
{
	return m_duration;
}

void SpellAuraHolder::SetAuraDuration(int32 duration)
{
	m_duration = duration;
}

uint8 SpellAuraHolder::GetAuraSlot() const
{
	return m_auraSlot;
}

void SpellAuraHolder::SetAuraSlot(uint8 slot)
{
	m_auraSlot = slot;
}

uint32 SpellAuraHolder::GetAuraCharges() const
{
	return m_procCharges;
}

void SpellAuraHolder::SetAuraCharges(uint32 charges, bool update)
{
	if (m_procCharges == charges)
	{
		return;
	}
	m_procCharges = charges;

	if (update)
	{
		SendAuraUpdate(false);
	}
}

bool SpellAuraHolder::DropAuraCharge()
{
	if (m_procCharges == 0)
	{
		return false;
	}

	--m_procCharges;
	SendAuraUpdate(false);
	return m_procCharges == 0;
}

bool SpellAuraHolder::HasMechanic(uint32 mechanic) const
{
    if (m_spellProto->Mechanic == mechanic)
    {
        return true;
    }

    return false;
}

bool SpellAuraHolder::HasMechanicMask(uint32 mechanicMask) const
{
    if ((1 << (m_spellProto->Mechanic - 1)) & mechanicMask)
    {
        return true;
    }

    return false;
}

bool SpellAuraHolder::IsAreaAura() const
{
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura* aura = m_auras[i])
        {
            if (aura->IsAreaAura())
            {
                return true;
            }
        }

    return false;
}

bool SpellAuraHolder::IsDeathPersistent() const
{
    return m_isDeathPersist;
}

bool SpellAuraHolder::IsPermanent() const
{
	return m_isPermanent;
}

void SpellAuraHolder::SetPermanent(bool permanent)
{
	m_isPermanent = permanent;
}

bool SpellAuraHolder::IsPassive() const
{
	return m_isPassive;
}

bool SpellAuraHolder::IsPersistent() const
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		if (Aura* aura = m_auras[i])
		{
			if (aura->IsPersistent())
			{
				return true;
			}
		}
	return false;
}

bool SpellAuraHolder::IsPositive() const
{
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		if (Aura* aura = m_auras[i])
		{
			if (!aura->IsPositive())
			{
				return false;
			}
		}

	return true;
}

bool SpellAuraHolder::IsNeedVisibleSlot(const Unit* caster) const
{
	// 被动光环不需要显示
	return !m_isPassive;
}

void SpellAuraHolder::UpdateHolder(uint32 diff)
{
	if (m_duration > 0)
	{
		m_duration -= diff;
		if (m_duration < 0)
		{
			m_duration = 0;
		}

		for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
			if (Aura* aura = m_auras[i])
			{
				aura->Update(diff);
			}
	}
}

Aura::Aura(const SpellEntry* proto,
	SpellEffectIndex eff,
	SpellAuraHolder* holder,
	Unit* target,
	Unit* caster,
	Item* castItem)
{
}

Aura::~Aura()
{
}

void Aura::Update(uint32 diff)
{
	SetInUse(true);
	UpdateAura(diff);
	SetInUse(false);
}

void Aura::SetModifier(AuraType type, int32 amount, uint32 periodictime, int32 miscValue)
{
	m_modifier.m_auraname = type;
	m_modifier.m_amount = amount;
	m_modifier.m_miscvalue = miscValue;
	m_modifier.periodictime = periodictime;
}

Modifier* Aura::GetModifier()
{
	return &m_modifier;
}

const Modifier* Aura::GetModifier() const
{
	return &m_modifier;
}

int32 Aura::GetMiscValue() const
{
	const SpellEntry* proto = m_spellAuraHolder->GetSpellProto();
	return proto ? proto->EffectMiscValue[m_effIndex] : 0;
}

void Aura::ApplyModifier(bool apply, bool Real)
{
	AuraType aura = m_modifier.m_auraname;

	GetHolder()->SetInUse(true);
	SetInUse(true);
	if (aura < TOTAL_AURAS)
	{
		(*this.*AuraHandler[aura])(apply, Real);
	}
	SetInUse(false);
	GetHolder()->SetInUse(false);
}

void Aura::SetInUse(bool state)
{
	if (state)
	{
		++m_in_use;
	}
	else
	{
		if (m_in_use)
		{
			--m_in_use;
		}
	}
}

void Aura::SetRemoveMode(AuraRemoveMode mode)
{
	m_removeMode = mode;
}

void Aura::SetLoadedState(int32 damage, uint32 periodicTime)
{
	m_modifier.m_amount = damage;
	m_modifier.periodictime = periodicTime;

	if (uint32 maxticks = GetAuraMaxTicks())
	{
		m_periodicTick = maxticks - GetHolder()->GetAuraDuration() / m_modifier.periodictime;
	}
}

uint32 Aura::GetAuraTicks() const
{
	return m_periodicTick;
}

uint32 Aura::GetAuraMaxTicks() const
{
	int32 maxDuration = GetHolder()->GetAuraMaxDuration();
	return maxDuration > 0 && m_modifier.periodictime > 0 ? maxDuration / m_modifier.periodictime : 0;
}

SpellEffectIndex Aura::GetEffIndex() const
{
	return m_effIndex;
}

SpellAuraHolder* Aura::GetHolder()
{
	return m_spellAuraHolder;
}

const SpellAuraHolder* Aura::GetHolder() const
{
	return m_spellAuraHolder;
}

Unit* Aura::GetTriggerTarget() const
{
	return m_spellAuraHolder->GetTarget();
}

bool Aura::IsAreaAura() const
{
	return m_isAreaAura;
}

bool Aura::IsInUse() const
{
	return m_in_use;
}

bool Aura::IsLastAuraOnHolder()
{
	for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
		if (i != GetEffIndex() && GetHolder()->m_auras[i])
		{
			return false;
		}
	return true;
}

bool Aura::IsPeriodic() const
{
	return m_isPeriodic;
}

bool Aura::IsPersistent() const
{
	return m_isPersistent;
}

bool Aura::IsPositive() const
{
	return m_isPositive;
}

bool Aura::IsAffectedOnSpell(const SpellEntry *proto) const
{
    return false;
}

bool Aura::CanProcFrom(const SpellEntry *proto, uint32 procFlag, uint32 EventProcEx, uint32 procEx, bool active, bool useClassMask) const
{
	return false;
}

void Aura::UpdateAura(uint32 diff)
{
	if (m_isPeriodic)
	{
		m_periodicTimer -= diff;
		// 判断m_periodicTimer == 0是为了防止m_duration == m_periodicTimer的场景丢失滴答数
		if (m_periodicTimer <= 0)
		{
			// 在处理周期效果前线更新时间，避免在PeriodicTick处理过程中光环被移除
			m_periodicTimer += m_modifier.periodictime;
			++m_periodicTick;
			PeriodicTick();
		}
	}
}

void Aura::PeriodicTick()
{
	Unit* target = GetHolder()->GetTarget();
	const SpellEntry* proto = GetHolder()->GetSpellProto();

	switch (m_modifier.m_auraname)
	{
		case SPELL_AURA_PERIODIC_DAMAGE:
			break;
		case SPELL_AURA_DUMMY:
		{
			PeriodicDummyTick();
			break;
		}
		case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
		{
			TriggerSpell();
			break;
		}
		default:
			break;
	}
}

void Aura::PeriodicDummyTick()
{
	Unit* target = GetHolder()->GetTarget();
	const SpellEntry* proto = GetHolder()->GetSpellProto();

	switch (proto->SpellFamilyName)
	{
		case SPELL_FAMILY_NONE:
		{
			break;
		}
	}
}

void Aura::TriggerSpell()
{
	ObjectGuid casterGUID = GetHolder()->GetCasterGuid();
	Unit* triggerTarget = GetTriggerTarget();

	if (!casterGUID || !triggerTarget)
	{
		return;
	}

	const SpellEntry* auraSpellInfo = GetHolder()->GetSpellProto();
	uint32 trigger_spell_id = auraSpellInfo->EffectTriggerSpell[m_effIndex];
	const SpellEntry* triggeredSpellInfo = sObjectMgr.GetSpellEntry(trigger_spell_id);
	uint32 auraId = auraSpellInfo->Id;
	Unit* target = GetHolder()->GetTarget();
	Unit* triggerCaster = triggerTarget;
	WorldObject* triggerTargetObject = nullptr;

	// 需要自定义处理的特殊触发法术
	if (triggeredSpellInfo == nullptr)
	{
	}
	else
	{
		// 虽然不是特殊触发法术，但是依然需要添加一些自定义处理
	}

	// 所有自定义处理已完成，开始正常施放触发法术
	if (triggeredSpellInfo)
	{
		if (triggerTargetObject)
		{
			triggerCaster->CastSpell(triggerTargetObject->GetPositionX(), triggerTargetObject->GetPositionY(), triggeredSpellInfo, true,
				nullptr, this, casterGUID);
		}
		else
		{
			triggerCaster->CastSpell(triggerTarget, triggeredSpellInfo, true, nullptr, this, casterGUID);
		}
	}
	else
	{
		sLog.outError("Aura::TriggerSpell: Spell %u have 0 in EffectTriggered[%d], not handled custom case?", auraSpellInfo->Id, m_effIndex);
	}
}

SpellAuraHolder* CreateSpellAuraHolder(const SpellEntry *proto, Unit* target, WorldObject* caster, Item* castItem)
{
    return new SpellAuraHolder(proto, target, caster, castItem);
}