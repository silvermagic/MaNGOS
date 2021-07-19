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

#include "SQLStorages.h"
#include "SQLStructure.h"
#include "TileMap.h"

const uint16 BITFLAG_IS_IMPASSABLE = 0x80;

TileMap::TileMap(uint32 mapId) : m_proto(sMapStore.LookupEntry<MapEntry>(mapId))
{
	MANGOS_ASSERT(m_proto);
	const TileMapInfo& info = TILE_MAP_INFOS[mapId];
	MANGOS_ASSERT(info.ok);
	m_tminfo.ok = true;
	m_tminfo.x = info.x;
	m_tminfo.y = info.y;
	m_tminfo.width = info.width;
	m_tminfo.height = info.height;
	std::copy(info.tiles.begin(), info.tiles.end(), std::back_inserter(m_tminfo.tiles));
}

uint16 TileMap::GetTile(CoordUnit x, CoordUnit y)
{
	uint16 value = tile(x, y);
	if ((value & BITFLAG_IS_IMPASSABLE) != 0)
		return 300;
	return rawTile(x, y);
}

uint16 TileMap::GetRawTile(CoordUnit x, CoordUnit y)
{
	return rawTile(x, y);
}

bool TileMap::IsPassable(CoordUnit x, CoordUnit y)
{
	return IsPassable(x, y, SOUTH) || IsPassable(x, y, EAST) || IsPassable(x, y, NORTH) || IsPassable(x, y, WEST);
}

bool TileMap::IsPassable(CoordUnit x, CoordUnit y, Towards towards)
{
	uint16 before_move = tile(x, y);
	uint16 after_move;

	switch (towards)
	{
	case SOUTH:
		after_move = tile(x, y - 1);
		break;
	case SOUTH_EAST:
		after_move = tile(x + 1, y - 1);
		break;
	case EAST:
		after_move = tile(x + 1, y);
		break;
	case NORTH_EAST:
		after_move = tile(x + 1, y + 1);
		break;
	case NORTH:
		after_move = tile(x, y + 1);
		break;
	case NORTH_WEST:
		after_move = tile(x - 1, y + 1);
		break;
	case WEST:
		after_move = tile(x - 1, y);
		break;
	case SOUTH_WEST:
		after_move = tile(x - 1, y - 1);
		break;
	default:
		return false;
	}

	if ((after_move & BITFLAG_IS_IMPASSABLE) == BITFLAG_IS_IMPASSABLE)
	{
		return false;
	}

	switch (towards)
	{
	case SOUTH:
		return (before_move & 0x02) == 0x02;
	case SOUTH_EAST:
		return (tile(x, y - 1) & 0x01) == 0x01 || (tile(x + 1, y) & 0x02) == 0x02;
	case EAST:
		return (before_move & 0x01) == 0x01;
	case NORTH_EAST:
		return (tile(x, y + 1) & 0x01) == 0x01;
	case NORTH:
		return (after_move & 0x02) == 0x02;
	case NORTH_WEST:
		return (after_move & 0x01) == 0x01 || (after_move & 0x02) == 0x02;
	case WEST:
		return (after_move & 0x01) == 0x01;
	case SOUTH_WEST:
		return (tile(x - 1, y) & 0x01) == 0x01;
	default:
		return false;
	}
}

bool TileMap::IsArrowPassable(CoordUnit x, CoordUnit y)
{
	return (rawTile(x, y) & 0x0E) != 0x00;
}

bool TileMap::IsArrowPassable(CoordUnit x, CoordUnit y, Towards towards)
{
	uint16 before_move = tile(x, y);
	uint16 after_move;

	switch (towards)
	{
	case SOUTH:
		after_move = tile(x, y - 1);
		if (IsExistDoor(x, y - 1))
			return false;
		break;
	case SOUTH_EAST:
		after_move = tile(x + 1, y - 1);
		if (IsExistDoor(x + 1, y - 1))
			return false;
		break;
	case EAST:
		after_move = tile(x + 1, y);
		if (IsExistDoor(x + 1, y))
			return false;
		break;
	case NORTH_EAST:
		after_move = tile(x + 1, y + 1);
		if (IsExistDoor(x + 1, y + 1))
			return false;
		break;
	case NORTH:
		after_move = tile(x, y + 1);
		if (IsExistDoor(x, y + 1))
			return false;
		break;
	case NORTH_WEST:
		after_move = tile(x - 1, y + 1);
		if (IsExistDoor(x - 1, y + 1))
			return false;
		break;
	case WEST:
		after_move = tile(x - 1, y);
		if (IsExistDoor(x - 1, y))
			return false;
		break;
	case SOUTH_WEST:
		after_move = tile(x - 1, y - 1);
		if (IsExistDoor(x - 1, y - 1))
			return false;
		break;
	default:
		return false;
	}

	switch (towards)
	{
	case SOUTH:
		return (before_move & 0x08) == 0x08;
	case SOUTH_EAST:
		return (tile(x, y - 1) & 0x04) == 0x04 || (tile(x + 1, y) & 0x08) == 0x08;
	case EAST:
		return (before_move & 0x04) == 0x04;
	case NORTH_EAST:
		return (tile(x, y + 1) & 0x04) == 0x04;
	case NORTH:
		return (after_move & 0x08) == 0x08;
	case NORTH_WEST:
		return (after_move & 0x04) == 0x04 || (after_move & 0x08) == 0x08;
	case WEST:
		return (after_move & 0x04) == 0x04;
	case SOUTH_WEST:
		return (tile(x - 1, y) & 0x08) == 0x08;
	default:
		return false;
	}
}

void TileMap::MarkPassable(CoordUnit x, CoordUnit y, bool ok)
{
	if (ok)
	{
		tile(x, y, tile(x, y) & (~BITFLAG_IS_IMPASSABLE));
	}
	else
	{
		tile(x, y, tile(x, y) | BITFLAG_IS_IMPASSABLE);
	}
}

bool TileMap::IsSafetyZone(CoordUnit x, CoordUnit y)
{
	return (rawTile(x, y) & 0x30) == 0x10;
}

bool TileMap::IsCombatZone(CoordUnit x, CoordUnit y)
{
	return (rawTile(x, y) & 0x30) == 0x20;
}

bool TileMap::IsNormalZone(CoordUnit x, CoordUnit y)
{
	return (rawTile(x, y) & 0x30) == 0x00;
}

bool TileMap::IsFishingZone(CoordUnit x, CoordUnit y)
{
	return rawTile(x, y) == 0x10;
}

bool TileMap::IsExistDoor(CoordUnit x, CoordUnit y)
{
	// todo: GameObject´´½¨
	return false;
}

bool TileMap::IsInLineOfSight(CoordUnit srcX, CoordUnit srcY, CoordUnit destX, CoordUnit destY)
{
	// todo: ÊÓÒ°ÕÚµ²ÅÐ¶Ï
	return false;
}

uint32 TileMap::GetId(void) const
{
	return m_proto->ID;
}

std::string TileMap::GetName() const
{
	return m_proto->name;
}

bool TileMap::Instanceable() const
{
	return m_proto && m_proto->Instanceable();
}

bool TileMap::IsEscapable() const
{
	return m_proto && m_proto->isEscapable;
}

bool TileMap::IsMarkable() const
{
	return m_proto->isMarkable;
}

bool TileMap::IsPainwand() const
{
	return m_proto->isPainwand;
}

bool TileMap::IsPenalty() const
{
	return m_proto->isPenalty;
}

bool TileMap::IsRecallPets() const
{
	return m_proto->isRecallPets;
}

bool TileMap::IsResurrection() const
{
	return m_proto->isResurrection;
}

bool TileMap::IsTakePets() const
{
	return m_proto->isTakePets;
}

bool TileMap::IsTeleportable() const
{
	return m_proto->isTeleportable;
}

bool TileMap::IsUnderWater() const
{
	return m_proto->isUnderWater;
}

bool TileMap::IsUsableItem() const
{
	return m_proto->isUsableItem;
}

bool TileMap::IsUsableSkill() const
{
	return m_proto->isUsableSkill;
}

bool TileMap::IsCoordPosValid(CoordUnit x, CoordUnit y) const
{
	return m_proto->IsInMap(x, y);
}

uint32 TileMap::GetTileID(CoordUnit x, CoordUnit y) const
{
	return (x - m_tminfo.x) + (y - m_tminfo.y) * m_tminfo.width;
}

void TileMap::tile(CoordUnit x, CoordUnit y, uint16 value)
{
	if (!m_proto->IsInMap(x, y))
	{
		return;
	}

	m_tminfo.tiles[GetTileID(x, y)] = value;
}

uint16 TileMap::tile(CoordUnit x, CoordUnit y)
{
	if (!m_proto->IsInMap(x, y))
	{
		return 0;
	}

	return m_tminfo.tiles[GetTileID(x, y)];
}

uint16 TileMap::rawTile(CoordUnit x, CoordUnit y)
{
	return tile(x, y) & (~BITFLAG_IS_IMPASSABLE);
}