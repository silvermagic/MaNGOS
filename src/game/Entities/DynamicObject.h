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

#ifndef MANGOSSERVER_DYNAMICOBJECT_H
#define MANGOSSERVER_DYNAMICOBJECT_H

#include "SharedDefines.h"
#include "Object.h"

enum DynamicObjectType
{
    DYNAMIC_OBJECT_PORTAL           = 0x0,
    DYNAMIC_OBJECT_AREA_SPELL       = 0x1, // 范围魔法产生的动态对象，例如火墙、治愈能量风暴
    DYNAMIC_OBJECT_FARSIGHT_FOCUS   = 0x2,
    DYNAMIC_OBJECT_RAID_MARKER      = 0x3,
};

class DynamicObject : public WorldObject
{
public:
	/**
	 * 在指定世界坐标处放置一个动态对象来实现持续范围魔法的施放，例如火墙、治愈能量风暴
	 *
	 * @param guidlow  动态对象GUID
	 * @param caster   创建动态对象的施法者
	 * @param spellId  创建动态对象的魔法编号
	 * @param effIndex 创建动态对象的魔法的效果编号
	 * @param x        动态对象世界坐标
	 * @param y        动态对象世界坐标
	 * @param duration 动态对象朝向
	 * @param radius   魔法效果半径
	 * @param type     动态对象类型
	 * @return true表示创建成功
	 */
	bool Create(uint32 guidlow, Unit* caster, uint32 spellId, SpellEffectIndex effIndex, CoordUnit x, CoordUnit y, int32 duration, uint8 radius, DynamicObjectType type);

public:
    explicit DynamicObject();

    void AddToWorld() override;                         // 添加动态对象到游戏世界
    void RemoveFromWorld() override;                    // 将动态对象从游戏世界移除

    void Update(uint32 update_diff, uint32 p_time) override;
    void Delete();                                      // 删除动态对象
	void Delay(int32 delaytime);                        // todo: 理解其用法

	void AddAffected(Unit* unit);                       // 添加受到影响的游戏对象
	void RemoveAffected(Unit* unit);                    // 移除未受到影响的游戏对象

	Unit* GetCaster() const;                            // 获取创建动态对象的施法者
	const ObjectGuid& GetCasterGuid() const;            // 获取创建动态对象的施法者GUID
	uint32 GetDuration() const;                         // 获取动态对象的剩余存活时间
	SpellEffectIndex GetEffIndex() const;               // 获取创建动态对象的魔法的效果编号
	GridReference<DynamicObject>& GetGridRef();         // 获取用于将自身挂载到网格上的双向链表节点
	uint8 GetRadius() const;                            // 获取魔法效果范围半径
    uint32 GetSpellId() const;                          // 获取法术编号
    DynamicObjectType GetType() const;                  // 获取动态对象类型

    bool IsAffecting(Unit* unit) const;                 // 判断对象是否受到动态对象产生的魔法效果影响
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override; // 判断玩家是否能看到自身

protected:
	ObjectGuid m_caster;         // 创建动态对象的施法者
	GuidSet m_affected;          // 受到魔法效果影响的对象GUID
	DynamicObjectType m_type;    // 动态对象类型
	int32 m_aliveDuration;       // 魔法效果剩余时间

	uint32 m_spellId;            // 创建动态对象的魔法编号
	SpellEffectIndex m_effIndex; // 创建动态对象的魔法的效果编号
	uint8 m_radius;              // 魔法效果范围半径
	bool m_positive;             // 魔法效果是否是增益的

private:
    GridReference<DynamicObject> m_gridRef; // 用于将自身挂载到网格上的双向链表节点
};
#endif
