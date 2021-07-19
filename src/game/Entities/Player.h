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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Database/DatabaseEnv.h"
#include "References/MapReference.h"
#include "WorldSession.h"
#include "Unit.h"
#include "UpdateFields.h"

class PcInventory;

enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM,
    PLAYER_LOGIN_QUERY_LOADAURAS,
    PLAYER_LOGIN_QUERY_LOADSPELLS,
	PLAYER_LOGIN_QUERY_LOADQUESTS,
	PLAYER_LOGIN_QUERY_LOADTELEPORT,
	PLAYER_LOGIN_QUERY_LOADINVENTORY,

    MAX_PLAYER_LOGIN_QUERY
};

class MANGOS_DLL_SPEC Player : public Unit
{
	friend class Item;
	friend class WorldSession;
public:
    static uint16 GetGfxId(Classes classes, Gender gender);
    static Classes GetClasses(uint16 gfxId);

public:
    explicit Player(WorldSession* session);
    ~Player();
    void CleanupsBeforeDelete() override;

    void AddToWorld() override;
    void RemoveFromWorld() override;

    void Update(uint32 update_diff, uint32 p_time) override;

	void KillPlayer();                                               // 杀死玩家

	void SendDirectMessage(WorldPacket* data);                       // 发送消息

	PcInventory* GetBag();                                           // 获取玩家背包
	WorldSession* GetSession() const;                                // 获取与客户端的会话连接

	bool IsGameMaster() const;                                       // 判断玩家是否为管理员
	bool IsFriendlyTo(const Unit* unit) const;                       // 与游戏对象是否友好
	bool IsHostileTo(const Unit* unit) const;                        // 与游戏对象是否敌视

	/**
	 * 天堂特色方法
	 */
	int32 GetMaxWeight() const;                                      // 计算角色最大负重量

protected:
	/*********************************************************/
	/***                    STAT SYSTEM                    ***/
	/*********************************************************/
	// 基础属性
	int8 m_baseStat[MAX_STATS];
	int8 m_createStat[MAX_STATS];

	// 基础近战/远程伤害
	int8 m_baseAttackDamage[MAX_ATTACK];
	// 初始近战/远程伤害
	int8 m_createAttackDamage[MAX_ATTACK];

	// 基础近战/远程命中
	int8 m_baseAttackHit[MAX_ATTACK];
	// 初始近战/远程命中
	int8 m_createAttackHit[MAX_ATTACK];

	// 初始法术伤害/命中/暴击
	int8 m_createMagicHit;
	int8 m_createMagicDamage;
	int8 m_createMagicCritical;

	// 基础防御
	int8 m_baseArmor;
	// 初始防御
	int8 m_createArmor;

	// 基础魔量
	int16 m_baseMaxMana;
	// 基础血量
	int16 m_baseMaxHealth;

	// 初始血量成长
	int8 m_createHealthGrowth;
	// 初始魔量成长
	int8 m_createManaGrowth;

	// 魔量回复
	int32 m_totalManaRegen;
	int8 m_manaRegen;
	// 初始魔量回复
	int8 m_createManaRegen;

	// 血量回复
	int32 m_totalHealthRegen;
	int8 m_healthRegen;
	// 初始血量回复
	int8 m_createHealthRegen;

	// 基础魔法防御
	int16 m_baseDefense;
	// 初始魔法防御
	int16 m_createDefense;

	// 基础法术强度
	int16 m_baseSp;

	// 基础闪避率
	int16 m_baseDodge;
	// 初始闪避率
	int16 m_createDodge;

	// 初始负重减免(力量、体质)
	int16 m_createStrengthWeightReduction;
	int16 m_createStaminaWeightReduction;

	// 初始魔法消耗减免
	int8 m_createMagicCostReduction;

	// 伤害减免
	int8 m_damageReduction;

	// 负重减免
	int8 m_weightReduction;

public:
	/// 刷新角色属性状态，在数据库加载、用户创建，或使用回忆蜡烛时调用
	void Refresh();

	/// 角色经验和体力回复量计算，在数据库加载或等级发生改变时调用
	void UpdateLevel();

	/// 设置当前血量魔量正义值，仅在数据库加载时direct == true
	void SetMana(int16 value, bool direct = false);
	void SetHealth(int16 value, bool direct = false);

	/// 角色力量、敏捷、体质、智力、精神、魅力基础属性计算
	/**
	 * 获取角色初始属性值
	 *
	 * @param stat 属性类型
	 * @return 初始属性值
	 */
	int8 GetCreateStat(Stats stat);
	/**
	 * 获取角色基础属性值
	 *
	 * @param stat 属性类型
	 * @return 基础属性值
	 */
	int8 GetBaseStat(Stats stat);
	/**
	 * 更新角色基础属性值，同时调整角色属性总值，基础属性值(characters表的str等字段)由角色创建时的初始属性(characters表的OriginalStr等字段)加上使用万能药水的奖励，
	 * 以及50级以后的升级奖励构成
	 *
	 * @param stat 属性类型
	 * @param diff 属性变动值
	 */
	void ModifyBaseStat(Stats stat, int32 diff);

	/// 近战/远程的(基础/初始)伤害加成 近战/远程的(基础/初始)命中加成 魔法初始伤害加成 魔法初始命中加成 魔法初始暴击加成计算
	/**
	 * 计算等级带来的命中加成，仅当角色等级发生变化时调用
	 */
	void UpdateBaseAttackHit();
	void UpdateBaseAttackDamage();
	/**
	 * 计算初始力量和敏捷属性带来的命中加成，仅当角色重置属性点数(回忆蜡烛)或角色登入时调用
	 */
	void UpdateCreateAttackHit();
	void UpdateCreateAttackDamage(); // 仅与人物创建时的初始力量和敏捷属性相关
	void UpdateCreateMagicHit();      // 仅与人物创建时的初始智力属性相关
	void UpdateCreateMagicDamage();   // 仅与人物创建时的初始智力属性相关
	void UpdateCreateMagicCritical();     // 仅与人物创建时的初始智力属性相关

	/// (基础/初始)防御加成
	/**
	 * 计算等级带来的防御加成，仅当角色等级发生变化时调用，且仅与基础敏捷属性和等级相关，
	 * 不受装备和法术奖励的敏捷属性影响
	 */
	void UpdateBaseArmor();
	/**
	 * 计算初始敏捷属性带来的防御加成，仅当角色重置属性点数(回忆蜡烛)或角色登入时调用
	 */
	void UpdateCreateArmor();

	/// 角色基础血量上限和基础魔量上限计算
	/**
	 * 更新角色基础血量上限，同时调整角色血量上限总值，基础属性值(characters表的MaxHp等字段)由角色每次升级时奖励的random(初始血量成长)累计构成，
	 * 人物创建或重置初始化时以及角色升级/降级时会触发此函数的调用
	 *
	 * @param diff 血量上限变化值
	 */
	void ModifyBaseMaxMana(int32 diff);
	void ModifyBaseMaxHealth(int32 diff);

	/// 角色初始血量和魔量成长计算
	void UpdateCreateManaGrowth();   // 仅与人物创建时的初始精神属性相关
	void UpdateCreateHealthGrowth(); // 仅与人物创建时的初始体质属性相关

	/// 角色血量和魔量回复计算
	int8 GetManaRegen() const;
	void ModifyManaRegen(int32 diff);

	int8 GetHealthRegen() const;
	void ModifyHealthRegen(int32 diff);

	/// 角色初始血量和初始魔量回复计算
	void UpdateCreateManaRegen();   // 仅与人物创建时的初始精神属性相关
	void UpdateCreateHealthRegen(); // 仅与人物创建时的初始体质属性相关

	/// 角色(基础/初始)魔法防御和基础魔法强度计算
	int16 GetBaseDefense() const;
	int16 GetBaseSpellPower() const;
	/**
	 * 计算等级和当前精神属性值带来的防御加成，当角色等级或当前精神属性发生变化时调用，受装备和法术影响
	 */
	void UpdateBaseDefense();
	void UpdateCreateDefense();  // 仅与人物创建时的初始精神属性相关

	/// 角色(基础/初始)闪避计算
	/**
	 * 计算等级和当前敏捷属性值带来的防御加成，当角色等级或当前敏捷属性发生变化时调用，受装备和法术影响
	 */
	void UpdateBaseDodge();
	void UpdateCreateDodge(); // 仅与人物创建时的初始敏捷属性相关

	/// 角色初始负重减免、魔法消耗减免计算
	void UpdateCreateWeightReduction();    // 仅与人物创建时的初始力量和体质属性相关
	void UpdateCreateMagicCostReduction(); // 仅与人物创建时的初始智力属性相关

	/// 角色伤害减免计算
	int8 GetDamageReduction() const;
	void ModifyDamageReduction(int8 diff);

	/// 角色负重减免计算
	int8 GetWeightReduction() const;
	void ModifyWeightReduction(int8 diff);

protected:
	/*********************************************************/
	/***                    GRID SYSTEM                    ***/
	/*********************************************************/
	Camera m_camera;                             // 玩家视角(默认是自身)
	GridReference<Player> m_gridRef;             // 用于将自身挂载到网格上的双向链表节点
	MapReference m_mapRef;                       // 用于将自身挂载到地图上的双向链表节点

public:
	GuidSet m_clientGUIDs;                       // 玩家客户端界面当前可见对象列表

public:
	Camera& GetCamera();                                             // 玩家视角
	GridReference<Player>& GetGridRef();                             // 获取用于将自身挂载到网格上的双向链表节点
	MapReference& GetMapRef();                                       // 获取用于将自身挂载到地图上的双向链表节点

	void UpdateVisibilityOf(WorldObject const* viewPoint, WorldObject* target); // 通过指定视角来判断是否能看见目标游戏对象，根据判断结果更新玩家自身可见对象列表
	void RelocateToHomebind(uint32 mapId, CoordUnit x, CoordUnit y);            // 将玩家传送回回城点
	bool HaveAtClient(const WorldObject* u);                                    // 判断玩家客户端界面是否能看到游戏对象

protected:
	/*********************************************************/
	/***                     DB SYSTEM                     ***/
	/*********************************************************/
	bool LoadFromDB(SqlQueryHolder* holder);
	void SaveToDB();

	void LoadSpells(QueryResult* result);

protected:
	WorldSession *m_session; // 与客户端的会话连接
	PcInventory *m_bag;      // 玩家背包
};

#endif
