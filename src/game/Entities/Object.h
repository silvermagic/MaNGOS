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

#ifndef _OBJECT_H
#define _OBJECT_H

#include <set>
#include <string>

#include "Common.h"
#include "Util.h"
#include "Utilities/ByteBuffer.h"
#include "Utilities/Timer.h"
#include "Maps/Camera.h"
#include "UpdateFields.h"
#include "UpdateData.h"
#include "ObjectGuid.h"

#define CONTACT_DISTANCE            1
#define ATTACK_DISTANCE             5
#define DEFAULT_VISIBILITY_DISTANCE 10     // default visible distance
#define DEFAULT_WORLD_OBJECT_SIZE   0      // currently used for bounding radius

enum TempSummonType
{
    TEMPSUMMON_TIMED_OR_DEAD_DESPAWN       = 1,             // despawns after a specified time OR when the creature disappears
    TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN     = 2,             // despawns after a specified time OR when the creature dies
    TEMPSUMMON_TIMED_DESPAWN               = 3,             // despawns after a specified time
    TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT = 4,             // despawns after a specified time after the creature is out of combat
    TEMPSUMMON_CORPSE_DESPAWN              = 5,             // despawns instantly after death
    TEMPSUMMON_CORPSE_TIMED_DESPAWN        = 6,             // despawns after a specified time after death
    TEMPSUMMON_DEAD_DESPAWN                = 7,             // despawns when the creature disappears
    TEMPSUMMON_MANUAL_DESPAWN              = 8              // despawns when UnSummon() is called
};

enum PhaseMasks
{
    PHASEMASK_NORMAL   = 0x00000001,
    PHASEMASK_ANYWHERE = 0xFFFFFFFF
};

enum Towards : uint8
{
    SOUTH      = 0,             // 正北方向 正前方
    SOUTH_EAST = 1,             // 东北方向 右前方
    EAST       = 2,             // 正东方向 右
    NORTH_EAST = 3,             // 东南方向 右后方
    NORTH      = 4,             // 正南方向 后方
    NORTH_WEST = 5,             // 西南方向 左后方
    WEST       = 6,             // 正西方向 左
    SOUTH_WEST = 7              // 西北方向 左前方
};

#define MAX_TOWARDS 8

class WorldPacket;
class WorldSession;
class Creature;
class Player;
class Unit;
class Map;
class TerrainInfo;
class UpdateData;
class UpdateMask;

typedef UNORDERED_MAP<Player*, UpdateData> UpdateDataMapType;

struct Position
{
    Position() : x(0), y(0), o(SOUTH) {}
    Position(CoordUnit _x, CoordUnit _y, uint8 _o) : x(_x), y(_y), o(Towards(_o)) {}
    CoordUnit x, y;
    Towards o;
};

struct WorldLocation
{
    uint32 mapid;
    CoordUnit coord_x;
    CoordUnit coord_y;
    Towards orientation;
    explicit WorldLocation(uint32 _mapid = 0, CoordUnit _x = 0, CoordUnit _y = 0, uint8 _o = 0)
        : mapid(_mapid), coord_x(_x), coord_y(_y), orientation(Towards(_o)) {}
    WorldLocation(WorldLocation const& loc)
        : mapid(loc.mapid), coord_x(loc.coord_x), coord_y(loc.coord_y), orientation(loc.orientation) {}
};

// 用来测量两次世界更新的间隔，这是网格激活后更新法术的必要条件
class WorldUpdateCounter
{
public:
    WorldUpdateCounter() : m_tmStart(0) {}

    time_t timeElapsed()
    {
        if (!m_tmStart)
            m_tmStart = WorldTimer::tickPrevTime();

        return WorldTimer::getMSTimeDiff(m_tmStart, WorldTimer::tickTime());
    }

    void Reset() { m_tmStart = WorldTimer::tickTime(); }

private:
    uint32 m_tmStart;
};

class MANGOS_DLL_SPEC Object
{
public:
    virtual ~Object();

    bool IsInWorld() const { return m_inWorld; }
    virtual void AddToWorld()
    {
        if (m_inWorld)
		{
        	return;
		}

        m_inWorld = true;
        ClearUpdateMask(false);
    }
    virtual void RemoveFromWorld()
    {
        ClearUpdateMask(true);
        m_inWorld = false;
    }

    // 播放游戏对象死亡动画
    virtual void DestroyForPlayer(Player* /* target */, bool /* anim = false */) const {}

    // 是否存在任务
    virtual bool HasQuest(uint32 /* quest_id */) const { return false; }

    // 是否参与了任务
    virtual bool HasInvolvedQuest(uint32 /* quest_id */) const { return false; }

    /*********************************************************/
    /***                  GENERAL FEATURES                 ***/
    /*********************************************************/

    ObjectGuid const& GetObjectGuid() const { return GetGuidValue(OBJECT_FIELD_GUID); }
    uint32 GetGUIDLow() const { return GetObjectGuid().GetCounter(); }
    std::string GetGuidStr() const { return GetObjectGuid().GetString(); }

    uint32 GetEntry() const { return GetUInt32Value(OBJECT_FIELD_ENTRY); }
    void SetEntry(uint32 entry) { SetUInt32Value(OBJECT_FIELD_ENTRY, entry); }

    uint8 GetTypeId() const { return m_objectTypeId; }
    bool isType(TypeMask mask) const { return (mask & m_objectType); }

    /*********************************************************/
    /***                 GAME GRID FRAMEWORK               ***/
    /*********************************************************/

    // 将对象添加到客户端更新列表，表示需要将自身的属性变更信息(外观、血量等)同步到客户端
    virtual void AddToClientUpdateList();

    // 将对象从客户端更新列表中移除
    virtual void RemoveFromClientUpdateList();

    // 标记对象发生了属性变化
    void MarkForClientUpdate();

    // 清除对象更新标记
    void ClearUpdateMask(bool remove);

    // 游戏对象首次出现在玩家视野时，将初始化信息写入UpdateData，最后使用UpdateData构造数据包并发送给玩家
    virtual void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const;

    // 游戏对象属性发生变更需要通知玩家时，将变更信息写入UpdateData
    void BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const;

    // 当游戏对象离开某个玩家视野时，将初始化信息写入UpdateData
    void BuildOutOfRangeUpdateBlock(UpdateData* data) const;

    // 直接将自身属性变更信息发送给周围玩家，并清除对象更新标记
    void SendForcedObjectUpdate();

    // 游戏对象首次出现在玩家视野时调用
    void SendCreateUpdateToPlayer(Player* player);

    /*********************************************************/
    /***              ATTRIBUTE STORAGE SYSTEM             ***/
    /*********************************************************/

    const int32& GetInt32Value(uint16 index) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return m_int32Values[ index ];
    }

    const uint32& GetUInt32Value(uint16 index) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return m_uint32Values[ index ];
    }

    const uint64& GetUInt64Value(uint16 index) const
    {
        MANGOS_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index , false));
        return *((uint64*) & (m_uint32Values[ index ]));
    }

    const float& GetFloatValue(uint16 index) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return m_floatValues[ index ];
    }

    uint8 GetByteValue(uint16 index, uint8 offset) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        MANGOS_ASSERT(offset < 4);
        return *(((uint8*)&m_uint32Values[ index ]) + offset);
    }

    uint16 GetUInt16Value(uint16 index, uint8 offset) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        MANGOS_ASSERT(offset < 2);
        return *(((uint16*)&m_uint32Values[ index ]) + offset);
    }

    ObjectGuid const& GetGuidValue(uint16 index) const { return *reinterpret_cast<ObjectGuid const*>(&GetUInt64Value(index)); }

    void SetInt32Value(uint16 index,        int32  value);
    void SetUInt32Value(uint16 index,       uint32  value);
    void SetUInt64Value(uint16 index, const uint64& value);
    void SetFloatValue(uint16 index,       float   value);
    void SetByteValue(uint16 index, uint8 offset, uint8 value);
    void SetUInt16Value(uint16 index, uint8 offset, uint16 value);
    void SetInt16Value(uint16 index, uint8 offset, int16 value) { SetUInt16Value(index, offset, (uint16)value); }
    void SetGuidValue(uint16 index, ObjectGuid const& value) { SetUInt64Value(index, value.GetRawValue()); }
    void SetStatFloatValue(uint16 index, float value);
    void SetStatInt32Value(uint16 index, int32 value);

    void ApplyModUInt32Value(uint16 index, int32 val, bool apply);
    void ApplyModInt32Value(uint16 index, int32 val, bool apply);
    void ApplyModUInt64Value(uint16 index, int32 val, bool apply);
    void ApplyModPositiveFloatValue(uint16 index, float val, bool apply);
    void ApplyModSignedFloatValue(uint16 index, float val, bool apply);

    void ApplyPercentModFloatValue(uint16 index, float val, bool apply)
    {
        val = val != -100.0f ? val : -99.9f ;
        SetFloatValue(index, GetFloatValue(index) * (apply ? (100.0f + val) / 100.0f : 100.0f / (100.0f + val)));
    }

    void SetFlag(uint16 index, uint32 newFlag);
    void RemoveFlag(uint16 index, uint32 oldFlag);

    void ToggleFlag(uint16 index, uint32 flag)
    {
        if (HasFlag(index, flag))
            RemoveFlag(index, flag);
        else
            SetFlag(index, flag);
    }

    bool HasFlag(uint16 index, uint32 flag) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return (m_uint32Values[ index ] & flag) != 0;
    }

    void ApplyModFlag(uint16 index, uint32 flag, bool apply)
    {
        if (apply)
            SetFlag(index, flag);
        else
            RemoveFlag(index, flag);
    }

    void SetByteFlag(uint16 index, uint8 offset, uint8 newFlag);
    void RemoveByteFlag(uint16 index, uint8 offset, uint8 newFlag);

    void ToggleByteFlag(uint16 index, uint8 offset, uint8 flag)
    {
        if (HasByteFlag(index, offset, flag))
            RemoveByteFlag(index, offset, flag);
        else
            SetByteFlag(index, offset, flag);
    }

    bool HasByteFlag(uint16 index, uint8 offset, uint8 flag) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        MANGOS_ASSERT(offset < 4);
        return (((uint8*)&m_uint32Values[index])[offset] & flag) != 0;
    }

    void ApplyModByteFlag(uint16 index, uint8 offset, uint32 flag, bool apply)
    {
        if (apply)
            SetByteFlag(index, offset, flag);
        else
            RemoveByteFlag(index, offset, flag);
    }

    void SetShortFlag(uint16 index, bool highpart, uint16 newFlag);
    void RemoveShortFlag(uint16 index, bool highpart, uint16 oldFlag);

    void ToggleShortFlag(uint16 index, bool highpart, uint8 flag)
    {
        if (HasShortFlag(index, highpart, flag))
            RemoveShortFlag(index, highpart, flag);
        else
            SetShortFlag(index, highpart, flag);
    }

    bool HasShortFlag(uint16 index, bool highpart, uint8 flag) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return (((uint16*)&m_uint32Values[index])[highpart ? 1 : 0] & flag) != 0;
    }

    void ApplyModShortFlag(uint16 index, bool highpart, uint32 flag, bool apply)
    {
        if (apply)
            SetShortFlag(index, highpart, flag);
        else
            RemoveShortFlag(index, highpart, flag);
    }

    void SetFlag64(uint16 index, uint64 newFlag)
    {
        uint64 oldval = GetUInt64Value(index);
        uint64 newval = oldval | newFlag;
        SetUInt64Value(index, newval);
    }

    void RemoveFlag64(uint16 index, uint64 oldFlag)
    {
        uint64 oldval = GetUInt64Value(index);
        uint64 newval = oldval & ~oldFlag;
        SetUInt64Value(index, newval);
    }

    void ToggleFlag64(uint16 index, uint64 flag)
    {
        if (HasFlag64(index, flag))
            RemoveFlag64(index, flag);
        else
            SetFlag64(index, flag);
    }

    bool HasFlag64(uint16 index, uint64 flag) const
    {
        MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index , false));
        return (GetUInt64Value(index) & flag) != 0;
    }

    void ApplyModFlag64(uint16 index, uint64 flag, bool apply)
    {
        if (apply)
            SetFlag64(index, flag);
        else
            RemoveFlag64(index, flag);
    }

    bool LoadValues(const char* data);

    uint16 GetValuesCount() const { return m_valuesCount; }

	Player* ToPlayer() { if (GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Player*>(this); else return nullptr; }
	Player const* ToPlayer() const { if (GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Player const*>(this); else return nullptr; }
	Creature* ToCreature() { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature*>(this); else return nullptr; }
	Creature const* ToCreature() const { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature const*>(this); else return nullptr; }
	Unit* ToUnit() { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit*>(this); else return nullptr; }
	Unit const* ToUnit() const { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit const*>(this); else return nullptr; }
	GameObject* ToGameObject() { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject*>(this); else return nullptr; }
	GameObject const* ToGameObject() const { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject const*>(this); else return nullptr; }
	Corpse* ToCorpse() { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse*>(this); else return nullptr; }
	Corpse const* ToCorpse() const { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse const*>(this); else return nullptr; }
	DynamicObject* ToDynObject() { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject*>(this); else return nullptr; }
	DynamicObject const* ToDynObject() const { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject const*>(this); else return nullptr; }

protected:
    Object();

    void _InitValues();
    void _Create(uint32 guidlow, uint32 entry, HighGuid guidhigh);

    uint16 m_objectType;

    uint8 m_objectTypeId;
    uint16 m_updateFlag;

    union
    {
        int32*  m_int32Values;
        uint32* m_uint32Values;
        float*  m_floatValues;
    };

    uint32* m_uint32Values_mirror;

    uint16 m_valuesCount;

    bool m_objectUpdated;

private:
    bool m_inWorld;

    Object(const Object&);                              // prevent generation copy constructor
    Object& operator=(Object const&);                   // prevent generation assigment operator

public:
    bool PrintIndexError(uint32 index, bool set) const;
    bool PrintEntryError(char const* descr) const;
};

class MANGOS_DLL_SPEC WorldObject : public Object
{
public:
    class MANGOS_DLL_SPEC UpdateHelper
    {
    public:
        explicit UpdateHelper(WorldObject* obj) : m_obj(obj) {}
        ~UpdateHelper() { }

        void Update(uint32 time_diff)
        {
            m_obj->Update(m_obj->m_updateTracker.timeElapsed(), time_diff);
            m_obj->m_updateTracker.Reset();
        }

    private:
        UpdateHelper(const UpdateHelper&);
        UpdateHelper& operator=(const UpdateHelper&);

        WorldObject* const m_obj;
    };

protected:
    explicit WorldObject();

public:
    virtual ~WorldObject() {}

    virtual void Update(uint32 /*update_diff*/, uint32 /*time_diff*/) {}

    // 游戏对象离开当前地图时，将对象添加到地图的待移除列表
    void AddObjectToRemoveList();

    // 更新视野内容
    void UpdateObjectVisibility();

    // 更新自身和观众(将视角锁定在当前游戏对象身上的玩家)的视野内容
    virtual void UpdateVisibilityAndView();

    // 在析构函数中或者在大批量删除生物删时使用，用来移除对已经删除对象的交叉引用
    virtual void CleanupsBeforeDelete();

    virtual void SaveRespawnTime() {}

    void _Create(uint32 guidlow, HighGuid guidhigh, uint32 phaseMask);
    Creature* SummonCreature(uint32 id, CoordUnit x, CoordUnit y, Towards ang, TempSummonType spwtype, uint32 despwtime, bool asActiveObject = false);

    bool PrintCoordinatesError(CoordUnit x, CoordUnit y, char const* descr) const;

    /*********************************************************/
    /***               WORLDOBJECT FEATURES                ***/
    /*********************************************************/

    // 获取/设置游戏对象名称
    std::string GetName() const { return m_name; }
    void SetName(const std::string& newname) { m_name = newname; }

    // 获取对象的碰撞检测半径
    virtual uint8 GetObjectBoundingRadius() const { return DEFAULT_WORLD_OBJECT_SIZE; }

    // 地图编号
    uint32 GetMapId() const;
    // 地图实例编号
    uint32 GetInstID() const;

    // 用来标记游戏对象所处任务阶段，魔兽世界团队任务中，如果玩家新加入团队，是看不到团队正在做的任务内容的(团队消息广播的时候，也会对阶段进行校验)，
    // 因为阶段不同，这个功能在天堂中暂时没有使用，因为天堂不存在团队任务
    virtual void SetPhaseMask(uint32 newPhaseMask, bool update);
    uint32 GetPhaseMask() const { return m_phaseMask; }
    bool InSamePhase(WorldObject const* obj) const { return InSamePhase(obj->GetPhaseMask()); }
    bool InSamePhase(uint32 phasemask) const { return (GetPhaseMask() & phasemask); }

    // 设置游戏对象所处地图
    void SetMap(Map* map);
    Map* GetMap() const { MANGOS_ASSERT(m_currMap); return m_currMap; }
    void ResetMap() { m_currMap = nullptr; }

    // 是否为活动对象(在客户端画面里动来动去的)
    bool isActiveObject() const { return m_isActiveObject; }
    // 标记为活动对象
    void SetActiveObjectState(bool active);

    // 是否被玩家控制
    bool IsControlledByPlayer() const { return false; }

    // 获取游戏对象视角
    ViewPoint& GetViewPoint() { return m_viewPoint; }

    // 以玩家的视角是否能看到当前游戏对象
    bool IsVisibleFor(const Player* player, const WorldObject* viewPoint) const { return IsVisibleForInState(player, viewPoint, false); }
    virtual bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const = 0;

    /*********************************************************/
    /***                 COORDINATE SYSTEM                 ***/
    /*********************************************************/

    // 游戏世界坐标信息查询
    CoordUnit GetPositionX() const { return m_position.x; }
    CoordUnit GetPositionY() const { return m_position.y; }
    void GetPosition(CoordUnit& x, CoordUnit& y) const { x = m_position.x; y = m_position.y; }
    void GetPosition(WorldLocation& loc) const { loc.mapid = loc.mapid; GetPosition(loc.coord_x, loc.coord_y); loc.orientation = GetOrientation(); }
    Towards GetOrientation() const { return m_position.o; }
    void SetOrientation(Towards orientation);
    bool IsPositionValid() const;

    // 移动游戏对象
    void Relocate(CoordUnit x, CoordUnit y, Towards orientation);
    void Relocate(CoordUnit x, CoordUnit y);

    // 获取游戏对象朝指定方向移动指定距离后的游戏世界坐标
    void GetNearPoint(CoordUnit& x, CoordUnit& y, Towards angle, int32 distance2d = CONTACT_DISTANCE) const;

    // 计算到目标世界坐标的直线距离
    float GetLineDistance(const WorldObject* target) const;
    float GetLineDistance(CoordUnit x, CoordUnit y) const;
    // 计算到目标世界坐标的坐标投影距离，即将与目标的连线分别投影到X轴和Y轴，取两者最大值
    int32 GetTileLineDistance(const WorldObject* target) const;
    int32 GetTileLineDistance(CoordUnit x, CoordUnit y) const;
    // 计算到目标世界坐标的瓦片距离，即沿着瓦片地图一格一格走需要多少格子，简单的计算方法就是将目标的连线分别投影到X轴和Y轴，并将两者相加
    int32 GetTileDistance(const WorldObject* target) const;
    int32 GetTileDistance(CoordUnit x, CoordUnit y) const;

    // 判断与目标的距离是否在指定范围内(用于向周围指定范围内玩家发送消息时使用)
    bool IsWithinDist(WorldObject const* target, int32 dist2compare);
    bool IsWithinDist(CoordUnit x, CoordUnit y, int32 dist2compare);

    // 判断目标是否位于当前游戏对象的屏幕内
    bool IsInScreen(const WorldObject* target) const;
    bool IsInScreen(CoordUnit x, CoordUnit y) const;

    // 判断目标是否在当前游戏对象视线内
	bool IsInMap(const WorldObject* obj) const;

	// 判断与目标之间有没有视野遮挡(例如在地监里面有没围墙)
	bool IsWithinLOS(CoordUnit ox, CoordUnit oy) const;
	bool IsWithinLOSInMap(const WorldObject* obj) const;

    /*********************************************************/
    /***                   MESSAGE SYSTEM                  ***/
    /*********************************************************/

    // 向游戏对象周围的玩家发送广播消息
    virtual void SendMessageToSet(WorldPacket* data, bool self);
    // 向游戏对象周围指定范围内的玩家发送广播消息
    virtual void SendMessageToSetInRange(WorldPacket* data, uint8 dist, bool self);
    // 向游戏对象周围的玩家发送广播消息
    void SendMessageToSetExcept(WorldPacket* data, const Player* skipped_receiver);

protected:
    std::string m_name;                 // 世界对象名称

private:
    Map* m_currMap;                     // 当前对象所在地图
    Position m_position;                // 当前对象坐标
    uint32 m_phaseMask;                 // 游戏对象任务所处阶段
    ViewPoint m_viewPoint;
    WorldUpdateCounter m_updateTracker; // 世界更新追踪器
    bool m_isActiveObject;              // 标记是否为活动对象
};

#endif
