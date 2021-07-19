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

	void UpdateObjectVisibility(WorldObject* obj, const GridPair& gridPair); // 更新游戏对象视野内容

	/**
	 * 查询网格地图内的对象
	 */

	Creature*      GetAnyTypeCreature(ObjectGuid guid); // 查询地图内生物
	Corpse*        GetCorpse(ObjectGuid guid);          // 查询地图内尸体
	Creature*      GetCreature(ObjectGuid guid);        // 查询地图内生物
	DynamicObject* GetDynamicObject(ObjectGuid guid);   // 查询地图内动态对象
	GameObject*    GetGameObject(ObjectGuid guid);      // 查询地图内游戏对象
	Pet*           GetPet(ObjectGuid guid);             // 查询地图内宠物
	Player*        GetPlayer(ObjectGuid guid);          // 查询地图内玩家
	Unit*          GetUnit(ObjectGuid guid);            // 查询地图内活动对象
	WorldObject*   GetWorldObject(ObjectGuid guid);     // 查询地图内对象

	MapStoredObjectTypesContainer& GetObjectsStore() { return m_objectsStore;  }
	const PlayerList& GetPlayers() const             { return m_mapRefManager; }

	/**
	 * 网格对象管理
	 */

	template<class T> void Add(T*);                        // 添加游戏对象到网格系统
	template<class T> void Remove(T*, bool);               // 删除游戏对象到网格系统

	virtual bool Add(Player*) = 0;                         // 添加游戏对象到网格系统
	virtual void Remove(Player*, bool) = 0;                // 删除游戏对象到网格系统

	void AddToActive(WorldObject* obj);                    // 添加活动游戏对象到网格系统(非玩家)
	void RemoveFromActive(WorldObject* obj);               // 删除活动游戏对象到网格系统(非玩家)

	virtual void RemoveAllObjectsInRemoveList();           // 移除延迟删除的游戏对象

	/**
	 * 网格对象遍历
	 */

	template<class T, class CONTAINER> void Visit(const GridPair& gridPair, TypeContainerVisitor<T, CONTAINER>& visitor) { m_grids[GetGridID(gridPair.gridx, gridPair.gridy)].Visit(visitor); }
	template<class T, class CONTAINER> void Visit(GridUnit x, GridUnit y, TypeContainerVisitor<T, CONTAINER>& visitor) { m_grids[GetGridID(x, y)].Visit(visitor); }

	/**
	 * 辅助函数
	 */

	GridPair ComputeGridPair(CoordUnit x, CoordUnit y) const; // 网格坐标系计算
	bool IsGridPosValid(const GridPair& gridPair) const;      // 判断网格坐标是否有效

protected:
	/**
	 * 网格对象管理
	 */
	template<class T>
	void AddToGrid(T*, const GridPair& gridPair);      // 向网格添加游戏对象
	template<class T>
	void RemoveFromGrid(T*, const GridPair& gridPair); // 从网格中删除游戏对象

	/**
	 * 网格遍历辅助位图
	 */
	uint32 GetGridID(GridUnit x, GridUnit y) const;     // 计算网格索引
	bool IsGridMarked(GridUnit x, GridUnit y) const;    // 判断网格单元是否已经被遍历过
	void MarkGrid(GridUnit x, GridUnit y);              // 标记网格单元，表示已遍历
	void ResetMarkedGrids();                            // 清理网格单元遍历标记

public:
	const GridMapInfo& m_gminfo;
	std::vector<GridType> m_grids; // 网格地图
	boost::dynamic_bitset<> m_bits; // 网格地图遍历标记

	MapStoredObjectTypesContainer m_objectsStore;      // 保存地图上所有非玩家对象
	std::set<WorldObject*> i_objectsToRemove;          // 保存需要延迟移除的地图上的对象

	MapRefManager m_mapRefManager;                     // 保存玩家对象
	MapRefManager::iterator m_mapRefIter;              // 遍历网格中的玩家所使用的迭代器
	ActiveNonPlayers m_activeNonPlayers;               // 保存活动游戏对象(非玩家)
	ActiveNonPlayers::iterator m_activeNonPlayersIter; // 遍历网格中的活动游戏对象(非玩家)所使用的迭代器
};

#endif //MANGOS_GRIDMAP_H
