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

#include "Entities/Player.h"
#include "SQLStorages.h"
#include "SQLStructure.h"

const char GetBackSrcFmt[] = "iiiiiiiiiiiiiiilx";
const char GetBackDstFmt[] = "iiiiiiiiiiiiiiil";
const char GetBackRestartSrcFmt[] = "ixiii";
const char GetBackRestartDstFmt[] = "iiii";
const char TownSrcFmt[] = "ixixiiiiii";
const char TownDstFmt[] = "iiiiiiii";
SQLMultiStorage sGetBackStore(GetBackSrcFmt, GetBackDstFmt, "area_mapid", "getback");
SQLHashStorage  sGetBackRestartStore(GetBackRestartSrcFmt, GetBackRestartDstFmt, "area","getback_restart");
SQLStorage      sTownStore(TownSrcFmt, TownDstFmt, "town_id", "town");

void GetBackPosition(uint32 town_id, uint32 &mapId, CoordUnit &x, CoordUnit &y)
{
	static const CoordUnit TALKING_ISLAND[][2] = {{32600, 32942}, {32574, 32944}, {32580, 32923}, {32557, 32975}, {32597, 32914}, {32580, 32974}};
	static const CoordUnit SILVER_KNIGHT_TOWN[][2] = {{33071, 33402}, {33091, 33396}, {33085, 33402}, {33097, 33366}, {33110, 33365}, {33072, 33392}};
	static const CoordUnit GLUDIO[][2] = {{32601, 32757}, {32625, 32809}, {32611, 32726}, {32612, 32781}, {32605, 32761}, {32614, 32739}, {32612, 32775}};
	static const CoordUnit ORCISH_FOREST[][2] = {{32750, 32435}, {32745, 32447}, {32738, 32452}, {32741, 32436}, {32749, 32446}};
	static const CoordUnit WINDAWOOD[][2] = {{32608, 33178}, {32626, 33185}, {32630, 33179}, {32625, 33207}, {32638, 33203}, {32621, 33179}};
	static const CoordUnit KENT[][2] = {{33048, 32750}, {33059, 32768}, {33047, 32761}, {33059, 32759}, {33051, 32775}, {33048, 32778}, {33064, 32773}, {33057, 32748}};
	static const CoordUnit GIRAN[][2] = {{33435, 32803}, {33439, 32817}, {33440, 32809}, {33419, 32810}, {33426, 32823}, {33418, 32818}, {33432, 32824}};
	static const CoordUnit HEINE[][2] = {{33593, 33242}, {33593, 33248}, {33604, 33236}, {33599, 33236}, {33610, 33247}, {33610, 33241}, {33599, 33252}, {33605, 33252}};
	static const CoordUnit WERLDAN[][2] = {{33702, 32492}, {33747, 32508}, {33696, 32498}, {33723, 32512}, {33710, 32521}, {33724, 32488}, {33693, 32513}};
	static const CoordUnit OREN[][2] = {{34086, 32280}, {34037, 32230}, {34022, 32254}, {34021, 32269}, {34044, 32290}, {34049, 32316}, {34081, 32249}, {34074, 32313}, {34064, 32230}};
	static const CoordUnit ELVEN_FOREST[][2] = {{33065, 32358}, {33052, 32313}, {33030, 32342}, {33068, 32320}, {33071, 32314}, {33030, 32370}, {33076, 32324}, {33068, 32336}};
	static const CoordUnit ADEN[][2] = {{33915, 33114}, {34061, 33115}, {34090, 33168}, {34011, 33136}, {34093, 33117}, {33959, 33156}, {33992, 33120}, {34047, 33156}};
	static const CoordUnit SILENT_CAVERN[][2] = {{32856, 32898}, {32860, 32916}, {32868, 32893}, {32875, 32903}, {32855, 32898}};
	static const CoordUnit OUM_DUNGEON[][2] = {{32818, 32805}, {32800, 32798}, {32815, 32819}, {32823, 32811}, {32817, 32828}};
	static const CoordUnit RESISTANCE[][2] = {{32570, 32667}, {32559, 32678}, {32564, 32683}, {32574, 32661}, {32576, 32669}, {32572, 32662}};
	static const CoordUnit PIRATE_ISLAND[][2] = {{32431, 33058}, {32407, 33054}};
	static const CoordUnit RECLUSE_VILLAGE[][2] = {{32599, 32916}, {32599, 32923}, {32603, 32908}, {32595, 32908}, {32591, 32918}};

	uint32 i = 0;
	mapId = 4;
	switch (town_id)
	{
		case 1:
			i = urand(0, sizeof(TALKING_ISLAND) / sizeof(TALKING_ISLAND[0]) - 1);
			x = TALKING_ISLAND[i][0];
			y = TALKING_ISLAND[i][1];
			mapId = 0;
			break;
		case 2:
			i = urand(0, sizeof(SILVER_KNIGHT_TOWN) / sizeof(SILVER_KNIGHT_TOWN[0]) - 1);
			x = SILVER_KNIGHT_TOWN[i][0];
			y = SILVER_KNIGHT_TOWN[i][1];
			break;
		case 3:
			i = urand(0, sizeof(GLUDIO) / sizeof(GLUDIO[0]) - 1);
			x = GLUDIO[i][0];
			y = GLUDIO[i][1];
			break;
		case 4:
			i = urand(0, sizeof(ORCISH_FOREST) / sizeof(ORCISH_FOREST[0]) - 1);
			x = ORCISH_FOREST[i][0];
			y = ORCISH_FOREST[i][1];
			break;
		case 5:
			i = urand(0, sizeof(WINDAWOOD) / sizeof(WINDAWOOD[0]) - 1);
			x = WINDAWOOD[i][0];
			y = WINDAWOOD[i][1];
			break;
		case 6:
			i = urand(0, sizeof(KENT) / sizeof(KENT[0]) - 1);
			x = KENT[i][0];
			y = KENT[i][1];
			break;
		case 7:
			i = urand(0, sizeof(KENT) / sizeof(KENT[0]) - 1);
			x = GIRAN[i][0];
			y = GIRAN[i][1];
			break;
		case 8:
			i = urand(0, sizeof(HEINE) / sizeof(HEINE[0]) - 1);
			x = HEINE[i][0];
			y = HEINE[i][1];
			break;
		case 9:
			i = urand(0, sizeof(WERLDAN) / sizeof(WERLDAN[0]) - 1);
			x = WERLDAN[i][0];
			y = WERLDAN[i][1];
			break;
		case 10:
			i = urand(0, sizeof(OREN) / sizeof(OREN[0]) - 1);
			x = OREN[i][0];
			y = OREN[i][1];
			break;
		case 11:
			i = urand(0, sizeof(ELVEN_FOREST) / sizeof(ELVEN_FOREST[0]) - 1);
			x = ELVEN_FOREST[i][0];
			y = ELVEN_FOREST[i][1];
			break;
		case 12:
			i = urand(0, sizeof(ADEN) / sizeof(ADEN[0]) - 1);
			x = ADEN[i][0];
			y = ADEN[i][1];
			break;
		case 13:
			i = urand(0, sizeof(SILENT_CAVERN) / sizeof(SILENT_CAVERN[0]) - 1);
			x = SILENT_CAVERN[i][0];
			y = SILENT_CAVERN[i][1];
			mapId = 304;
			break;
		case 14:
			i = urand(0, sizeof(OUM_DUNGEON) / sizeof(OUM_DUNGEON[0]) - 1);
			x = OUM_DUNGEON[i][0];
			y = OUM_DUNGEON[i][1];
			mapId = 310;
			break;
		case 15:
			i = urand(0, sizeof(RESISTANCE) / sizeof(RESISTANCE[0]) - 1);
			x = RESISTANCE[i][0];
			y = RESISTANCE[i][1];
			mapId = 400;
			break;
		case 16:
			i = urand(0, sizeof(PIRATE_ISLAND) / sizeof(PIRATE_ISLAND[0]) - 1);
			x = PIRATE_ISLAND[i][0];
			y = PIRATE_ISLAND[i][1];
			mapId = 440;
			break;
		case 17:
			i = urand(0, sizeof(RECLUSE_VILLAGE) / sizeof(RECLUSE_VILLAGE[0]) - 1);
			x = RECLUSE_VILLAGE[i][0];
			y = RECLUSE_VILLAGE[i][1];
			mapId = 400;
			break;
		default:
			i = urand(0, sizeof(SILVER_KNIGHT_TOWN) / sizeof(SILVER_KNIGHT_TOWN[0]) - 1);
			x = SILVER_KNIGHT_TOWN[i][0];
			y = SILVER_KNIGHT_TOWN[i][1];
			break;
	}
}

void GetBackPosition(Player *player)
{
	const GetBackEntry *found = NULL;
	auto bounds = sGetBackStore.getBounds<GetBackEntry>(player->GetMapId());
	for (auto iter = bounds.first; iter != bounds.second; ++iter)
	{
		auto area = iter.getValue();
		if (!area->IsSpecifyArea() ||
			(area->start_x <= player->GetPositionX() && player->GetPositionX() <= area->end_x
				&& area->start_y <= player->GetPositionY() && player->GetPositionY() <= area->end_y))
		{
			found = area;
			break;
		}
	}

	uint32 mapId = 4;
	CoordUnit x = 33089;
	CoordUnit y = 33397;
	if (found) {
		if (player->GetClass() == CLASS_ELF && found->elfHomeTownID > 0) {
			GetBackPosition(found->elfHomeTownID, mapId, x, y);
		} else if (player->GetClass() == CLASS_DARK_ELF && found->darkElfHomeTownID > 0) {
			GetBackPosition(found->darkElfHomeTownID, mapId, x, y);
		} else if (found->homeTownID > 0) {
			GetBackPosition(found->homeTownID, mapId, x, y);
		} else {
			mapId = found->mapID;
			found->GetBackPosition(urand(0, 2), x, y);
		}
	}
	player->RelocateToHomebind(mapId, x, y);
}
