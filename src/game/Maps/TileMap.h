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

#ifndef MANGOS_TILEMAP_H
#define MANGOS_TILEMAP_H

#include <bitset>
#include <list>
#include "Platform/Define.h"
#include "Utilities/Singleton.h"
#include "Entities/Object.h"

struct MapEntry;

class MANGOS_DLL_SPEC TileMap
{
public:
	TileMap(uint32 mapId);

	/**
	 * ��ײ��Ϣ
	 */

	uint16 GetTile(CoordUnit x, CoordUnit y);    // ��ȡ���������Ӧ��Ƭ��Ϣ
	uint16 GetRawTile(CoordUnit x, CoordUnit y); // ��ȡ���������Ӧ��Ƭԭʼ��Ϣ(��ͼԭʼ��Ϣ��������վ����Ƭ�ϵ���Һ�����)

	bool IsPassable(CoordUnit x, CoordUnit y);                       // �����������������Ƿ�����ͨ��
	bool IsPassable(CoordUnit x, CoordUnit y, Towards towards);      // ������������ָ�������Ƿ�����ͨ��
	bool IsArrowPassable(CoordUnit x, CoordUnit y);                  // �����������������Ƿ������ʸͨ��
	bool IsArrowPassable(CoordUnit x, CoordUnit y, Towards towards); // ������������ָ�������Ƿ������ʸͨ��
	void MarkPassable(CoordUnit x, CoordUnit y, bool ok);            // �������������Ƿ�����ͨ��

	bool IsSafetyZone(CoordUnit x, CoordUnit y);  // �ж��������괦�Ƿ�Ϊ��ȫ����
	bool IsCombatZone(CoordUnit x, CoordUnit y);  // �ж��������괦�Ƿ�Ϊս������
	bool IsNormalZone(CoordUnit x, CoordUnit y);  // �ж��������괦�Ƿ�Ϊһ������
	bool IsFishingZone(CoordUnit x, CoordUnit y); // �ж��������괦�Ƿ�Ϊ��������
	bool IsExistDoor(CoordUnit x, CoordUnit y);   // �ж��������괦�Ƿ������

	bool IsInLineOfSight(CoordUnit srcX, CoordUnit srcY, CoordUnit destX, CoordUnit destY); // �ж�����֮���Ƿ���������ڵ�(���������϶�Ѩ�����м��Ƿ񱻸�����)

	/**
	 * ��������
	 */

	uint32 GetId(void) const;     // ��ͼ���
	std::string GetName() const;  // ��ͼ����
	bool Instanceable() const;    // �жϵ�ͼ�Ƿ��ܶ��ʵ����(������ͼ)
	bool IsEscapable() const;     // �жϵ�ͼ�Ƿ�����س�
	bool IsMarkable() const;      // �жϵ�ͼ�Ƿ������������
	bool IsPainwand() const;      // �жϵ�ͼ�Ƿ�������
	bool IsPenalty() const;       // �жϵ�ͼ�����Ƿ��ܵ��ͷ�
	bool IsRecallPets() const;    // �жϵ�ͼ�Ƿ������ٻ�����
	bool IsResurrection() const;  // �жϵ�ͼ�Ƿ�������
	bool IsTakePets() const;      // �жϵ�ͼ�Ƿ�����Я������
	bool IsTeleportable() const;  // �жϵ�ͼ�Ƿ������������
	bool IsUnderWater() const;    // �жϵ�ͼ�Ƿ�λ��ˮ��
	bool IsUsableItem() const;    // �жϵ�ͼ�Ƿ�����ʹ�õ���
	bool IsUsableSkill() const;   // �жϵ�ͼ�Ƿ�����ʹ��ħ��

	bool IsCoordPosValid(CoordUnit x, CoordUnit y) const; // �ж������Ƿ�Ϸ�

protected:
	/**
	 * ��ȡ��Ƭ��Ϣ
	 */
	uint32 GetTileID(CoordUnit x, CoordUnit y) const;    // ������Ƭ����

	void   tile(CoordUnit x, CoordUnit y, uint16 value); // �������������Ӧ��Ƭ��Ϣ
	uint16 tile(CoordUnit x, CoordUnit y);               // ��ȡ���������Ӧ��Ƭ��Ϣ
	uint16 rawTile(CoordUnit x, CoordUnit y);            // ��ȡ���������Ӧ��Ƭԭʼ��Ϣ(��Ƭ��ͼԭʼ��Ϣ����������Һ�����)

protected:
	const MapEntry* m_proto;     // ��ͼ��Ϣ
	TileMapInfo m_tminfo;        // ��ͼ��Ƭ��Ϣ
};



#endif
