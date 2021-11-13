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

#ifndef MANGOS_SQLSTORAGES_H
#define MANGOS_SQLSTORAGES_H

#include "Database/SQLStorage.h"
#include "Common.h"
#include "SQLStructure.h"

class Player;

// ��ȡ��һسǵ�
void GetBackPosition(Player *player);

// ������������Ӧ�ȼ�������ۼƾ���ֵ(Lv1 - 100)
uint32 CalcExp(uint8 level);
// �����������辭��
uint32 CalcLevelUpExp(uint8 level);
// �����ۼƾ���ֵ���㵱ǰ�ȼ�
uint8 CalcLevel(uint32 exp);
// ��ȡ����۰ٷֱ�
uint8 CalcExpPercentage(uint8 level, uint32 exp);

/**
 * ����ORM
 */
extern std::map<uint32, ItemEntry> ITEM_PROTOS;
extern std::map<uint32, SpellEntry> SPELL_PROTOS;

/**
 * ��ORM
 */
extern SQLHashStorage  sMapStore;
extern SQLMultiStorage sGetBackStore;
extern SQLHashStorage  sGetBackRestartStore;

void LoadSQLStorage();

#endif
