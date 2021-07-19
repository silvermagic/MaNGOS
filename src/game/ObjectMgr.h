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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include <string>
#include <map>
#include <limits>
#include "Common.h"
#include "Log.h"
#include "Utilities/Singleton.h"
#include "Entities/ObjectGuid.h"
#include "Items/Item.h"

class SQLStorage;
class Player;

#define MAX_PLAYER_NAME          12                         // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length ( > MAX_PLAYER_NAME for support declined names )
#define MAX_PET_NAME             12                         // max allowed by client name length
#define MAX_CHARTER_NAME         24                         // max allowed by client name length

typedef std::set<std::wstring> ReservedNamesMap;

bool normalizePlayerName(std::string& name); // 格式化玩家姓名

/**
 * GUID生成器
 */
template<typename T>
class IdGenerator
{
public:
    explicit IdGenerator(char const* _name) : m_name(_name), m_nextGuid(1) {}

public:
    void Set(T val) { m_nextGuid = val; }
    T Generate();

public:
    T GetNextAfterMaxUsed() const { return m_nextGuid; }

private:
    char const* m_name;
    T m_nextGuid;
};

class ObjectMgr
{
    friend class MaNGOS::OperatorNew<ObjectMgr>;
protected:
    ObjectMgr();
    ~ObjectMgr();
    ObjectMgr(const ObjectMgr&) = delete;
    ObjectMgr& operator=(const ObjectMgr&) = delete;

public:
	static Player* GetPlayer(const char* name);                     // 根据玩家名称查询玩家对象
	static Player* GetPlayer(ObjectGuid guid, bool inWorld = true); // 查询玩家GUID查询玩家对象
	static const ItemEntry* GetItemEntry(uint32 id);                // 获取道具模板
	static const SpellEntry* GetSpellEntry(uint32 id);              // 获取法术模板

	static uint8 CheckPlayerName(const std::string& name, bool create = false); // 检查玩家名称是否合法
	static bool IsValidCharacterName(const std::string& name);                  // 检查玩家名称是否合法

protected:
	/*********************************************************/
	/***                GUID GENERATOR SYSTEM              ***/
	/*********************************************************/
	IdGenerator<uint32> m_MailIds;         // 邮件GUID生成器
	uint32 m_FirstTemporaryCreatureGuid;   // 地图本地生物对象的GUID生成器起始编号
	uint32 m_FirstTemporaryGameObjectGuid; // 地图本地游戏对象的GUID生成器起始编号
	ObjectGuidGenerator<HIGHGUID_UNIT>       m_StaticCreatureGuids;   // 用于初始化地图生物GUID生成器
	ObjectGuidGenerator<HIGHGUID_GAMEOBJECT> m_StaticGameObjectGuids; // 用于初始化地图生物GUID生成器
	ObjectGuidGenerator<HIGHGUID_PLAYER>     m_CharGuids; // 角色GUID生成器，创建角色时使用(需要存到character表时使用)
	ObjectGuidGenerator<HIGHGUID_ITEM>       m_ItemGuids; // 道具GUID生成器，克隆道具、创建新道具时使用(需要存到character_items表时使用)

public:
    void SetHighestGuids();

    uint32 GetFirstTemporaryCreatureLowGuid() const;   // 获取地图本地生物对象的GUID生成器起始编号
    uint32 GetFirstTemporaryGameObjectLowGuid() const; // 获取地图本地游戏对象的GUID生成器起始编号
    uint32 GenerateStaticCreatureLowGuid();            // 生成一个可用的生物GUID(GM通过命令创建的生物，需要存入数据库中)
    uint32 GenerateStaticGameObjectLowGuid();          // 生成一个可用的游戏对象GUID(GM通过命令创建的游戏对象，需要存入数据库中)
    uint32 GeneratePlayerLowGuid();                    // 生成一个可用的角色GUID
    uint32 GenerateItemLowGuid();                      // 生成一个可用的道具GUID

protected:
	/*********************************************************/
	/***              NAME VERIFICATION SYSTEM             ***/
	/*********************************************************/
	ReservedNamesMap m_ReservedNames; // 预留名称列表

public:
	void LoadReservedPlayersNames();                    // 加载预留的玩家名称列表，例如GM等
	bool IsReservedName(const std::string& name) const; // 判断用户是否使用了预留名称(创建角色时使用)

public:
	/*********************************************************/
	/***              PLAYER INFORMATION SYSTEM            ***/
	/*********************************************************/
	ObjectGuid GetPlayerGuidByName(std::string name) const;                    // 根据角色名称查询角色GUID
	bool GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const;        // 根据角色GUID查询角色名称
	std::string GetPlayerAccountIdByGUID(ObjectGuid guid) const;               // 根据角色GUID查询账户ID
	std::string GetPlayerAccountIdByPlayerName(const std::string& name) const; // 根据角色名称查询账户ID
};

#define sObjectMgr MaNGOS::Singleton<ObjectMgr>::Instance()

#endif
