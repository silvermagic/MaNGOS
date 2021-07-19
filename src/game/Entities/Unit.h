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

#ifndef __UNIT_H
#define __UNIT_H

#include <boost/algorithm/clamp.hpp>
#include "Utilities/EventProcessor.h"
#include "SharedDefines.h"
#include "Object.h"

struct SpellEntry;

class Aura;
class SpellAuraHolder;
class Creature;
class Spell;
class DynamicObject;
class GameObject;
class Item;
class Pet;

class MANGOS_DLL_SPEC Unit : public WorldObject
{
public:
    friend class Spell;
    friend class SpellEvent;
    friend class SpellAuraHolder;

public:
    typedef std::set<Unit*> AttackerSet;
    typedef std::multimap<uint32 /*spellId*/, SpellAuraHolder*> SpellAuraHolderMap;
    typedef std::pair<SpellAuraHolderMap::iterator, SpellAuraHolderMap::iterator> SpellAuraHolderBounds;
    typedef std::pair<SpellAuraHolderMap::const_iterator, SpellAuraHolderMap::const_iterator> SpellAuraHolderConstBounds;
    typedef std::list<SpellAuraHolder*> SpellAuraHolderList;
    typedef std::list<Aura*> AuraList;
    typedef std::map<uint8 /*slot*/, SpellAuraHolder* /*spellId*/> VisibleAuraMap;

public:
    Unit();
    virtual ~Unit();

    void Update(uint32 update_diff, uint32 time) override;

	void AddToWorld() override;
	void RemoveFromWorld() override;
	void CleanupsBeforeDelete() override;      // 清理在游戏世界中产生的数据
	void OnRelocated();                        // 移动后的回调函数

	Unit* GetOwner() const;                    // 获取对象拥有者
	const ObjectGuid& GetCreatorGuid() const;  // 获取对象的创造者
	const ObjectGuid& GetOwnerGuid() const;    // 获取对象的召唤者
	const ObjectGuid& GetPetGuid() const;      // 获取召唤的宠物
	const ObjectGuid& GetTargetGuid() const;   // 获取目标

	void SetCreatorGuid(ObjectGuid creator);   // 设置对象的创造者
	void SetOwnerGuid(ObjectGuid owner);       // 设置对象的召唤者
	void SetPetGuid(ObjectGuid pet);           // 设置召唤的宠物
	void SetTargetGuid(ObjectGuid targetGuid); // 设置目标

protected:
	/*********************************************************/
	/***                     SPELL SYSTEM                  ***/
	/*********************************************************/
    SpellAuraHolderMap m_spellAuraHolders;                         // 光环持有者列表，保存当前生效的光环，同一法术命中可能产生多个光环持有者
    SpellAuraHolderMap::iterator m_spellAuraHoldersUpdateIterator; // 迭代器，在遍历并更新光环持有者列表时会产生删除操作，为了不影响后续遍历，需要保存当前遍历迭代器指针
    AuraList m_deletedAuras;                                       // 待删除光环列表，一般法术效果删除需要先将光环从光环持有者中移至待删除光环列表，最后统一删除，避免
    SpellAuraHolderList m_deletedHolders;                          // 待删除光环持有者列表，当光环持有者的所有光环都被移至待删除光环列表后，将光环持有者移至待删除光环持有者列表，最后统一删除，避免其他地方引用了数据
    AuraList m_modifyAuras[TOTAL_AURAS];                           // 记录单一光环效果的来源列表，例如装备防具会带来防御加成，使用防御类法术也会带来防御加成，对于防御加成这一光环效果，需要用列表都记录下来，同时还能实现不可叠加的光环效果(检查列表是否已经存在其他来源)
    VisibleAuraMap m_visibleAuras;                                 // 可见光环列表，保存那些在客户端右上角有图标显示的光环
    uint32 m_cooldown;                                             // 技能冷却时间

public:
	/**
	 * 更新Unit身上的技能效果
	 *
	 * @param time 距离上次更新，已经经过的时间(单位毫秒)
	 */
    void UpdateSpells(uint32 time);

    /**
     * 清理延迟删除的待光环持有者列表和待删除光环列表，这边直接删除对象，效果删除已经在RemoveAura里面处理了
     */
    void CleanupDeletedAuras();

    /**
     * 根据法术信息创建Spell对象，并设置法术目标信息，最后调用Spell::Prepare完成施法动作
     *
     * @param Victim          当前法术击中的目标
     * @param proto           当前法术信息
     * @param triggered       当前法术是外部环境触发的，还是主动施放的，true表示外部触发
     * @param castItem        施放当前法术的道具(可为nullptr)
     * @param triggeredByAura 触发当前法术的光环(可为nullptr)
     * @param originalCaster  诸如火墙之类的法术，需要提供真正的原始施法者，但通常情况下就是Unit本身
     * @param triggeredBy     触发当前法术的法术信息(可为nullptr)
     */
    void CastSpell(Unit* Victim, const SpellEntry* proto, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    void CastSpell(Unit* Victim, uint32 spellId, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    /**
     * 根据法术信息创建Spell对象，并设置法术目标信息，最后调用Spell::Prepare完成施法动作
     *
     * @param x               当前法术目标的X轴坐标(对空地施放法术)
     * @param y               当前法术目标的Y轴坐标(对空地施放法术)
     * @param proto           当前法术信息
     * @param triggered       当前法术是外部环境触发的，还是主动施放的，true表示外部触发
     * @param castItem        施放当前法术的道具(可为nullptr)
     * @param triggeredByAura 触发当前法术的光环(可为nullptr)
     * @param originalCaster  诸如火墙之类的法术，需要提供真正的原始施法者，但通常情况下就是Unit本身
     * @param triggeredBy     触发当前法术的法术信息(可为nullptr)
     */
    void CastSpell(CoordUnit x, CoordUnit y, const SpellEntry* proto, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    void CastSpell(CoordUnit x, CoordUnit y, uint32 spellId, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);

    /**
     * 计算法术命中结果，结果分为抵抗、免疫、反射或命中
     *
     * @param pVictim    被攻击者，法术攻击对象
     * @param proto      法术信息
     * @param canReflect 法术是否允许反射，true表示允许
     * @return 法术命中结果
     */
    SpellMissInfo SpellHitResult(Unit* pVictim, const SpellEntry *proto, bool canReflect = false);
    SpellMissInfo MeleeSpellHitResult(Unit* pVictim, const SpellEntry *proto);
    SpellMissInfo MagicSpellHitResult(Unit* pVictim, const SpellEntry *proto);

    /**
     * 添加/删除光环
     *
     * 光环是持续型法术效果(例如buff)的载体，所以删除光环，就是移除持续型法术效果。一个法术命中对象后可能产生多个持续型法术效果(光环)，
     * 所以使用光环持有者来保存单次法术命中效果实例，即光环持有者是单次法术所产生的所有持续型法术效果(光环)的载体，删除光环持有者就是移除法术所有的持续型法术效果。
     * 下面一系列Remove方法作用都是删除光环/光环持有者，主要区别在于待删除的光环/光环持有者获取方式不同(分为直接指定和根据指定参数查找)，以及删除的原因不同。
     * 需要注意的是，相同法术命中后，新产生的法术效果可能会覆盖旧的法术效果，也可能会同时存在，例如DoT类法术，即可能存在一个法术编号对应多个光环持有者的情况，
     * 所以根据法术编号来获取待删除的光环/光环持有者时的处理需要更加仔细。
     */

    /**
     * 添加光环
     *
     * @param aura 光环对象
     */
    void AddAuraToModifyList(Aura* aura);
    /**
     * 添加光环持有者
     *
     * @param holder 光环持有者对象
     * @return
     */
    bool AddSpellAuraHolder(SpellAuraHolder* holder);
    /**
     * todo: 这个干吗的?
     *
     * @param spellId
     * @param delaytime
     * @param casterGuid
     */
	void DelaySpellAuraHolder(uint32 spellId, int32 delaytime, ObjectGuid casterGuid);
    /**
     * 移除光环，待移除的光环由参数直接提供
     *
     * @param aura 待移除的光环对象
     * @param mode 移除原因
     */
    void RemoveAura(Aura* aura, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 移除光环，待移除的光环需要根据参数查找
     *
     * @param spellId 法术编号
     * @param eff     法术效果编号
     * @param except  如果不为空，则跳过删除指定的光环
     */
    void RemoveAura(uint32 spellId, SpellEffectIndex eff, Aura* except = nullptr);
    /**
     * 移除光环，待移除的光环由参数直接提供，如果是光环持有者拥有的最后一个光环，那么参照移除光环持有者处理
     *
     * @param holder 光环持有者
     * @param eff    法术效果编号
     * @param mode   移除原因
     */
    void RemoveSingleAuraFromSpellAuraHolder(SpellAuraHolder* holder, SpellEffectIndex eff, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 移除光环，待移除的光环需要根据参数查找
     *
     * @param spellId    法术编号
     * @param eff        法术效果编号
     * @param casterGuid 施法者GUID
     * @param mode       移除原因
     */
    void RemoveSingleAuraFromSpellAuraHolder(uint32 spellId, SpellEffectIndex eff, ObjectGuid casterGuid, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 移除光环持有者，待移除的光环持有者由参数直接提供
     *
     * @param holder 光环持有者
     * @param mode   移除原因
     */
    void RemoveSpellAuraHolder(SpellAuraHolder* holder, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除指定法术产生的相关法术效果
     *
     * @param spellId 法术编号
     * @param except  如果不为空，则跳过删除指定的光环持有者
     * @param mode    移除原因
     */
    void RemoveAurasDueToSpell(uint32 spellId, SpellAuraHolder* except = nullptr, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除指定法术道具产生的相关法术效果
     *
     * @param castItem 法术道具编号
     * @param spellId  法术编号
     */
    void RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除指定施法者产生的相关法术效果
     *
     * @param spellId    法术编号
     * @param casterGuid 施法者GUID
     */
    void RemoveAurasByCasterSpell(uint32 spellId, ObjectGuid casterGuid);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除指定法术机制的相关法术效果
     *
     * @param mechMask      法术机制
     * @param exceptSpellId 如果不为空，则跳过删除指定的法术的光环持有者
     * @param non_positive  是否只删除负面法术效果，true表示不删除增益法术效果
     */
    void RemoveAurasAtMechanicImmunity(uint32 mechMask, uint32 exceptSpellId, bool non_positive = false);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除指定光环类型的相关法术效果
     *
     * @param auraType 光环类型
     */
    void RemoveSpellsCausingAura(AuraType auraType);
    void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except);
    void RemoveSpellsCausingAura(AuraType auraType, ObjectGuid casterGuid);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，中断光环
     *
     * @param flags 光环中断标志
     */
    void RemoveAurasWithInterruptFlags(uint32 flags);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，移除存在指定法术属性的相关法术效果
     *
     * @param flags 法术属性
     */
    void RemoveAurasWithAttribute(uint32 flags);
    /**
     * 移除光环持有者，待移除的光环持有者需要根据参数查找，驱散光环
     *
     * @param type       驱散类型
     * @param casterGuid 施法者GUID
     */
    void RemoveAurasWithDispelType(DispelType type, ObjectGuid casterGuid = ObjectGuid());
    /**
     * 移除所有光环持有者
     */
    void RemoveAllAuras(AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * 死亡后移除所有光环持有者
     */
    void RemoveAllAurasOnDeath();

    /**
     * 技能冷却
     */
    uint32 GetCooldown() const;         // 获取技能冷却剩余时间
	void ResetCooldown(uint32 gcd = 0); // 重置技能冷却时间

    /**
     * 帮助函数
     */

	Aura* GetAura(uint32 spellId, SpellEffectIndex effindex);
	Aura* GetAura(AuraType type, SpellFamily family, ObjectGuid casterGuid = ObjectGuid());
	/**
	 * 获取指定类型的光环列表
	 *
	 * @param type 光环类型
	 * @return 光环列表
	 */
	const AuraList& GetAurasByType(AuraType type) const { return m_modifyAuras[type]; }

    /**
     * 获取所有法术命中产生的效果实例
     *
     * @return 光环持有者列表
     */
    SpellAuraHolderMap& GetSpellAuraHolderMap() { return m_spellAuraHolders; }
    const SpellAuraHolderMap& GetSpellAuraHolderMap() const { return m_spellAuraHolders; }

    /**
     * 获取指定法术命中产生的效果实例
     *
     * @param spell_id 法术编号
     * @return 光环持有者列表
     */
    SpellAuraHolderBounds GetSpellAuraHolderBounds(uint32 spellId) { return m_spellAuraHolders.equal_range(spellId); }
    SpellAuraHolderConstBounds GetSpellAuraHolderBounds(uint32 spellId) const { return m_spellAuraHolders.equal_range(spellId); }

    /**
     * 获取指定光环槽内可见光环的光环持有者，光环槽是一个虚拟的概念，你可以认为是在服务端创建一个对应客户端右上角图标列表的概念，
     * 便于针对需要显示和不需要显示的光环做特殊处理，例如可见光环消失，需要向客户端发送光环效果消失数据包
     *
     * @param slot 光环槽编号
     * @return 可见光环的光环持有者
     */
    SpellAuraHolder* GetVisibleAura(uint8 slot) const;
    const VisibleAuraMap& GetVisibleAuras() const { return m_visibleAuras; }
    uint8 GetVisibleAurasCount() const { return m_visibleAuras.size(); }
    void SetVisibleAura(uint8 slot, SpellAuraHolder* holder);

    /**
     * 找到指定施法者使用指定法术击中所产生的效果实例
     *
     * @param spell_id   法术编号
     * @param casterGUID 施法者GUID
     * @return 光环持有者
     */
    SpellAuraHolder* GetSpellAuraHolder(uint32 spellId, ObjectGuid casterGUID) const;
    SpellAuraHolder* GetSpellAuraHolder(uint32 spellId) const;


    bool HasAura(AuraType auraType) const;                       // 判断身上是否存在指定光环
    bool HasAura(uint32 spellId, SpellEffectIndex eff) const;
    bool HasAura(uint32 spellId) const;
	bool HasGlobalCooldown() const;                              // 判断技能是否处于冷却中
    virtual bool HasSpell(uint32 spellId) const;                 // 判断是否已经习得法术
    bool IsBrave() const;                                        // 判断是否处于狂化状态
    bool IsFastMovable() const;                                  // 判断是否处于快速移动状态
    bool IsFrozen() const;                                       // 判断是否处于冻结状态
    bool IsInvisble() const;                                     // 判断是否存在隐身效果
    bool IsPolymorphed() const;                                  // 判断是否处于变形状态

	/**
	 * 法术伤害处理
	 */

	/**
	 * 计算法术基础伤害
	 *
	 * @param target
	 * @param proto
	 * @param eff
	 * @param basePoints
	 * @return
	 */
	int32 CalculateSpellDamage(const Unit* target, const SpellEntry* proto, SpellEffectIndex eff, const int32* basePoints = nullptr);

	/**
	 * 计算近战伤害加成(施法者部分)
	 *
	 * @param pVictim     被攻击者
	 * @param damage      对被攻击者造成的初始伤害值
	 * @param att         攻击类型
	 * @param proto       法术信息
	 * @param damagetype  伤害类型
	 * @param stack       堆叠次数
	 * @return 处理后的伤害值
	 */
	uint32 MeleeDamageBonusDone(Unit* pVictim, uint32 damage, WeaponAttackType att, const SpellEntry* proto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);
	/**
	 * 计算近战伤害加成(受害者部分)
	 *
	 * @param pCaster    施法者
	 * @param damage     对被攻击者造成的初始伤害值
	 * @param att        攻击类型
	 * @param proto      法术信息
	 * @param damagetype 伤害类型
	 * @param stack      堆叠次数
	 * @return 处理后的伤害值
	 */
	uint32 MeleeDamageBonusTaken(Unit* pCaster, uint32 damage, WeaponAttackType att, const SpellEntry* proto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);

	/**
	 * 计算治疗加成(施法者部分)
	 *
	 * @param pVictim
	 * @param proto
	 * @param healamount
	 * @param damagetype
	 * @param stack
	 * @return
	 */
	uint32 SpellHealingBonusDone(Unit* pVictim, const SpellEntry* proto, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);
	/**
	 *
	 * @param pCaster
	 * @param proto
	 * @param healamount
	 * @param damagetype
	 * @param stack
	 * @return
	 */
	uint32 SpellHealingBonusTaken(Unit* pCaster, const SpellEntry* proto, int32 healamount, DamageEffectType damagetype, uint32 stack = 1);

protected:
	//// 例如天堂的火墙或者魔兽世界的陷阱等法术，需要在目标地点显示火墙或陷阱，这类法术的实现都是在目标地点创建一个临时的DynamicObject对象
    typedef std::list<GameObject*> GameObjectList;
    typedef std::map<uint32, ObjectGuid> WildGameObjectMap;
    GuidList m_dynObjGUIDs;           // 记录法术效果创建的DynamicObject对象列表
    GameObjectList m_gameObj;         // 召唤生物列表
    WildGameObjectMap m_wildGameObjs; // 控制的野生生物列表 ?

public:
    DynamicObject* GetDynObject(uint32 spellId, SpellEffectIndex effIndex);
    DynamicObject* GetDynObject(uint32 spellId);
    void AddDynObject(DynamicObject* dynObj);
    void RemoveDynObject(uint32 spellId);
    void RemoveDynObjectWithGUID(ObjectGuid guid);
    void RemoveAllDynObjects();

    GameObject* GetGameObject(uint32 spellId) const;
    void AddGameObject(GameObject* gameObj);
    void AddWildGameObject(GameObject* gameObj);
    void RemoveGameObject(GameObject* gameObj, bool del);
    void RemoveGameObject(uint32 spellId, bool del);
    void RemoveAllGameObjects();

protected:
	/*********************************************************/
    /***                      STAT SYSTEM                  ***/
    /*********************************************************/
    // 属性
	int32 m_totalStat[MAX_STATS];
	int8 m_stat[MAX_STATS];

    // 近战/远程伤害
    int32 m_totalAttackDamage[MAX_ATTACK];
    int8 m_attackDamage[MAX_ATTACK];

	// 近战/远程命中
	int32 m_totalAttackHit[MAX_ATTACK];
	int8 m_attackHit[MAX_ATTACK];

	// 防御
	int32 m_totalAc;
	int8 m_ac;

	// 血量、魔量
	int16 m_mana;   // 当前魔量
	int16 m_health; // 当前血量
	int32 m_totalMaxMana;
	int16 m_maxMana;
	int32 m_totalMaxHealth;
	int16 m_maxHealth;

	// 魔法防御、属性防御
	int32 m_totalDefense[MAX_SPELL_SCHOOL];
	int16 m_defense[MAX_SPELL_SCHOOL];

	// 法术强度
	int32 m_totalSp;
	int16 m_baseSp;
	int16 m_sp;

	// 闪避率
	int32 m_totalDodge;
	int16 m_dodge;

	// 抗性
	int32 m_totalResist[MAX_STORED_MECHANICS];
	int8 m_resist[MAX_STORED_MECHANICS];

public:
    /// 角色力量、敏捷、体质、智力、精神、魅力属性计算

	/**
	 * 获取角色属性有效值
	 *
	 * @param stat 属性类型
	 * @return 属性有效值
	 */
	int8 GetStat(Stats stat) const;
    /**
     * 更新角色属性总值，并计算角色属性有效值，防具/武器或魔法产生的属性值变化会触发此函数更新角色属性总值，
     * 角色属性总值 = 角色基础力量属性值 + 装备奖励 + 魔法奖励
     *
     * @param stat 属性类型
     * @param diff 属性变动值
     */
    void ModifyStat(Stats stat, int32 diff);

	/// 近战/远程伤害加成 近战/远程命中加成 魔法伤害加成 魔法命中加成 魔法暴击加成计算
	int8 GetAttackHit(WeaponAttackType att) const;
	int8 GetAttackDamage(WeaponAttackType att) const;
	void ModifyAttackHit(WeaponAttackType att, int32 diff);
	void ModifyAttackDamage(WeaponAttackType att, int32 diff);

	/// 防御计算
	int8 GetArmor() const;
	void ModifyArmor(int32 diff);

	/// 血量魔量正义值计算
	int16 GetMana() const;
	int16 GetMaxMana() const;
	void ModifyMaxMana(int32 diff);

	int16 GetHealth() const;
	int16 GetMaxHealth() const;
	void ModifyMaxHealth(int32 diff);

	int16 GetLawful() const;

	/// 设置当前血量魔量正义值，仅在数据库加载时direct == true
	void SetMana(int16 value, bool direct = false);
	void SetHealth(int16 value, bool direct = false);
	void SetLawful(int16 value);

	/// 魔法防御、属性防御、法术强度计算
	int16 GetDefense(SpellSchools school) const;
	void ModifyDefense(SpellSchools school, int32 diff);

	/// 法强计算
	int16 GetSpellPower() const;
	void ModifySpellPower(int32 diff);
	void UpdateBaseSpellPower(); // 仅与等级和当前智力属性相关
	int32 GetSpellLevel() const; // 根据角色等级来计算法术强度加成
	int32 GetSpellBonus() const; // 计算角色智力有效值带来的法术强度加成

	/// 闪避计算
	int8 GetDodge() const;
	void ModifyDodge(int32 diff);

	/// 抗性计算
	int8 GetResist(MechanicsType type) const;
	void ModifyResist(MechanicsType type, int32 diff);

	/// 函数封装
	int16 GetPower(uint8 power);
	void ModifyPower(uint8 power, int16 value);

	/**
	 * 获取指定光环效果产生的属性系数奖励，例如精准射击会将闪避率置零，则其属性系数奖励应该是0.0f，
	 * 即其法术效果的Modifier.m_amount = -100
	 *
	 * @param auratype 光环类型
	 * @return 属性系数奖励
	 */
	float GetTotalAuraMultiplier(AuraType auratype) const;

protected:
    /*********************************************************/
    /***                       ATTRIBUTE                   ***/
    /*********************************************************/
    std::string m_name;       // 名称
    std::string m_title;      // 称号
    DeathState m_deathState;  // 死亡状态
    uint32 m_state;           // 活动状态，由事件驱动，不可编辑
    UnitMoveType m_moveSpeed; // 一段加速(绿色药水、风之疾走、神圣疾走)
    bool m_braveSpeed;        // 二段加速(勇气药水)

public:
    bool GetBrave() const;            // 获取二段加速状态
    Classes GetClass() const;         // 职业
    uint32 GetClassMask() const;      // 职业掩码
    DeathState GetDeathState() const; // 获取死亡状态
	uint8 GetFood() const;            // 获取饱食度
    uint8  GetGender() const;         // 性别
    uint8  GetLevel() const;          // 等级
    std::string GetName() const;      // 名称
    std::string GetTitle() const;     // 称号
    UnitMoveType GetSpeed() const;    // 获取一段加速状态

    void AddUnitState(uint32 flag);               // 添加状态标志
    void ClearUnitState(uint32 flag);             // 清理状态标志
    void SetSpeed(UnitMoveType type);             // 设置一段加速
    void SetBrave(bool enabled = false);          // 设置二段加速
    virtual void SetDeathState(DeathState state); // 设置死亡状态
	void SetFood(uint8 value);                    // 设置饱食度
    virtual void SetRoot(bool enabled);           // 设置束缚状态

    bool CanFreeMove() const;                                    // 判断是否能移动
    bool HasUnitState(uint32 flag) const;                        // 判断是否存在状态标志
    bool IsAlive() const;                                        // 判断是否存活
    bool IsDead() const;                                         // 判断是否死亡
	virtual bool IsFriendlyTo(const Unit* unit) const = 0;       // 与游戏对象是否友好
	virtual bool IsHostileTo(const Unit* unit) const = 0;        // 与游戏对象是否敌视
    bool IsTargetableForAttack(bool inverseAlive = false) const; // 判断是否能被攻击
    bool IsPassiveToHostile() const;                             // 被动攻击
	bool IsVisibleForOrDetect(const Unit* u, const WorldObject* viewPoint, bool detect, bool inVisibleList = false) const;
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override;

protected:
    EventProcessor m_Events; // 处理法术等事件
};

#endif
