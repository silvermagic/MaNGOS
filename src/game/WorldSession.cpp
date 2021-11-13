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

/// ���ݵ�ǰ�Ự״̬��ѡ�ź��ʵĲ�������Map::Update�д���
static bool MapSessionFilterHelper(WorldSession* session, OpcodeHandler const& opHandle)
{
    // ���ܴ�����̰߳�ȫ�����ݰ�
    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
	{
    	return false;
	}

    // ���ܴ���δ������Ϸ��������ݰ�
    Player* player = session->GetPlayer();
    if (!player)
	{
    	return false;
	}

    // Map::Update()ֻ�ܴ��������Ϸ�������ҵ����ݰ�
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

/// ���Ǳ��봦����Щ�������δ������Ϸ��������Ļ���̰߳�ȫ�����ݰ�
bool WorldSessionFilter::Process(WorldPacket* packet)
{
    OpcodeHandler const& opHandle = opcodeTable[packet->GetOpcode()];
    if (opHandle.packetProcessing == PROCESS_INPLACE)
	{
    	return true;
	}

    // ���ܴ�����Щͨ����Map::Update()�д���Ĳ�����
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
    ///- ��������δ�뿪��Ϸ����ǿ���߳�
    if (m_player)
	{
    	LogoutPlayer(true);
	}

    /// - ����׽�����δ�رգ���ǿ�ƹر�
    if (m_Socket)
    {
        m_Socket->CloseSocket();
        m_Socket = nullptr;
    }

    ///- ������ݰ����ն���
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

    ///- ���ú�ħ�����粻ͬ�����õķ���˲�����Ϳͻ��˵Ĳ��������ص��ģ�������߲���Ӧ�ô�У��
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
    // �ӽ��ն�����ȡ���յ������ݰ��������ú��ʵľ�����д�������׽����ѹرգ��򲻴����κ����ݰ�
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
                    // �˺ŵ����Ŷ��б�������
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

    // ����׽����Ѿ��رգ������׽��ֲ�ָ��
    if (m_Socket && m_Socket->IsClosed())
    {
        m_Socket = nullptr;
    }

    // ����Ƿ��ܰ�ȫ�Ĵ���ǳ�����ɫ�ǳ������������World::UpdateSessions()�����з���
    if (updater.ProcessLogout())
    {
        if (!m_Socket || !m_playerLoading)
		{
        	LogoutPlayer(true);
		}

        if (!m_Socket)
		{
        	// �����µ�ǰ�Ự������Ự�б����Ƴ�
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

		// todo: �Ƿ���Ҫ�������ݰ�

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
