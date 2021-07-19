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

#include "Database/DatabaseEnv.h"
#include "Entities/Player.h"
#include "Common.h"
#include "Log.h"
#include "Util.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSocket.h"
#include "WorldSession.h"
#include "World.h"

/// 根据当前会话状态，选着合适的操作码在Map::Update中处理
static bool MapSessionFilterHelper(WorldSession* session, OpcodeHandler const& opHandle)
{
    // 不能处理非线程安全的数据包
    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
	{
    	return false;
	}

    // 不能处理未登入游戏的玩家数据包
    Player* player = session->GetPlayer();
    if (!player)
	{
    	return false;
	}

    // Map::Update()只能处理进入游戏世界的玩家的数据包
    return player->IsInWorld();
}

bool MapSessionFilter::Process(WorldPacket* packet)
{
    OpcodeHandler const& opHandle = opcodeTable[packet->GetOpcode()];
    if (opHandle.packetProcessing == PROCESS_INPLACE)
	{
    	return true;
	}

    return MapSessionFilterHelper(m_pSession, opHandle);
}

/// 我们必须处理那些在玩家尚未进入游戏世界产生的或非线程安全的数据包
bool WorldSessionFilter::Process(WorldPacket* packet)
{
    OpcodeHandler const& opHandle = opcodeTable[packet->GetOpcode()];
    if (opHandle.packetProcessing == PROCESS_INPLACE)
	{
    	return true;
	}

    // 不能处理那些通常在Map::Update()中处理的操作码
    return !MapSessionFilterHelper(m_pSession, opHandle);
}

WorldSession::WorldSession(std::string id, WorldSocket *ws, AccountTypes sec, uint8 characterSlot) :
    m_player(nullptr), m_Socket(ws), m_security(sec), m_accountId(id), m_characterSlot(characterSlot),
    m_inQueue(false), m_playerLoading(false), m_playerLogout(false), m_playerSave(false)
{
    if (ws)
    {
        m_Address = ws->GetRemoteAddress();
    }
}

/// WorldSession destructor
WorldSession::~WorldSession()
{
    ///- 如果玩家尚未离开游戏，则强制踢出
    if (m_player)
	{
    	LogoutPlayer(true);
	}

    /// - 如果套接字尚未关闭，则强制关闭
    if (m_Socket)
    {
        m_Socket->CloseSocket();
        m_Socket = nullptr;
    }

    ///- 清空数据包接收队列
    WorldPacket* packet = nullptr;
    while (m_recvQueue.next(packet))
	{
    	delete packet;
	}
}

void WorldSession::SendPacket(WorldPacket const* packet)
{
    if (!m_Socket)
	{
    	return;
	}

    ///- 天堂和魔兽世界不同，天堂的服务端操作码和客户端的操作码是重叠的，所以这边不能应用此校验
	/*if (opcodeTable[packet->GetOpcode()].status == STATUS_UNHANDLED)
	{
		sLog.outError("SESSION: tried to send an unhandled opcode 0x%.4X", packet->GetOpcode());
		return;
	}*/

    m_Socket->SendPacket(*packet);
}

void WorldSession::QueuePacket(WorldPacket* new_packet)
{
    m_recvQueue.add(new_packet);
}

void WorldSession::LogUnexpectedOpcode(WorldPacket* packet, const char* reason)
{
    sLog.outError("SESSION: received unexpected opcode %s (0x%.4X) %s",
                  LookupOpcodeName(packet->GetOpcode()),
                  packet->GetOpcode(),
                  reason);
}

bool WorldSession::Update(PacketFilter& updater)
{
    // 从接收队列中取出收到的数据包，并调用合适的句柄进行处理，如果套接字已关闭，则不处理任何数据包
    WorldPacket* packet = NULL;
    while (m_Socket && !m_Socket->IsClosed() && m_recvQueue.next(packet, updater))
    {
        OpcodeHandler const& opHandle = opcodeTable[packet->GetOpcode()];
        try
        {
            switch (opHandle.status)
            {
                case STATUS_LOGGEDIN:
                	if (!m_player && m_player->IsInWorld())
					{
						ExecuteOpcode(opHandle, packet);
					}
                    break;
                case STATUS_LOGGEDIN_OR_RECENTLY_LOGGEDOUT:
                    break;
                case STATUS_TRANSFER:
                    break;
                case STATUS_AUTHED:
                    // 账号登入排队中避免作弊
                    if (m_inQueue)
                    {
                        LogUnexpectedOpcode(packet, "the player not pass queue yet");
                        break;
                    }

                    ExecuteOpcode(opHandle, packet);
                    break;
                case STATUS_NEVER:
                    sLog.outError("SESSION: received not allowed opcode %s (0x%.4X)",
                                  LookupOpcodeName(packet->GetOpcode()),
                                  packet->GetOpcode());
                    break;
                case STATUS_UNHANDLED:
                    sLog.outError("SESSION: received not handled opcode %s (0x%.4X)",
                                  LookupOpcodeName(packet->GetOpcode()),
                                  packet->GetOpcode());
                    break;
                default:
                    sLog.outError("SESSION: received wrong-status-req opcode %s (0x%.4X)",
                                  LookupOpcodeName(packet->GetOpcode()),
                                  packet->GetOpcode());
                    break;
            }
        }
        catch (ByteBufferException&)
        {
            sLog.outError("WorldSession::Update ByteBufferException occured while parsing a packet (opcode: %u) from client %s, accountid=%s.",
                          packet->GetOpcode(), GetRemoteAddress().c_str(), GetAccountId().c_str());
            if (sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))
            {
                DEBUG_LOG("Dumping error causing packet:");
                packet->hexlike();
            }

            if (sWorld.GetConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET))
            {
                DETAIL_LOG("Disconnecting session [account id %s / address %s] for badly formatted packet.",
                           GetAccountId().c_str(), GetRemoteAddress().c_str());

                KickPlayer();
            }
        }

        delete packet;
    }

    // 如果套接字已经关闭，清理套接字层指针
    if (m_Socket && m_Socket->IsClosed())
    {
        m_Socket = nullptr;
    }

    // 检查是否能安全的处理登出，角色登出处理仅允许在World::UpdateSessions()方法中发生
    if (updater.ProcessLogout())
    {
        if (!m_Socket || !m_playerLoading)
		{
        	LogoutPlayer(true);
		}

        if (!m_Socket)
		{
        	// 将导致当前会话从世界会话列表中移除
        	return false;
		}
    }

    return true;
}

void WorldSession::SetPlayer(Player* player)
{
    m_player = player;
    if (m_player)
	{
    	m_GUIDLow = m_player->GetGUIDLow();
	}
}

void WorldSession::LogoutPlayer(bool Save)
{
	m_playerLogout = true;
	m_playerSave = Save;

	if (m_player)
	{
		sLog.outChar("Account: %s (IP: %s) Logout Character:[%s] (guid: %u)",
			GetAccountId().c_str(),
			GetRemoteAddress().c_str(),
			m_player->GetName().c_str(),
			m_player->GetGUIDLow());

		if (Save)
		{
			m_player->SaveToDB();
		}

		if (m_player->IsInWorld())
		{
			Map* pMap = m_player->GetMap();
			pMap->Remove(m_player, true);
		}
		else
		{
			m_player->CleanupsBeforeDelete();
			Map::DeleteFromWorld(m_player);
		}

		SetPlayer(nullptr);

		// todo: 是否需要发送数据包

		static SqlStatementID updChars;
		SqlStatement stmt = CharacterDatabase.CreateStatement(updChars, "UPDATE characters SET OnlineStatus = 0 WHERE objid = ?");
		stmt.PExecute(m_GUIDLow);
	}

	m_playerLogout = false;
	m_playerSave = false;
}

void WorldSession::KickPlayer()
{
    if (m_Socket)
    {
        m_Socket->CloseSocket();
    }
}

void WorldSession::Handle_NULL(WorldPacket& recvPacket)
{
    DEBUG_LOG("SESSION: received unimplemented opcode %s (0x%.4X)",
              LookupOpcodeName(recvPacket.GetOpcode()),
              recvPacket.GetOpcode());
}

void WorldSession::Handle_EarlyProccess(WorldPacket& recvPacket)
{
    sLog.outError("SESSION: received opcode %s (0x%.4X) that must be processed in WorldSocket::OnRead",
                  LookupOpcodeName(recvPacket.GetOpcode()),
                  recvPacket.GetOpcode());
}

void WorldSession::Handle_ServerSide(WorldPacket& recvPacket)
{
    sLog.outError("SESSION: received server-side opcode %s (0x%.4X)",
                  LookupOpcodeName(recvPacket.GetOpcode()),
                  recvPacket.GetOpcode());
}

void WorldSession::Handle_Deprecated(WorldPacket& recvPacket)
{
    sLog.outError("SESSION: received deprecated opcode %s (0x%.4X)",
                  LookupOpcodeName(recvPacket.GetOpcode()),
                  recvPacket.GetOpcode());
}

void WorldSession::ExecuteOpcode(OpcodeHandler const& opHandle, WorldPacket* packet)
{
    (this->*opHandle.handler)(*packet);
}
