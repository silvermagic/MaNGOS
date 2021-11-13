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

#ifndef MANGOSSERVER_DYNAMICOBJECT_H
#define MANGOSSERVER_DYNAMICOBJECT_H

#include "SharedDefines.h"
#include "Object.h"

enum DynamicObjectType
{
    DYNAMIC_OBJECT_PORTAL           = 0x0,
    DYNAMIC_OBJECT_AREA_SPELL       = 0x1, // ��Χħ�������Ķ�̬���������ǽ�����������籩
    DYNAMIC_OBJECT_FARSIGHT_FOCUS   = 0x2,
    DYNAMIC_OBJECT_RAID_MARKER      = 0x3,
};

class DynamicObject : public WorldObject
{
public:
	/**
	 * ��ָ���������괦����һ����̬������ʵ�ֳ�����Χħ����ʩ�ţ������ǽ�����������籩
	 *
	 * @param guidlow  ��̬����GUID
	 * @param caster   ������̬�����ʩ����
	 * @param spellId  ������̬�����ħ�����
	 * @param effIndex ������̬�����ħ����Ч�����
	 * @param x        ��̬������������
	 * @param y        ��̬������������
	 * @param duration ��̬������
	 * @param radius   ħ��Ч���뾶
	 * @param type     ��̬��������
	 * @return true��ʾ�����ɹ�
	 */
	bool Create(uint32 guidlow, Unit* caster, uint32 spellId, SpellEffectIndex effIndex, CoordUnit x, CoordUnit y, int32 duration, uint8 radius, DynamicObjectType type);

public:
    explicit DynamicObject();

    void AddToWorld() override;                         // ��Ӷ�̬������Ϸ����
    void RemoveFromWorld() override;                    // ����̬�������Ϸ�����Ƴ�

    void Update(uint32 update_diff, uint32 p_time) override;
    void Delete();                                      // ɾ����̬����
	void Delay(int32 delaytime);                        // todo: ������÷�

	void AddAffected(Unit* unit);                       // ����ܵ�Ӱ�����Ϸ����
	void RemoveAffected(Unit* unit);                    // �Ƴ�δ�ܵ�Ӱ�����Ϸ����

	Unit* GetCaster() const;                            // ��ȡ������̬�����ʩ����
	const ObjectGuid& GetCasterGuid() const;            // ��ȡ������̬�����ʩ����GUID
	uint32 GetDuration() const;                         // ��ȡ��̬�����ʣ����ʱ��
	SpellEffectIndex GetEffIndex() const;               // ��ȡ������̬�����ħ����Ч�����
	GridReference<DynamicObject>& GetGridRef();         // ��ȡ���ڽ�������ص������ϵ�˫������ڵ�
	uint8 GetRadius() const;                            // ��ȡħ��Ч����Χ�뾶
    uint32 GetSpellId() const;                          // ��ȡ�������
    DynamicObjectType GetType() const;                  // ��ȡ��̬��������

    bool IsAffecting(Unit* unit) const;                 // �ж϶����Ƿ��ܵ���̬���������ħ��Ч��Ӱ��
	bool IsVisibleForInState(const Player* player, const WorldObject* viewPoint, bool inVisibleList) const override; // �ж�����Ƿ��ܿ�������

protected:
	ObjectGuid m_caster;         // ������̬�����ʩ����
	GuidSet m_affected;          // �ܵ�ħ��Ч��Ӱ��Ķ���GUID
	DynamicObjectType m_type;    // ��̬��������
	int32 m_aliveDuration;       // ħ��Ч��ʣ��ʱ��

	uint32 m_spellId;            // ������̬�����ħ�����
	SpellEffectIndex m_effIndex; // ������̬�����ħ����Ч�����
	uint8 m_radius;              // ħ��Ч����Χ�뾶
	bool m_positive;             // ħ��Ч���Ƿ��������

private:
    GridReference<DynamicObject> m_gridRef; // ���ڽ�������ص������ϵ�˫������ڵ�
};
#endif
