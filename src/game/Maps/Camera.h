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

#ifndef MANGOSSERVER_CAMERA_H
#define MANGOSSERVER_CAMERA_H

#include "GridDefines.h"

class ViewPoint;
class WorldObject;
class UpdateData;
class WorldPacket;
class Player;

// 以魔兽世界的猎人法术 - 野兽之眼(直接控制你的宠物，并以它的视角行动)为例，
// 此处摄像机的拥有者和视角来源一开始都是玩家，法术释放后，宠物就成了ViewPoint，摄像机的视角来源就变成了宠物，可以看做一开始玩家自己扛着摄像机，
// 玩家客户端的画面就是玩家视野内看到的内容，施法后变成宠物扛着摄像机到处跑，这时候客户端的画面就变成宠物视野内看到的内容

// 在天堂里面也有类似的场景，例如幽灵之家，其实可以看做玩家将视角设置到南瓜怪身上，然后控制其比赛

// 对象接受者，接收附近游戏对象的广播数据包，更改对象可见性并将其发送到客户端
class MANGOS_DLL_SPEC Camera
{
    friend class ViewPoint;
public:

    explicit Camera(Player* pl);
    ~Camera();

    WorldObject* GetBody() { return m_source;}
    Player* GetOwner() { return &m_owner;}

    // 将相机的视角设置为游戏对象，需要注意的是游戏对象必须和相机拥有者(玩家)处在同一张地图
    void SetView(WorldObject* obj, bool update_far_sight_field = true);

    // 重置相机视角为玩家
    void ResetView(bool update_far_sight_field = true);

    template<class T>
    void UpdateVisibilityOf(T* obj, std::set<WorldObject*>& vis);
    void UpdateVisibilityOf(WorldObject* obj);

    // 更新相机拥有者(玩家)视角周围的可见游戏对象
    void UpdateVisibilityForOwner();

    // 接受附近游戏对象的广播数据包
    void ReceivePacket(WorldPacket* data);

private:
    // 当相机视角的可见状态变更时调用
    void Event_AddedToWorld();
    void Event_RemovedFromWorld();
    void Event_Moved();
    void Event_ViewPointVisibilityChanged();

    Player& m_owner;
    WorldObject* m_source;

    void UpdateForCurrentViewPoint();

public:
    GridReference<Camera>& GetGridRef() { return m_gridRef; }
    bool isActiveObject() const { return false; }
private:
    GridReference<Camera> m_gridRef;
};

// 对象观察者，将视野内的状态变更通知给连接到它的摄影机
class MANGOS_DLL_SPEC ViewPoint
{
    friend class Camera;

    typedef std::list<Camera*> CameraList;

    CameraList m_cameras;
    GridType* m_grid;

    void Attach(Camera* c) { m_cameras.push_back(c); }
    void Detach(Camera* c) { m_cameras.remove(c); }

    void CameraCall(void (Camera::*handler)())
    {
        if (!m_cameras.empty())
        {
            for (CameraList::iterator itr = m_cameras.begin(); itr != m_cameras.end();)
            {
                Camera* c = *(itr++);
                (c->*handler)();
            }
        }
    }

public:

    ViewPoint() : m_grid(0) {}
    ~ViewPoint();

    // 是否存在观众
    bool hasViewers() const { return !m_cameras.empty(); }

    // these events are called when viewpoint changes visibility state
    void Event_AddedToWorld(GridType* grid)
    {
        m_grid = grid;
        CameraCall(&Camera::Event_AddedToWorld);
    }

    void Event_RemovedFromWorld()
    {
        m_grid = NULL;
        CameraCall(&Camera::Event_RemovedFromWorld);
    }

    void Event_GridChanged(GridType* grid)
    {
        m_grid = grid;
        CameraCall(&Camera::Event_Moved);
    }

    void Event_ViewPointVisibilityChanged()
    {
        CameraCall(&Camera::Event_ViewPointVisibilityChanged);
    }

    void Call_UpdateVisibilityForOwner()
    {
        CameraCall(&Camera::UpdateVisibilityForOwner);
    }
};

#endif
