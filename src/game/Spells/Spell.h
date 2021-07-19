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

#ifndef MANGOS_SPELL_H
#define MANGOS_SPELL_H

#include "Entities/ObjectGuid.h"
#include "Entities/Unit.h"
#include "Entities/Player.h"
#include "Common.h"
#include "SharedDefines.h"
#include "SQLStructure.h"

class WorldSession;
class WorldPacket;
class Item;
class GameObject;
class Aura;

/**
 * 法术准备阶段使用的法术目标，但不一定是最终的法术效果目标，例如天堂法术燃烧的火球，其施放时只需提供一个目标对象或一个目标地址，
 * 但最终目标对象或目标地址周围4格范围内的生物都会受到伤害，这边一开始提供的目标对象或目标地址就是法术目标(SpellCastTargets)，
 * 但最终受到法术影响的法术效果目标，需要根据SpellEntry.EffectImplicitTarget的规则进行计算
 */
class SpellCastTargets
{
public:
	SpellCastTargets();
	~SpellCastTargets();

	/// 法术目标一般使用GUID记录，因为在施法前或法术命中前(魔兽世界的奥术飞弹有弹道，但天堂都是瞬发)，目标可能消失(瞬飞、传送)，所以并不直接存储游戏对象，而是直到Spell::Cast时才将GUID转换成真正的游戏对象
	void Update(Unit* caster);

	SpellCastTargets& operator=(const SpellCastTargets& target);

	/// 设置法术起始地址和目标地址
	void SetDestination(CoordUnit x, CoordUnit y);
	void SetSource(CoordUnit x, CoordUnit y);
	void GetDestination(CoordUnit& x, CoordUnit& y) const;
	void GetSource(CoordUnit& x, CoordUnit& y) const;

	/// 法术目标是Unit
	void SetUnitTarget(Unit* target);
	ObjectGuid GetUnitTargetGuid() const;
	Unit* GetUnitTarget() const;

	/// 法术目标是GameObject
	void SetGOTarget(GameObject* target);
	ObjectGuid GetGOTargetGuid() const;
	GameObject* GetGOTarget() const;

	/// 法术目标是尸体
	void SetCorpseTarget(Corpse* corpse);
	ObjectGuid GetCorpseTargetGuid() const;

	/// 法术目标是道具
	void SetItemTarget(Item* item);
	ObjectGuid GetItemTargetGuid() const;
	Item* GetItemTarget() const;
	uint32 GetItemTargetEntry() const;

	/// 不存在法术目标对象
	bool IsEmpty() const;

public:
    std::string m_strTarget;

	CoordUnit m_srcX, m_srcY;   /// 法术释放地址
	CoordUnit m_destX, m_destY; /// 法术目标地址

	uint32 m_targetMask;        /// 法术目标匹配规则

protected:
	/// 法术目标对象(Spell::Cast会调用SpellCastTargets::Update将GUID转换为目标对象)
	Unit* m_unitTarget;
	GameObject* m_GOTarget;
	Item* m_itemTarget;

	/// 法术目标对象GUID
	ObjectGuid m_unitTargetGUID;
	ObjectGuid m_GOTargetGUID;
	ObjectGuid m_CorpseTargetGUID;
	ObjectGuid m_itemTargetGUID;
	uint32 m_itemTargetEntry;
};

class Spell
{
public:
    friend class Unit;
    friend class SpellEvent;

public:
	Spell(Unit* caster,
		const SpellEntry* info,
		bool triggered,
		ObjectGuid originalCasterGUID = ObjectGuid(),
		const SpellEntry* triggeredBy = nullptr);
	~Spell();

	void Update(uint32 diff); /// 更新法术，魔兽世界大部分都是读条法术，而天堂都是瞬时法术(m_timer == 0)，所以更新一般只会调用一遍

    void Prepare(const SpellCastTargets* targets, Aura* triggeredByAura = nullptr); /// 法术准备，检验是否满足施法条件
	void Cast(bool skipCheck = false);                                              /// 施法
	void Finish(bool ok = true);                                                    /// 施法结束
	void Cancel();                                                                  /// 取消施法(天堂法术都是瞬发的，所以理论上不会被调用)

    void HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, SpellEffectIndex eff); // 处理法术效果

    void SendCastResult(SpellCastResult result); /// 发送施法结果

	uint32 GetState() const;     /// 获取当前法术状态
	void SetState(uint32 state); /// 设置法术当前状态

    bool IsDeletable() const;    /// 是否能安全的删除法术实例

protected:
    void HandleImmediate(); /// 应用法术效果

	SpellCastResult CheckCast(bool strict);   /// 检查是否满足法术施放条件
	SpellCastResult CheckItems();             /// 法术道具(例如闪电魔杖)使用次数是否充足、施法材料是否充足
	SpellCastResult CheckRange(bool strict);  /// 施法者和施法目标直接的距离是否超过了最大施法距离
	SpellCastResult CheckPower();             /// 施法者是否有足够施放法术所需的能量(体力、魔力)
	SpellCastResult CheckCasterAuras() const; /// 目标是否存在免疫光环(例如绝对屏障)

	void CalculatePowerCost(const SpellEntry *proto, Unit* caster, Item* castItem); /// 计算施法所需的能量
	void TakePower();    /// 施法能量消耗结算，扣除施法所需的魔力、体力等
	void TakeReagents(); /// 施法材料消耗结算，扣除施法所需的魔法宝石、精灵玉等
	void TakeCastItem(); /// 法术道具消耗结算，扣除法术道具使用次数(闪电魔杖)或消耗道具(魔法卷轴)

	bool HasCooldown();     /// 是否处于技能冷却
	void TriggerCooldown(); /// 开始技能冷却
	void CancelCooldown();  /// 取消技能冷却

protected:
	uint32 m_state;                 /// 法术当前状态
	uint32 m_timer;                 /// 法术读条计时器(天堂都是瞬时法术，所以这个值永远为0)
	uint32 m_duration;              /// 法术持续时间
	int32 m_powerCost[MAX_POWERS];  /// 法术能量消耗(只能在Spell::Prepare中初始化)
	bool m_canReflect;              /// 法术是否能被反射
    bool m_executedCurrently;       /// 防止在法术处理期间删除法术对象

	WeaponAttackType m_attackType;

	const SpellEntry* m_proto; /// 法术模板信息

protected:
	SpellCastTargets m_targets;      /// 施法对象信息
    Unit* m_caster;                  /// 施法者
    Unit* m_originalCaster;          /// 原始施法者(当施法者是GameObject类型对象时，需要找到其拥有者)
    ObjectGuid m_originalCasterGUID; /// 原始施法者GUID
	CoordUnit m_castPositionX;       /// 施法者X坐标
	CoordUnit m_castPositionY;       /// 施法者Y坐标
	Towards m_castOrientation;       /// 施法者朝向
    Item* m_castItem;                /// 道具施法者
    // 仅在SpellEffects处理中使用
    Unit* m_unitTarget;              /// 法术目标(Unit类型)
    Item* m_itemTarget;              /// 法术目标(道具类型)
    GameObject* m_gameObjTarget;     /// 法术目标(GameObject类型)

    /**
     * 在施法前保存的都是GUID，避免出现法术目标突然消失的情况(闪现)，在Spell::Cast的时候再将GUID转换成游戏对象指针
     */
    void UpdatePointers();              /// 更新施法者对象指针和法术目标对象指针
    void UpdateOriginalCasterPointer(); /// 更新施法者对象指针

	Unit* GetCaster() const;                       /// 施法者
	WorldObject* GetAffectiveCasterObject() const; /// 真正的法术效果来源者，例如DoT法术的施法者或者GameObject的拥有者，可能为空
	Unit* GetAffectiveCaster() const;              /// 上面方法的优化版，优先返回原始施法者，如果不存在则返回施法者
	WorldObject* GetCastingObject() const;         /// 根据原始施法者的GUID获取对应的游戏对象

protected:
    /*********************************************************/
    /***              Spell trigger subsystem              ***/
    /*********************************************************/
    bool   m_canTrigger;         /// 是否允许法术触发
	uint8  m_negativeEffectMask; /// 仅负面法术效果命中法术目标时才进行法术触发判定(规避法术中的正面效果，以及负面法术未命中的情况)
    uint32 m_procAttacker;       /// 法术触发条件(攻击者)，例如在攻击者成功命中受害者时触发
    uint32 m_procVictim;         /// 法术触发条件(被攻击者)，例如被攻击者成功收到伤害时触发

    typedef std::list<SpellEntry const*> SpellInfoList;
    SpellInfoList m_TriggerSpells;            /// 成功施法后，继续对目标施放触发的法术
    bool m_IsTriggeredSpell;                  /// 表示当前法术是否是因为触发产生的
    const SpellEntry* m_triggeredByAuraSpell; /// 记录触发当前法术的法术

    void PrepareDataForTriggerSystem(); /// 设置法术触发条件
    void CastTriggerSpells();           /// 施放触发的法术

protected:
	/*********************************************************/
	/***              Spell target subsystem               ***/
	/*********************************************************/
	typedef std::list<Unit*> UnitList;

	struct TargetInfo
	{
		ObjectGuid targetGUID;
		uint64 timeDelay;
		uint32 HitInfo;
		uint32 damage;
		SpellMissInfo missCondition: 8;
		SpellMissInfo reflectResult: 8;
		uint8 effectMask: 8;
		bool processed: 1;
	};

	struct GOTargetInfo
	{
		ObjectGuid targetGUID;
		uint64 timeDelay;
		uint8 effectMask: 8;
		bool processed: 1;
	};

	struct ItemTargetInfo
	{
		Item* item;
		uint8 effectMask;
	};

    /// 法术效果目标列表，根据根据法术效果目标匹配规则(SpellEntry.EffectImplicitTarget)和法术目标(SpellCastTargets)，计算出最终受到法术影响的法术效果目标
	typedef std::list<TargetInfo> TargetList;
	typedef std::list<GOTargetInfo> GOTargetList;
	typedef std::list<ItemTargetInfo> ItemTargetList;
	TargetList m_UniqueTargetInfo;     /// Unit类型的法术目标列表
	GOTargetList m_UniqueGOTargetInfo; /// GameObject类型的法术目标列表
	ItemTargetList m_UniqueItemInfo;   /// 道具类型的法术目标列表

	int32 m_damage;  /// 法术造成的伤害量
	int32 m_healing; /// 法术造成的治疗量
	SpellAuraHolder* m_spellAuraHolder; /// 法术光环持有者，仅当该法术具有光环效果时创建(在DoSpellHitOnUnit中使用)

	/// 清理法术目标列表
	void CleanupTargetList();
	void ClearCastItem();

	/// 计算法术效果目标
    void FillTargetMap();
	void SetTargetMap(SpellEffectIndex eff, uint32 targetMode, UnitList& targetUnitMap);

	/// 过滤不符合要求的法术目标
    bool CheckTarget(Unit* target, SpellEffectIndex eff);
    bool CheckTargetCreatureType(Unit* target) const;
	bool IgnoreItemRequirements() const;

	/// 记录受到法术效果影响的游戏对象
	void AddUnitTarget(Unit* pVictim, SpellEffectIndex eff);
	void AddUnitTarget(ObjectGuid unitGuid, SpellEffectIndex eff);
	void AddGOTarget(GameObject* target, SpellEffectIndex eff);
	void AddGOTarget(ObjectGuid goGuid, SpellEffectIndex eff);
	void AddItemTarget(Item* target, SpellEffectIndex eff);

	/// 应用法术效果
    void DoAllEffectOnTarget(TargetInfo* target);
	void DoSpellHitOnUnit(Unit* unit, uint32 effectMask);
    void DoAllEffectOnTarget(GOTargetInfo* target);
    void DoAllEffectOnTarget(ItemTargetInfo* target);

    void ResetEffectDamageAndHeal();

public:
	void EffectEmpty(SpellEffectIndex eff);
	void EffectNULL(SpellEffectIndex eff);
	void EffectUnused(SpellEffectIndex eff);
	void EffectDummy(SpellEffectIndex eff);

	void EffectManaDrain(SpellEffectIndex eff);
	void EffectHealthLeech(SpellEffectIndex eff);
	void EffectHeal(SpellEffectIndex eff);
};

typedef void(Spell::*pEffect)(SpellEffectIndex eff_idx);

class SpellEvent : public BasicEvent
{
public:
	SpellEvent(Spell* spell);
	virtual ~SpellEvent();

	virtual bool Execute(uint64 e_time, uint32 p_time) override;
	virtual void Abort(uint64 e_time) override;
	virtual bool IsDeletable() const override;
protected:
	Spell* m_Spell;
};
#endif
