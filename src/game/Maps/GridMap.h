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

#ifndef MANGOS_GRIDMAP_H
#define MANGOS_GRIDMAP_H

#include <boost/dynamic_bitset.hpp>
#include "Entities/ObjectGuid.h"
#include "References/MapRefManager.h"
#include "GridDefines.h"

class Unit;

class MANGOS_DLL_SPEC GridMap : public GridRefManager<GridType>
{
protected:
	typedef MapRefManager PlayerList;
	typedef std::set<WorldObject*> ActiveNonPlayers;
	typedef TypeUnorderedMapContainer<AllMapStoredObjectTypes, ObjectGuid> MapStoredObjectTypesContainer;
public:
	GridMap(uint32 mapId);

	void UpdateObjectVisibility(WorldObject* obj, const GridPair& gridPair); // ������Ϸ������Ұ����

	/**
	 * ��ѯ�����ͼ�ڵĶ���
	 */

	Creature*      GetAnyTypeCreature(ObjectGuid guid); // ��ѯ��ͼ������
	Corpse*        GetCorpse(ObjectGuid guid);          // ��ѯ��ͼ��ʬ��
	Creature*      GetCreature(ObjectGuid guid);        // ��ѯ��ͼ������
	DynamicObject* GetDynamicObject(ObjectGuid guid);   // ��ѯ��ͼ�ڶ�̬����
	GameObject*    GetGameObject(ObjectGuid guid);      // ��ѯ��ͼ����Ϸ����
	Pet*           GetPet(ObjectGuid guid);             // ��ѯ��ͼ�ڳ���
	Player*        GetPlayer(ObjectGuid guid);          // ��ѯ��ͼ�����
	Unit*          GetUnit(ObjectGuid guid);            // ��ѯ��ͼ�ڻ����
	WorldObject*   GetWorldObject(ObjectGuid guid);     // ��ѯ��ͼ�ڶ���

	MapStoredObjectTypesContainer& GetObjectsStore() { return m_objectsStore;  }
	const PlayerList& GetPlayers() const             { return m_mapRefManager; }

	/**
	 * ����������
	 */

	template<class T> void Add(T*);                        // �����Ϸ��������ϵͳ
	template<class T> void Remove(T*, bool);               // ɾ����Ϸ��������ϵͳ

	virtual bool Add(Player*) = 0;                         // �����Ϸ��������ϵͳ
	virtual void Remove(Player*, bool) = 0;                // ɾ����Ϸ��������ϵͳ

	void AddToActive(WorldObject* obj);                    // ��ӻ��Ϸ��������ϵͳ(�����)
	void RemoveFromActive(WorldObject* obj);               // ɾ�����Ϸ��������ϵͳ(�����)

	virtual void RemoveAllObjectsInRemoveList();           // �Ƴ��ӳ�ɾ������Ϸ����

	/**
	 * ����������
	 */

	template<class T, class CONTAINER> void Visit(const GridPair& gridPair, TypeContainerVisitor<T, CONTAINER>& visitor) { m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].Visit(visitor); }
	template<class T, class CONTAINER> void Visit(GridUnit x, GridUnit y, TypeContainerVisitor<T, CONTAINER>& visitor) { m_grids[GetGridID(x, y)].Visit(visitor); }

	/**
	 * ��������
	 */

	GridPair ComputeGridPair(CoordUnit x, CoordUnit y) const; // ��������ϵ����
	bool IsGridPosValid(const GridPair& gridPair) const;      // �ж����������Ƿ���Ч

protected:
	/**
	 * ����������
	 */
	template<class T>
	void AddToGrid(T*, const GridPair& gridPair);      // �����������Ϸ����
	template<class T>
	void RemoveFromGrid(T*, const GridPair& gridPair); // ��������ɾ����Ϸ����

	/**
	 * �����������λͼ
	 */
	uint32 GetGridID(GridUnit x, GridUnit y) const;     // ������������
	bool IsGridMarked(GridUnit x, GridUnit y) const;    // �ж�����Ԫ�Ƿ��Ѿ���������
	void MarkGrid(GridUnit x, GridUnit y);              // �������Ԫ����ʾ�ѱ���
	void ResetMarkedGrids();                            // ��������Ԫ�������

public:
	const GridMapInfo& m_gminfo;
	std::vector<GridType> m_grids; // �����ͼ
	boost::dynamic_bitset<> m_bits; // �����ͼ�������

	MapStoredObjectTypesContainer m_objectsStore;      // �����ͼ�����з���Ҷ���
	std::set<WorldObject*> i_objectsToRemove;          // ������Ҫ�ӳ��Ƴ��ĵ�ͼ�ϵĶ���

	MapRefManager m_mapRefManager;                     // ������Ҷ���
	MapRefManager::iterator m_mapRefIter;              // ���������е������ʹ�õĵ�����
	ActiveNonPlayers m_activeNonPlayers;               // ������Ϸ����(�����)
	ActiveNonPlayers::iterator m_activeNonPlayersIter; // ���������еĻ��Ϸ����(�����)��ʹ�õĵ�����
};

#endif //MANGOS_GRIDMAP_H
