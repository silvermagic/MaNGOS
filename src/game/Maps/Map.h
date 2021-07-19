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

    // 更新地图
    virtual void Update(const uint32&);

    // 添加游戏对象到移除列表
    void AddObjectToRemoveList(WorldObject* obj);

	// 移动游戏对象
	void PlayerRelocation(Player*, CoordUnit x, CoordUnit y, Towards orientation);
	void CreatureRelocation(Creature* creature, CoordUnit x, CoordUnit y, Towards orientation);
	bool CreatureRespawnRelocation(Creature* creature);

	// 广播消息
	void MessageBroadcast(Player*, WorldPacket*, bool to_self);
	void MessageBroadcast(WorldObject*, WorldPacket*);
	void MessageDistBroadcast(Player*, WorldPacket*, int32 dist, bool to_self, bool own_team_only = false);
	void MessageDistBroadcast(WorldObject*, WorldPacket*, int32 dist);

	/**
	 * 网格对象管理
	 */

	bool Add(Player*);                       // 添加游戏对象到网格系统
	void Remove(Player*, bool);              // 删除游戏对象到网格系统

	/**
	 * 辅助函数
	 */

	uint32 GetInstID() const;                       // 获取地图实例编号
	uint32 GenerateLocalLowGuid(HighGuid guidhigh); // 为地图内创建的动态对象分配GUID，即那些被杀死又重新产生的怪物

protected:
	uint32 m_instId;

	ObjectGuidGenerator<HIGHGUID_UNIT> m_CreatureGuids;           // 生物GUID生成器
	ObjectGuidGenerator<HIGHGUID_GAMEOBJECT> m_GameObjectGuids;   // 游戏对象GUID生成器
	ObjectGuidGenerator<HIGHGUID_DYNAMICOBJECT> m_DynObjectGuids; // 动态对象GUID生成器
	ObjectGuidGenerator<HIGHGUID_PET> m_PetGuids;                 // 宠物GUID生成器
};

class MANGOS_DLL_SPEC WorldMap : public Map
{
public:
    WorldMap(uint32 id) : Map(id, 0) {}
    ~WorldMap() {}
};

#endif
