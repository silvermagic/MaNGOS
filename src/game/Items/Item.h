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
	 * 使用道具模板初始化道具实例，但是不对道具实例属性做随机初始化，调用者在拿到道具实例对象后，
	 * 根据实际情况进行进一步初始化，例如设置闪电魔杖的实际使用次数
	 *
	 * @param item   道具模板
	 * @param count  道具数量
	 * @return 道具实例
	 */
	static Item* CreateItem(uint32 item, uint32 count);
	Item* CloneItem(uint32 count) const;                // 克隆当前道具
	/**
	 * 道具初始化，仅设置其GUID、道具模板，不对其余属性做随机初始化，例如闪电魔杖的实际使用次数不在此处设置，
	 * 只有在实际存入背包或者仓库的时候再设置，从数据库加载时调用，或者被上面的函数调用用于初始化新的道具实例
	 *
	 * @param guidlow 道具GUID
	 * @param itemid  道具编号
	 * @return true表示创建成功
	 */
	bool Create(uint32 guidlow, uint32 itemid);

	/**
	 * 获取道具状态的二进制描述,用于发送道具封包到客户端
	 */
	void GetStatusBytes(ByteBuffer &buffer) const;

	/**
	 * 道具属性
	 */

	ObjectGuid const& GetOwnerGuid() const;
	void SetOwnerGuid(ObjectGuid guid);
	Player* GetOwner() const;

	uint32 GetItemId() const;                                  // 获取道具模板编号
	const ItemEntry* GetProto() const;                     // 获取道具模板

	std::string GetName() const;                               // 获取道具名称
	std::string GetName(bool view) const;                      // 获取道具显示名称，view为true表示是用于仓库显示，false表示用于日志记录
	std::string GetNumberedName(bool view, int32 count) const; // 获取带个数的道具显示名称

	uint32 GetWeight() const;                                  // 获取道具重量

	uint8 GetSlot() const;                                     // 获取道具装备位置
	void SetSlot(uint8 slot);                                  // 设置道具装备位置

	/**
	 * 属性设置(与数据库表中字段对应)
	 */

	int32 GetCount() const;                                    // 获取道具数目
	void SetCount(int32 number);                               // 设置道具数目

	uint32 GetDurability() const;                              // 获取道具耐久度
	void SetDurability(int32 durability);                      // 设置道具耐久度

	uint32 GetDuration() const;                                // 获取道具剩余使用时长
	void SetDuration(uint32 duration);                         // 设置道具剩余使用时长

	int32 GetChargeCount() const;                              // 获取道具剩余使用次数
	void SetChargeCount(int32 number);                         // 设置道具剩余使用次数

	int32 GetEnchantLevel() const;                             // 获取道具强化等级
	void SetEnchantLevel(int32 lvl);                           // 设置道具强化等级

	uint8 GetBless() const;                                    // 获取道具祝福状态 0-普通 1-祝福 2-诅咒
	void SetBless(uint8 bless);                                // 设置道具祝福状态

	ItemAttrEnchantType GetAttrEnchantKind() const;            // 获取道具属性强化类型
	int32 GetAttrEnchantLevel() const;                         // 获取道具属性强化等级
	void SetAttrEnchantKind(ItemAttrEnchantType type);         // 设置道具属性强化类型
	void SetAttrEnchantLevel(int32 lvl);                       // 设置道具属性强化等级

	int32 GetDefense(SpellSchools school) const;               // 获取道具魔法防御加成
	void SetDefense(SpellSchools school, int32 value);         // 设置道具魔法防御加成

	int32 GetSpellPower() const;                               // 获取道具法术强度加成
	void SetSpellPower(int32 value);                           // 设置道具法术强度加成

	int32 GetMaxMana() const;                                  // 获取道具魔力上限加成
	void SetMaxMana(int32 value);                              // 设置道具魔力上限加成

	int32 GetManaRegen() const;                                // 获取道具魔力回复加成
	void SetManaRegen(int32 value);                            // 设置道具魔力回复加成

	int32 GetMaxHealth() const;                                // 获取道具血量上限加成
	void SetMaxHealth(int32 value);                            // 设置道具血量上限加成

	int32 GetHealthegen() const;                               // 获取道具体力回复加成
	void SetHealthRegen(int32 value);                          // 设置道具体力回复加成

	int32 GetStat(Stats stat);                                 // 获取道具属性额外加成
	void SetStat(Stats stat, int32 value);                     // 设置道具属性额外加成

	void SetEquipped(bool equipped);                           // 设置道具装备状态
	void SetIdentified(bool identified);                       // 设置道具鉴定状态

	bool IsEquipped() const;                                   // 道具是否已被装备
	bool IsIdentified() const;                                 // 道具是否已经鉴定
	bool IsStackable() const;                                  // 道具是否允许堆叠

	/**
	 * 法术强化
	 */
	void SetAuraEffect(const ItemAuraEffect& effect);  // 给道具施加法术效果
	void SetAuraEffectDuration(uint32 duration);       // 更新道具法术效果时长
	void ClearAuraEffect();                            // 驱散道具的法术效果
	int8 GetAuraEffect(ItemAuraEffectType type) const; // 获取道具法术效果

	// 缺省实现
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override;

protected:
	uint8 m_slot;                 // 道具装备的位置
	const ItemEntry* m_proto; // 道具模板
	uint32 mapId;                 // 当前对象所在地图
	Position m_position;          // 当前对象坐标
	ItemAuraEffect m_effect;      // 施加在道具上的法术效果
};

#endif
