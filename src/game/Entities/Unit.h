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
	void CleanupsBeforeDelete() override;      // ��������Ϸ�����в���������
	void OnRelocated();                        // �ƶ���Ļص�����

	Unit* GetOwner() const;                    // ��ȡ����ӵ����
	const ObjectGuid& GetCreatorGuid() const;  // ��ȡ����Ĵ�����
	const ObjectGuid& GetOwnerGuid() const;    // ��ȡ������ٻ���
	const ObjectGuid& GetPetGuid() const;      // ��ȡ�ٻ��ĳ���
	const ObjectGuid& GetTargetGuid() const;   // ��ȡĿ��

	void SetCreatorGuid(ObjectGuid creator);   // ���ö���Ĵ�����
	void SetOwnerGuid(ObjectGuid owner);       // ���ö�����ٻ���
	void SetPetGuid(ObjectGuid pet);           // �����ٻ��ĳ���
	void SetTargetGuid(ObjectGuid targetGuid); // ����Ŀ��

protected:
	/*********************************************************/
	/***                     SPELL SYSTEM                  ***/
	/*********************************************************/
    SpellAuraHolderMap m_spellAuraHolders;                         // �⻷�������б����浱ǰ��Ч�Ĺ⻷��ͬһ�������п��ܲ�������⻷������
    SpellAuraHolderMap::iterator m_spellAuraHoldersUpdateIterator; // ���������ڱ��������¹⻷�������б�ʱ�����ɾ��������Ϊ�˲�Ӱ�������������Ҫ���浱ǰ����������ָ��
    AuraList m_deletedAuras;                                       // ��ɾ���⻷�б�һ�㷨��Ч��ɾ����Ҫ�Ƚ��⻷�ӹ⻷��������������ɾ���⻷�б����ͳһɾ��������
    SpellAuraHolderList m_deletedHolders;                          // ��ɾ���⻷�������б����⻷�����ߵ����й⻷����������ɾ���⻷�б�󣬽��⻷������������ɾ���⻷�������б����ͳһɾ�������������ط�����������
    AuraList m_modifyAuras[TOTAL_AURAS];                           // ��¼��һ�⻷Ч������Դ�б�����װ�����߻���������ӳɣ�ʹ�÷����෨��Ҳ����������ӳɣ����ڷ����ӳ���һ�⻷Ч������Ҫ���б���¼������ͬʱ����ʵ�ֲ��ɵ��ӵĹ⻷Ч��(����б��Ƿ��Ѿ�����������Դ)
    VisibleAuraMap m_visibleAuras;                                 // �ɼ��⻷�б�������Щ�ڿͻ������Ͻ���ͼ����ʾ�Ĺ⻷
    uint32 m_cooldown;                                             // ������ȴʱ��

public:
	/**
	 * ����Unit���ϵļ���Ч��
	 *
	 * @param time �����ϴθ��£��Ѿ�������ʱ��(��λ����)
	 */
    void UpdateSpells(uint32 time);

    /**
     * �����ӳ�ɾ���Ĵ��⻷�������б�ʹ�ɾ���⻷�б����ֱ��ɾ������Ч��ɾ���Ѿ���RemoveAura���洦����
     */
    void CleanupDeletedAuras();

    /**
     * ���ݷ�����Ϣ����Spell���󣬲����÷���Ŀ����Ϣ��������Spell::Prepare���ʩ������
     *
     * @param Victim          ��ǰ�������е�Ŀ��
     * @param proto           ��ǰ������Ϣ
     * @param triggered       ��ǰ�������ⲿ���������ģ���������ʩ�ŵģ�true��ʾ�ⲿ����
     * @param castItem        ʩ�ŵ�ǰ�����ĵ���(��Ϊnullptr)
     * @param triggeredByAura ������ǰ�����Ĺ⻷(��Ϊnullptr)
     * @param originalCaster  �����ǽ֮��ķ�������Ҫ�ṩ������ԭʼʩ���ߣ���ͨ������¾���Unit����
     * @param triggeredBy     ������ǰ�����ķ�����Ϣ(��Ϊnullptr)
     */
    void CastSpell(Unit* Victim, const SpellEntry* proto, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    void CastSpell(Unit* Victim, uint32 spellId, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    /**
     * ���ݷ�����Ϣ����Spell���󣬲����÷���Ŀ����Ϣ��������Spell::Prepare���ʩ������
     *
     * @param x               ��ǰ����Ŀ���X������(�Կյ�ʩ�ŷ���)
     * @param y               ��ǰ����Ŀ���Y������(�Կյ�ʩ�ŷ���)
     * @param proto           ��ǰ������Ϣ
     * @param triggered       ��ǰ�������ⲿ���������ģ���������ʩ�ŵģ�true��ʾ�ⲿ����
     * @param castItem        ʩ�ŵ�ǰ�����ĵ���(��Ϊnullptr)
     * @param triggeredByAura ������ǰ�����Ĺ⻷(��Ϊnullptr)
     * @param originalCaster  �����ǽ֮��ķ�������Ҫ�ṩ������ԭʼʩ���ߣ���ͨ������¾���Unit����
     * @param triggeredBy     ������ǰ�����ķ�����Ϣ(��Ϊnullptr)
     */
    void CastSpell(CoordUnit x, CoordUnit y, const SpellEntry* proto, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);
    void CastSpell(CoordUnit x, CoordUnit y, uint32 spellId, bool triggered, Item* castItem = nullptr, Aura* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid(), const SpellEntry* triggeredBy = nullptr);

    /**
     * ���㷨�����н���������Ϊ�ֿ������ߡ����������
     *
     * @param pVictim    �������ߣ�������������
     * @param proto      ������Ϣ
     * @param canReflect �����Ƿ������䣬true��ʾ����
     * @return �������н��
     */
    SpellMissInfo SpellHitResult(Unit* pVictim, const SpellEntry *proto, bool canReflect = false);
    SpellMissInfo MeleeSpellHitResult(Unit* pVictim, const SpellEntry *proto);
    SpellMissInfo MagicSpellHitResult(Unit* pVictim, const SpellEntry *proto);

    /**
     * ���/ɾ���⻷
     *
     * �⻷�ǳ����ͷ���Ч��(����buff)�����壬����ɾ���⻷�������Ƴ������ͷ���Ч����һ���������ж������ܲ�����������ͷ���Ч��(�⻷)��
     * ����ʹ�ù⻷�����������浥�η�������Ч��ʵ�������⻷�������ǵ��η��������������г����ͷ���Ч��(�⻷)�����壬ɾ���⻷�����߾����Ƴ��������еĳ����ͷ���Ч����
     * ����һϵ��Remove�������ö���ɾ���⻷/�⻷�����ߣ���Ҫ�������ڴ�ɾ���Ĺ⻷/�⻷�����߻�ȡ��ʽ��ͬ(��Ϊֱ��ָ���͸���ָ����������)���Լ�ɾ����ԭ��ͬ��
     * ��Ҫע����ǣ���ͬ�������к��²����ķ���Ч�����ܻḲ�Ǿɵķ���Ч����Ҳ���ܻ�ͬʱ���ڣ�����DoT�෨���������ܴ���һ��������Ŷ�Ӧ����⻷�����ߵ������
     * ���Ը��ݷ����������ȡ��ɾ���Ĺ⻷/�⻷������ʱ�Ĵ�����Ҫ������ϸ��
     */

    /**
     * ��ӹ⻷
     *
     * @param aura �⻷����
     */
    void AddAuraToModifyList(Aura* aura);
    /**
     * ��ӹ⻷������
     *
     * @param holder �⻷�����߶���
     * @return
     */
    bool AddSpellAuraHolder(SpellAuraHolder* holder);
    /**
     * todo: ��������?
     *
     * @param spellId
     * @param delaytime
     * @param casterGuid
     */
	void DelaySpellAuraHolder(uint32 spellId, int32 delaytime, ObjectGuid casterGuid);
    /**
     * �Ƴ��⻷�����Ƴ��Ĺ⻷�ɲ���ֱ���ṩ
     *
     * @param aura ���Ƴ��Ĺ⻷����
     * @param mode �Ƴ�ԭ��
     */
    void RemoveAura(Aura* aura, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �Ƴ��⻷�����Ƴ��Ĺ⻷��Ҫ���ݲ�������
     *
     * @param spellId �������
     * @param eff     ����Ч�����
     * @param except  �����Ϊ�գ�������ɾ��ָ���Ĺ⻷
     */
    void RemoveAura(uint32 spellId, SpellEffectIndex eff, Aura* except = nullptr);
    /**
     * �Ƴ��⻷�����Ƴ��Ĺ⻷�ɲ���ֱ���ṩ������ǹ⻷������ӵ�е����һ���⻷����ô�����Ƴ��⻷�����ߴ���
     *
     * @param holder �⻷������
     * @param eff    ����Ч�����
     * @param mode   �Ƴ�ԭ��
     */
    void RemoveSingleAuraFromSpellAuraHolder(SpellAuraHolder* holder, SpellEffectIndex eff, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �Ƴ��⻷�����Ƴ��Ĺ⻷��Ҫ���ݲ�������
     *
     * @param spellId    �������
     * @param eff        ����Ч�����
     * @param casterGuid ʩ����GUID
     * @param mode       �Ƴ�ԭ��
     */
    void RemoveSingleAuraFromSpellAuraHolder(uint32 spellId, SpellEffectIndex eff, ObjectGuid casterGuid, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷�������ɲ���ֱ���ṩ
     *
     * @param holder �⻷������
     * @param mode   �Ƴ�ԭ��
     */
    void RemoveSpellAuraHolder(SpellAuraHolder* holder, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�ָ��������������ط���Ч��
     *
     * @param spellId �������
     * @param except  �����Ϊ�գ�������ɾ��ָ���Ĺ⻷������
     * @param mode    �Ƴ�ԭ��
     */
    void RemoveAurasDueToSpell(uint32 spellId, SpellAuraHolder* except = nullptr, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�ָ���������߲�������ط���Ч��
     *
     * @param castItem �������߱��
     * @param spellId  �������
     */
    void RemoveAurasDueToItemSpell(Item* castItem, uint32 spellId);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�ָ��ʩ���߲�������ط���Ч��
     *
     * @param spellId    �������
     * @param casterGuid ʩ����GUID
     */
    void RemoveAurasByCasterSpell(uint32 spellId, ObjectGuid casterGuid);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�ָ���������Ƶ���ط���Ч��
     *
     * @param mechMask      ��������
     * @param exceptSpellId �����Ϊ�գ�������ɾ��ָ���ķ����Ĺ⻷������
     * @param non_positive  �Ƿ�ֻɾ�����淨��Ч����true��ʾ��ɾ�����淨��Ч��
     */
    void RemoveAurasAtMechanicImmunity(uint32 mechMask, uint32 exceptSpellId, bool non_positive = false);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�ָ���⻷���͵���ط���Ч��
     *
     * @param auraType �⻷����
     */
    void RemoveSpellsCausingAura(AuraType auraType);
    void RemoveSpellsCausingAura(AuraType auraType, SpellAuraHolder* except);
    void RemoveSpellsCausingAura(AuraType auraType, ObjectGuid casterGuid);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��жϹ⻷
     *
     * @param flags �⻷�жϱ�־
     */
    void RemoveAurasWithInterruptFlags(uint32 flags);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң��Ƴ�����ָ���������Ե���ط���Ч��
     *
     * @param flags ��������
     */
    void RemoveAurasWithAttribute(uint32 flags);
    /**
     * �Ƴ��⻷�����ߣ����Ƴ��Ĺ⻷��������Ҫ���ݲ������ң���ɢ�⻷
     *
     * @param type       ��ɢ����
     * @param casterGuid ʩ����GUID
     */
    void RemoveAurasWithDispelType(DispelType type, ObjectGuid casterGuid = ObjectGuid());
    /**
     * �Ƴ����й⻷������
     */
    void RemoveAllAuras(AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    /**
     * �������Ƴ����й⻷������
     */
    void RemoveAllAurasOnDeath();

    /**
     * ������ȴ
     */
    uint32 GetCooldown() const;         // ��ȡ������ȴʣ��ʱ��
	void ResetCooldown(uint32 gcd = 0); // ���ü�����ȴʱ��

    /**
     * ��������
     */

	Aura* GetAura(uint32 spellId, SpellEffectIndex effindex);
	Aura* GetAura(AuraType type, SpellFamily family, ObjectGuid casterGuid = ObjectGuid());
	/**
	 * ��ȡָ�����͵Ĺ⻷�б�
	 *
	 * @param type �⻷����
	 * @return �⻷�б�
	 */
	const AuraList& GetAurasByType(AuraType type) const { return m_modifyAuras[type]; }

    /**
     * ��ȡ���з������в�����Ч��ʵ��
     *
     * @return �⻷�������б�
     */
    SpellAuraHolderMap& GetSpellAuraHolderMap() { return m_spellAuraHolders; }
    const SpellAuraHolderMap& GetSpellAuraHolderMap() const { return m_spellAuraHolders; }

    /**
     * ��ȡָ���������в�����Ч��ʵ��
     *
     * @param spell_id �������
     * @return �⻷�������б�
     */
    SpellAuraHolderBounds GetSpellAuraHolderBounds(uint32 spellId) { return m_spellAuraHolders.equal_range(spellId); }
    SpellAuraHolderConstBounds GetSpellAuraHolderBounds(uint32 spellId) const { return m_spellAuraHolders.equal_range(spellId); }

    /**
     * ��ȡָ���⻷���ڿɼ��⻷�Ĺ⻷�����ߣ��⻷����һ������ĸ���������Ϊ���ڷ���˴���һ����Ӧ�ͻ������Ͻ�ͼ���б�ĸ��
     * ���������Ҫ��ʾ�Ͳ���Ҫ��ʾ�Ĺ⻷�����⴦������ɼ��⻷��ʧ����Ҫ��ͻ��˷��͹⻷Ч����ʧ���ݰ�
     *
     * @param slot �⻷�۱��
     * @return �ɼ��⻷�Ĺ⻷������
     */
    SpellAuraHolder* GetVisibleAura(uint8 slot) const;
    const VisibleAuraMap& GetVisibleAuras() const { return m_visibleAuras; }
    uint8 GetVisibleAurasCount() const { return m_visibleAuras.size(); }
    void SetVisibleAura(uint8 slot, SpellAuraHolder* holder);

    /**
     * �ҵ�ָ��ʩ����ʹ��ָ������������������Ч��ʵ��
     *
     * @param spell_id   �������
     * @param casterGUID ʩ����GUID
     * @return �⻷������
     */
    SpellAuraHolder* GetSpellAuraHolder(uint32 spellId, ObjectGuid casterGUID) const;
    SpellAuraHolder* GetSpellAuraHolder(uint32 spellId) const;


    bool HasAura(AuraType auraType) const;                       // �ж������Ƿ����ָ���⻷
    bool HasAura(uint32 spellId, SpellEffectIndex eff) const;
    bool HasAura(uint32 spellId) const;
	bool HasGlobalCooldown() const;                              // �жϼ����Ƿ�����ȴ��
    virtual bool HasSpell(uint32 spellId) const;                 // �ж��Ƿ��Ѿ�ϰ�÷���
    bool IsBrave() const;                                        // �ж��Ƿ��ڿ�״̬
    bool IsFastMovable() const;                                  // �ж��Ƿ��ڿ����ƶ�״̬
    bool IsFrozen() const;                                       // �ж��Ƿ��ڶ���״̬
    bool IsInvisble() const;                                     // �ж��Ƿ��������Ч��
    bool IsPolymorphed() const;                                  // �ж��Ƿ��ڱ���״̬

	/**
	 * �����˺�����
	 */

	/**
	 * ���㷨�������˺�
	 *
	 * @param target
	 * @param proto
	 * @param eff
	 * @param basePoints
	 * @return
	 */
	int32 CalculateSpellDamage(const Unit* target, const SpellEntry* proto, SpellEffectIndex eff, const int32* basePoints = nullptr);

	/**
	 * �����ս�˺��ӳ�(ʩ���߲���)
	 *
	 * @param pVictim     ��������
	 * @param damage      �Ա���������ɵĳ�ʼ�˺�ֵ
	 * @param att         ��������
	 * @param proto       ������Ϣ
	 * @param damagetype  �˺�����
	 * @param stack       �ѵ�����
	 * @return �������˺�ֵ
	 */
	uint32 MeleeDamageBonusDone(Unit* pVictim, uint32 damage, WeaponAttackType att, const SpellEntry* proto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);
	/**
	 * �����ս�˺��ӳ�(�ܺ��߲���)
	 *
	 * @param pCaster    ʩ����
	 * @param damage     �Ա���������ɵĳ�ʼ�˺�ֵ
	 * @param att        ��������
	 * @param proto      ������Ϣ
	 * @param damagetype �˺�����
	 * @param stack      �ѵ�����
	 * @return �������˺�ֵ
	 */
	uint32 MeleeDamageBonusTaken(Unit* pCaster, uint32 damage, WeaponAttackType att, const SpellEntry* proto = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, uint32 stack = 1);

	/**
	 * �������Ƽӳ�(ʩ���߲���)
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
	//// �������õĻ�ǽ����ħ�����������ȷ�������Ҫ��Ŀ��ص���ʾ��ǽ�����壬���෨����ʵ�ֶ�����Ŀ��ص㴴��һ����ʱ��DynamicObject����
    typedef std::list<GameObject*> GameObjectList;
    typedef std::map<uint32, ObjectGuid> WildGameObjectMap;
    GuidList m_dynObjGUIDs;           // ��¼����Ч��������DynamicObject�����б�
    GameObjectList m_gameObj;         // �ٻ������б�
    WildGameObjectMap m_wildGameObjs; // ���Ƶ�Ұ�������б� ?

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
    // ����
	int32 m_totalStat[MAX_STATS];
	int8 m_stat[MAX_STATS];

    // ��ս/Զ���˺�
    int32 m_totalAttackDamage[MAX_ATTACK];
    int8 m_attackDamage[MAX_ATTACK];

	// ��ս/Զ������
	int32 m_totalAttackHit[MAX_ATTACK];
	int8 m_attackHit[MAX_ATTACK];

	// ����
	int32 m_totalAc;
	int8 m_ac;

	// Ѫ����ħ��
	int16 m_mana;   // ��ǰħ��
	int16 m_health; // ��ǰѪ��
	int32 m_totalMaxMana;
	int16 m_maxMana;
	int32 m_totalMaxHealth;
	int16 m_maxHealth;

	// ħ�����������Է���
	int32 m_totalDefense[MAX_SPELL_SCHOOL];
	int16 m_defense[MAX_SPELL_SCHOOL];

	// ����ǿ��
	int32 m_totalSp;
	int16 m_baseSp;
	int16 m_sp;

	// ������
	int32 m_totalDodge;
	int16 m_dodge;

	// ����
	int32 m_totalResist[MAX_STORED_MECHANICS];
	int8 m_resist[MAX_STORED_MECHANICS];

public:
    /// ��ɫ���������ݡ����ʡ������������������Լ���

	/**
	 * ��ȡ��ɫ������Чֵ
	 *
	 * @param stat ��������
	 * @return ������Чֵ
	 */
	int8 GetStat(Stats stat) const;
    /**
     * ���½�ɫ������ֵ���������ɫ������Чֵ������/������ħ������������ֵ�仯�ᴥ���˺������½�ɫ������ֵ��
     * ��ɫ������ֵ = ��ɫ������������ֵ + װ������ + ħ������
     *
     * @param stat ��������
     * @param diff ���Ա䶯ֵ
     */
    void ModifyStat(Stats stat, int32 diff);

	/// ��ս/Զ���˺��ӳ� ��ս/Զ�����мӳ� ħ���˺��ӳ� ħ�����мӳ� ħ�������ӳɼ���
	int8 GetAttackHit(WeaponAttackType att) const;
	int8 GetAttackDamage(WeaponAttackType att) const;
	void ModifyAttackHit(WeaponAttackType att, int32 diff);
	void ModifyAttackDamage(WeaponAttackType att, int32 diff);

	/// ��������
	int8 GetArmor() const;
	void ModifyArmor(int32 diff);

	/// Ѫ��ħ������ֵ����
	int16 GetMana() const;
	int16 GetMaxMana() const;
	void ModifyMaxMana(int32 diff);

	int16 GetHealth() const;
	int16 GetMaxHealth() const;
	void ModifyMaxHealth(int32 diff);

	int16 GetLawful() const;

	/// ���õ�ǰѪ��ħ������ֵ���������ݿ����ʱdirect == true
	void SetMana(int16 value, bool direct = false);
	void SetHealth(int16 value, bool direct = false);
	void SetLawful(int16 value);

	/// ħ�����������Է���������ǿ�ȼ���
	int16 GetDefense(SpellSchools school) const;
	void ModifyDefense(SpellSchools school, int32 diff);

	/// ��ǿ����
	int16 GetSpellPower() const;
	void ModifySpellPower(int32 diff);
	void UpdateBaseSpellPower(); // ����ȼ��͵�ǰ�����������
	int32 GetSpellLevel() const; // ���ݽ�ɫ�ȼ������㷨��ǿ�ȼӳ�
	int32 GetSpellBonus() const; // �����ɫ������Чֵ�����ķ���ǿ�ȼӳ�

	/// ���ܼ���
	int8 GetDodge() const;
	void ModifyDodge(int32 diff);

	/// ���Լ���
	int8 GetResist(MechanicsType type) const;
	void ModifyResist(MechanicsType type, int32 diff);

	/// ������װ
	int16 GetPower(uint8 power);
	void ModifyPower(uint8 power, int16 value);

	/**
	 * ��ȡָ���⻷Ч������������ϵ�����������羫׼����Ὣ���������㣬��������ϵ������Ӧ����0.0f��
	 * ���䷨��Ч����Modifier.m_amount = -100
	 *
	 * @param auratype �⻷����
	 * @return ����ϵ������
	 */
	float GetTotalAuraMultiplier(AuraType auratype) const;

protected:
    /*********************************************************/
    /***                       ATTRIBUTE                   ***/
    /*********************************************************/
    std::string m_name;       // ����
    std::string m_title;      // �ƺ�
    DeathState m_deathState;  // ����״̬
    uint32 m_state;           // �״̬�����¼����������ɱ༭
    UnitMoveType m_moveSpeed; // һ�μ���(��ɫҩˮ����֮���ߡ���ʥ����)
    bool m_braveSpeed;        // ���μ���(����ҩˮ)

public:
    bool GetBrave() const;            // ��ȡ���μ���״̬
    Classes GetClass() const;         // ְҵ
    uint32 GetClassMask() const;      // ְҵ����
    DeathState GetDeathState() const; // ��ȡ����״̬
	uint8 GetFood() const;            // ��ȡ��ʳ��
    uint8  GetGender() const;         // �Ա�
    uint8  GetLevel() const;          // �ȼ�
    std::string GetName() const;      // ����
    std::string GetTitle() const;     // �ƺ�
    UnitMoveType GetSpeed() const;    // ��ȡһ�μ���״̬

    void AddUnitState(uint32 flag);               // ���״̬��־
    void ClearUnitState(uint32 flag);             // ����״̬��־
    void SetSpeed(UnitMoveType type);             // ����һ�μ���
    void SetBrave(bool enabled = false);          // ���ö��μ���
    virtual void SetDeathState(DeathState state); // ��������״̬
	void SetFood(uint8 value);                    // ���ñ�ʳ��
    virtual void SetRoot(bool enabled);           // ��������״̬

    bool CanFreeMove() const;                                    // �ж��Ƿ����ƶ�
    bool HasUnitState(uint32 flag) const;                        // �ж��Ƿ����״̬��־
    bool IsAlive() const;                                        // �ж��Ƿ���
    bool IsDead() const;                                         // �ж��Ƿ�����
	virtual bool IsFriendlyTo(const Unit* unit) const = 0;       // ����Ϸ�����Ƿ��Ѻ�
	virtual bool IsHostileTo(const Unit* unit) const = 0;        // ����Ϸ�����Ƿ����
    bool IsTargetableForAttack(bool inverseAlive = false) const; // �ж��Ƿ��ܱ�����
    bool IsPassiveToHostile() const;                             // ��������
	bool IsVisibleForOrDetect(const Unit* u, const WorldObject* viewPoint, bool detect, bool inVisibleList = false) const;
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override;

protected:
    EventProcessor m_Events; // ���������¼�
};

#endif
