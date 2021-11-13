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

#ifndef MANGOS_GETBACK_H_
#define MANGOS_GETBACK_H_

#include "Common.h"
#include "SharedDefines.h"

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct GetBackEntry // ���ʹ�ûسǾ����ض�λ����
{
	uint32 start_x;              // ������Χ
	uint32 end_x;
	uint32 start_y;
	uint32 end_y;
	uint32 areaID;               // �������ʶ(�����ڴ�½MAP_ID=4����ͬ�ط�ʹ�ûسǻص��ĵط�Ҳ��ͬ��������Ҫ���ֳ�������)
	uint32 coords[3][2];         // ����������꣬���ѡȡ�����е�һ��
	uint32 mapID;                // ��ͼ��ʶ
	uint32 homeTownID;           // ��ׯ��ʶ
	uint32 elfHomeTownID;        // ������ׯ(����ɭ��)
	uint32 darkElfHomeTownID;    // �ڰ������ׯ(��Ĭ��Ѩ)
	bool isScrollEscape;         // �Ƿ�����ʹ�ûؼҾ�
	// DBString note;

	bool IsSpecifyArea() const { return start_x != 0 && end_x != 0 && start_y != 0 && end_y != 0; }
	void GetBackPosition(uint8 i, CoordUnit &x, CoordUnit &y) const { i %=3; x = coords[i][0]; y = coords[i][1]; }
};

struct GetBackRestartEntry // ������µ������ض�λ����
{
	uint32 ID;        // �������ʶ
	// DBString note;
	uint32 coords[2]; // ��������
	uint32 mapID;     // ��ͼ��ʶ
};

struct TownEntry
{
	uint32 ID; // ��ׯID
	// DBString name; // ��ׯ����
	uint32 village_head; // �峤ID
	// DBString village_head_name; // �峤����
	int32 tax_rate; // ˰��
	int32 tax_rate_reserved;
	int32 sales_money; // ��ׯ���۶�
	int32 sales_money_yesterday;
	int32 town_tax; // ˰��
	int32 town_fix_tax;
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif //MANGOS_GETBACK_H_
