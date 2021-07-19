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

	// todo: 法术攻击类型初始化
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

	// 丢失法术目标时取消施法
	if (!m_targets.GetUnitTargetGuid().IsEmpty() && !m_targets.GetUnitTarget())
	{
		Cancel();
		return;
	}

	// 如果法术目标进入隐身状态，那么按照失去法术目标处理，取消施法
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

        // 读条结束，施放法术，天堂所有法术都是瞬时法术，这里只是走个过场
        if (m_timer == 0)
        {
            Cast();
        }
    }
}

void Spell::Prepare(const SpellCastTargets* targets, Aura* triggeredByAura)
{
    // 设置法术目标
    m_targets = *targets;

    // 设置施法状态
    m_state = SPELL_STATE_PREPARING;

    // 设置施法者坐标和朝向
    m_castPositionX = m_caster->GetPositionX();
    m_castPositionY = m_caster->GetPositionY();
    m_castOrientation = m_caster->GetOrientation();

    // 设置光环触发的法术
    if (triggeredByAura)
        m_triggeredByAuraSpell = triggeredByAura->GetHolder()->GetSpellProto();

    // 创建法术事件，并添加到施法者的事件队列中
    SpellEvent* Event = new SpellEvent(this);
    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    // 计算施法消耗
	CalculatePowerCost(m_proto, m_caster, m_castItem);

    // 检查是否满足施法条件
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

    // 设置法术触发条件，例如反击屏障的触发条件是被近战攻击命中
    PrepareDataForTriggerSystem();

    // 设置法术持续时间，例如气脉通畅术
    m_duration = m_proto->Duration;

    // 施放负面法术将解除施法者的隐身状态
    if (!m_IsTriggeredSpell && IsSpellBreakStealth(m_proto))
    {
        m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);
    }

    // 普通施法
    if (!m_IsTriggeredSpell)
    {
        // 技能冷却
        TriggerCooldown();
    }
    else if (m_timer == 0) // 触发法术
	{
    	Cast(true);
	}
}

void Spell::Cast(bool skipCheck)
{
    // 避免在施法过程中删除法术对象
    m_executedCurrently = true;
    std::shared_ptr<void> guard(nullptr, [this](void*) { m_executedCurrently = false; });

	// 根据GUID更新对象指针，防止访问已经不存在的对象
	UpdatePointers();

	// 丢失法术目标时取消施法
	if (!m_targets.GetUnitTarget() && m_targets.GetUnitTargetGuid()
		&& m_targets.GetUnitTargetGuid() != m_caster->GetObjectGuid())
	{
		Cancel();
		return;
	}

	// 检查施法者能量是否充足
	SpellCastResult castResult = CheckPower();
	if (castResult != SPELL_CAST_OK)
	{
		SendCastResult(castResult);
		Finish(false);
		return;
	}

	// 检查是否满足施法条件
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

	// 找出所有受到法术影响的游戏对象(法术命中判定)
	FillTargetMap();

	// 如果法术在FillTargetMap过程中被标记为已结束，则停止施法
	if (m_state == SPELL_STATE_FINISHED)
	{
		return;
	}

	// 扣除法术消耗(魔力、体力、施法材料)
	TakePower();
	TakeReagents();

	// 发送施法结果
	SendCastResult(castResult);

	// 即时应用法术效果(进行伤害结算、特效显示等操作)
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

    // 剩余的代码仅与施法成功的法术相关
    if (!ok)
    {
        return;
    }

    // 只有在成功施放时才能调用法术触发
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

    // 取消技能冷却
    if (m_state == SPELL_STATE_PREPARING)
    {
        CancelCooldown();
    }

    // 施法失败
    Finish(false);

    // 删除法术创建的DynamicObject和GameObject
    m_caster->RemoveDynObject(m_proto->Id);
    m_caster->RemoveGameObject(m_proto->Id, true);
}

void Spell::HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, SpellEffectIndex i)
{
    m_unitTarget = pUnitTarget;
    m_itemTarget = pItemTarget;
    m_gameObjTarget = pGOTarget;

    // 法术效果类型
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
		// todo: NPC施法
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
	// 处理道具法术效果
    for (ItemTargetList::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

    // 处理地面区域法术效果(火墙)
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        if (m_proto->Effect[j] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            HandleEffects(nullptr, nullptr, nullptr, SpellEffectIndex(j));
        }
    }

    // 处理Unit法术效果
    for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

	// 处理GameObject法术效果
    for (GOTargetList::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        DoAllEffectOnTarget(&(*ihit));
    }

    // 施法道具消耗结算
    TakeCastItem();

    // 施法结束
    Finish(true);
}

SpellCastResult Spell::CheckCast(bool strict)
{
	// 检查技能冷却防止作弊
	if (strict && !m_IsTriggeredSpell && HasCooldown())
	{
		return SPELL_FAILED_NOT_READY;
	}

	// 检查魔量
	{
		SpellCastResult castResult = CheckPower();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// 检查施法材料
	if (!IsPassiveSpell(m_proto))
	{
		SpellCastResult castResult = CheckItems();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// 检查施法距离
	if (!m_IsTriggeredSpell)
	{
		SpellCastResult castResult = CheckRange(strict);
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// 检查法术免疫
	if (!m_IsTriggeredSpell)
	{
		SpellCastResult castResult = CheckCasterAuras();
		if (castResult != SPELL_CAST_OK)
		{
			return castResult;
		}
	}

	// todo: 检查其他施法条件，比如宠物超过上限，是否能对空地施放还是必须得有目标

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

		// 法术道具的使用次数用完了
		if (proto->max_charge_count && m_castItem->GetChargeCount() == 0)
		{
			return SPELL_FAILED_NO_CHARGES_REMAIN;
		}
	}

	// todo: 检查法术是否对装备有要求，例如某些法术必须穿特定的装备才能施放

	// 检查施法道具数量是否充足
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

	// todo: 检查其他施法条件，比如创造道具需要检查背包是否满了

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
	// 道具触发的法术不消耗能量
	if (m_castItem)
	{
		return SPELL_CAST_OK;
	}

	// 检查施法者能量是否充足
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
	// 法术道具施放的法术不消耗能量
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

	// todo: 法术消耗减免
}

void Spell::TakePower()
{
	// 法术道具或者触发型法术不耗魔
	if (m_castItem || m_IsTriggeredSpell)
	{
		return;
	}

	// 扣除魔耗
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
	// 没有使用法术道具，则跳过处理
    if (!m_castItem || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        return;
    }

    // 触发的法术不需要考虑法术道具消耗，例如装备武器、防具带来的效果
    /*if (m_IsTriggeredSpell && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM))
    {
        return;
    }*/

    const ItemEntry *proto = m_castItem->GetProto();
    if (!proto)
    {
        // 这种情况不应该出现
        sLog.outError("Cast item (%s) has no item prototype", m_castItem->GetGuidStr().c_str());
        return;
    }

	// 扣除使用次数
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
        // todo: 特殊职业技能处理
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

    // 有些法术既存在负面效果也存在正面效果，但仅当法术中的负面效果命中法术目标时才进行法术触发判定，所以这边记录下法术中的负面效果
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
    // 临时存储每个法术效果的目标列表
    UnitList tmpUnitLists[MAX_EFFECT_INDEX];
    // 法术效果的目标列表为tmpUnitLists[effToIndex[i]]，用来处理多个法术效果的目标匹配规则相同的情况，可减少法术目标搜索次数
    uint8 effToIndex[MAX_EFFECT_INDEX] = {0, 1, 2};
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (m_proto->Effect[i] == SPELL_EFFECT_NONE)
        {
            continue;
        }

        // 区域型光环总是将施法者视作法术目标
        if (IsAreaAuraEffect(m_proto->Effect[i]))
        {
            AddUnitTarget(m_caster, SpellEffectIndex(i));
        }

        // 如果当前法术效果的目标规则和前面的一样，则不用重复设置
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

        // 说明跟之前的法术效果的目标匹配规则不同，需要重新搜索法术目标
        if (effToIndex[i] == i)
        {
			// todo: 搜索目标
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

        // 如果玩家攻击了玩家的宠物，那么它的主人也将自动进入战斗状态
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
        	// todo: 主人也将自动进入战斗状态
        }

        // 对临时目标列表进行过滤
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

        // 将临时目标列表中的对象单元添加到法术目标列表中，同时进行命中判定
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

	// todo: 检查是施法者是否有增加施法范围的光环

	// todo: 是否需要限制法术影响的游戏对象个数

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
	// todo: 负面法术有些需要过滤自身，例如沉睡之雾，有些不需要，例如光箭就能强制攻击自己

    // 检查目标的生物类型，删除不合适的
    if (m_proto->EffectImplicitTarget[eff] != TARGET_SELF)
    {
        if (!CheckTargetCreatureType(target))
        {
            return false;
        }
    }

    // 检查目标是否能被攻击或被选中，删除不合适的
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

    // 如果玩家不可见或者是管理员，将其从目标列表移除
    if (target != m_caster && target->GetTypeId() == TYPEID_PLAYER)
    {
    	// todo: 不可见校验

        if (((Player*)target)->IsGameMaster())
        {
            return false;
        }
    }

    return true;
}

bool Spell::CheckTargetCreatureType(Unit* target) const
{
    // todo: 起死回生只能对不死系施放
    return true;
}

void Spell::AddUnitTarget(Unit* pVictim, SpellEffectIndex eff)
{
	if (m_proto->Effect[eff] == 0)
	{
		return;
	}

	// todo: 检查目标是否免疫法术效果
	// bool immuned = pVictim->IsImmuneToSpellEffect(m_proto, eff, pVictim == m_caster);
	bool immuned = false;
	ObjectGuid targetGUID = pVictim->GetObjectGuid();

	// 查找列表中已存在的目标
	for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
	{
		if (targetGUID == ihit->targetGUID)
		{
			if (!immuned)
			{
				// 仅记录未免疫的法术效果索引
				ihit->effectMask |= 1 << eff;
			}
			return;
		}
	}

	// 获取目标的法术命中结果
	TargetInfo target;
	target.targetGUID = targetGUID;               // 存储目标GUID
	target.effectMask = immuned ? 0 : (1 << eff); // 存储未免疫的法术效果索引
	target.processed = false;                     // 法术效果尚未生效
	target.missCondition = m_caster->SpellHitResult(pVictim, m_proto, m_canReflect); // 计算法术命中结果
	target.timeDelay = UI64LIT(0); // 魔兽世界的奥术飞弹需要飞行一段距离才能结算伤害，所以可能存在延迟，但是天堂的法术在施法瞬间就结算伤害
	// 处理法术反射场景，例如天堂的精灵魔法镜反射
	if (target.missCondition == SPELL_MISS_REFLECT)
	{
		target.reflectResult = m_caster->SpellHitResult(m_caster, m_proto, m_canReflect);

		// 不可能再次反射
		if (target.reflectResult == SPELL_MISS_REFLECT)
		{
			target.reflectResult = SPELL_MISS_RESIST;
		}
	}
	else
	{
		target.reflectResult = SPELL_MISS_NONE;
	}

	// 添加对象到目标列表
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
	// 避免对象多次应用法术效果
    if (target->processed)
    {
        return;
    }
    target->processed = true;

    // 法术效果掩码，记录了法术目标
    uint32 mask = target->effectMask;
    Unit* unit = m_caster->GetObjectGuid() == target->targetGUID ? m_caster : sObjectAccessor.GetUnit(*m_caster, target->targetGUID);
    if (!unit)
    {
        return;
    }

    // 获取原始施法者(如果存在)，并且使用他的数据来计算伤害/治疗量
    // 例如天堂的火墙这类法术，它的实现是创建一个GameObject，对象本身是不具备计算法术伤害所需数据，必须找到火墙这个GameObject的拥有者
    Unit* real_caster = GetAffectiveCaster();
    Unit* caster = real_caster ? real_caster : m_caster;

    SpellMissInfo missInfo = target->missCondition;
    // 需要使用默认单元来初始化m_unitTarget(可以在法术反射代码中改变)
    m_unitTarget = unit;

    // 重置治疗和伤害统计
    ResetEffectDamageAndHeal();

    // 初始化法术触发条件，在当前法术进行命中判定后，执行后续的法术触发判定
    uint32 procAttacker = m_procAttacker;
    uint32 procVictim   = m_procVictim;
    uint32 procEx       = PROC_EX_NONE;

    // 如果法术目标在负面法术中没有受到负面效果的影响，则清除触发条件，不进行法术触发判定
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
    	// todo: 法术触发判定
		m_unitTarget->ModifyPower(POWER_HEALTH, m_healing);
    }
    else if (m_damage)
    {
    }
    else if (procAttacker || procVictim) // 法术触发判定
    {
    }

    // todo: 生物AI处理
    /*if (unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
    {
        ((Creature*)unit)->AI()->SpellHit(m_caster, m_proto);
    }*/

    // todo: 生物AI处理
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
	// 天堂没有成就系统，也不存在延迟法术
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
			// todo: 进入战斗
		}
	}

	// 法术效果是创建光环
	if (IsSpellAppliesAura(m_proto, effectMask))
	{
		m_spellAuraHolder = CreateSpellAuraHolder(m_proto, unit, real_caster, m_castItem);
	}
	else
	{
		m_spellAuraHolder = nullptr;
	}

	// 执行法术效果
	for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
	{
		if (effectMask & (1 << i))
		{
			HandleEffects(unit, nullptr, nullptr, SpellEffectIndex(i));
		}
	}

	// 现在应用所有创建的光环
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
	// 更新尚未结束的法术
	if (m_Spell->GetState() != SPELL_STATE_FINISHED)
		m_Spell->Update(p_time);

	// 检查法术结束状态
	switch (m_Spell->GetState())
	{
		case SPELL_STATE_FINISHED:
		{
			// 法术已经结束，检查是否允许删除
			if (m_Spell->IsDeletable())
			{
				// 删除事件
				return true;
			}
			break;
		}
		default:
            break;
	}

	// 法术尚未处理完成，重入队列
	m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);

	// 事件尚未处理完成
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
