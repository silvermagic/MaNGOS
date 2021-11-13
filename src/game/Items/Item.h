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

#ifndef MANGOSSERVER_ITEM_H
#define MANGOSSERVER_ITEM_H

#include "Entities/Object.h"
#include "Common.h"
#include "SQLStructure.h"

struct SpellEntry;
class Inventory;

struct ItemSetEffect
{
	uint32 setid;
	uint32 item_count;
	SpellEntry const* spells[8];
};

struct ItemAuraEffect
{
	ItemAuraEffectType types[MAX_ITEM_AURA_EFFECT];
	int8 amounts[MAX_ITEM_AURA_EFFECT];
	uint32 duration;
};

class MANGOS_DLL_SPEC Item : public WorldObject
{
	friend class Inventory;
public:
	Item();

	/**
	 * ʹ�õ���ģ���ʼ������ʵ�������ǲ��Ե���ʵ�������������ʼ�������������õ�����ʵ�������
	 * ����ʵ��������н�һ����ʼ����������������ħ�ȵ�ʵ��ʹ�ô���
	 *
	 * @param item   ����ģ��
	 * @param count  ��������
	 * @return ����ʵ��
	 */
	static Item* CreateItem(uint32 item, uint32 count);
	Item* CloneItem(uint32 count) const;                // ��¡��ǰ����
	/**
	 * ���߳�ʼ������������GUID������ģ�壬�������������������ʼ������������ħ�ȵ�ʵ��ʹ�ô������ڴ˴����ã�
	 * ֻ����ʵ�ʴ��뱳�����ֿ߲��ʱ�������ã������ݿ����ʱ���ã����߱�����ĺ����������ڳ�ʼ���µĵ���ʵ��
	 *
	 * @param guidlow ����GUID
	 * @param itemid  ���߱��
	 * @return true��ʾ�����ɹ�
	 */
	bool Create(uint32 guidlow, uint32 itemid);

	/**
	 * ��ȡ����״̬�Ķ���������,���ڷ��͵��߷�����ͻ���
	 */
	void GetStatusBytes(ByteBuffer &buffer) const;

	/**
	 * ��������
	 */

	ObjectGuid const& GetOwnerGuid() const;
	void SetOwnerGuid(ObjectGuid guid);
	Player* GetOwner() const;

	uint32 GetItemId() const;                                  // ��ȡ����ģ����
	const ItemEntry* GetProto() const;                     // ��ȡ����ģ��

	std::string GetName() const;                               // ��ȡ��������
	std::string GetName(bool view) const;                      // ��ȡ������ʾ���ƣ�viewΪtrue��ʾ�����ڲֿ���ʾ��false��ʾ������־��¼
	std::string GetNumberedName(bool view, int32 count) const; // ��ȡ�������ĵ�����ʾ����

	uint32 GetWeight() const;                                  // ��ȡ��������

	uint8 GetSlot() const;                                     // ��ȡ����װ��λ��
	void SetSlot(uint8 slot);                                  // ���õ���װ��λ��

	/**
	 * ��������(�����ݿ�����ֶζ�Ӧ)
	 */

	int32 GetCount() const;                                    // ��ȡ������Ŀ
	void SetCount(int32 number);                               // ���õ�����Ŀ

	uint32 GetDurability() const;                              // ��ȡ�����;ö�
	void SetDurability(int32 durability);                      // ���õ����;ö�

	uint32 GetDuration() const;                                // ��ȡ����ʣ��ʹ��ʱ��
	void SetDuration(uint32 duration);                         // ���õ���ʣ��ʹ��ʱ��

	int32 GetChargeCount() const;                              // ��ȡ����ʣ��ʹ�ô���
	void SetChargeCount(int32 number);                         // ���õ���ʣ��ʹ�ô���

	int32 GetEnchantLevel() const;                             // ��ȡ����ǿ���ȼ�
	void SetEnchantLevel(int32 lvl);                           // ���õ���ǿ���ȼ�

	uint8 GetBless() const;                                    // ��ȡ����ף��״̬ 0-��ͨ 1-ף�� 2-����
	void SetBless(uint8 bless);                                // ���õ���ף��״̬

	ItemAttrEnchantType GetAttrEnchantKind() const;            // ��ȡ��������ǿ������
	int32 GetAttrEnchantLevel() const;                         // ��ȡ��������ǿ���ȼ�
	void SetAttrEnchantKind(ItemAttrEnchantType type);         // ���õ�������ǿ������
	void SetAttrEnchantLevel(int32 lvl);                       // ���õ�������ǿ���ȼ�

	int32 GetDefense(SpellSchools school) const;               // ��ȡ����ħ�������ӳ�
	void SetDefense(SpellSchools school, int32 value);         // ���õ���ħ�������ӳ�

	int32 GetSpellPower() const;                               // ��ȡ���߷���ǿ�ȼӳ�
	void SetSpellPower(int32 value);                           // ���õ��߷���ǿ�ȼӳ�

	int32 GetMaxMana() const;                                  // ��ȡ����ħ�����޼ӳ�
	void SetMaxMana(int32 value);                              // ���õ���ħ�����޼ӳ�

	int32 GetManaRegen() const;                                // ��ȡ����ħ���ظ��ӳ�
	void SetManaRegen(int32 value);                            // ���õ���ħ���ظ��ӳ�

	int32 GetMaxHealth() const;                                // ��ȡ����Ѫ�����޼ӳ�
	void SetMaxHealth(int32 value);                            // ���õ���Ѫ�����޼ӳ�

	int32 GetHealthegen() const;                               // ��ȡ���������ظ��ӳ�
	void SetHealthRegen(int32 value);                          // ���õ��������ظ��ӳ�

	int32 GetStat(Stats stat);                                 // ��ȡ�������Զ���ӳ�
	void SetStat(Stats stat, int32 value);                     // ���õ������Զ���ӳ�

	void SetEquipped(bool equipped);                           // ���õ���װ��״̬
	void SetIdentified(bool identified);                       // ���õ��߼���״̬

	bool IsEquipped() const;                                   // �����Ƿ��ѱ�װ��
	bool IsIdentified() const;                                 // �����Ƿ��Ѿ�����
	bool IsStackable() const;                                  // �����Ƿ�����ѵ�

	/**
	 * ����ǿ��
	 */
	void SetAuraEffect(const ItemAuraEffect& effect);  // ������ʩ�ӷ���Ч��
	void SetAuraEffectDuration(uint32 duration);       // ���µ��߷���Ч��ʱ��
	void ClearAuraEffect();                            // ��ɢ���ߵķ���Ч��
	int8 GetAuraEffect(ItemAuraEffectType type) const; // ��ȡ���߷���Ч��

	// ȱʡʵ��
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override;

protected:
	uint8 m_slot;                 // ����װ����λ��
	const ItemEntry* m_proto; // ����ģ��
	uint32 mapId;                 // ��ǰ�������ڵ�ͼ
	Position m_position;          // ��ǰ��������
	ItemAuraEffect m_effect;      // ʩ���ڵ����ϵķ���Ч��
};

#endif
