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

#ifndef MANGOS_MAP_H
#define MANGOS_MAP_H

#include <list>
#include <boost/dynamic_bitset.hpp>
#include "GridMap.h"
#include "TileMap.h"

class MANGOS_DLL_SPEC Map : public GridMap, public TileMap
{
public:
    static void DeleteFromWorld(Player* player);

protected:
    Map(uint32 id, uint32 InstanceId);

public:
    virtual ~Map();

    // ���µ�ͼ
    virtual void Update(const uint32&);

    // �����Ϸ�����Ƴ��б�
    void AddObjectToRemoveList(WorldObject* obj);

	// �ƶ���Ϸ����
	void PlayerRelocation(Player*, CoordUnit x, CoordUnit y, Towards orientation);
	void CreatureRelocation(Creature* creature, CoordUnit x, CoordUnit y, Towards orientation);
	bool CreatureRespawnRelocation(Creature* creature);

	// �㲥��Ϣ
	void MessageBroadcast(Player*, WorldPacket*, bool to_self);
	void MessageBroadcast(WorldObject*, WorldPacket*);
	void MessageDistBroadcast(Player*, WorldPacket*, int32 dist, bool to_self, bool own_team_only = false);
	void MessageDistBroadcast(WorldObject*, WorldPacket*, int32 dist);

	/**
	 * ����������
	 */

	bool Add(Player*);                       // �����Ϸ��������ϵͳ
	void Remove(Player*, bool);              // ɾ����Ϸ��������ϵͳ

	/**
	 * ��������
	 */

	uint32 GetInstID() const;                       // ��ȡ��ͼʵ�����
	uint32 GenerateLocalLowGuid(HighGuid guidhigh); // Ϊ��ͼ�ڴ����Ķ�̬�������GUID������Щ��ɱ�������²����Ĺ���

protected:
	uint32 m_instId;

	ObjectGuidGenerator<HIGHGUID_UNIT> m_CreatureGuids;           // ����GUID������
	ObjectGuidGenerator<HIGHGUID_GAMEOBJECT> m_GameObjectGuids;   // ��Ϸ����GUID������
	ObjectGuidGenerator<HIGHGUID_DYNAMICOBJECT> m_DynObjectGuids; // ��̬����GUID������
	ObjectGuidGenerator<HIGHGUID_PET> m_PetGuids;                 // ����GUID������
};

class MANGOS_DLL_SPEC WorldMap : public Map
{
public:
    WorldMap(uint32 id) : Map(id, 0) {}
    ~WorldMap() {}
};

#endif
