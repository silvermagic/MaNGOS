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

#include <limits>
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Utilities/SingletonImpl.h"
#include "Entities/Player.h"
#include "Log.h"
#include "Util.h"
#include "AccountMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "SQLStorages.h"
#include "World.h"

INSTANTIATE_SINGLETON_1(ObjectMgr);

bool normalizePlayerName(std::string& name)
{
	if (name.empty())
		return false;

	wchar_t wstr_buf[MAX_INTERNAL_PLAYER_NAME + 1];
	size_t wstr_len = MAX_INTERNAL_PLAYER_NAME;

	if (!Utf8toWStr(name, &wstr_buf[0], wstr_len))
		return false;

	wstr_buf[0] = wcharToUpper(wstr_buf[0]);
	for (size_t i = 1; i < wstr_len; ++i)
		wstr_buf[i] = wcharToLower(wstr_buf[i]);

	if (!WStrToUtf8(wstr_buf, wstr_len, name))
		return false;

	return true;
}

bool isValidString(std::wstring wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
	if (strictMask == 0) // 任意语言
	{
		if (isExtendedLatinString(wstr, numericOrSpace))
			return true;
		if (isEastAsianString(wstr, numericOrSpace))
			return true;
		return false;
	}

	if (strictMask & 0x2) // 根据游戏服务区决定校验语言
	{
		switch (sConfig.GetIntDefault("L1J.ClientLanguage", 5))
		{
			case LOCALE_zhTW:
			case LOCALE_zhCN:
				if (isEastAsianString(wstr, numericOrSpace))
					return true;
				break;
			default:
				if (isBasicLatinString(wstr, numericOrSpace))
					return true;
				break;
		}
	}

	if (strictMask & 0x1) // 拉丁语
	{
		if (isBasicLatinString(wstr, numericOrSpace))
			return true;
	}

	return false;
}

ObjectMgr::ObjectMgr() : m_MailIds("Mail ids")
{
}

ObjectMgr::~ObjectMgr()
{
}

Player* ObjectMgr::GetPlayer(const char* name)
{
	return ObjectAccessor::FindPlayerByName(name);
}

Player* ObjectMgr::GetPlayer(ObjectGuid guid, bool inWorld /*=true*/)
{
	return ObjectAccessor::FindPlayer(guid, inWorld);
}

const ItemEntry* ObjectMgr::GetItemEntry(uint32 id)
{
	auto iter = ITEM_PROTOS.find(id);
	return (iter != ITEM_PROTOS.end()) ? &iter->second : nullptr;
}

const SpellEntry* ObjectMgr::GetSpellEntry(uint32 id)
{
	auto iter = SPELL_PROTOS.find(id);
	return (iter != SPELL_PROTOS.end()) ? &iter->second : nullptr;
}

uint8 ObjectMgr::CheckPlayerName(const std::string& name, bool create)
{
	std::wstring wname;
	if (!Utf8toWStr(name, wname))
		return CHAR_NAME_INVALID_CHARACTER;

	if (wname.size() > MAX_PLAYER_NAME)
		return CHAR_NAME_TOO_LONG;

	uint32 minName = sWorld.GetConfig(CONFIG_UINT32_MIN_PLAYER_NAME);
	if (wname.size() < minName)
		return CHAR_NAME_TOO_SHORT;

	uint32 strictMask = sWorld.GetConfig(CONFIG_UINT32_STRICT_PLAYER_NAMES);
	if (!isValidString(wname, strictMask, false, create))
		return CHAR_NAME_MIXED_LANGUAGES;

	return CHAR_NAME_SUCCESS;
}

bool ObjectMgr::IsValidCharacterName(const std::string& name)
{
	std::wstring wname;
	if (!Utf8toWStr(name, wname))
		return false;

	if (wname.size() > MAX_CHARTER_NAME)
		return false;

	uint32 minName = sWorld.GetConfig(CONFIG_UINT32_MIN_CHARTER_NAME);
	if (wname.size() < minName)
		return false;

	uint32 strictMask = sWorld.GetConfig(CONFIG_UINT32_STRICT_CHARTER_NAMES);

	return isValidString(wname, strictMask, true);
}

void ObjectMgr::SetHighestGuids()
{
	QueryResult* result = CharacterDatabase.Query("SELECT MAX(objid) FROM characters");
	if (result)
	{
		m_CharGuids.Set((*result)[0].GetUInt32() + 1);
		delete result;
	}

	result = CharacterDatabase.Query(
		"select max(id)+1 as nextid from (select id from character_items "
		"union all select id from character_warehouse "
		"union all select id from character_elf_warehouse "
		"union all select id from clan_warehouse) t");
	if (result)
	{
		m_ItemGuids.Set((*result)[0].GetUInt32() + 1);
		delete result;
	}

	result = CharacterDatabase.Query("SELECT MAX(id) FROM mail");
	if (result)
	{
		m_MailIds.Set((*result)[0].GetUInt32() + 1);
		delete result;
	}

	m_StaticCreatureGuids.Set(m_FirstTemporaryCreatureGuid);
	m_FirstTemporaryCreatureGuid += sWorld.GetConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE);

	m_StaticGameObjectGuids.Set(m_FirstTemporaryGameObjectGuid);
	m_FirstTemporaryGameObjectGuid += sWorld.GetConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT);
}

uint32 ObjectMgr::GetFirstTemporaryCreatureLowGuid() const
{
	return m_FirstTemporaryCreatureGuid;
}
uint32 ObjectMgr::GetFirstTemporaryGameObjectLowGuid() const
{
	return m_FirstTemporaryGameObjectGuid;
}

uint32 ObjectMgr::GenerateStaticCreatureLowGuid()
{
	if (m_StaticCreatureGuids.GetNextAfterMaxUsed() >= m_FirstTemporaryCreatureGuid)
	{
		return 0;
	}

	return m_StaticCreatureGuids.Generate();
}

uint32 ObjectMgr::GenerateStaticGameObjectLowGuid()
{
	if (m_StaticGameObjectGuids.GetNextAfterMaxUsed() >= m_FirstTemporaryGameObjectGuid)
	{
		return 0;
	}

	return m_StaticGameObjectGuids.Generate();
}

uint32 ObjectMgr::GeneratePlayerLowGuid()
{
	return m_CharGuids.Generate();
}

uint32 ObjectMgr::GenerateItemLowGuid()
{
	return m_ItemGuids.Generate();
}

void ObjectMgr::LoadReservedPlayersNames()
{
	m_ReservedNames.clear();
}

bool ObjectMgr::IsReservedName(const std::string& name) const
{
	std::wstring wstr;
	if (!Utf8toWStr(name, wstr))
	{
		return false;
	}

	wstrToLower(wstr);

	return m_ReservedNames.find(wstr) != m_ReservedNames.end();
}

ObjectGuid ObjectMgr::GetPlayerGuidByName(std::string name) const
{
	ObjectGuid guid;

	CharacterDatabase.escape_string(name);
	QueryResult* result = CharacterDatabase.PQuery("SELECT objid FROM characters WHERE char_name = '%s'", name.c_str());
	if (result)
	{
		guid = ObjectGuid(HIGHGUID_PLAYER, (*result)[0].GetUInt32());

		delete result;
	}

	return guid;
}

bool ObjectMgr::GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const
{
	if (Player* player = GetPlayer(guid))
	{
		name = player->GetName();
		return true;
	}

	uint32 lowguid = guid.GetCounter();
	QueryResult* result = CharacterDatabase.PQuery("SELECT char_name FROM characters WHERE objid = '%u'", lowguid);
	if (result)
	{
		name = (*result)[0].GetCppString();
		delete result;
		return true;
	}

	return false;
}

std::string ObjectMgr::GetPlayerAccountIdByGUID(ObjectGuid guid) const
{
	if (!guid.IsPlayer())
	{
		return 0;
	}
	if (Player* player = GetPlayer(guid))
	{
		return player->GetSession()->GetAccountId();
	}

	uint32 lowguid = guid.GetCounter();
	QueryResult* result = CharacterDatabase.PQuery("SELECT account_name FROM characters WHERE objid = '%u'", lowguid);
	if (result)
	{
		std::string acc = (*result)[0].GetCppString();
		delete result;
		return acc;
	}

	return "";
}

std::string ObjectMgr::GetPlayerAccountIdByPlayerName(const std::string& name) const
{
	QueryResult* result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE char_name = '%s'",
		name.c_str());
	if (result)
	{
		std::string acc = (*result)[0].GetCppString();
		delete result;
		return acc;
	}

	return "";
}