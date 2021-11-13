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

bool normalizePlayerName(std::string& name); // ��ʽ���������

/**
 * GUID������
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
	static Player* GetPlayer(const char* name);                     // ����������Ʋ�ѯ��Ҷ���
	static Player* GetPlayer(ObjectGuid guid, bool inWorld = true); // ��ѯ���GUID��ѯ��Ҷ���
	static const ItemEntry* GetItemEntry(uint32 id);                // ��ȡ����ģ��
	static const SpellEntry* GetSpellEntry(uint32 id);              // ��ȡ����ģ��

	static uint8 CheckPlayerName(const std::string& name, bool create = false); // �����������Ƿ�Ϸ�
	static bool IsValidCharacterName(const std::string& name);                  // �����������Ƿ�Ϸ�

protected:
	/*********************************************************/
	/***                GUID GENERATOR SYSTEM              ***/
	/*********************************************************/
	IdGenerator<uint32> m_MailIds;         // �ʼ�GUID������
	uint32 m_FirstTemporaryCreatureGuid;   // ��ͼ������������GUID��������ʼ���
	uint32 m_FirstTemporaryGameObjectGuid; // ��ͼ������Ϸ�����GUID��������ʼ���
	ObjectGuidGenerator<HIGHGUID_UNIT>       m_StaticCreatureGuids;   // ���ڳ�ʼ����ͼ����GUID������
	ObjectGuidGenerator<HIGHGUID_GAMEOBJECT> m_StaticGameObjectGuids; // ���ڳ�ʼ����ͼ����GUID������
	ObjectGuidGenerator<HIGHGUID_PLAYER>     m_CharGuids; // ��ɫGUID��������������ɫʱʹ��(��Ҫ�浽character��ʱʹ��)
	ObjectGuidGenerator<HIGHGUID_ITEM>       m_ItemGuids; // ����GUID����������¡���ߡ������µ���ʱʹ��(��Ҫ�浽character_items��ʱʹ��)

public:
    void SetHighestGuids();

    uint32 GetFirstTemporaryCreatureLowGuid() const;   // ��ȡ��ͼ������������GUID��������ʼ���
    uint32 GetFirstTemporaryGameObjectLowGuid() const; // ��ȡ��ͼ������Ϸ�����GUID��������ʼ���
    uint32 GenerateStaticCreatureLowGuid();            // ����һ�����õ�����GUID(GMͨ��������������Ҫ�������ݿ���)
    uint32 GenerateStaticGameObjectLowGuid();          // ����һ�����õ���Ϸ����GUID(GMͨ�����������Ϸ������Ҫ�������ݿ���)
    uint32 GeneratePlayerLowGuid();                    // ����һ�����õĽ�ɫGUID
    uint32 GenerateItemLowGuid();                      // ����һ�����õĵ���GUID

protected:
	/*********************************************************/
	/***              NAME VERIFICATION SYSTEM             ***/
	/*********************************************************/
	ReservedNamesMap m_ReservedNames; // Ԥ�������б�

public:
	void LoadReservedPlayersNames();                    // ����Ԥ������������б�����GM��
	bool IsReservedName(const std::string& name) const; // �ж��û��Ƿ�ʹ����Ԥ������(������ɫʱʹ��)

public:
	/*********************************************************/
	/***              PLAYER INFORMATION SYSTEM            ***/
	/*********************************************************/
	ObjectGuid GetPlayerGuidByName(std::string name) const;                    // ���ݽ�ɫ���Ʋ�ѯ��ɫGUID
	bool GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const;        // ���ݽ�ɫGUID��ѯ��ɫ����
	std::string GetPlayerAccountIdByGUID(ObjectGuid guid) const;               // ���ݽ�ɫGUID��ѯ�˻�ID
	std::string GetPlayerAccountIdByPlayerName(const std::string& name) const; // ���ݽ�ɫ���Ʋ�ѯ�˻�ID
};

#define sObjectMgr MaNGOS::Singleton<ObjectMgr>::Instance()

#endif
