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

// 获取玩家回城点
void GetBackPosition(Player *player);

// 计算提升到对应等级所需的累计经验值(Lv1 - 100)
uint32 CalcExp(uint8 level);
// 根据升级所需经验
uint32 CalcLevelUpExp(uint8 level);
// 根据累计经验值计算当前等级
uint8 CalcLevel(uint32 exp);
// 获取经验槽百分比
uint8 CalcExpPercentage(uint8 level, uint32 exp);

/**
 * 不能ORM
 */
extern std::map<uint32, ItemEntry> ITEM_PROTOS;
extern std::map<uint32, SpellEntry> SPELL_PROTOS;

/**
 * 能ORM
 */
extern SQLHashStorage  sMapStore;
extern SQLMultiStorage sGetBackStore;
extern SQLHashStorage  sGetBackRestartStore;

void LoadSQLStorage();

#endif
