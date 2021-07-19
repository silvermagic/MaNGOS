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

#include "Entities/Player.h"
#include "ObjectMgr.h"
#include "Inventory.h"
#include "Item.h"

std::map<ItemAttrEnchantType, std::vector<std::string>> ATTR_ENCHANT_NAME = {
	{ITEM_ATTR_ENCHANT_TYPE_NONE, {"", "", "", ""}},
	{ITEM_ATTR_ENCHANT_TYPE_EARTH, {"", "$6124 ", "$6125 ", "$6126 "}},
	{ITEM_ATTR_ENCHANT_TYPE_FIRE, {"", "$6115 ", "$6116 ", "$6117 "}},
	{ITEM_ATTR_ENCHANT_TYPE_WATER, {"", "$6118 ", "$6119 ", "$6120 "}},
	{ITEM_ATTR_ENCHANT_TYPE_WIND, {"", "$6121 ", "$6122 ", "$6123 "}},
};

Item* Item::CreateItem(uint32 item, uint32 count)
{
	if (count < 1)
	{
		return nullptr;
	}

	const ItemEntry* proto = ObjectMgr::GetItemEntry(item);
	if (proto)
	{
		Item* pItem = new Item;
		if (pItem->Create(sObjectMgr.GenerateItemLowGuid(), item))
		{
			pItem->SetCount(count);
			return pItem;
		}
		else
		{
			delete pItem;
		}
	}

	return nullptr;
}

Item* Item::CloneItem(uint32 count) const
{
	Item* newItem = CreateItem(GetEntry(), count);
	if (!newItem)
	{
		return nullptr;
	}
	newItem->SetOwnerGuid(GetOwnerGuid());
	newItem->SetUInt32Value(ITEM_FIELD_FLAGS,     GetUInt32Value(ITEM_FIELD_FLAGS));
	newItem->SetInt32Value(ITEM_FIELD_ENCHANT_LEVEL,  GetInt32Value(ITEM_FIELD_ENCHANT_LEVEL));
	newItem->SetUInt32Value(ITEM_FIELD_DURABILITY,  GetInt32Value(ITEM_FIELD_DURABILITY));
	newItem->SetInt32Value(ITEM_FIELD_CHARGE_COUNT,  GetInt32Value(ITEM_FIELD_CHARGE_COUNT));
	newItem->SetUInt32Value(ITEM_FIELD_DURATION,  GetUInt32Value(ITEM_FIELD_DURATION));
	newItem->SetUInt32Value(ITEM_FIELD_LAST_USED,  GetUInt32Value(ITEM_FIELD_LAST_USED));
	return newItem;
}

bool Item::Create(uint32 guidlow, uint32 itemid)
{
	Object::_Create(guidlow, 0, HIGHGUID_ITEM);

	SetEntry(itemid);
	SetGuidValue(ITEM_FIELD_OWNER, ObjectGuid());
	const ItemEntry* proto = ObjectMgr::GetItemEntry(itemid);
	if (!proto)
	{
		return false;
	}
	m_proto = proto;
	SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);

	return true;
}

Item::Item() : m_slot(NULL_SLOT)
{
	m_valuesCount = ITEM_END;
}

void Item::GetStatusBytes(ByteBuffer &buffer) const
{
	if (m_proto->Class == ITEM_CLASS_GENERIC)
	{
		switch (m_proto->SubClass)
		{
			case ITEM_SUBCLASS_GENERIC_LIGHT:
				buffer << uint8(22);
				buffer << uint16(0); // todo: 道具照明范围
				break;
			case ITEM_SUBCLASS_GENERIC_FOOD:
				buffer << uint8(21);
				buffer << uint16(m_proto->food);
				break;
			case ITEM_SUBCLASS_GENERIC_ARROW:
			case ITEM_SUBCLASS_GENERIC_STING:
				buffer << uint8(1);
				buffer << uint8(m_proto->ammo_damage_tiny);
				buffer << uint8(m_proto->ammo_damage_large);
				break;
			default:
				buffer << uint8(23);
				break;
		}
		buffer << uint8(m_proto->material);
		buffer << GetWeight();
	}
	else if (m_proto->Class == ITEM_CLASS_ARMOR || m_proto->Class == ITEM_CLASS_WEAPON)
	{
		if (m_proto->Class == ITEM_CLASS_WEAPON)
		{
			buffer << uint8(1);
			buffer << uint8(m_proto->damage_tiny);
			buffer << uint8(m_proto->damage_large);
			buffer << uint8(m_proto->material);
			buffer << GetWeight();
		}
		else
		{
			buffer << uint8(19);
			// todo: 计算道具防御值
			buffer << uint16(0);
			buffer << uint8(m_proto->material);
			buffer << GetWeight();
		}

		if (GetEnchantLevel() != 0)
		{
			buffer << uint8(2);
			if (m_proto->Class == ITEM_CLASS_ARMOR)
			{
				switch (m_proto->SubClass)
				{
					case ITEM_SUBCLASS_ARMOR_NECKLACE:
					case ITEM_SUBCLASS_ARMOR_RING:
					case ITEM_SUBCLASS_ARMOR_BELT:
					case ITEM_SUBCLASS_ARMOR_EARRING:
						buffer << uint8(0);
						break;
					default:
						buffer << uint8(GetEnchantLevel());
						break;
				}
			}
			else
			{
				buffer << uint8(GetEnchantLevel());
			}
		}

		if (GetDurability() != 0)
		{
			buffer << uint8(3);
			buffer << uint8(GetDurability());
		}

		if (m_proto->IsTwoHandWeapon())
		{
			buffer << uint8(4);
		}

		if (m_proto->melee_hit_chance != 0)
		{
			buffer << uint8(5);
			buffer << uint8(m_proto->melee_hit_chance);
		}
		if (m_proto->melee_damage_done != 0)
		{
			buffer << uint8(6);
			buffer << uint8(m_proto->melee_damage_done);
		}

		buffer << uint8(7);
		buffer << uint8(m_proto->allowable);

		if (m_proto->ranged_hit_chance != 0)
		{
			buffer << uint8(24);
			buffer << uint8(m_proto->ranged_hit_chance);
		}
		if (m_proto->ranged_damage_done != 0)
		{
			buffer << uint8(35);
			buffer << uint8(m_proto->ranged_damage_done);
		}

		if (m_proto->ItemId == MANA_WAND || m_proto->ItemId == IRON_MANA_WAND)
		{
			buffer << uint8(16);
		}

		if (m_proto->ItemId == RUIN_SWORD)
		{
			buffer << uint8(34);
		}

		if (m_proto->strength != 0)
		{
			buffer << uint8(8);
			buffer << uint8(m_proto->strength);
		}
		if (m_proto->agility != 0)
		{
			buffer << uint8(9);
			buffer << uint8(m_proto->agility);
		}
		if (m_proto->stamina != 0)
		{
			buffer << uint8(10);
			buffer << uint8(m_proto->stamina);
		}
		if (m_proto->spirit != 0)
		{
			buffer << uint8(11);
			buffer << uint8(m_proto->spirit);
		}
		if (m_proto->intellect != 0)
		{
			buffer << uint8(12);
			buffer << uint8(m_proto->intellect);
		}
		if (m_proto->charm != 0)
		{
			buffer << uint8(13);
			buffer << uint8(m_proto->charm);
		}

		if (m_proto->IsHaste())
		{
			buffer << uint8(18);
		}

		if (m_proto->defense_fire != 0 || GetDefense(SPELL_SCHOOL_FIRE) != 0)
		{
			buffer << uint8(27);
			buffer << uint8(m_proto->defense_fire + GetDefense(SPELL_SCHOOL_FIRE));
		}
		if (m_proto->defense_water != 0 || GetDefense(SPELL_SCHOOL_WATER) != 0)
		{
			buffer << uint8(28);
			buffer << uint8(m_proto->defense_water + GetDefense(SPELL_SCHOOL_WATER));
		}
		if (m_proto->defense_wind != 0 || GetDefense(SPELL_SCHOOL_WIND) != 0)
		{
			buffer << uint8(29);
			buffer << uint8(m_proto->defense_wind + GetDefense(SPELL_SCHOOL_WIND));
		}
		if (m_proto->defense_earth != 0 || GetDefense(SPELL_SCHOOL_EARTH) != 0)
		{
			buffer << uint8(30);
			buffer << uint8(m_proto->defense_earth + GetDefense(SPELL_SCHOOL_EARTH));
		}

		if (m_proto->health != 0 || GetMaxHealth() != 0)
		{
			buffer << uint8(14);
			buffer << uint16(m_proto->health + GetMaxHealth());
		}
		if (m_proto->mana != 0 || GetMaxMana() != 0)
		{
			buffer << uint8(32);
			buffer << uint16(m_proto->mana + GetMaxMana());
		}
		if (m_proto->spell_power != 0 || GetSpellPower())
		{
			buffer << uint8(17);
			buffer << uint16(m_proto->spell_power + GetSpellPower());
		}

		if (m_proto->resist_freeze != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_freeze);
			buffer << uint8(33);
			buffer << uint8(1);
		}
		if (m_proto->resist_stone != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_stone);
			buffer << uint8(33);
			buffer << uint8(2);
		}
		if (m_proto->resist_sleep != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_sleep);
			buffer << uint8(33);
			buffer << uint8(3);
		}
		if (m_proto->resist_blind != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_blind);
			buffer << uint8(33);
			buffer << uint8(4);
		}
		if (m_proto->resist_stun != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_stun);
			buffer << uint8(33);
			buffer << uint8(5);
		}
		if (m_proto->resist_sustain != 0)
		{
			buffer << uint8(15);
			buffer << uint8(m_proto->resist_sustain);
			buffer << uint8(33);
			buffer << uint8(6);
		}
	}
}

const ObjectGuid& Item::GetOwnerGuid() const
{
	return GetGuidValue(ITEM_FIELD_OWNER);
}

void Item::SetOwnerGuid(ObjectGuid guid)
{
	SetGuidValue(ITEM_FIELD_OWNER, guid);
}

Player* Item::GetOwner() const
{
	return sObjectMgr.GetPlayer(GetOwnerGuid());
}

uint32 Item::GetItemId() const
{
	MANGOS_ASSERT(m_proto != nullptr);

	return m_proto->ItemId;
}

const ItemEntry* Item::GetProto() const
{
	MANGOS_ASSERT(m_proto != nullptr);

	return m_proto;
}

std::string Item::GetName() const
{
	MANGOS_ASSERT(m_proto != nullptr);

	return m_proto->name;
}

std::string Item::GetName(bool view) const
{
	return GetNumberedName(view, GetCount());
}

std::string Item::GetNumberedName(bool view, int32 count) const
{
	std::string name;
	if (IsIdentified())
	{
		if (m_proto->Class == ITEM_CLASS_WEAPON)
		{
			name += ATTR_ENCHANT_NAME[GetAttrEnchantKind()][GetAttrEnchantLevel()];
		}
		if (m_proto->Class == ITEM_CLASS_WEAPON || m_proto->Class == ITEM_CLASS_ARMOR)
		{
			int32 lvl = GetEnchantLevel();
			if (lvl > 0)
			{
				name += "+";
			}
			name += std::to_string(lvl) + " ";
		}
		name += m_proto->identified;
		if (m_proto->max_charge_count)
		{
			name += "(" + std::to_string(GetChargeCount()) + ")";
		}
		if (m_proto->max_use_time && m_proto->Class != ITEM_CLASS_GENERIC)
		{
			name += "(" + std::to_string(GetDuration()) + ")";
		}
	}
	else
	{
		name += m_proto->unidentified;
	}

	if (count > 1)
	{
		name += "(" + std::to_string(count) + ")";
	}

	if (view)
	{
		// todo: 宠物项圈

		// todo: 灯

		if (IsEquipped())
		{
			switch (m_proto->Class)
			{
			case ITEM_CLASS_WEAPON:
				name += " ($9)";
				break;
			case ITEM_CLASS_ARMOR:
				name += " ($117)";
				break;
			case ITEM_CLASS_GENERIC:
				if (m_proto->SubClass == ITEM_SUBCLASS_GENERIC_PETITEM)
				{
					name += " ($117)";
				}
				break;
			}
		}
	}

	return name;
}

uint32 Item::GetWeight() const
{
	if (m_proto->weight == 0)
	{
		return 0;
	}

	return std::max(GetCount() * m_proto->weight / 1000, uint32(1));
}

uint8 Item::GetSlot() const
{
	return m_slot;
}

void Item::SetSlot(uint8 slot)
{
	m_slot = slot;
}

int32 Item::GetCount() const
{
	return GetInt32Value(ITEM_FIELD_STACK_COUNT);
}

void Item::SetCount(int32 number)
{
	SetUInt32Value(ITEM_FIELD_STACK_COUNT, number > 0 ? number : 0);
}

uint32 Item::GetDurability() const
{
	return GetUInt32Value(ITEM_FIELD_DURABILITY);
}

void Item::SetDurability(int32 durability)
{
	SetUInt32Value(ITEM_FIELD_DURABILITY, boost::algorithm::clamp(durability, 0, 127));
}

uint32 Item::GetDuration() const
{
	return GetUInt32Value(ITEM_FIELD_DURATION);
}

void Item::SetDuration(uint32 duration)
{
	SetUInt32Value(ITEM_FIELD_DURATION, duration);
}

int32 Item::GetChargeCount() const
{
	return GetInt32Value(ITEM_FIELD_CHARGE_COUNT);
}

void Item::SetChargeCount(int32 number)
{
	SetUInt32Value(ITEM_FIELD_CHARGE_COUNT, number > 0 ? number : 0);
}

int32 Item::GetEnchantLevel() const
{
	return GetInt32Value(ITEM_FIELD_ENCHANT_LEVEL);
}

void Item::SetEnchantLevel(int32 lvl)
{
	SetInt32Value(ITEM_FIELD_ENCHANT_LEVEL, boost::algorithm::clamp(lvl, -127, 127));
}

uint8 Item::GetBless() const
{
	return GetByteValue(ITEM_FIELD_FLAGS, 0);
}

void Item::SetBless(uint8 bless)
{
	SetByteValue(ITEM_FIELD_FLAGS, 0, bless);
}

ItemAttrEnchantType Item::GetAttrEnchantKind() const
{
	return ItemAttrEnchantType(GetUInt32Value(ITEM_FIELD_ATTR_ENCHANT_KIND));
}

int32 Item::GetAttrEnchantLevel() const
{
	return GetInt32Value(ITEM_FIELD_ENCHANT_LEVEL);
}

void Item::SetAttrEnchantKind(ItemAttrEnchantType type)
{
	SetUInt32Value(ITEM_FIELD_ATTR_ENCHANT_LEVEL, type);
}

void Item::SetAttrEnchantLevel(int32 lvl)
{
	SetInt32Value(ITEM_FIELD_ATTR_ENCHANT_LEVEL, boost::algorithm::clamp(lvl, 0, 3));
}

int32 Item::GetDefense(SpellSchools school) const
{
	MANGOS_ASSERT(m_proto != nullptr);

	switch (school)
	{
	case SPELL_SCHOOL_NORMAL:
		// todo: 特殊道具强化处理 抗魔法斗篷
		return m_proto->defense_magic;
	case SPELL_SCHOOL_WIND:
		return GetInt32Value(ITTEM_FIELD_DEFENSE_WIND);
	case SPELL_SCHOOL_WATER:
		return GetInt32Value(ITTEM_FIELD_DEFENSE_WATER);
	case SPELL_SCHOOL_FIRE:
		return GetInt32Value(ITTEM_FIELD_DEFENSE_FIRE);
	case SPELL_SCHOOL_EARTH:
		return GetInt32Value(ITTEM_FIELD_DEFENSE_EARTH);
	default:
		return 0;
	}
}

void Item::SetDefense(SpellSchools school, int32 value)
{
	switch (school)
	{
	case SPELL_SCHOOL_WIND:
		SetInt32Value(ITTEM_FIELD_DEFENSE_WIND, value);
		break;
	case SPELL_SCHOOL_WATER:
		SetInt32Value(ITTEM_FIELD_DEFENSE_WATER, value);
		break;
	case SPELL_SCHOOL_FIRE:
		SetInt32Value(ITTEM_FIELD_DEFENSE_FIRE, value);
		break;
	case SPELL_SCHOOL_EARTH:
		SetInt32Value(ITTEM_FIELD_DEFENSE_EARTH, value);
		break;
	default:
		break;
	}
}

int32 Item::GetSpellPower() const
{
	return GetInt32Value(ITEM_FIELD_SPELL_POWER);
}

void Item::SetSpellPower(int32 value)
{
	SetInt32Value(ITEM_FIELD_SPELL_POWER, value);
}

int32 Item::GetMaxMana() const
{
	return GetInt32Value(ITEM_FIELD_MANA);
}

void Item::SetMaxMana(int32 value)
{
	SetInt32Value(ITEM_FIELD_MANA, value);
}

int32 Item::GetManaRegen() const
{
	return GetInt32Value(ITEM_FIELD_MANA_REGEN);
}

void Item::SetManaRegen(int32 value)
{
	SetInt32Value(ITEM_FIELD_MANA_REGEN, value);
}

int32 Item::GetMaxHealth() const
{
	return GetInt32Value(ITEM_FIELD_HEALTH);
}

void Item::SetMaxHealth(int32 value)
{
	SetInt32Value(ITEM_FIELD_HEALTH, value);
}

int32 Item::GetHealthegen() const
{
	return GetInt32Value(ITEM_FIELD_HEALTH_REGEN);
}

void Item::SetHealthRegen(int32 value)
{
	SetInt32Value(ITEM_FIELD_HEALTH_REGEN, value);
}

int32 Item::GetStat(Stats stat)
{
	switch (stat)
	{
	case STAT_STRENGTH:
		return GetInt32Value(ITEM_FIELD_INTELLECT);
	case STAT_AGILITY:
		return GetInt32Value(ITEM_FIELD_AGILITY);
	case STAT_STAMINA:
		return GetInt32Value(ITEM_FIELD_STAMINA);
	case STAT_SPIRIT:
		return GetInt32Value(ITEM_FIELD_SPIRIT);
	case STAT_CHARM:
		return GetInt32Value(ITEM_FIELD_CHARM);
	case STAT_INTELLECT:
		return GetInt32Value(ITEM_FIELD_INTELLECT);
	}

	return 0;
}

void Item::SetStat(Stats stat, int32 value)
{
	switch (stat)
	{
	case STAT_STRENGTH:
		SetInt32Value(ITEM_FIELD_INTELLECT, value);
		break;
	case STAT_AGILITY:
		SetInt32Value(ITEM_FIELD_AGILITY, value);
		break;
	case STAT_STAMINA:
		SetInt32Value(ITEM_FIELD_STAMINA, value);
		break;
	case STAT_SPIRIT:
		SetInt32Value(ITEM_FIELD_SPIRIT, value);
		break;
	case STAT_CHARM:
		SetInt32Value(ITEM_FIELD_CHARM, value);
		break;
	case STAT_INTELLECT:
		SetInt32Value(ITEM_FIELD_INTELLECT, value);
		break;
	}
}

void Item::SetEquipped(bool equipped)
{
	if (equipped)
	{
		SetFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_EQUIPPED);
	}
	else
	{
		RemoveFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_EQUIPPED);
	}
}

void Item::SetIdentified(bool identified)
{
	if (identified)
	{
		SetFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_IDENTIFIED);
	}
	else
	{
		RemoveFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_IDENTIFIED);
	}
}

bool Item::IsEquipped() const
{
	return HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_EQUIPPED);
}

bool Item::IsIdentified() const
{
	return HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_IDENTIFIED);
}

bool Item::IsStackable() const
{
	MANGOS_ASSERT(m_proto != nullptr);

	return m_proto->IsStackable();
}

void Item::SetAuraEffect(const ItemAuraEffect& effect)
{
	for (uint8 i = 0; i < MAX_ITEM_AURA_EFFECT; ++i)
	{
		m_effect.types[i] = effect.types[i];
		m_effect.amounts[i] = effect.amounts[i];
	}
	m_effect.duration = effect.duration;
}

void Item::SetAuraEffectDuration(uint32 duration)
{
	m_effect.duration = duration;
}

void Item::ClearAuraEffect()
{
	for (uint8 i = 0; i < MAX_ITEM_AURA_EFFECT; ++i)
	{
		m_effect.types[i] = ITEM_AURA_EFFECT_TYPE_NONE;
		m_effect.amounts[i] = 0;
	}
	m_effect.duration = 0;
}

int8 Item::GetAuraEffect(ItemAuraEffectType type) const
{
	if (m_effect.duration <= 0)
	{
		return 0;
	}

	for (uint8 i = 0; i < MAX_ITEM_AURA_EFFECT; ++i)
	{
		if (m_effect.types[i] == type)
		{
			return m_effect.amounts[i];
		}
	}

	return 0;
}

bool Item::IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const
{
	return true;
}