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

#include "Database/SQLStorage.h"
#include "Entities/Player.h"
#include "SQLStorages.h"

extern SQLStorage sExpStore;

void LoadItemsFromWeapon();
void LoadItemsFromArmor();
void LoadItemsFromOther();
void LoadSpells();

std::map<uint32, GridMapInfo>   GRID_MAP_INFOS;
std::map<uint32, TileMapInfo>   TILE_MAP_INFOS;

void LoadSQLStorage()
{
	sLog.outString("Loading l1jdb mapids...");
	sMapStore.Load();
	sLog.outString("Loading l1jdb getback...");
	sGetBackStore.Load();
	sLog.outString("Loading l1jdb getback_restart...");
	sGetBackRestartStore.Load();
	sLog.outString("Loading l1jdb exp...");
	sExpStore.Load();

	for (auto iter = sMapStore.getDataBegin<MapEntry>(); iter < sMapStore.getDataEnd<MapEntry>(); ++iter)
	{
		const MapEntry *proto = iter.getValue();
		MANGOS_ASSERT(proto != nullptr);
		GridMapInfo &gminfo = GRID_MAP_INFOS[proto->ID];
		gminfo.ok = true;
		gminfo.x = proto->start_x;
		gminfo.y = proto->start_y;
		gminfo.width = proto->GetWidth() / SIZE_OF_GRID;
		gminfo.height = proto->GetHeight() / SIZE_OF_GRID;
	}

	// 加载道具
	LoadItemsFromWeapon();
	LoadItemsFromArmor();
	LoadItemsFromOther();

	// 加载技能
	LoadSpells();
}