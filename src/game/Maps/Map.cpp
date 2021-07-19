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

#include "Entities/Corpse.h"
#include "Entities/Creature.h"
#include "Entities/DynamicObject.h"
#include "Entities/GameObject.h"
#include "Entities/Pet.h"
#include "Entities/Player.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "CellImpl.h"
#include "GridMapImpl.h"
#include "GridNotifiersImpl.h"

void Map::DeleteFromWorld(Player* player)
{
	sObjectAccessor.RemoveObject(player);
    delete player;
}

Map::Map(uint32 id, uint32 InstanceId)
    : GridMap(id), TileMap(id), m_instId(InstanceId)
{
	m_CreatureGuids.Set(sObjectMgr.GetFirstTemporaryCreatureLowGuid());
	m_GameObjectGuids.Set(sObjectMgr.GetFirstTemporaryGameObjectLowGuid());
}

Map::~Map()
{
}

void Map::Update(const uint32& t_diff)
{
    // 更新玩家会话
    for (m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* player = m_mapRefIter->getSource();
        if (player && player->IsInWorld())
        {
            WorldSession* pSession = player->GetSession();
            MapSessionFilter updater(pSession);
            pSession->Update(updater);
        }
    }

    // 执行玩家更新操作，此处上面的玩家会话更新不同，会话更新可以看做是对来自客户端数据包的处理，
    // 而此处的更新更多是对玩家在游戏世界中产生的运行时数据处理，例如体力恢复、法术持续效果处理等
    for (m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* player = m_mapRefIter->getSource();
        if (player && player->IsInWorld())
        {
            WorldObject::UpdateHelper helper(player);
            helper.Update(t_diff);
        }
    }

    // 遍历玩家附件网格内的活动对象，并执行其更新操作(主要是让NPC执行AI操作)，不在玩家附件的网格不需要遍历，让其保持空闲状态即可，可提升处理效率
    // 遍历开始前先清除所有网格的遍历标记(每遍历一个CELL前，都先打上标记，防止重复遍历网格内的游戏对象)
    ResetMarkedGrids();

    // 下面的操作可以看做
    // for (auto obj : vector<Type>(objs))
    //     obj.Update(t_diff);
    // 其中objs就是CELL中的GridTypeMapContainer/WorldTypeMapContainer容器保存的游戏对象
    // 而obj.Update(t_diff)就是ObjectUpdater::Visit实现的逻辑
    MaNGOS::ObjectUpdater updater(t_diff);
    // 遍历网格内的游戏生物，并执行其更新操作
    TypeContainerVisitor<MaNGOS::ObjectUpdater, GridTypeMapContainer> grid_object_update(updater);
    // 遍历网格内的宠物，并执行其更新操作
    TypeContainerVisitor<MaNGOS::ObjectUpdater, WorldTypeMapContainer> world_object_update(updater);

    // 尝试遍历所有玩家视野范围内的网格
    for (m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* player = m_mapRefIter->getSource();
        // 跳过尚未进入游戏或坐标不合法的玩家对象
        if (!player->IsInWorld() || !player->IsPositionValid())
		{
        	continue;
		}

        // 计算玩家视野所覆盖的网格区域
        GridArea area = MaNGOS::ComputeGridArea(this->GetId(), player->GetPositionX(), player->GetPositionY(), SIZE_OF_SCREEN);

        // 遍历区域内的网格单元
        for (GridUnit x = area.gridlow.gridx; x <= area.gridhigh.gridx; ++x)
        {
            for (GridUnit y = area.gridlow.gridy; y <= area.gridhigh.gridy; ++y)
            {
                // 检查网格单元遍历标记，防止玩家视野重叠导致网格单元被重复遍历
                if (!IsGridMarked(x, y))
                {
                    // 打上遍历标记，防止重复遍历
                    MarkGrid(x, y);
                    Visit(x, y, grid_object_update);
                    Visit(x, y, world_object_update);
                }
            }
        }
    }

    // 处理那些不在玩家视野范围内，但是又处于活动状态的游戏对象，即处理上面遍历的漏网之鱼
    if (!m_activeNonPlayers.empty())
    {
        for (m_activeNonPlayersIter = m_activeNonPlayers.begin(); m_activeNonPlayersIter != m_activeNonPlayers.end();)
        {
            WorldObject* obj = *m_activeNonPlayersIter;

            // 提前调整遍历指针，防止在遍历过程中出现当前对象被Map::Remove调用移除的情况，如果遍历到末尾，那么新添加的对象将等待下一次更新
            ++m_activeNonPlayersIter;

            // 跳过尚未进入游戏或坐标不合法的游戏对象
            if (!obj->IsInWorld() || !obj->IsPositionValid())
			{
            	continue;
			}

            // 计算游戏对象视野所覆盖的网格区域
            GridArea area = MaNGOS::ComputeGridArea(this->GetId(), obj->GetPositionX(), obj->GetPositionY(), SIZE_OF_SCREEN);

            for (GridUnit x = area.gridlow.gridx; x <= area.gridhigh.gridx; ++x)
            {
                for (GridUnit y = area.gridlow.gridy; y <= area.gridhigh.gridy; ++y)
                {
                    // 检查网格单元遍历标记，防止视野重叠导致网格单元被重复遍历
                    if (!IsGridMarked(x, y))
                    {
                        MarkGrid(x, y);
                        Visit(x, y, grid_object_update);
                        Visit(x, y, world_object_update);
                    }
                }
            }
        }
    }
}

void Map::AddObjectToRemoveList(WorldObject* obj)
{
    MANGOS_ASSERT(obj->GetMapId() == GetId() && obj->GetMap()->GetInstID() == GetInstID());

    obj->CleanupsBeforeDelete();

    i_objectsToRemove.insert(obj);
}

void Map::PlayerRelocation(Player* player, CoordUnit x, CoordUnit y, Towards orientation)
{
    MANGOS_ASSERT(player);

    // 计算玩家移动前后所处网格单元
    GridPair old_grid = ComputeGridPair(player->GetPositionX(), player->GetPositionY());
    if (!IsGridPosValid(old_grid)) {
        sLog.outError("Map::PlayerRelocation: Player %s have invalid coordinates X:%d Y:%d grid [%u:%u]", player->GetName().c_str(), player->GetPositionX(), player->GetPositionY(), old_grid.gridx, old_grid.gridy);
        return;
    }
    GridPair new_grid = ComputeGridPair(x, y);
    if (!IsGridPosValid(new_grid)) {
        sLog.outError("Map::PlayerRelocation: Player %s have invalid coordinates X:%d Y:%d grid [%u:%u]", player->GetName().c_str(), player->GetPositionX(), player->GetPositionY(), new_grid.gridx, new_grid.gridy);
        return;
    }
    bool same_grid = (old_grid == new_grid);

    // 重新调整玩家坐标
    player->Relocate(x, y, orientation);

    // 如果玩家所处网格或网格单元发生变化
    if (!same_grid)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Player %s relocation grid[%u,%u]->grid[%u,%u]", player->GetName().c_str(), old_grid.gridx, old_grid.gridy, new_grid.gridx, new_grid.gridy);
        // 将玩家从旧的网格单元中移除，并添加到新的网格单元
        RemoveFromGrid(player, old_grid);
        AddToGrid(player, new_grid);

        // 更新自身视野内容
        player->GetViewPoint().Event_GridChanged(&m_grids[GetGridID(new_grid.gridx, new_grid.gridy)]);
    }

    // 玩家坐标重定位回调处理
    player->OnRelocated();
}

void Map::CreatureRelocation(Creature* creature, CoordUnit x, CoordUnit y, Towards orientation)
{
    MANGOS_ASSERT(creature->GetMap() == this);

    // 检测移动前后是否处于同一网格单元
    GridPair old_grid = creature->GetCurrentGrid();
    if (!IsGridPosValid(old_grid)) {
        sLog.outError("Map::CreatureRelocation: Creature (GUID: %u Entry: %u) have invalid coordinates X:%d Y:%d grid [%u:%u]", creature->GetGUIDLow(), creature->GetEntry(), creature->GetPositionX(), creature->GetPositionY(), old_grid.gridx, old_grid.gridy);
        return;
    }
    GridPair new_grid = ComputeGridPair(x, y);
    if (!IsGridPosValid(old_grid)) {
        sLog.outError("Map::CreatureRelocation: Creature (GUID: %u Entry: %u) have invalid coordinates X:%d Y:%d grid [%u:%u]", creature->GetGUIDLow(), creature->GetEntry(), creature->GetPositionX(), creature->GetPositionY(), old_grid.gridx, old_grid.gridy);
        return;
    }
    bool same_grid = (old_grid == new_grid);

    // 重新调整生物坐标
    creature->Relocate(x, y, orientation);

    // 如果玩家所处网格发生变化
    if (!same_grid)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u) relocation grid[%u,%u]->grid[%u,%u].", creature->GetGUIDLow(), creature->GetEntry(), old_grid.gridx, old_grid.gridy, new_grid.gridx, new_grid.gridy);
        // 将生物从旧的网格中移除，并添加到新的网格
        RemoveFromGrid(creature, old_grid);
        AddToGrid(creature, new_grid);

        // 更新自身视野内容
        creature->GetViewPoint().Event_GridChanged(&m_grids[GetGridID(new_grid.gridx, new_grid.gridy)]);
    }

    // 生物坐标重定位回调处理
    creature->OnRelocated();
}

bool Map::CreatureRespawnRelocation(Creature* creature)
{
    // todo: 怪物追击过远后自动移动回出生点
    return false;
}

void Map::MessageBroadcast(Player* player, WorldPacket* msg, bool to_self)
{
    MaNGOS::MessageDeliverer post_man(*player, msg, to_self);
    TypeContainerVisitor<MaNGOS::MessageDeliverer, WorldTypeMapContainer > message(post_man);
	MaNGOS::Visit(message, this, player, SIZE_OF_SCREEN);
}

void Map::MessageBroadcast(WorldObject* obj, WorldPacket* msg)
{
    MaNGOS::ObjectMessageDeliverer post_man(*obj, msg);
    TypeContainerVisitor<MaNGOS::ObjectMessageDeliverer, WorldTypeMapContainer> message(post_man);
	MaNGOS::Visit(message, this, obj, SIZE_OF_SCREEN);
}

void Map::MessageDistBroadcast(Player* player, WorldPacket* msg, int32 dist, bool to_self, bool own_team_only)
{
    MaNGOS::MessageDistDeliverer post_man(*player, msg, SIZE_OF_SCREEN, to_self, own_team_only);
    TypeContainerVisitor<MaNGOS::MessageDistDeliverer , WorldTypeMapContainer> message(post_man);
	MaNGOS::Visit(message, this, player, dist);
}

void Map::MessageDistBroadcast(WorldObject* obj, WorldPacket* msg, int32 dist)
{
    MaNGOS::ObjectMessageDistDeliverer post_man(*obj, msg, SIZE_OF_SCREEN);
    TypeContainerVisitor<MaNGOS::ObjectMessageDistDeliverer, WorldTypeMapContainer > message(post_man);
	MaNGOS::Visit(message, this, obj, dist);
}

bool Map::Add(Player* player)
{
	// 计算玩家对象所处网格单元坐标
	GridPair gridPair = ComputeGridPair(player->GetPositionX(), player->GetPositionY());
	if (!IsGridPosValid(gridPair))
	{
		sLog.outError("GridMap::Add: Object (GUID: %u TypeId: %u) have invalid coordinates X:%d Y:%d grid [%u:%u]", player->GetGUIDLow(), player->GetTypeId(), player->GetPositionX(), player->GetPositionY(), gridPair.gridx, gridPair.gridy);
		return false;
	}

	// 设置游戏对象所在地图
	player->GetMapRef().link(this, player);
	player->SetMap(this);

	// 添加玩家对象到网格
	AddToGrid(player, gridPair);
	player->AddToWorld();

	// 通知玩家对象视野范围内的其他游戏对象更新自身视野内容
	// player->GetViewPoint().Event_AddedToWorld(&m_grids[GetGridID(gridPair.gridx, gridPair.gridy)]);
	// UpdateObjectVisibility(player, gridPair);

	return true;
}

void Map::Remove(Player* player, bool remove)
{
	// 执行删除前的清理动作
	if (remove)
	{
		player->CleanupsBeforeDelete();
	}
	else
	{
		player->RemoveFromWorld();
	}

	// 如果Map::Update正在处理玩家对象，且要删除的玩家对象正好是下一个待处理的玩家对象，那么需要手动调整迭代器，之所以使用nocheck_prev，
	// 是因为这样的话++m_mapRefIter能自动修正指向下一个正确的玩家对象，并且nocheck_prev不会返回NULL，避免将m_mapRefIter指针悬空
	if (m_mapRefIter == player->GetMapRef())
	{
		m_mapRefIter = m_mapRefIter->nocheck_prev();
	}
	player->GetMapRef().unlink();

	// 计算玩家对象所处网格单元坐标
	GridPair gridPair = ComputeGridPair(player->GetPositionX(), player->GetPositionY());
	if (IsGridPosValid(gridPair))
	{
		DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Remove player %s from grid[%u,%u]", player->GetName().c_str(), gridPair.gridx, gridPair.gridy);

		// 将游戏对象从网格中移除，并通知视野范围内的其他游戏对象更新自身视野内容
		RemoveFromGrid(player, gridPair);
		UpdateObjectVisibility(player, gridPair);
	}

	// 删除对象对象所在地图信息
	player->ResetMap();

	// 删除玩家对象
	if (remove)
	{
		DeleteFromWorld(player);
	}
}

uint32 Map::GetInstID() const
{
	return m_instId;
}

uint32 Map::GenerateLocalLowGuid(HighGuid guidhigh)
{
	switch (guidhigh)
	{
		case HIGHGUID_UNIT:
			return m_CreatureGuids.Generate();
		case HIGHGUID_GAMEOBJECT:
			return m_GameObjectGuids.Generate();
		case HIGHGUID_DYNAMICOBJECT:
			return m_DynObjectGuids.Generate();
		case HIGHGUID_PET:
			return m_PetGuids.Generate();
		default:
			MANGOS_ASSERT(false);
			return 0;
	}
}