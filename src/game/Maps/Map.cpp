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
    // ������һỰ
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

    // ִ����Ҹ��²������˴��������һỰ���²�ͬ���Ự���¿��Կ����Ƕ����Կͻ������ݰ��Ĵ���
    // ���˴��ĸ��¸����Ƕ��������Ϸ�����в���������ʱ���ݴ������������ָ�����������Ч�������
    for (m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* player = m_mapRefIter->getSource();
        if (player && player->IsInWorld())
        {
            WorldObject::UpdateHelper helper(player);
            helper.Update(t_diff);
        }
    }

    // ������Ҹ��������ڵĻ���󣬲�ִ������²���(��Ҫ����NPCִ��AI����)��������Ҹ�����������Ҫ���������䱣�ֿ���״̬���ɣ�����������Ч��
    // ������ʼǰ�������������ı������(ÿ����һ��CELLǰ�����ȴ��ϱ�ǣ���ֹ�ظ����������ڵ���Ϸ����)
    ResetMarkedGrids();

    // ����Ĳ������Կ���
    // for (auto obj : vector<Type>(objs))
    //     obj.Update(t_diff);
    // ����objs����CELL�е�GridTypeMapContainer/WorldTypeMapContainer�����������Ϸ����
    // ��obj.Update(t_diff)����ObjectUpdater::Visitʵ�ֵ��߼�
    MaNGOS::ObjectUpdater updater(t_diff);
    // ���������ڵ���Ϸ�����ִ������²���
    TypeContainerVisitor<MaNGOS::ObjectUpdater, GridTypeMapContainer> grid_object_update(updater);
    // ���������ڵĳ����ִ������²���
    TypeContainerVisitor<MaNGOS::ObjectUpdater, WorldTypeMapContainer> world_object_update(updater);

    // ���Ա������������Ұ��Χ�ڵ�����
    for (m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* player = m_mapRefIter->getSource();
        // ������δ������Ϸ�����겻�Ϸ�����Ҷ���
        if (!player->IsInWorld() || !player->IsPositionValid())
		{
        	continue;
		}

        // ���������Ұ�����ǵ���������
        GridArea area = MaNGOS::ComputeGridArea(this->GetId(), player->GetPositionX(), player->GetPositionY(), SIZE_OF_SCREEN);

        // ���������ڵ�����Ԫ
        for (GridUnit x = area.gridlow.gridx; x <= area.gridhigh.gridx; ++x)
        {
            for (GridUnit y = area.gridlow.gridy; y <= area.gridhigh.gridy; ++y)
            {
                // �������Ԫ������ǣ���ֹ�����Ұ�ص���������Ԫ���ظ�����
                if (!IsGridMarked(x, y))
                {
                    // ���ϱ�����ǣ���ֹ�ظ�����
                    MarkGrid(x, y);
                    Visit(x, y, grid_object_update);
                    Visit(x, y, world_object_update);
                }
            }
        }
    }

    // ������Щ���������Ұ��Χ�ڣ������ִ��ڻ״̬����Ϸ���󣬼��������������©��֮��
    if (!m_activeNonPlayers.empty())
    {
        for (m_activeNonPlayersIter = m_activeNonPlayers.begin(); m_activeNonPlayersIter != m_activeNonPlayers.end();)
        {
            WorldObject* obj = *m_activeNonPlayersIter;

            // ��ǰ��������ָ�룬��ֹ�ڱ��������г��ֵ�ǰ����Map::Remove�����Ƴ�����������������ĩβ����ô����ӵĶ��󽫵ȴ���һ�θ���
            ++m_activeNonPlayersIter;

            // ������δ������Ϸ�����겻�Ϸ�����Ϸ����
            if (!obj->IsInWorld() || !obj->IsPositionValid())
			{
            	continue;
			}

            // ������Ϸ������Ұ�����ǵ���������
            GridArea area = MaNGOS::ComputeGridArea(this->GetId(), obj->GetPositionX(), obj->GetPositionY(), SIZE_OF_SCREEN);

            for (GridUnit x = area.gridlow.gridx; x <= area.gridhigh.gridx; ++x)
            {
                for (GridUnit y = area.gridlow.gridy; y <= area.gridhigh.gridy; ++y)
                {
                    // �������Ԫ������ǣ���ֹ��Ұ�ص���������Ԫ���ظ�����
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

    // ��������ƶ�ǰ����������Ԫ
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

    // ���µ����������
    player->Relocate(x, y, orientation);

    // �������������������Ԫ�����仯
    if (!same_grid)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Player %s relocation grid[%u,%u]->grid[%u,%u]", player->GetName().c_str(), old_grid.gridx, old_grid.gridy, new_grid.gridx, new_grid.gridy);
        // ����ҴӾɵ�����Ԫ���Ƴ�������ӵ��µ�����Ԫ
        RemoveFromGrid(player, old_grid);
        AddToGrid(player, new_grid);

        // ����������Ұ����
        player->GetViewPoint().Event_GridChanged(&m_grids[GetGridID(new_grid.gridx, new_grid.gridy)]);
    }

    // ��������ض�λ�ص�����
    player->OnRelocated();
}

void Map::CreatureRelocation(Creature* creature, CoordUnit x, CoordUnit y, Towards orientation)
{
    MANGOS_ASSERT(creature->GetMap() == this);

    // ����ƶ�ǰ���Ƿ���ͬһ����Ԫ
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

    // ���µ�����������
    creature->Relocate(x, y, orientation);

    // �����������������仯
    if (!same_grid)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u) relocation grid[%u,%u]->grid[%u,%u].", creature->GetGUIDLow(), creature->GetEntry(), old_grid.gridx, old_grid.gridy, new_grid.gridx, new_grid.gridy);
        // ������Ӿɵ��������Ƴ�������ӵ��µ�����
        RemoveFromGrid(creature, old_grid);
        AddToGrid(creature, new_grid);

        // ����������Ұ����
        creature->GetViewPoint().Event_GridChanged(&m_grids[GetGridID(new_grid.gridx, new_grid.gridy)]);
    }

    // ���������ض�λ�ص�����
    creature->OnRelocated();
}

bool Map::CreatureRespawnRelocation(Creature* creature)
{
    // todo: ����׷����Զ���Զ��ƶ��س�����
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
	// ������Ҷ�����������Ԫ����
	GridPair gridPair = ComputeGridPair(player->GetPositionX(), player->GetPositionY());
	if (!IsGridPosValid(gridPair))
	{
		sLog.outError("GridMap::Add: Object (GUID: %u TypeId: %u) have invalid coordinates X:%d Y:%d grid [%u:%u]", player->GetGUIDLow(), player->GetTypeId(), player->GetPositionX(), player->GetPositionY(), gridPair.gridx, gridPair.gridy);
		return false;
	}

	// ������Ϸ�������ڵ�ͼ
	player->GetMapRef().link(this, player);
	player->SetMap(this);

	// �����Ҷ�������
	AddToGrid(player, gridPair);
	player->AddToWorld();

	// ֪ͨ��Ҷ�����Ұ��Χ�ڵ�������Ϸ�������������Ұ����
	// player->GetViewPoint().Event_AddedToWorld(&m_grids[GetGridID(gridPair.gridx, gridPair.gridy)]);
	// UpdateObjectVisibility(player, gridPair);

	return true;
}

void Map::Remove(Player* player, bool remove)
{
	// ִ��ɾ��ǰ��������
	if (remove)
	{
		player->CleanupsBeforeDelete();
	}
	else
	{
		player->RemoveFromWorld();
	}

	// ���Map::Update���ڴ�����Ҷ�����Ҫɾ������Ҷ�����������һ�����������Ҷ�����ô��Ҫ�ֶ�������������֮����ʹ��nocheck_prev��
	// ����Ϊ�����Ļ�++m_mapRefIter���Զ�����ָ����һ����ȷ����Ҷ��󣬲���nocheck_prev���᷵��NULL�����⽫m_mapRefIterָ������
	if (m_mapRefIter == player->GetMapRef())
	{
		m_mapRefIter = m_mapRefIter->nocheck_prev();
	}
	player->GetMapRef().unlink();

	// ������Ҷ�����������Ԫ����
	GridPair gridPair = ComputeGridPair(player->GetPositionX(), player->GetPositionY());
	if (IsGridPosValid(gridPair))
	{
		DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Remove player %s from grid[%u,%u]", player->GetName().c_str(), gridPair.gridx, gridPair.gridy);

		// ����Ϸ������������Ƴ�����֪ͨ��Ұ��Χ�ڵ�������Ϸ�������������Ұ����
		RemoveFromGrid(player, gridPair);
		UpdateObjectVisibility(player, gridPair);
	}

	// ɾ������������ڵ�ͼ��Ϣ
	player->ResetMap();

	// ɾ����Ҷ���
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