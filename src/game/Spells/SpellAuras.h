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
 * �⻷�������ڴ���⻷Ч��ʱʹ��
 * Modifier demo = {
 *   .m_auraname   = SPELL_AURA_MOD_RESISTANCE,
 *   .m_miscvalue  = SPELL_SCHOOL_MASK_NORMAL,
 *   .m_amount     = -27,
 *   .periodictime = 0
 * };
 * ��ʾ�⻷Ч��Ϊ����27��Unit������������⻷Ӧ��ʱ������ӦЧ�������ڹ⻷�Ƴ�ʱ�ָ�27��Unit���������
 */
struct Modifier
{
    AuraType m_auraname; // �⻷���� enum AuraType
    int32 m_miscvalue;   // Ч�����ͣ��京��ȡ����AuraType
    int32 m_amount;      // Ч��ֵ
    uint32 periodictime; // �����⻷��Ч������Ƶ�ʣ������ж�������ʱ���ڶ��ڼ�Ѫ���������Ϊ0(�������Թ⻷)����ֻ����һ�Σ����ڹ⻷�Ƴ�ʱɾ����ӦЧ��
};

class Unit;
class WorldObject;
class Player;
class Item;
class Aura;
struct ProcTriggerSpell;

/**
 * ����һ�����������й⻷Ч����ÿ��������������������Ч�������󲿷ַ���Ч�������ڹ⻷��ʵ�֣�����ÿ����������������⻷Ч����
 * ��Ҳ�ǹ⻷����������ܳ��еĹ⻷Ч�����������¼ʩ������Ϣ��������Ϣ��ʩ��Ŀ����Ϣ���Լ��Ƿ��ǵ��ߴ����ķ����ȣ�ͬʱ��������������(DOT�෨��)��
 * Ӧ�ò�ͬ�⻷Ч����Modifier(�޸���)�ȡ�
 */
class MANGOS_DLL_SPEC SpellAuraHolder {
public:
    Aura* m_auras[MAX_EFFECT_INDEX];

public:
    SpellAuraHolder(const SpellEntry*, Unit* target, WorldObject* caster, Item* castItem);
    ~SpellAuraHolder();

    void Update(uint32 diff);                               // ����
    void RefreshHolder();                                   // ˢ��
    void Add();                                             // ���
    void Remove();                                          // ɾ��
    void AddAura(Aura* aura, SpellEffectIndex index);       // ��ӹ⻷
    void RemoveAura(SpellEffectIndex index);                // ɾ���⻷
    void ApplyAuraModifiers(bool apply, bool real = false); // Ӧ�ù⻷Ч��
    void CleanupTriggeredSpells();                          // ����������

    void SendAuraUpdate(bool remove) const;         // ���͹⻷������Ϣ

    void SetDeleted();                       // ����ɾ�����
    void SetInUse(bool state);               // �������ü���
    void SetLoadedState(const ObjectGuid& casterGUID, const ObjectGuid& itemGUID, uint32 charges, int32 maxduration, int32 duration); // ���ü���״̬
    void SetRemoveMode(AuraRemoveMode mode); // ����ɾ��ԭ��
    void SetVisibleAura(bool remove);        // ���ÿɼ��⻷��Ŀ�����Ĺ⻷�ۻ����Ŀ�����Ĺ⻷�����Ƴ�

    bool IsInUse() const;              // �Ƿ����ڱ�ʹ��
    bool IsDeleted() const;            // �Ƿ��Ѿ���ɾ��
    bool IsEmptyHolder() const;        // �Ƿ��ٳ����κι⻷Ч��

    /*********************************************************/
    /***                  GENERAL FEATURES                 ***/
    /*********************************************************/

    uint32 GetId() const;                                     // �������
    const SpellEntry* GetSpellProto() const;                  // ����ģ��
    Aura* GetAuraByEffectIndex(SpellEffectIndex index) const; // ��ȡָ����ŵĹ⻷

    const ObjectGuid& GetCasterGuid() const;   // ��ȡʩ���߱��
    void SetCasterGuid(ObjectGuid guid);
    const ObjectGuid& GetCastItemGuid() const; // ���������ĵ��߱��
    Unit* GetCaster() const;                   // ʩ���߶���
    Unit* GetTarget() const;                   // ʩ��Ŀ�����
    void SetTarget(Unit* target);

    time_t GetAuraApplyTime() const;           // ʩ��ʱ��
    int32 GetAuraMaxDuration() const;          // ����ʱ��
    void SetAuraMaxDuration(int32 duration);
    int32 GetAuraDuration() const;             // ʣ��ʱ��
    void SetAuraDuration(int32 duration);

    uint8 GetAuraSlot() const;                               // �⻷�۱��(�ͻ������Ͻǹ⻷ͼ��)
    void SetAuraSlot(uint8 slot);
    uint32 GetAuraCharges() const;                           // ��ȡЧ����������
    void SetAuraCharges(uint32 charges, bool update = true); // ����Ч���������������編��ħ�����ϵ�Ч����15��ֻ�ܵ���һ�θ��淨��������Ч������������Ϊ1
    bool DropAuraCharge();                                   // ���һ��Ч������

    bool HasMechanic(uint32 mechanic) const;         // �Ƿ����ض���������
    bool HasMechanicMask(uint32 mechanicMask) const; // �Ƿ�����ض���������

    bool IsAreaAura() const;           // �����͹⻷(������Ϣ)
    bool IsDeathPersistent() const;    // ������Ҳ������ʧ�Ĺ⻷(�ٻ�)
    bool IsPermanent() const;          // �����͹⻷
    void SetPermanent(bool permanent);
    bool IsPassive() const;            // �����͹⻷
    bool IsPersistent() const;         // �־��͹⻷
    bool IsPositive() const;           // �����͹⻷
    bool IsNeedVisibleSlot(const Unit *caster) const; // �Ƿ�Ҫ�ڿͻ��˵Ĺ⻷����ʾͼ��

protected:
    void UpdateHolder(uint32 diff);  // ���¹⻷������

private:
    const SpellEntry *m_spellProto; // ����ģ��

    Unit* m_target;            // ʩ��Ŀ�����
    ObjectGuid m_casterGuid;   // ʩ��������
    ObjectGuid m_castItemGuid; // ���������ĵ��߱�ţ���ֱ�Ӵ洢����ָ�룬��ֹ����ɾ������ָ������

    uint8 m_auraSlot;     // �⻷��(���ڿͻ�����ʾ)
    uint32 m_procCharges; // ��������
    int32 m_maxDuration;  // ����ʱ��
    int32 m_duration;     // ʣ��ʱ��
    time_t m_applyTime;   // ʩ��ʱ��

    AuraRemoveMode m_removeMode; // ɾ��ԭ��

    bool m_isDeathPersist;       // �Ƿ�������Ҳ������ʧ
    bool m_isPermanent;          // �Ƿ�������Ч��
    bool m_isPassive;            // �Ƿ��Ǳ���Ч��
    bool m_deleted;              // �Ƿ�ɾ��
    uint32 m_in_use;             // ���ü�����,��SpellAuraHolder::ApplyModifiers��SpellAuraHolder::Update�ȴ���ʱ����Ҫ�������ü�������ֹ��ɾ��
};

typedef void(Aura::*pAuraHandler)(bool Apply, bool Real);

class MANGOS_DLL_SPEC Aura
{
protected:
    friend Aura* CreateAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32* currentBasePoints, SpellAuraHolder* holder, Unit* target, Unit* caster, Item* castItem);

public:
    /*********************************************************/
    /***                   �⻷Ч��������                   ***/
    /*********************************************************/

    void HandleNULL(bool, bool);              // û��ʵ��
    void HandleUnused(bool, bool);            // �����κη���ʹ��
    void HandleNoImmediateEffect(bool, bool); // �⻷��add/remove��ʱ��û��������Ч��Ч��

public:
    virtual ~Aura();

    void Update(uint32 diff); // ����

    void SetModifier(AuraType type, int32 amount, uint32 periodictime, int32 miscValue); // ���ù⻷����
    Modifier* GetModifier();                                                             // ��ȡ�⻷����
    const Modifier* GetModifier() const;
    int32 GetMiscValue() const;                                                          // ��ȡ�⻷Ч������
    void ApplyModifier(bool apply, bool Real = false);                                   // Ӧ�ù⻷Ч��

    void SetInUse(bool state);                              // �������ü���
    void SetRemoveMode(AuraRemoveMode mode);                // ���ù⻷�Ƴ�ԭ��
    void SetLoadedState(int32 damage, uint32 periodicTime); // ���ù⻷����ʱ״̬

    uint32 GetAuraTicks() const;      // ʣ��δ���
    uint32 GetAuraMaxTicks() const;   // �����δ���

    SpellEffectIndex GetEffIndex() const;     // ����Ч�����
    SpellAuraHolder* GetHolder();             // ��ȡ�⻷������
    const SpellAuraHolder* GetHolder() const;
    virtual Unit* GetTriggerTarget() const;   // ��ȡ�����ķ�����Ŀ��

	bool IsAreaAura() const;   // �⻷Ӱ�췶Χ�ǲ�������
    bool IsInUse() const;      // �⻷�Ƿ����ڱ�ʹ��
    bool IsLastAuraOnHolder(); // �Ƿ��ǹ⻷Ч�������������е����һ���⻷Ч��
	bool IsPeriodic() const;   // �⻷�ڳ���ʱ�����Ƿ�ᶨ�ڴ���Ч��
	bool IsPersistent() const; // �⻷��˲��Ļ��ǳ־õ�
	bool IsPositive() const;   // �⻷����Ļ��Ǹ����

    bool IsAffectedOnSpell(const SpellEntry *proto) const;
    bool CanProcFrom(const SpellEntry *proto, uint32 procFlag, uint32 EventProcEx, uint32 procEx, bool active, bool useClassMask) const;

protected:
    Aura(const SpellEntry *proto, SpellEffectIndex eff, SpellAuraHolder* holder, Unit* target, Unit* caster = NULL, Item* castItem = NULL);

    virtual void UpdateAura(uint32 diff);

    void PeriodicTick();          // �����͹⻷Ч������
    void PeriodicDummyTick();
    void TriggerSpell();          // ��������

protected:
    SpellAuraHolder*  m_spellAuraHolder; // �⻷������

    Modifier m_modifier;   // �⻷����

    time_t m_applyTime;    // �⻷��Чʱ��
    int32 m_periodicTimer; // ����Ч���⻷��ʱ��
    uint32 m_periodicTick; // ����Ч���⻷�δ���

    AuraRemoveMode m_removeMode; // ��¼�⻷�Ƴ�ԭ��

    SpellEffectIndex m_effIndex; // ����Ч�����

    bool m_isPositive;   // �����͹⻷
    bool m_isPeriodic;   // �����͹⻷
    bool m_isAreaAura;   // �����͹⻷
    bool m_isPersistent; // �־��͹⻷

    uint32 m_in_use; // ���ü�����(��Aura::ApplyModifier��Aura::Update������ʹ��)
};

Aura* CreateAura(const SpellEntry *proto, SpellEffectIndex eff, SpellAuraHolder* holder, Unit* target, Unit* caster = nullptr, Item* castItem = nullptr);
SpellAuraHolder* CreateSpellAuraHolder(const SpellEntry *proto, Unit* target, WorldObject* caster, Item* castItem = nullptr);

#endif //MANGOS_SPELLAURAS_H
