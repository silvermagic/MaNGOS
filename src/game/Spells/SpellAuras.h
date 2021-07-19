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

#ifndef MANGOS_SPELLAURAS_H
#define MANGOS_SPELLAURAS_H

#include "Entities/ObjectGuid.h"
#include "Common.h"
#include "SQLStructure.h"
#include "SharedDefines.h"

/**
 * 光环参数，在处理光环效果时使用
 * Modifier demo = {
 *   .m_auraname   = SPELL_AURA_MOD_RESISTANCE,
 *   .m_miscvalue  = SPELL_SCHOOL_MASK_NORMAL,
 *   .m_amount     = -27,
 *   .periodictime = 0
 * };
 * 表示光环效果为降低27点Unit的物理防御，光环应用时触发对应效果，并在光环移除时恢复27点Unit的物理防御
 */
struct Modifier
{
    AuraType m_auraname; // 光环类型 enum AuraType
    int32 m_miscvalue;   // 效果类型，其含义取决于AuraType
    int32 m_amount;      // 效果值
    uint32 periodictime; // 决定光环的效果触发频率，例如中毒，持续时间内定期减血，如果设置为0(非周期性光环)，则只触发一次，并在光环移除时删除相应效果
};

class Unit;
class WorldObject;
class Player;
class Item;
class Aura;
struct ProcTriggerSpell;

/**
 * 持有一个法术的所有光环效果，每个法术最多产生三个法术效果，而大部分法术效果都基于光环来实现，所以每个法术最多有三个光环效果，
 * 这也是光环持有者最多能持有的光环效果。它负责记录施法者信息、法术信息、施法目标信息，以及是否是道具触发的法术等，同时它还负责法术叠加(DOT类法术)，
 * 应用不同光环效果的Modifier(修改器)等。
 */
class MANGOS_DLL_SPEC SpellAuraHolder {
public:
    Aura* m_auras[MAX_EFFECT_INDEX];

public:
    SpellAuraHolder(const SpellEntry*, Unit* target, WorldObject* caster, Item* castItem);
    ~SpellAuraHolder();

    void Update(uint32 diff);                               // 更新
    void RefreshHolder();                                   // 刷新
    void Add();                                             // 添加
    void Remove();                                          // 删除
    void AddAura(Aura* aura, SpellEffectIndex index);       // 添加光环
    void RemoveAura(SpellEffectIndex index);                // 删除光环
    void ApplyAuraModifiers(bool apply, bool real = false); // 应用光环效果
    void CleanupTriggeredSpells();                          // 清理触发法术

    void SendAuraUpdate(bool remove) const;         // 发送光环更新消息

    void SetDeleted();                       // 设置删除标记
    void SetInUse(bool state);               // 设置引用计数
    void SetLoadedState(const ObjectGuid& casterGUID, const ObjectGuid& itemGUID, uint32 charges, int32 maxduration, int32 duration); // 设置加载状态
    void SetRemoveMode(AuraRemoveMode mode); // 设置删除原因
    void SetVisibleAura(bool remove);        // 设置可见光环到目标对象的光环槽或将其从目标对象的光环槽中移除

    bool IsInUse() const;              // 是否正在被使用
    bool IsDeleted() const;            // 是否已经被删除
    bool IsEmptyHolder() const;        // 是否不再持有任何光环效果

    /*********************************************************/
    /***                  GENERAL FEATURES                 ***/
    /*********************************************************/

    uint32 GetId() const;                                     // 法术编号
    const SpellEntry* GetSpellProto() const;                  // 法术模板
    Aura* GetAuraByEffectIndex(SpellEffectIndex index) const; // 获取指定编号的光环

    const ObjectGuid& GetCasterGuid() const;   // 获取施法者编号
    void SetCasterGuid(ObjectGuid guid);
    const ObjectGuid& GetCastItemGuid() const; // 触发法术的道具编号
    Unit* GetCaster() const;                   // 施法者对象
    Unit* GetTarget() const;                   // 施法目标对象
    void SetTarget(Unit* target);

    time_t GetAuraApplyTime() const;           // 施法时间
    int32 GetAuraMaxDuration() const;          // 持续时间
    void SetAuraMaxDuration(int32 duration);
    int32 GetAuraDuration() const;             // 剩余时间
    void SetAuraDuration(int32 duration);

    uint8 GetAuraSlot() const;                               // 光环槽编号(客户端右上角光环图标)
    void SetAuraSlot(uint8 slot);
    uint32 GetAuraCharges() const;                           // 获取效果触发次数
    void SetAuraCharges(uint32 charges, bool update = true); // 设置效果触发次数，例如法术魔法屏障的效果是15秒只能抵御一次负面法术，则其效果触发次数就为1
    bool DropAuraCharge();                                   // 标记一次效果触发

    bool HasMechanic(uint32 mechanic) const;         // 是否是特定法术机制
    bool HasMechanicMask(uint32 mechanicMask) const; // 是否包含特定法术机制

    bool IsAreaAura() const;           // 区域型光环(寒冰气息)
    bool IsDeathPersistent() const;    // 死亡后也不会消失的光环(召唤)
    bool IsPermanent() const;          // 永久型光环
    void SetPermanent(bool permanent);
    bool IsPassive() const;            // 被动型光环
    bool IsPersistent() const;         // 持久型光环
    bool IsPositive() const;           // 增益型光环
    bool IsNeedVisibleSlot(const Unit *caster) const; // 是否要在客户端的光环槽显示图标

protected:
    void UpdateHolder(uint32 diff);  // 更新光环持有者

private:
    const SpellEntry *m_spellProto; // 法术模板

    Unit* m_target;            // 施法目标对象
    ObjectGuid m_casterGuid;   // 施法对象编号
    ObjectGuid m_castItemGuid; // 触发法术的道具编号，不直接存储对象指针，防止对象删除导致指针悬空

    uint8 m_auraSlot;     // 光环槽(用于客户端显示)
    uint32 m_procCharges; // 触发次数
    int32 m_maxDuration;  // 持续时间
    int32 m_duration;     // 剩余时间
    time_t m_applyTime;   // 施法时间

    AuraRemoveMode m_removeMode; // 删除原因

    bool m_isDeathPersist;       // 是否死亡后也不会消失
    bool m_isPermanent;          // 是否是永久效果
    bool m_isPassive;            // 是否是被动效果
    bool m_deleted;              // 是否被删除
    uint32 m_in_use;             // 引用计数器,在SpellAuraHolder::ApplyModifiers或SpellAuraHolder::Update等处理时，需要增加引用计数，防止被删除
};

typedef void(Aura::*pAuraHandler)(bool Apply, bool Real);

class MANGOS_DLL_SPEC Aura
{
protected:
    friend Aura* CreateAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32* currentBasePoints, SpellAuraHolder* holder, Unit* target, Unit* caster, Item* castItem);

public:
    /*********************************************************/
    /***                   光环效果处理句柄                   ***/
    /*********************************************************/

    void HandleNULL(bool, bool);              // 没有实现
    void HandleUnused(bool, bool);            // 不被任何法术使用
    void HandleNoImmediateEffect(bool, bool); // 光环在add/remove的时候没有立即生效的效果

public:
    virtual ~Aura();

    void Update(uint32 diff); // 更新

    void SetModifier(AuraType type, int32 amount, uint32 periodictime, int32 miscValue); // 设置光环参数
    Modifier* GetModifier();                                                             // 获取光环参数
    const Modifier* GetModifier() const;
    int32 GetMiscValue() const;                                                          // 获取光环效果类型
    void ApplyModifier(bool apply, bool Real = false);                                   // 应用光环效果

    void SetInUse(bool state);                              // 设置引用计数
    void SetRemoveMode(AuraRemoveMode mode);                // 设置光环移除原因
    void SetLoadedState(int32 damage, uint32 periodicTime); // 设置光环加载时状态

    uint32 GetAuraTicks() const;      // 剩余滴答数
    uint32 GetAuraMaxTicks() const;   // 持续滴答数

    SpellEffectIndex GetEffIndex() const;     // 法术效果编号
    SpellAuraHolder* GetHolder();             // 获取光环持有者
    const SpellAuraHolder* GetHolder() const;
    virtual Unit* GetTriggerTarget() const;   // 获取触发的法术的目标

	bool IsAreaAura() const;   // 光环影响范围是不是区域
    bool IsInUse() const;      // 光环是否正在被使用
    bool IsLastAuraOnHolder(); // 是否是光环效果持有者所持有的最后一个光环效果
	bool IsPeriodic() const;   // 光环在持续时间内是否会定期触发效果
	bool IsPersistent() const; // 光环是瞬间的还是持久的
	bool IsPositive() const;   // 光环增益的还是负面的

    bool IsAffectedOnSpell(const SpellEntry *proto) const;
    bool CanProcFrom(const SpellEntry *proto, uint32 procFlag, uint32 EventProcEx, uint32 procEx, bool active, bool useClassMask) const;

protected:
    Aura(const SpellEntry *proto, SpellEffectIndex eff, SpellAuraHolder* holder, Unit* target, Unit* caster = NULL, Item* castItem = NULL);

    virtual void UpdateAura(uint32 diff);

    void PeriodicTick();          // 周期型光环效果处理
    void PeriodicDummyTick();
    void TriggerSpell();          // 法术触发

protected:
    SpellAuraHolder*  m_spellAuraHolder; // 光环持有者

    Modifier m_modifier;   // 光环参数

    time_t m_applyTime;    // 光环生效时间
    int32 m_periodicTimer; // 周期效果光环计时器
    uint32 m_periodicTick; // 周期效果光环滴答数

    AuraRemoveMode m_removeMode; // 记录光环移除原因

    SpellEffectIndex m_effIndex; // 法术效果编号

    bool m_isPositive;   // 增益型光环
    bool m_isPeriodic;   // 周期型光环
    bool m_isAreaAura;   // 区域型光环
    bool m_isPersistent; // 持久型光环

    uint32 m_in_use; // 引用计数器(在Aura::ApplyModifier和Aura::Update调用中使用)
};

Aura* CreateAura(const SpellEntry *proto, SpellEffectIndex eff, SpellAuraHolder* holder, Unit* target, Unit* caster = nullptr, Item* castItem = nullptr);
SpellAuraHolder* CreateSpellAuraHolder(const SpellEntry *proto, Unit* target, WorldObject* caster, Item* castItem = nullptr);

#endif //MANGOS_SPELLAURAS_H
