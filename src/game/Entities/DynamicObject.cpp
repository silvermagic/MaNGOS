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
#include "Maps/CellImpl.h"
#include "Maps/GridNotifiersImpl.h"
#include "Spells/SpellAuras.h"
#include "Spells/SpellMgr.h"
#include "Common.h"
#include "Opcodes.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"

bool DynamicObject::Create(uint32 guidlow, Unit* caster, uint32 spellId, SpellEffectIndex effIndex, CoordUnit x, CoordUnit y, int32 duration, uint8 radius, DynamicObjectType type)
{
	WorldObject::_Create(guidlow, HIGHGUID_DYNAMICOBJECT, caster->GetPhaseMask());
	SetMap(caster->GetMap());
	Relocate(x, y, SOUTH);

	if (!IsPositionValid())
	{
		sLog.outError("DynamicObject (spell %u eff %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", spellId, effIndex, GetPositionX(), GetPositionY());
		return false;
	}

	const SpellEntry* proto = sObjectMgr.GetSpellEntry(spellId);
	if (!proto)
	{
		sLog.outError("DynamicObject (spell %u) not created. Spell not exist!", spellId);
		return false;
	}

	m_aliveDuration = duration;
	m_radius = radius;
	m_effIndex = effIndex;
	m_spellId = spellId;
	m_positive = IsPositiveEffect(proto, m_effIndex);

	return true;
}

DynamicObject::DynamicObject() : WorldObject()
{
	m_objectType |= TYPEMASK_DYNAMICOBJECT;
	m_objectTypeId = TYPEID_DYNAMICOBJECT;

	m_valuesCount = DYNAMICOBJECT_END;
}

void DynamicObject::AddToWorld()
{
	if (!IsInWorld())
	{
		GetMap()->GetObjectsStore().insert<DynamicObject>(GetObjectGuid(), (DynamicObject*)this);
	}

	Object::AddToWorld();
}

void DynamicObject::RemoveFromWorld()
{
	if (IsInWorld())
	{
		GetMap()->GetObjectsStore().erase<DynamicObject>(GetObjectGuid(), (DynamicObject*)NULL);
		GetViewPoint().Event_RemovedFromWorld();
	}

	Object::RemoveFromWorld();
}

void DynamicObject::Update(uint32 /*update_diff*/, uint32 p_time)
{
	Unit* caster = GetCaster();
	if (!caster)
	{
		Delete();
		return;
	}

	bool deleteThis = false;
	if (m_aliveDuration > int32(p_time))
	{
		m_aliveDuration -= p_time;
	}
	else
	{
		deleteThis = true;
	}

	if (m_radius)
	{
		MaNGOS::DynamicObjectUpdater notifier(*this, caster, m_positive);
		MaNGOS::VisitAllObjects(this, notifier, m_radius);
	}

	if (deleteThis)
	{
		caster->RemoveDynObjectWithGUID(GetObjectGuid());
		Delete();
	}
}

void DynamicObject::Delete()
{
	// todo: 发送消失消息
	// SendObjectDeSpawnAnim(GetObjectGuid());
	AddObjectToRemoveList();
}

void DynamicObject::Delay(int32 delaytime)
{
	m_aliveDuration -= delaytime;
	for (auto iter = m_affected.begin(); iter != m_affected.end();)
	{
		Unit* target = GetMap()->GetUnit((*iter));
		if (target)
		{
			SpellAuraHolder* holder = target->GetSpellAuraHolder(m_spellId, GetCasterGuid());
			if (!holder)
			{
				++iter;
				continue;
			}

			bool foundAura = false;
			for (uint8 i = m_effIndex; i < MAX_EFFECT_INDEX; ++i)
			{
				const SpellEntry* proto = holder->GetSpellProto();
				if(!proto)
				{
					continue;
				}
				if ((proto->Effect[i] == SPELL_EFFECT_PERSISTENT_AREA_AURA) && holder->m_auras[i])
				{
					foundAura = true;
					break;
				}
			}

			if (foundAura)
			{
				++iter;
				continue;
			}

			target->DelaySpellAuraHolder(m_spellId, delaytime, GetCasterGuid());
			++iter;
		}
		else
		{
			m_affected.erase(iter++);
		}
	}
}

void DynamicObject::AddAffected(Unit* unit)
{
	m_affected.insert(unit->GetObjectGuid());
}

void DynamicObject::RemoveAffected(Unit* unit)
{
	m_affected.erase(unit->GetObjectGuid());
}

Unit* DynamicObject::GetCaster() const
{
	return ObjectAccessor::GetUnit(*this, GetCasterGuid());
}

const ObjectGuid& DynamicObject::GetCasterGuid() const
{
	return m_caster;
}

uint32 DynamicObject::GetDuration() const
{
	return m_aliveDuration;
}

SpellEffectIndex DynamicObject::GetEffIndex() const
{
	return m_effIndex;
}

GridReference<DynamicObject>& DynamicObject::GetGridRef()
{
	return m_gridRef;
}
uint8 DynamicObject::GetRadius() const
{
	return m_radius;
}

uint32 DynamicObject::GetSpellId() const
{
	return m_spellId;
}

DynamicObjectType DynamicObject::GetType() const
{
	return m_type;
}

bool DynamicObject::IsAffecting(Unit* unit) const
{
	return m_affected.find(unit->GetObjectGuid()) != m_affected.end();
}

bool DynamicObject::IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const
{
	if (!IsInWorld() || !player->IsInWorld())
	{
		return false;
	}

	if (GetCasterGuid() == player->GetObjectGuid())
	{
		return true;
	}

	return IsInScreen(viewPoint);
}


