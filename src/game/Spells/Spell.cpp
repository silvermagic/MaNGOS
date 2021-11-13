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

#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Items/PcInventory.h"
#include "Packets/S_DoActionGFX.hpp"
#include "Packets/S_ServerMessage.hpp"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Spell.h"

extern pEffect SpellEffects[TOTAL_SPELL_EFFECTS];

SpellCastTargets::SpellCastTargets()
{
}

SpellCastTargets::~SpellCastTargets()
{
}

void SpellCastTargets::Update(Unit* caster)
{
    m_GOTarget = m_GOTargetGUID ? caster->GetMap()->GetGameObject(m_GOTargetGUID) : nullptr;
    if (m_unitTargetGUID)
    {
        if (m_unitTargetGUID == caster->GetObjectGuid())
        {
            m_unitTarget = caster;
        }
        else
        {
            m_unitTarget = ObjectAccessor::GetUnit(*caster, m_unitTargetGUID);
        }
    }
    else
    {
        m_unitTarget = nullptr;
    }
    m_itemTarget = nullptr;

    if (caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ((Player*)caster);

        /*if (m_targetMask & TARGET_FLAG_ITEM)
        {
            m_itemTarget = player->GetBag()->GetItem(m_itemTargetGUID);
        }*/

        if (m_itemTarget)
            m_itemTargetEntry = m_itemTarget->GetEntry();
    }
}

SpellCastTargets& SpellCastTargets::operator=(const SpellCastTargets& target)
{
    m_unitTarget = target.m_unitTarget;
    m_itemTarget = target.m_itemTarget;
    m_GOTarget = target.m_GOTarget;

    m_unitTargetGUID = target.m_unitTargetGUID;
    m_GOTargetGUID = target.m_GOTargetGUID;
    m_CorpseTargetGUID = target.m_CorpseTargetGUID;
    m_itemTargetGUID = target.m_itemTargetGUID;

    m_itemTargetEntry = target.m_itemTargetEntry;

    m_srcX = target.m_srcX;
    m_srcY = target.m_srcY;

    m_destX = target.m_destX;
    m_destY = target.m_destY;

    m_strTarget = target.m_strTarget;

    m_targetMask = target.m_targetMask;

    return *this;
}

void SpellCastTargets::SetDestination(CoordUnit x, CoordUnit y)
{
}

void SpellCastTargets::SetSource(CoordUnit x, CoordUnit y)
{
}

void SpellCastTargets::GetDestination(CoordUnit& x, CoordUnit& y) const
{
    x = m_destX;
    y = m_destY;
}

void SpellCastTargets::GetSource(CoordUnit& x, CoordUnit& y) const
{
    x = m_srcX;
    y = m_srcY;
}

void SpellCastTargets::SetUnitTarget(Unit* target)
{
}

ObjectGuid SpellCastTargets::GetUnitTargetGuid() const
{
    return m_unitTargetGUID;
}

Unit* SpellCastTargets::GetUnitTarget() const
{
    return m_unitTarget;
}

void SpellCastTargets::SetGOTarget(GameObject* target)
{
}

ObjectGuid SpellCastTargets::GetGOTargetGuid() const
{
    return m_GOTargetGUID;
}

GameObject* SpellCastTargets::GetGOTarget() const
{
    return m_GOTarget;
}

void SpellCastTargets::SetCorpseTarget(Corpse* corpse)
{
}

ObjectGuid SpellCastTargets::GetCorpseTargetGuid() const
{
    return m_CorpseTargetGUID;
}

void SpellCastTargets::SetItemTarget(Item* item)
{
}

ObjectGuid SpellCastTargets::GetItemTargetGuid() const
{
    return m_itemTargetGUID;
}

Item* SpellCastTargets::GetItemTarget() const
{
    return m_itemTarget;
}

uint32 SpellCastTargets::GetItemTargetEntry() const
{
    return m_itemTargetEntry;
}

bool SpellCastTargets::IsEmpty() const
{
    return !m_GOTargetGUID && !m_unitTargetGUID && !m_itemTarget && !m_CorpseTargetGUID;
}

Spell::Spell(Unit* caster,
	const SpellEntry* info,
	bool triggered,
	ObjectGuid originalCasterGUID,
	const SpellEntry* triggeredBy)
{
	MANGOS_ASSERT(caster != nullptr && info != nullptr);
	MANGOS_ASSERT(info == sObjectMgr.GetSpellEntry(info->Id) && "`info` must be pointer to sSpellStore element");

	m_proto = info;
    m_triggeredByAuraSpell = triggeredBy;
	m_caster = caster;

	// todo: �����������ͳ�ʼ��
	// m_attackType = GetWeaponAttackType(m_proto);
	m_originalCasterGUID = originalCasterGUID ? originalCasterGUID : m_caster->GetObjectGuid();

	UpdateOriginalCasterPointer();

	m_state = SPELL_STATE_PREPARING;

	m_castPositionX = m_castPositionY = 0;
	m_TriggerSpells.clear();
	m_IsTriggeredSpell = triggered;
	m_triggeredByAuraSpell = nullptr;

	m_unitTarget = nullptr;
	m_itemTarget = nullptr;
	m_gameObjTarget = nullptr;

	m_timer = 0;
	m_duration = 0;

	for (uint8 i = 0 ; i < MAX_POWERS; ++i)
    {
        m_powerCost[i] = 0;
    }

	m_canReflect = false;

	if (m_proto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
	{
		for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
		{
			if (m_proto->Effect[i] == 0)
			{
				continue;
			}

			if (!IsPositiveTarget(m_proto->EffectImplicitTarget[i]))
			{
				m_canReflect = true;
			}
			else
			{
				m_canReflect = m_proto->HasAttribute(SPELL_ATTR_NEGATIVE);
			}

			if (m_canReflect)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	CleanupTargetList();
}

Spell::~Spell()
{
}

void Spell::Update(uint32 diff)
{
	UpdatePointers();

	// ��ʧ����Ŀ��ʱȡ��ʩ��
	if (!m_targets.GetUnitTargetGuid().IsEmpty() && !m_targets.GetUnitTarget())
	{
		Cancel();
		return;
	}

	// �������Ŀ���������״̬����ô����ʧȥ����Ŀ�괦��ȡ��ʩ��
	if (Unit* target = m_targets.GetUnitTarget())
	{
		if (!target->IsVisibleForOrDetect(m_caster, m_caster, true))
		{
			if (m_caster->GetTargetGuid() == target->GetObjectGuid())
			{
				m_caster->SetTargetGuid(ObjectGuid());
			}
			Cancel();
			return;
		}
	}

	if (m_state == SPELL_STATE_PREPARING)
    {
        if (m_timer)
        {
            if (diff > m_timer)
            {
                m_timer = 0;
            }
            else
            {
                m_timer -= diff;
            }
        }

        // ����������ʩ�ŷ������������з�������˲ʱ����������ֻ���߸�����
        if (m_timer == 0)
        {
            Cast();
        }
    }
}

void Spell::Prepare(const SpellCastTargets* targets, Aura* triggeredByAura)
{
    // ���÷���Ŀ��
    m_targets = *targets;

    // ����ʩ��״̬
    m_state = SPELL_STATE_PREPARING;

    // ����ʩ��������ͳ���
    m_castPositionX = m_caster->GetPositionX();
    m_castPositionY = m_caster->GetPositionY();
    m_castOrientation = m_caster->GetOrientation();

    // ���ù⻷�����ķ���
    if (triggeredByAura)
        m_triggeredByAuraSpell = triggeredByAura->GetHolder()->GetSpellProto();

    // ���������¼�������ӵ�ʩ���ߵ��¼�������
    SpellEvent* Event = new SpellEvent(this);
    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    // ����ʩ������
	CalculatePowerCost(m_proto, m_caster, m_castItem);

    // ����Ƿ�����ʩ������
    SpellCastResult result = CheckCast(true);
    if (result != SPELL_CAST_OK)
    {
        if (triggeredByAura)
        {
            triggeredByAura->GetHolder()->SetAuraDuration(0);
        }
        SendCastResult(result);
        Finish(false);
        return;
    }

    // ���÷����������������練�����ϵĴ��������Ǳ���ս��������
    PrepareDataForTriggerSystem();

    // ���÷�������ʱ�䣬��������ͨ����
    m_duration = m_proto->Duration;

    // ʩ�Ÿ��淨�������ʩ���ߵ�����״̬
    if (!m_IsTriggeredSpell && IsSpellBreakStealth(m_proto))
    {
        m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);
    }

    // ��ͨʩ��
    if (!m_IsTriggeredSpell)
    {
        // ������ȴ
        TriggerCooldown();
    }
    else if (m_timer == 0) // ��������
	{
    	Cast(true);
	}
}

void Spell::Cast(bool skipCheck)
{
    // ������ʩ��������ɾ����������
    m_executedCurrently = true;
    std::shared_ptr<void> guard(nullptr, [this](void*) { m_executedCurrently = false; });

	// ����GUID���¶���ָ�룬��ֹ�����Ѿ������ڵĶ���
	UpdatePointers();

	// ��ʧ����Ŀ��ʱȡ��ʩ��
	if (!m_targets.GetUnitTarget() && m_targets.GetUnitTargetGuid()
		&& m_targets.GetUnitTargetGuid() != m_caster->GetObjectGuid())
	{
		Cancel();
		return;
	}

	// ���ʩ���������Ƿ����
	SpellCastResult castResult = CheckPower();
	if (castResult != SPELL_CAST_OK)
	{
		SendCastResult(castResult);
		Finish(false);
		return;
	}

	// ����Ƿ�����ʩ������
	if (!skipCheck)
	{
		castResult = CheckCast(false);
		if (castResult != SPELL_CAST_OK)
		{
			SendCastResult(castResult);
			Finish(false);
			return;
		}
	}

	// �ҳ������ܵ�����Ӱ�����Ϸ����(���������ж�)
	FillTargetMap();

	// ���������FillTargetMap�����б����Ϊ�ѽ�������ֹͣʩ��
	if (m_state == SPELL_STATE_FINISHED)
	{
		return;
	}

	// �۳���������(ħ����������ʩ������)
	TakePower();
	TakeReagents();

	// ����ʩ�����
	SendCastResult(castResult);

	// ��ʱӦ�÷���Ч��(�����˺����㡢��Ч��ʾ�Ȳ���)
	HandleImmediate();
}

void Spell::Finish(bool ok)
{
    if (!m_caster)
    {
        return;
    }

    if (m_state == SPELL_STATE_FINISHED)
    {
        return;
    }

    m_state = SPELL_STATE_FINISHED;

    // ʣ��Ĵ������ʩ���ɹ��ķ������
    if (!ok)
    {
        return;
    }

    // ֻ���ڳɹ�ʩ��ʱ���ܵ��÷�������
    if (!m_TriggerSpells.empty())
    {
        CastTriggerSpells();
    }
}

void Spell::Cancel()
{
    if (m_state == SPELL_STATE_FINISHED)
    {
        return;
    }

    // ȡ��������ȴ
    if (m_state == SPELL_STATE_PREPARING)
    {
        CancelCooldown();
    }

    // ʩ��ʧ��
    Finish(false);

    // ɾ������������DynamicObject��GameObject
    m_caster->RemoveDynObject(m_proto->Id);
    m_caster->RemoveGameObject(m_proto->Id, true);
}

void Spell::HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, SpellEffectIndex i)
{
    m_unitTarget = pUnitTarget;
    m_itemTarget = pItemTarget;
    m_gameObjTarget = pGOTarget;

    // ����Ч������
    uint8 eff = m_proto->Effect[i];
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u Effect%d : %u Targets: %s, %s, %s",
    	m_proto->Id, i, eff,
    	m_unitTarget ? m_unitTarget->GetGuidStr().c_str() : "-",
    	m_itemTarget ? m_itemTarget->GetGuidStr().c_str() : "-",
    	m_gameObjTarget ? m_gameObjTarget->GetGuidStr().c_str() : "-");

    if (eff < TOTAL_SPELL_EFFECTS)
    {
        (*this.*SpellEffects[eff])(i);
    }
    else
    {
        sLog.outError("WORLD: Spell FX %d > TOTAL_SPELL_EFFECTS ", eff);
    }
}

void Spell::SendCastResult(SpellCastResult result)
{
	if (m_caster->GetTypeId() != TYPEID_PLAYER)
	{
		// todo: NPCʩ��
		return;
	}

	S_DoActionGFX pkt((Player *)m_caster, m_proto->ActionID);
	m_caster->SendMessageToSet(&pkt, true);

	if (result != SPELL_CAST_OK)
	{
		if (m_proto->MessageID[2] > 0)
		{
			S_ServerMessage pkt(m_proto->MessageID[2]);
			((Player*)m_caster)->SendDirectMessage(&pkt);
		}
	}
}

uint32 Spell::GetState() const
{
	return m_state;
}

void Spell::SetState(uint32 state)
{
	m_state = state;
}

bool Spell::IsDeletable() const
{
    return !m_executedCurrently;
}

void Spell::HandleImmediate()
{
	// ������߷���Ч��
    for (ItemTargetList::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

    // �������������Ч��(��ǽ)
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        if (m_proto->Effect[j] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            HandleEffects(nullptr, nullptr, nullptr, SpellEffectIndex(j));
        }
    }

    // ����Unit����Ч��
    for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

	// ����GameObject����Ч��
    for (GOTargetList::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

    // ʩ���������Ľ���
    TakeCastItem();

    // ʩ������
    Finish(true);
}

SpellCastResult Spell::CheckCast(bool strict)
{
	// ��鼼����ȴ��ֹ����
	if (strict && !m_IsTriggeredSpell && HasCooldown())
	{
		return SPELL_FAILED_NOT_READY;
	}

	// ���ħ��
	{
		SpellCastResult castResult = CheckPower();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// ���ʩ������
	if (!IsPassiveSpell(m_proto))
	{
		SpellCastResult castResult = CheckItems();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// ���ʩ������
	if (!m_IsTriggeredSpell)
	{
		SpellCastResult castResult = CheckRange(strict);
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// ��鷨������
	if (!m_IsTriggeredSpell)
	{
		SpellCastResult castResult = CheckCasterAuras();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// todo: �������ʩ��������������ﳬ�����ޣ��Ƿ��ܶԿյ�ʩ�Ż��Ǳ������Ŀ��

	return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckItems()
{
	if (m_caster->GetTypeId() != TYPEID_PLAYER)
	{
		return SPELL_CAST_OK;
	}

	Player* caster = (Player*)m_caster;
	if (m_castItem)
	{
		const ItemEntry* proto = m_castItem->GetProto();
		if (!proto)
		{
			return SPELL_FAILED_ITEM_NOT_READY;
		}

		// �������ߵ�ʹ�ô���������
		if (proto->max_charge_count && m_castItem->GetChargeCount() == 0)
		{
			return SPELL_FAILED_NO_CHARGES_REMAIN;
		}
	}

	// todo: ��鷨���Ƿ��װ����Ҫ������ĳЩ�������봩�ض���װ������ʩ��

	// ���ʩ�����������Ƿ����
	for (uint8 i = 0; i < MAX_SPELL_REAGENTS; ++i)
	{
		if (m_proto->Reagent[i] <= 0)
		{
			continue;
		}

		if (!caster->GetBag()->CheckItemCount(m_proto->Reagent[i], m_proto->ReagentCount[i]))
		{
			return SPELL_FAILED_ITEM_NOT_READY;
		}
	}

	// todo: �������ʩ�����������紴�������Ҫ��鱳���Ƿ�����

	return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckRange(bool strict)
{
	Unit* target = m_targets.GetUnitTarget();

	if (target && target != m_caster)
	{
		uint8 dist = m_caster->GetTileDistance(target);
		if (dist > m_proto->Range)
		{
			return SPELL_FAILED_OUT_OF_RANGE;
		}
	}

	return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPower()
{
	// ���ߴ����ķ�������������
	if (m_castItem)
	{
		return SPELL_CAST_OK;
	}

	// ���ʩ���������Ƿ����
	for (uint8 i = 0; i < MAX_POWERS; ++i)
	{
		if (m_powerCost[i] && (m_caster->GetPower(i) < m_powerCost[i]))
		{
			return SPELL_FAILED_NO_POWER;
		}
	}

	return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckCasterAuras() const
{
	return SPELL_CAST_OK;
}

void Spell::CalculatePowerCost(const SpellEntry *proto, Unit* caster, Item* castItem)
{
	// ��������ʩ�ŵķ�������������
	if (m_castItem)
	{
		return;
	}

	for (uint8 i = 0; i < MAX_POWERS; ++i)
	{
		if (proto->PowerCost[i])
		{
			m_powerCost[i] = proto->PowerCost[i];
		}
	}

	// todo: �������ļ���
}

void Spell::TakePower()
{
	// �������߻��ߴ����ͷ�������ħ
	if (m_castItem || m_IsTriggeredSpell)
	{
		return;
	}

	// �۳�ħ��
	for (uint8 i = 0; i < MAX_POWERS; ++i)
	{
		if (m_powerCost[i] > 0)
		{
			m_caster->ModifyPower(i, -m_powerCost[i]);
		}
	}
}

void Spell::TakeReagents()
{
	if (m_caster->GetTypeId() != TYPEID_PLAYER)
	{
		return;
	}

	Player* caster = (Player*)m_caster;
	for (uint8 i = 0; i < MAX_SPELL_REAGENTS; ++i)
	{
		if (m_proto->Reagent[i] <= 0)
		{
			continue;
		}

		caster->GetBag()->CostItem(m_proto->Reagent[i], m_proto->ReagentCount[i]);
	}
}

void Spell::TakeCastItem()
{
	// û��ʹ�÷������ߣ�����������
    if (!m_castItem || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        return;
    }

    // �����ķ�������Ҫ���Ƿ����������ģ�����װ�����������ߴ�����Ч��
    /*if (m_IsTriggeredSpell && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM))
    {
        return;
    }*/

    const ItemEntry *proto = m_castItem->GetProto();
    if (!proto)
    {
        // ���������Ӧ�ó���
        sLog.outError("Cast item (%s) has no item prototype", m_castItem->GetGuidStr().c_str());
        return;
    }

	// �۳�ʹ�ô���
	m_castItem->SetChargeCount(std::max(0, m_castItem->GetChargeCount() - 1));
}

bool Spell::HasCooldown()
{
	if (m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		return ((Player*)m_caster)->GetCooldown() != 0;
	}
	else
	{
		return false;
	}
}

void Spell::TriggerCooldown()
{
	int32 gcd = m_proto->RecoveryTime;
	if (!gcd)
	{
		return;
	}

	if (m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		((Player*)m_caster)->ResetCooldown(gcd);
	}
}

void Spell::CancelCooldown()
{
	if (!m_proto->RecoveryTime)
	{
		return;
	}

	if (m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		((Player*)m_caster)->ResetCooldown(0);
	}
}


void Spell::UpdatePointers()
{
	UpdateOriginalCasterPointer();

	m_targets.Update(m_caster);
}

void Spell::UpdateOriginalCasterPointer()
{
	if (m_originalCasterGUID == m_caster->GetObjectGuid())
	{
		m_originalCaster = m_caster;
	}
	else if (m_originalCasterGUID.IsGameObject())
	{
		GameObject* go = m_caster->IsInWorld() ? m_caster->GetMap()->GetGameObject(m_originalCasterGUID) : nullptr;
		m_originalCaster = go ? go->GetOwner() : nullptr;
	}
	else
	{
		Unit* unit = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
		m_originalCaster = unit && unit->IsInWorld() ? unit : nullptr;
	}
}

Unit* Spell::GetCaster() const
{
	return m_caster;
}

WorldObject* Spell::GetAffectiveCasterObject() const
{
	if (!m_originalCasterGUID)
	{
		return m_caster;
	}

	if (m_originalCasterGUID.IsGameObject() && m_caster->IsInWorld())
	{
		return m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
	}
	return m_originalCaster;
}

Unit* Spell::GetAffectiveCaster() const
{
	return m_originalCasterGUID ? m_originalCaster : m_caster;
}

WorldObject* Spell::GetCastingObject() const
{
	if (m_originalCasterGUID.IsGameObject())
	{
		return m_caster->IsInWorld() ? m_caster->GetMap()->GetGameObject(m_originalCasterGUID) : nullptr;
	}
	else
	{
		return m_caster;
	}
}

void Spell::PrepareDataForTriggerSystem()
{
    m_canTrigger = false;

    if (m_castItem)
    {
        m_canTrigger = false;
    }
    else if (!m_IsTriggeredSpell)
    {
        m_canTrigger = true;
    }
    else if (!m_triggeredByAuraSpell)
    {
        m_canTrigger = true;
    }

    if (!m_canTrigger)
    {
        // todo: ����ְҵ���ܴ���
    }

    switch (m_proto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
        {
            m_procAttacker = PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT;
            m_procVictim   = PROC_FLAG_TAKEN_MELEE_SPELL_HIT;
            break;
        }
        case SPELL_DAMAGE_CLASS_RANGED:
        {
            m_procAttacker = PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT;
            m_procVictim   = PROC_FLAG_TAKEN_RANGED_SPELL_HIT;
            break;
        }
        default:
        {
            if (IsPositiveSpell(m_proto))
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL;
                m_procVictim   = PROC_FLAG_TAKEN_POSITIVE_SPELL;
            }
            else
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT;
                m_procVictim   = PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT;
            }
            break;
        }
    }

    // ��Щ�����ȴ��ڸ���Ч��Ҳ��������Ч���������������еĸ���Ч�����з���Ŀ��ʱ�Ž��з��������ж���������߼�¼�·����еĸ���Ч��
	m_negativeEffectMask = 0x0;
	for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (m_proto->Effect[i] != SPELL_EFFECT_NONE && !IsPositiveEffect(m_proto, SpellEffectIndex(i)))
		{
			m_negativeEffectMask |= (1 << i);
		}
	}
}

void Spell::CastTriggerSpells()
{
    for (SpellInfoList::const_iterator iter = m_TriggerSpells.begin(); iter != m_TriggerSpells.end(); ++iter)
    {
        Spell* spell = new Spell(m_caster, (*iter), true, m_originalCasterGUID);
        spell->Prepare(&m_targets);
    }
}

void Spell::CleanupTargetList()
{
	m_UniqueTargetInfo.clear();
	m_UniqueGOTargetInfo.clear();
	m_UniqueItemInfo.clear();
}

void Spell::ClearCastItem()
{
	if (m_castItem == m_targets.GetItemTarget())
	{
		m_targets.SetItemTarget(nullptr);
	}

	m_castItem = nullptr;
}

void Spell::FillTargetMap()
{
    // ��ʱ�洢ÿ������Ч����Ŀ���б�
    UnitList tmpUnitLists[MAX_EFFECT_INDEX];
    // ����Ч����Ŀ���б�ΪtmpUnitLists[effToIndex[i]]����������������Ч����Ŀ��ƥ�������ͬ��������ɼ��ٷ���Ŀ����������
    uint8 effToIndex[MAX_EFFECT_INDEX] = {0, 1, 2};
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (m_proto->Effect[i] == SPELL_EFFECT_NONE)
        {
            continue;
        }

        // �����͹⻷���ǽ�ʩ������������Ŀ��
        if (IsAreaAuraEffect(m_proto->Effect[i]))
        {
            AddUnitTarget(m_caster, SpellEffectIndex(i));
        }

        // �����ǰ����Ч����Ŀ������ǰ���һ���������ظ�����
        for (int j = 0; j < i; ++j)
        {
            if (m_proto->EffectImplicitTarget[i] == m_proto->EffectImplicitTarget[j]
                && m_proto->Effect[j] != SPELL_EFFECT_NONE && !IsAreaAuraEffect(m_proto->Effect[i])
                && !IsAreaAuraEffect(m_proto->Effect[j]))
            {
                effToIndex[i] = j;
                break;
            }
        }

        // ˵����֮ǰ�ķ���Ч����Ŀ��ƥ�����ͬ����Ҫ������������Ŀ��
        if (effToIndex[i] == i)
        {
			// todo: ����Ŀ��
            switch (m_proto->EffectImplicitTarget[i])
			{
				case TARGET_NONE:
					break;
				case TARGET_SELF:
					SetTargetMap(SpellEffectIndex(i), TARGET_SELF, tmpUnitLists[i]);
					break;
				default:
					break;
			}
        }

        // �����ҹ�������ҵĳ����ô��������Ҳ���Զ�����ս��״̬
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
        	// todo: ����Ҳ���Զ�����ս��״̬
        }

        // ����ʱĿ���б���й���
        for (UnitList::iterator iter = tmpUnitLists[effToIndex[i]].begin(); iter != tmpUnitLists[effToIndex[i]].end();)
        {
            if (!CheckTarget(*iter, SpellEffectIndex(i)))
            {
                iter = tmpUnitLists[effToIndex[i]].erase(iter);
                continue;
            }
            else
            {
                ++iter;
            }
        }

        // ����ʱĿ���б��еĶ���Ԫ��ӵ�����Ŀ���б��У�ͬʱ���������ж�
        for (UnitList::const_iterator iter = tmpUnitLists[effToIndex[i]].begin(); iter != tmpUnitLists[effToIndex[i]].end(); ++iter)
        {
            AddUnitTarget((*iter), SpellEffectIndex(i));
        }
    }
}

void Spell::SetTargetMap(SpellEffectIndex eff, uint32 targetMode, UnitList& targetUnitMap)
{
	int32 radius = 0;
	if (m_proto->Effect[eff] != SPELL_EFFECT_NONE)
	{
		radius = m_proto->EffectRadius[eff];
	}

	// todo: �����ʩ�����Ƿ�������ʩ����Χ�Ĺ⻷

	// todo: �Ƿ���Ҫ���Ʒ���Ӱ�����Ϸ�������

	switch (targetMode)
	{
		case TARGET_SELF:
			targetUnitMap.push_back(m_caster);
			break;
		default:
			sLog.outError( "SPELL: Unknown implicit target (%u) for spell ID %u", targetMode, m_proto->Id);
			break;
	}
}

bool Spell::CheckTarget(Unit* target, SpellEffectIndex eff)
{
	// todo: ���淨����Щ��Ҫ�������������˯֮����Щ����Ҫ������������ǿ�ƹ����Լ�

    // ���Ŀ����������ͣ�ɾ�������ʵ�
    if (m_proto->EffectImplicitTarget[eff] != TARGET_SELF)
    {
        if (!CheckTargetCreatureType(target))
        {
            return false;
        }
    }

    // ���Ŀ���Ƿ��ܱ�������ѡ�У�ɾ�������ʵ�
    if (target != m_caster && target->GetOwnerGuid() != m_caster->GetObjectGuid())
    {
        /*if (target->HasFlag(UNIT_FLAG_NON_ATTACKABLE))
        {
            return false;
        }

        if ((!m_IsTriggeredSpell || target != m_targets.getUnitTarget()) &&
            target->HasFlag(UNIT_FLAG_NOT_SELECTABLE))
        {
            return false;
        }*/
    }

    // �����Ҳ��ɼ������ǹ���Ա�������Ŀ���б��Ƴ�
    if (target != m_caster && target->GetTypeId() == TYPEID_PLAYER)
    {
    	// todo: ���ɼ�У��

        if (((Player*)target)->IsGameMaster())
        {
            return false;
        }
    }

    return true;
}

bool Spell::CheckTargetCreatureType(Unit* target) const
{
    // todo: ��������ֻ�ܶԲ���ϵʩ��
    return true;
}

void Spell::AddUnitTarget(Unit* pVictim, SpellEffectIndex eff)
{
	if (m_proto->Effect[eff] == 0)
	{
		return;
	}

	// todo: ���Ŀ���Ƿ����߷���Ч��
	// bool immuned = pVictim->IsImmuneToSpellEffect(m_proto, eff, pVictim == m_caster);
	bool immuned = false;
	ObjectGuid targetGUID = pVictim->GetObjectGuid();

	// �����б����Ѵ��ڵ�Ŀ��
	for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
	{
		if (targetGUID == ihit->targetGUID)
		{
			if (!immuned)
			{
				// ����¼δ���ߵķ���Ч������
				ihit->effectMask |= 1 << eff;
			}
			return;
		}
	}

	// ��ȡĿ��ķ������н��
	TargetInfo target;
	target.targetGUID = targetGUID;               // �洢Ŀ��GUID
	target.effectMask = immuned ? 0 : (1 << eff); // �洢δ���ߵķ���Ч������
	target.processed = false;                     // ����Ч����δ��Ч
	target.missCondition = m_caster->SpellHitResult(pVictim, m_proto, m_canReflect); // ���㷨�����н��
	target.timeDelay = UI64LIT(0); // ħ������İ����ɵ���Ҫ����һ�ξ�����ܽ����˺������Կ��ܴ����ӳ٣��������õķ�����ʩ��˲��ͽ����˺�
	// ���������䳡�����������õľ���ħ��������
	if (target.missCondition == SPELL_MISS_REFLECT)
	{
		target.reflectResult = m_caster->SpellHitResult(m_caster, m_proto, m_canReflect);

		// �������ٴη���
		if (target.reflectResult == SPELL_MISS_REFLECT)
		{
			target.reflectResult = SPELL_MISS_RESIST;
		}
	}
	else
	{
		target.reflectResult = SPELL_MISS_NONE;
	}

	// ��Ӷ���Ŀ���б�
	m_UniqueTargetInfo.push_back(target);
}

void Spell::AddUnitTarget(ObjectGuid unitGuid, SpellEffectIndex eff)
{
	if (Unit* unit = m_caster->GetObjectGuid() == unitGuid ? m_caster : sObjectAccessor.GetUnit(*m_caster, unitGuid))
	{
		AddUnitTarget(unit, eff);
	}
}

void Spell::AddGOTarget(GameObject* pVictim, SpellEffectIndex eff)
{
	if (m_proto->Effect[eff] == 0)
	{
		return;
	}

	ObjectGuid targetGUID = pVictim->GetObjectGuid();

	for (GOTargetList::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
	{
		if (targetGUID == ihit->targetGUID)
		{
			ihit->effectMask |= (1 << eff);
			return;
		}
	}

	GOTargetInfo target;
	target.targetGUID = targetGUID;
	target.effectMask = (1 << eff);
	target.processed = false;
	target.timeDelay = UI64LIT(0);

	m_UniqueGOTargetInfo.push_back(target);
}

void Spell::AddGOTarget(ObjectGuid goGuid, SpellEffectIndex eff)
{
	if (GameObject* go = m_caster->GetMap()->GetGameObject(goGuid))
	{
		AddGOTarget(go, eff);
	}
}

void Spell::AddItemTarget(Item* pVictim, SpellEffectIndex eff)
{
	if (m_proto->Effect[eff] == 0)
	{
		return;
	}

	for (ItemTargetList::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
	{
		if (pVictim == ihit->item)
		{
			ihit->effectMask |= (1 << eff);
			return;
		}
	}

	ItemTargetInfo target;
	target.item = pVictim;
	target.effectMask = (1 << eff);

	m_UniqueItemInfo.push_back(target);
}

void Spell::DoAllEffectOnTarget(TargetInfo* target)
{
	// ���������Ӧ�÷���Ч��
    if (target->processed)
    {
        return;
    }
    target->processed = true;

    // ����Ч�����룬��¼�˷���Ŀ��
    uint32 mask = target->effectMask;
    Unit* unit = m_caster->GetObjectGuid() == target->targetGUID ? m_caster : sObjectAccessor.GetUnit(*m_caster, target->targetGUID);
    if (!unit)
    {
        return;
    }

    // ��ȡԭʼʩ����(�������)������ʹ�����������������˺�/������
    // �������õĻ�ǽ���෨��������ʵ���Ǵ���һ��GameObject���������ǲ��߱����㷨���˺��������ݣ������ҵ���ǽ���GameObject��ӵ����
    Unit* real_caster = GetAffectiveCaster();
    Unit* caster = real_caster ? real_caster : m_caster;

    SpellMissInfo missInfo = target->missCondition;
    // ��Ҫʹ��Ĭ�ϵ�Ԫ����ʼ��m_unitTarget(�����ڷ�����������иı�)
    m_unitTarget = unit;

    // �������ƺ��˺�ͳ��
    ResetEffectDamageAndHeal();

    // ��ʼ�����������������ڵ�ǰ�������������ж���ִ�к����ķ��������ж�
    uint32 procAttacker = m_procAttacker;
    uint32 procVictim   = m_procVictim;
    uint32 procEx       = PROC_EX_NONE;

    // �������Ŀ���ڸ��淨����û���ܵ�����Ч����Ӱ�죬��������������������з��������ж�
    if (((procAttacker | procVictim) & NEGATIVE_TRIGGER_MASK) && !(target->effectMask & m_negativeEffectMask) && missInfo == SPELL_MISS_NONE)
    {
        procAttacker = PROC_FLAG_NONE;
        procVictim   = PROC_FLAG_NONE;
    }

    if (missInfo == SPELL_MISS_NONE)
    {
        DoSpellHitOnUnit(unit, mask);
    }
    else if (missInfo == SPELL_MISS_REFLECT)
    {
        if (target->reflectResult == SPELL_MISS_NONE)
        {
            DoSpellHitOnUnit(m_caster, mask);
            m_unitTarget = m_caster;
        }
    }
    else if (missInfo == SPELL_MISS_MISS || missInfo == SPELL_MISS_RESIST)
    {
        if (!IsPositiveSpell(m_proto) && m_caster->IsVisibleForOrDetect(unit, unit, false))
        {
        	// todo: AI
        }
    }

    if (m_healing)
    {
    	// todo: ���������ж�
		m_unitTarget->ModifyPower(POWER_HEALTH, m_healing);
    }
    else if (m_damage)
    {
    }
    else if (procAttacker || procVictim) // ���������ж�
    {
    }

    // todo: ����AI����
    /*if (unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
    {
        ((Creature*)unit)->AI()->SpellHit(m_caster, m_proto);
    }*/

    // todo: ����AI����
    /*if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
    {
        ((Creature*)m_caster)->AI()->SpellHitTarget(unit, m_proto);
    }

    if (real_caster && real_caster != m_caster && real_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)real_caster)->AI())
    {
        ((Creature*)real_caster)->AI()->SpellHitTarget(unit, m_proto);
    }*/
}

void Spell::DoSpellHitOnUnit(Unit* unit, uint32 effectMask)
{
	if (!unit || !effectMask)
		return;

	Unit* real_caster = GetAffectiveCaster();
	// ����û�гɾ�ϵͳ��Ҳ�������ӳٷ���
	if (real_caster && real_caster != unit)
	{
		if (!real_caster->IsFriendlyTo(unit))
		{
			if (!IsPositiveSpell(m_proto) && m_caster->IsVisibleForOrDetect(unit, unit, false))
			{
				m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);

				// todo: AI
				/*if (!unit->IsInCombat() && unit->GetTypeId() != TYPEID_PLAYER && ((Creature*)unit)->AI())
				{
					unit->AttackedBy(real_caster);
				}

				unit->AddThreat(real_caster);
				unit->SetInCombatWith(real_caster);
				real_caster->SetInCombatWith(unit);*/
			}
		}
		else
		{
			// todo: ����ս��
		}
	}

	// ����Ч���Ǵ����⻷
	if (IsSpellAppliesAura(m_proto, effectMask))
	{
		m_spellAuraHolder = CreateSpellAuraHolder(m_proto, unit, real_caster, m_castItem);
	}
	else
	{
		m_spellAuraHolder = nullptr;
	}

	// ִ�з���Ч��
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (effectMask & (1 << i))
		{
			HandleEffects(unit, nullptr, nullptr, SpellEffectIndex(i));
		}
	}

	// ����Ӧ�����д����Ĺ⻷
	if (m_spellAuraHolder)
	{
		if (!m_spellAuraHolder->IsEmptyHolder())
		{
			unit->AddSpellAuraHolder(m_spellAuraHolder);
		}
		else
		{
			delete m_spellAuraHolder;
		}
	}
}

void Spell::DoAllEffectOnTarget(GOTargetInfo* target)
{
    if (target->processed)
    {
        return;
    }
    target->processed = true;

    uint32 mask = target->effectMask;
    if (!mask)
    {
        return;
    }

    GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
    if (!go)
    {
        return;
    }

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (mask & (1 << i))
        {
            HandleEffects(nullptr, nullptr, go, SpellEffectIndex(i));
        }
    }
}

void Spell::DoAllEffectOnTarget(ItemTargetInfo* target)
{
    uint32 mask = target->effectMask;
    if (!target->item || !mask)
    {
        return;
    }

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (mask & (1 << i))
        {
            HandleEffects(nullptr, target->item, nullptr, SpellEffectIndex(i));
        }
    }
}

void Spell::ResetEffectDamageAndHeal()
{
	m_damage = 0;
	m_healing = 0;
}

SpellEvent::SpellEvent(Spell* spell) : BasicEvent()
{
	m_Spell = spell;
}

SpellEvent::~SpellEvent()
{
	if (m_Spell->GetState() != SPELL_STATE_FINISHED)
		m_Spell->Cancel();

	if (m_Spell->IsDeletable())
	{
		delete m_Spell;
	}
	else
	{
		sLog.outError("~SpellEvent: %s %u tried to delete non-deletable spell %u. Was not deleted, causes memory leak.",
			(m_Spell->GetCaster()->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"),
			m_Spell->GetCaster()->GetGUIDLow(),
			m_Spell->m_proto->Id);
	}
}

bool SpellEvent::Execute(uint64 e_time, uint32 p_time)
{
	// ������δ�����ķ���
	if (m_Spell->GetState() != SPELL_STATE_FINISHED)
		m_Spell->Update(p_time);

	// ��鷨������״̬
	switch (m_Spell->GetState())
	{
		case SPELL_STATE_FINISHED:
		{
			// �����Ѿ�����������Ƿ�����ɾ��
			if (m_Spell->IsDeletable())
			{
				// ɾ���¼�
				return true;
			}
			break;
		}
		default:
            break;
	}

	// ������δ������ɣ��������
	m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);

	// �¼���δ�������
	return false;
}

void SpellEvent::Abort(uint64 /*e_time*/)
{
	if (m_Spell->GetState() != SPELL_STATE_FINISHED)
		m_Spell->Cancel();
}

bool SpellEvent::IsDeletable() const
{
	return m_Spell->IsDeletable();
}
