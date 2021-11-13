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

	void KillPlayer();                                               // ɱ�����

	void SendDirectMessage(WorldPacket* data);                       // ������Ϣ

	PcInventory* GetBag();                                           // ��ȡ��ұ���
	WorldSession* GetSession() const;                                // ��ȡ��ͻ��˵ĻỰ����

	bool IsGameMaster() const;                                       // �ж�����Ƿ�Ϊ����Ա
	bool IsFriendlyTo(const Unit* unit) const;                       // ����Ϸ�����Ƿ��Ѻ�
	bool IsHostileTo(const Unit* unit) const;                        // ����Ϸ�����Ƿ����

	/**
	 * ������ɫ����
	 */
	int32 GetMaxWeight() const;                                      // �����ɫ�������

protected:
	/*********************************************************/
	/***                    STAT SYSTEM                    ***/
	/*********************************************************/
	// ��������
	int8 m_baseStat[MAX_STATS];
	int8 m_createStat[MAX_STATS];

	// ������ս/Զ���˺�
	int8 m_baseAttackDamage[MAX_ATTACK];
	// ��ʼ��ս/Զ���˺�
	int8 m_createAttackDamage[MAX_ATTACK];

	// ������ս/Զ������
	int8 m_baseAttackHit[MAX_ATTACK];
	// ��ʼ��ս/Զ������
	int8 m_createAttackHit[MAX_ATTACK];

	// ��ʼ�����˺�/����/����
	int8 m_createMagicHit;
	int8 m_createMagicDamage;
	int8 m_createMagicCritical;

	// ��������
	int8 m_baseArmor;
	// ��ʼ����
	int8 m_createArmor;

	// ����ħ��
	int16 m_baseMaxMana;
	// ����Ѫ��
	int16 m_baseMaxHealth;

	// ��ʼѪ���ɳ�
	int8 m_createHealthGrowth;
	// ��ʼħ���ɳ�
	int8 m_createManaGrowth;

	// ħ���ظ�
	int32 m_totalManaRegen;
	int8 m_manaRegen;
	// ��ʼħ���ظ�
	int8 m_createManaRegen;

	// Ѫ���ظ�
	int32 m_totalHealthRegen;
	int8 m_healthRegen;
	// ��ʼѪ���ظ�
	int8 m_createHealthRegen;

	// ����ħ������
	int16 m_baseDefense;
	// ��ʼħ������
	int16 m_createDefense;

	// ��������ǿ��
	int16 m_baseSp;

	// ����������
	int16 m_baseDodge;
	// ��ʼ������
	int16 m_createDodge;

	// ��ʼ���ؼ���(����������)
	int16 m_createStrengthWeightReduction;
	int16 m_createStaminaWeightReduction;

	// ��ʼħ�����ļ���
	int8 m_createMagicCostReduction;

	// �˺�����
	int8 m_damageReduction;

	// ���ؼ���
	int8 m_weightReduction;

public:
	/// ˢ�½�ɫ����״̬�������ݿ���ء��û���������ʹ�û�������ʱ����
	void Refresh();

	/// ��ɫ����������ظ������㣬�����ݿ���ػ�ȼ������ı�ʱ����
	void UpdateLevel();

	/// ���õ�ǰѪ��ħ������ֵ���������ݿ����ʱdirect == true
	void SetMana(int16 value, bool direct = false);
	void SetHealth(int16 value, bool direct = false);

	/// ��ɫ���������ݡ����ʡ����������������������Լ���
	/**
	 * ��ȡ��ɫ��ʼ����ֵ
	 *
	 * @param stat ��������
	 * @return ��ʼ����ֵ
	 */
	int8 GetCreateStat(Stats stat);
	/**
	 * ��ȡ��ɫ��������ֵ
	 *
	 * @param stat ��������
	 * @return ��������ֵ
	 */
	int8 GetBaseStat(Stats stat);
	/**
	 * ���½�ɫ��������ֵ��ͬʱ������ɫ������ֵ����������ֵ(characters���str���ֶ�)�ɽ�ɫ����ʱ�ĳ�ʼ����(characters���OriginalStr���ֶ�)����ʹ������ҩˮ�Ľ�����
	 * �Լ�50���Ժ��������������
	 *
	 * @param stat ��������
	 * @param diff ���Ա䶯ֵ
	 */
	void ModifyBaseStat(Stats stat, int32 diff);

	/// ��ս/Զ�̵�(����/��ʼ)�˺��ӳ� ��ս/Զ�̵�(����/��ʼ)���мӳ� ħ����ʼ�˺��ӳ� ħ����ʼ���мӳ� ħ����ʼ�����ӳɼ���
	/**
	 * ����ȼ����������мӳɣ�������ɫ�ȼ������仯ʱ����
	 */
	void UpdateBaseAttackHit();
	void UpdateBaseAttackDamage();
	/**
	 * �����ʼ�������������Դ��������мӳɣ�������ɫ�������Ե���(��������)���ɫ����ʱ����
	 */
	void UpdateCreateAttackHit();
	void UpdateCreateAttackDamage(); // �������ﴴ��ʱ�ĳ�ʼ�����������������
	void UpdateCreateMagicHit();      // �������ﴴ��ʱ�ĳ�ʼ�����������
	void UpdateCreateMagicDamage();   // �������ﴴ��ʱ�ĳ�ʼ�����������
	void UpdateCreateMagicCritical();     // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// (����/��ʼ)�����ӳ�
	/**
	 * ����ȼ������ķ����ӳɣ�������ɫ�ȼ������仯ʱ���ã��ҽ�������������Ժ͵ȼ���أ�
	 * ����װ���ͷ�����������������Ӱ��
	 */
	void UpdateBaseArmor();
	/**
	 * �����ʼ�������Դ����ķ����ӳɣ�������ɫ�������Ե���(��������)���ɫ����ʱ����
	 */
	void UpdateCreateArmor();

	/// ��ɫ����Ѫ�����޺ͻ���ħ�����޼���
	/**
	 * ���½�ɫ����Ѫ�����ޣ�ͬʱ������ɫѪ��������ֵ����������ֵ(characters���MaxHp���ֶ�)�ɽ�ɫÿ������ʱ������random(��ʼѪ���ɳ�)�ۼƹ��ɣ�
	 * ���ﴴ�������ó�ʼ��ʱ�Լ���ɫ����/����ʱ�ᴥ���˺����ĵ���
	 *
	 * @param diff Ѫ�����ޱ仯ֵ
	 */
	void ModifyBaseMaxMana(int32 diff);
	void ModifyBaseMaxHealth(int32 diff);

	/// ��ɫ��ʼѪ����ħ���ɳ�����
	void UpdateCreateManaGrowth();   // �������ﴴ��ʱ�ĳ�ʼ�����������
	void UpdateCreateHealthGrowth(); // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// ��ɫѪ����ħ���ظ�����
	int8 GetManaRegen() const;
	void ModifyManaRegen(int32 diff);

	int8 GetHealthRegen() const;
	void ModifyHealthRegen(int32 diff);

	/// ��ɫ��ʼѪ���ͳ�ʼħ���ظ�����
	void UpdateCreateManaRegen();   // �������ﴴ��ʱ�ĳ�ʼ�����������
	void UpdateCreateHealthRegen(); // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// ��ɫ(����/��ʼ)ħ�������ͻ���ħ��ǿ�ȼ���
	int16 GetBaseDefense() const;
	int16 GetBaseSpellPower() const;
	/**
	 * ����ȼ��͵�ǰ��������ֵ�����ķ����ӳɣ�����ɫ�ȼ���ǰ�������Է����仯ʱ���ã���װ���ͷ���Ӱ��
	 */
	void UpdateBaseDefense();
	void UpdateCreateDefense();  // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// ��ɫ(����/��ʼ)���ܼ���
	/**
	 * ����ȼ��͵�ǰ��������ֵ�����ķ����ӳɣ�����ɫ�ȼ���ǰ�������Է����仯ʱ���ã���װ���ͷ���Ӱ��
	 */
	void UpdateBaseDodge();
	void UpdateCreateDodge(); // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// ��ɫ��ʼ���ؼ��⡢ħ�����ļ������
	void UpdateCreateWeightReduction();    // �������ﴴ��ʱ�ĳ�ʼ�����������������
	void UpdateCreateMagicCostReduction(); // �������ﴴ��ʱ�ĳ�ʼ�����������

	/// ��ɫ�˺��������
	int8 GetDamageReduction() const;
	void ModifyDamageReduction(int8 diff);

	/// ��ɫ���ؼ������
	int8 GetWeightReduction() const;
	void ModifyWeightReduction(int8 diff);

protected:
	/*********************************************************/
	/***                    GRID SYSTEM                    ***/
	/*********************************************************/
	Camera m_camera;                             // ����ӽ�(Ĭ��������)
	GridReference<Player> m_gridRef;             // ���ڽ�������ص������ϵ�˫������ڵ�
	MapReference m_mapRef;                       // ���ڽ�������ص���ͼ�ϵ�˫������ڵ�

public:
	GuidSet m_clientGUIDs;                       // ��ҿͻ��˽��浱ǰ�ɼ������б�

public:
	Camera& GetCamera();                                             // ����ӽ�
	GridReference<Player>& GetGridRef();                             // ��ȡ���ڽ�������ص������ϵ�˫������ڵ�
	MapReference& GetMapRef();                                       // ��ȡ���ڽ�������ص���ͼ�ϵ�˫������ڵ�

	void UpdateVisibilityOf(WorldObject const* viewPoint, WorldObject* target); // ͨ��ָ���ӽ����ж��Ƿ��ܿ���Ŀ����Ϸ���󣬸����жϽ�������������ɼ������б�
	void RelocateToHomebind(uint32 mapId, CoordUnit x, CoordUnit y);            // ����Ҵ��ͻػسǵ�
	bool HaveAtClient(const WorldObject* u);                                    // �ж���ҿͻ��˽����Ƿ��ܿ�����Ϸ����

protected:
	/*********************************************************/
	/***                     DB SYSTEM                     ***/
	/*********************************************************/
	bool LoadFromDB(SqlQueryHolder* holder);
	void SaveToDB();

	void LoadSpells(QueryResult* result);

protected:
	WorldSession *m_session; // ��ͻ��˵ĻỰ����
	PcInventory *m_bag;      // ��ұ���
};

#endif
