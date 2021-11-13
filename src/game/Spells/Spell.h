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
 * ����׼���׶�ʹ�õķ���Ŀ�꣬����һ�������յķ���Ч��Ŀ�꣬�������÷���ȼ�յĻ�����ʩ��ʱֻ���ṩһ��Ŀ������һ��Ŀ���ַ��
 * ������Ŀ������Ŀ���ַ��Χ4��Χ�ڵ����ﶼ���ܵ��˺������һ��ʼ�ṩ��Ŀ������Ŀ���ַ���Ƿ���Ŀ��(SpellCastTargets)��
 * �������ܵ�����Ӱ��ķ���Ч��Ŀ�꣬��Ҫ����SpellEntry.EffectImplicitTarget�Ĺ�����м���
 */
class SpellCastTargets
{
public:
	SpellCastTargets();
	~SpellCastTargets();

	/// ����Ŀ��һ��ʹ��GUID��¼����Ϊ��ʩ��ǰ��������ǰ(ħ������İ����ɵ��е����������ö���˲��)��Ŀ�������ʧ(˲�ɡ�����)�����Բ���ֱ�Ӵ洢��Ϸ���󣬶���ֱ��Spell::Castʱ�Ž�GUIDת������������Ϸ����
	void Update(Unit* caster);

	SpellCastTargets& operator=(const SpellCastTargets& target);

	/// ���÷�����ʼ��ַ��Ŀ���ַ
	void SetDestination(CoordUnit x, CoordUnit y);
	void SetSource(CoordUnit x, CoordUnit y);
	void GetDestination(CoordUnit& x, CoordUnit& y) const;
	void GetSource(CoordUnit& x, CoordUnit& y) const;

	/// ����Ŀ����Unit
	void SetUnitTarget(Unit* target);
	ObjectGuid GetUnitTargetGuid() const;
	Unit* GetUnitTarget() const;

	/// ����Ŀ����GameObject
	void SetGOTarget(GameObject* target);
	ObjectGuid GetGOTargetGuid() const;
	GameObject* GetGOTarget() const;

	/// ����Ŀ����ʬ��
	void SetCorpseTarget(Corpse* corpse);
	ObjectGuid GetCorpseTargetGuid() const;

	/// ����Ŀ���ǵ���
	void SetItemTarget(Item* item);
	ObjectGuid GetItemTargetGuid() const;
	Item* GetItemTarget() const;
	uint32 GetItemTargetEntry() const;

	/// �����ڷ���Ŀ�����
	bool IsEmpty() const;

public:
    std::string m_strTarget;

	CoordUnit m_srcX, m_srcY;   /// �����ͷŵ�ַ
	CoordUnit m_destX, m_destY; /// ����Ŀ���ַ

	uint32 m_targetMask;        /// ����Ŀ��ƥ�����

protected:
	/// ����Ŀ�����(Spell::Cast�����SpellCastTargets::Update��GUIDת��ΪĿ�����)
	Unit* m_unitTarget;
	GameObject* m_GOTarget;
	Item* m_itemTarget;

	/// ����Ŀ�����GUID
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

	void Update(uint32 diff); /// ���·�����ħ������󲿷ֶ��Ƕ��������������ö���˲ʱ����(m_timer == 0)�����Ը���һ��ֻ�����һ��

    void Prepare(const SpellCastTargets* targets, Aura* triggeredByAura = nullptr); /// ����׼���������Ƿ�����ʩ������
	void Cast(bool skipCheck = false);                                              /// ʩ��
	void Finish(bool ok = true);                                                    /// ʩ������
	void Cancel();                                                                  /// ȡ��ʩ��(���÷�������˲���ģ����������ϲ��ᱻ����)

    void HandleEffects(Unit* pUnitTarget, Item* pItemTarget, GameObject* pGOTarget, SpellEffectIndex eff); // ������Ч��

    void SendCastResult(SpellCastResult result); /// ����ʩ�����

	uint32 GetState() const;     /// ��ȡ��ǰ����״̬
	void SetState(uint32 state); /// ���÷�����ǰ״̬

    bool IsDeletable() const;    /// �Ƿ��ܰ�ȫ��ɾ������ʵ��

protected:
    void HandleImmediate(); /// Ӧ�÷���Ч��

	SpellCastResult CheckCast(bool strict);   /// ����Ƿ����㷨��ʩ������
	SpellCastResult CheckItems();             /// ��������(��������ħ��)ʹ�ô����Ƿ���㡢ʩ�������Ƿ����
	SpellCastResult CheckRange(bool strict);  /// ʩ���ߺ�ʩ��Ŀ��ֱ�ӵľ����Ƿ񳬹������ʩ������
	SpellCastResult CheckPower();             /// ʩ�����Ƿ����㹻ʩ�ŷ������������(������ħ��)
	SpellCastResult CheckCasterAuras() const; /// Ŀ���Ƿ�������߹⻷(�����������)

	void CalculatePowerCost(const SpellEntry *proto, Unit* caster, Item* castItem); /// ����ʩ�����������
	void TakePower();    /// ʩ���������Ľ��㣬�۳�ʩ�������ħ����������
	void TakeReagents(); /// ʩ���������Ľ��㣬�۳�ʩ�������ħ����ʯ���������
	void TakeCastItem(); /// �����������Ľ��㣬�۳���������ʹ�ô���(����ħ��)�����ĵ���(ħ������)

	bool HasCooldown();     /// �Ƿ��ڼ�����ȴ
	void TriggerCooldown(); /// ��ʼ������ȴ
	void CancelCooldown();  /// ȡ��������ȴ

protected:
	uint32 m_state;                 /// ������ǰ״̬
	uint32 m_timer;                 /// ����������ʱ��(���ö���˲ʱ�������������ֵ��ԶΪ0)
	uint32 m_duration;              /// ��������ʱ��
	int32 m_powerCost[MAX_POWERS];  /// ������������(ֻ����Spell::Prepare�г�ʼ��)
	bool m_canReflect;              /// �����Ƿ��ܱ�����
    bool m_executedCurrently;       /// ��ֹ�ڷ��������ڼ�ɾ����������

	WeaponAttackType m_attackType;

	const SpellEntry* m_proto; /// ����ģ����Ϣ

protected:
	SpellCastTargets m_targets;      /// ʩ��������Ϣ
    Unit* m_caster;                  /// ʩ����
    Unit* m_originalCaster;          /// ԭʼʩ����(��ʩ������GameObject���Ͷ���ʱ����Ҫ�ҵ���ӵ����)
    ObjectGuid m_originalCasterGUID; /// ԭʼʩ����GUID
	CoordUnit m_castPositionX;       /// ʩ����X����
	CoordUnit m_castPositionY;       /// ʩ����Y����
	Towards m_castOrientation;       /// ʩ���߳���
    Item* m_castItem;                /// ����ʩ����
    // ����SpellEffects������ʹ��
    Unit* m_unitTarget;              /// ����Ŀ��(Unit����)
    Item* m_itemTarget;              /// ����Ŀ��(��������)
    GameObject* m_gameObjTarget;     /// ����Ŀ��(GameObject����)

    /**
     * ��ʩ��ǰ����Ķ���GUID��������ַ���Ŀ��ͻȻ��ʧ�����(����)����Spell::Cast��ʱ���ٽ�GUIDת������Ϸ����ָ��
     */
    void UpdatePointers();              /// ����ʩ���߶���ָ��ͷ���Ŀ�����ָ��
    void UpdateOriginalCasterPointer(); /// ����ʩ���߶���ָ��

	Unit* GetCaster() const;                       /// ʩ����
	WorldObject* GetAffectiveCasterObject() const; /// �����ķ���Ч����Դ�ߣ�����DoT������ʩ���߻���GameObject��ӵ���ߣ�����Ϊ��
	Unit* GetAffectiveCaster() const;              /// ���淽�����Ż��棬���ȷ���ԭʼʩ���ߣ�����������򷵻�ʩ����
	WorldObject* GetCastingObject() const;         /// ����ԭʼʩ���ߵ�GUID��ȡ��Ӧ����Ϸ����

protected:
    /*********************************************************/
    /***              Spell trigger subsystem              ***/
    /*********************************************************/
    bool   m_canTrigger;         /// �Ƿ�����������
	uint8  m_negativeEffectMask; /// �����淨��Ч�����з���Ŀ��ʱ�Ž��з��������ж�(��ܷ����е�����Ч�����Լ����淨��δ���е����)
    uint32 m_procAttacker;       /// ������������(������)�������ڹ����߳ɹ������ܺ���ʱ����
    uint32 m_procVictim;         /// ������������(��������)�����类�����߳ɹ��յ��˺�ʱ����

    typedef std::list<SpellEntry const*> SpellInfoList;
    SpellInfoList m_TriggerSpells;            /// �ɹ�ʩ���󣬼�����Ŀ��ʩ�Ŵ����ķ���
    bool m_IsTriggeredSpell;                  /// ��ʾ��ǰ�����Ƿ�����Ϊ����������
    const SpellEntry* m_triggeredByAuraSpell; /// ��¼������ǰ�����ķ���

    void PrepareDataForTriggerSystem(); /// ���÷�����������
    void CastTriggerSpells();           /// ʩ�Ŵ����ķ���

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

    /// ����Ч��Ŀ���б����ݸ��ݷ���Ч��Ŀ��ƥ�����(SpellEntry.EffectImplicitTarget)�ͷ���Ŀ��(SpellCastTargets)������������ܵ�����Ӱ��ķ���Ч��Ŀ��
	typedef std::list<TargetInfo> TargetList;
	typedef std::list<GOTargetInfo> GOTargetList;
	typedef std::list<ItemTargetInfo> ItemTargetList;
	TargetList m_UniqueTargetInfo;     /// Unit���͵ķ���Ŀ���б�
	GOTargetList m_UniqueGOTargetInfo; /// GameObject���͵ķ���Ŀ���б�
	ItemTargetList m_UniqueItemInfo;   /// �������͵ķ���Ŀ���б�

	int32 m_damage;  /// ������ɵ��˺���
	int32 m_healing; /// ������ɵ�������
	SpellAuraHolder* m_spellAuraHolder; /// �����⻷�����ߣ������÷������й⻷Ч��ʱ����(��DoSpellHitOnUnit��ʹ��)

	/// ������Ŀ���б�
	void CleanupTargetList();
	void ClearCastItem();

	/// ���㷨��Ч��Ŀ��
    void FillTargetMap();
	void SetTargetMap(SpellEffectIndex eff, uint32 targetMode, UnitList& targetUnitMap);

	/// ���˲�����Ҫ��ķ���Ŀ��
    bool CheckTarget(Unit* target, SpellEffectIndex eff);
    bool CheckTargetCreatureType(Unit* target) const;
	bool IgnoreItemRequirements() const;

	/// ��¼�ܵ�����Ч��Ӱ�����Ϸ����
	void AddUnitTarget(Unit* pVictim, SpellEffectIndex eff);
	void AddUnitTarget(ObjectGuid unitGuid, SpellEffectIndex eff);
	void AddGOTarget(GameObject* target, SpellEffectIndex eff);
	void AddGOTarget(ObjectGuid goGuid, SpellEffectIndex eff);
	void AddItemTarget(Item* target, SpellEffectIndex eff);

	/// Ӧ�÷���Ч��
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
