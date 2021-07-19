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

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "Common.h"
#include "Log.h"
#include "Util.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Utilities/ByteBuffer.h"
#include "AccountMgr.h"
#include "Opcodes.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSocket.h"
#include "WorldSession.h"
#include "WorldSocketMgr.h"

enum LoginOpResult : uint8
{
	LOR_OK = 0x00,
	LOR_ACCESS_FAILED = 0x08,
	LOR_USER_OR_PASS_WRONG = 0x08,
	LOR_PASS_WRONG = 0x08
};

WorldSocket::WorldSocket(boost::asio::io_context &ioContext, tcp::socket socket) :
	m_Socket(std::move(socket)),
	m_Closed(false),
	m_LastPingTime(steady_clock::duration::zero()),
	m_Address(m_Socket.remote_endpoint().address().to_string()),
	m_Seed(2147483647),
	//m_Seed(urand(1, 2147483648))
	m_Session(nullptr),
	m_Tx(ioContext)
{
	m_InBuffer.Reset();
	m_HeaderBuffer.Resize(sizeof(ClientPktHeader));
	m_PacketBuffer.Reset();
}

WorldSocket::~WorldSocket(void)
{
	m_Closed = true;
	boost::system::error_code ec;
	m_Socket.close(ec);
}

bool WorldSocket::IsClosed(void) const
{
	return m_Closed;
}

void WorldSocket::CloseSocket(void)
{
	if (m_Closed.exchange(true))
	{
		return;
	}

	boost::system::error_code ec;
	m_Socket.shutdown(boost::asio::socket_base::shutdown_send, ec);
	if (ec)
	{
		DEBUG_LOG("WorldSocket::CloseSocket ");
	}

	{
		GuardType guard(m_SessionLock);
		m_Session = nullptr;
	}
}

const std::string& WorldSocket::GetRemoteAddress(void) const
{
	return m_Address;
}

void WorldSocket::SendPacket(const WorldPacket& pct)
{
	if (IsClosed())
	{
		return;
	}

	sLog.outWorldPacketDump(uint32(m_Socket.native_handle()),
		pct.GetOpcode(),
		LookupOpcodeName(pct.GetOpcode()),
		&pct,
		false);

	m_OutPacket.Enqueue(new WorldPacket(pct));
}

void WorldSocket::handle_input()
{
	if (IsClosed())
	{
		return;
	}

	m_InBuffer.Normalize();
	m_InBuffer.EnsureFreeSpace();
	m_Socket.async_read_some(boost::asio::buffer(m_InBuffer.GetWritePointer(), m_InBuffer.GetRemainingSpace()),
		[this](boost::system::error_code ec, size_t bytes_transferred)
		{
			if (!ec)
			{
				m_InBuffer.WriteCompleted(bytes_transferred);
				handle_input_missing_data();
			}
			else
			{
				sLog.outError("%s", m_Address.c_str());
				CloseSocket();
			}
		});
}

bool WorldSocket::handle_output()
{
	static const uint8 placeholder = 0x00;

	if (IsClosed())
	{
		return false;
	}

	WorldPacket* packet;
	while (m_OutPacket.Dequeue(packet))
	{
		/// 构造发送帧
		ServerPktHeader header;
		uint8 align = packet->IsNeedToAlign() ? (sizeof(OpcodeType) + packet->size()) % 4 : 0;
		if (align)
		{
			align = 4 - align;
		}
		header.Size = sizeof(ServerPktHeader) + sizeof(OpcodeType) + packet->size() + align;
		MessageBuffer buffer(header.Size);

		/// 将数据包封装到发送帧中
		EndianConvert(header.Size);
		buffer.Write(&header, sizeof(ServerPktHeader));
		OpcodeType opcode = packet->GetOpcode();
		EndianConvert(opcode);
		buffer.Write(&opcode, sizeof(OpcodeType));
		buffer.Write(packet->contents(), packet->size());
		if (align)
		{
			for (size_t i = 0; i < align; ++i)
			{
				buffer.Write(&placeholder, sizeof(uint8));
			}
		}

		/// 加密发送帧
		m_Crypt.EncryptSend(buffer.GetBasePointer() + sizeof(ServerPktHeader),
			buffer.GetBufferSize() - sizeof(ServerPktHeader));

		/// 发送数据帧到客户端
		handle_output(buffer);

		delete packet;
	}
	return true;
}

void WorldSocket::handle_output(MessageBuffer &buffer)
{
	m_Tx.post([this, buf = std::move(buffer)]()
	{
		bool write_in_progress = !m_OutBuffer.empty();
		m_OutBuffer.push(std::move(buf));
		if (!write_in_progress)
		{
			boost::asio::async_write(m_Socket,
				boost::asio::buffer(m_OutBuffer.front().GetReadPointer(), m_OutBuffer.front().GetActiveSize()),
				boost::bind(&WorldSocket::handle_output_queue, this,
					boost::asio::placeholders::error));
		}
	});
}

void WorldSocket::handle_output_queue(const boost::system::error_code& error)
{
	if (!error)
	{
		m_OutBuffer.pop();
		if (!m_Closed && !m_OutBuffer.empty())
		{
			MessageBuffer& buffer = m_OutBuffer.front();
			boost::asio::async_write(m_Socket,
				boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()),
				boost::bind(&WorldSocket::handle_output_queue, this,
					boost::asio::placeholders::error));
		}
	}
	else
	{
		CloseSocket();
	}
}

void WorldSocket::Start()
{
	static const uint8 FIRST_PACKET[] = { 0xF4, 0x0a, 0x8d, 0x23, 0x6f, 0x7f, 0x04, 0x00, 0x05, 0x08, 0x00 };
	/// 构造游戏初始包
	WorldPacket packet(S_OPCODE_INITPACKET, sizeof(uint32) + sizeof(FIRST_PACKET) / sizeof(uint8));
	packet << m_Seed;
	packet.append(FIRST_PACKET, sizeof(FIRST_PACKET) / sizeof(uint8));

	/// 构造发送帧
	ServerPktHeader header;
	header.Size = sizeof(ServerPktHeader) + sizeof(OpcodeType) + packet.size();
	MessageBuffer buffer(header.Size);

	/// 将数据包封装到发送帧中
	EndianConvert(header.Size);
	buffer.Write(&header, sizeof(ServerPktHeader));
	OpcodeType opcode = S_OPCODE_INITPACKET;
	EndianConvert(opcode);
	buffer.Write(&opcode, sizeof(OpcodeType));
	buffer.Write(packet.contents(), packet.size());

	/// 发送到客户端
	handle_output(buffer);

	/// 密钥初始化，后续包都需要使用密钥加解密
	m_Crypt.Init(m_Seed);

	/// 开始从客户端接收数据帧
	handle_input();
}

bool WorldSocket::Update(void)
{
	if (IsClosed())
	{
		return false;
	}

	return handle_output();
}

bool WorldSocket::handle_input_header(void)
{
	MANGOS_ASSERT(m_HeaderBuffer.GetActiveSize() == sizeof(ClientPktHeader));

	ClientPktHeader& header = *((ClientPktHeader*)m_HeaderBuffer.GetBasePointer());
	EndianConvert(header.Size);
	if ((header.Size < 2) || (header.Size > 10240))
	{
		sLog.outError("WorldSocket::handle_input_header: client sent malformed packet size = %d", header.Size);
		return false;
	}
	header.Size -= sizeof(ClientPktHeader);

	m_PacketBuffer.Resize(header.Size);
	return true;
}

bool WorldSocket::handle_input_payload(void)
{
	MANGOS_ASSERT(m_PacketBuffer.GetActiveSize() != 0);
	MANGOS_ASSERT(m_HeaderBuffer.GetActiveSize() != 0);

	m_Crypt.DecryptRecv(m_PacketBuffer.GetBasePointer(), m_PacketBuffer.GetBufferSize());
	WorldPacket packet(std::move(m_PacketBuffer));
	OpcodeType opcode;
	packet >> opcode;
	packet.SetOpcode(opcode);
	// todo: 是否需要根据返回值做一些处理？还是就让客户端报错来断开连接
	ProcessIncoming(packet);
	m_HeaderBuffer.Reset();

	return true;
}

void WorldSocket::handle_input_missing_data(void)
{
	if (IsClosed())
	{
		return;
	}

	/// 尝试从读取的字节流中找出一个完整的数据帧
	while (m_InBuffer.GetActiveSize() > 0)
	{
		/// 读数据帧头部
		if (m_HeaderBuffer.GetRemainingSpace() > 0)
		{
			size_t size = std::min(m_InBuffer.GetActiveSize(), m_HeaderBuffer.GetRemainingSpace());
			m_HeaderBuffer.Write(m_InBuffer.GetReadPointer(), size);
			m_InBuffer.ReadCompleted(size);

			/// 头部数据尚未读完
			if (m_HeaderBuffer.GetRemainingSpace() > 0)
			{
				MANGOS_ASSERT(m_InBuffer.GetActiveSize() == 0);
				break;
			}

			/// 已经得到一个完整的数据帧头部，对其进行解析校验
			if (!handle_input_header())
			{
				CloseSocket();
				return;
			}
		}

		/// 我们上面已经读取了数据帧的头部了，已经获知了数据帧的数据长度，现在开始读取数据帧的数据部分
		if (m_PacketBuffer.GetRemainingSpace() > 0)
		{
			size_t size = std::min(m_InBuffer.GetActiveSize(), m_PacketBuffer.GetRemainingSpace());
			m_PacketBuffer.Write(m_InBuffer.GetReadPointer(), size);
			m_InBuffer.ReadCompleted(size);

			/// 数据部分尚未读完
			if (m_PacketBuffer.GetRemainingSpace() > 0)
			{
				MANGOS_ASSERT(m_InBuffer.GetActiveSize() == 0);
				break;
			}

			/// 已经得到完整的数据帧了，对其进行解密，并交由会话层处理
			if (!handle_input_payload())
			{
				CloseSocket();
				return;
			}
		}
	}

	/// 继续读取来来自客户端的字节流
	handle_input();
}

int WorldSocket::ProcessIncoming(WorldPacket& new_pct)
{
	const OpcodeType opcode = new_pct.GetOpcode();
	if (IsClosed())
	{
		return -1;
	}

	sLog.outWorldPacketDump(uint32(m_Socket.native_handle()),
		new_pct.GetOpcode(),
		LookupOpcodeName(new_pct.GetOpcode()),
		&new_pct,
		true);

	try
	{
		switch (opcode)
		{
			case C_OPCODE_CLIENTVERSION:
				return HandleClientVersion(new_pct);
			case C_OPCODE_LOGINPACKET:
				if (m_Session)
				{
					sLog.outError("WorldSocket::ProcessIncoming: Player send C_OPCODE_LOGINPACKET again");
					return -1;
				}
				return HandleAuthSession(new_pct);
			case C_OPCODE_KEEPALIVE:
				// todo: 定时消息
				m_LastPingTime = steady_clock::now();
				break;
			default:
			{
				GuardType guard(m_SessionLock);

				if (m_Session != nullptr)
				{
					m_Session->QueuePacket(new WorldPacket(std::move(new_pct)));
					return 0;
				}
				else
				{
					sLog.outError("WorldSocket::ProcessIncoming: Client not authed opcode = 0x%X", uint32(opcode));
					return -1;
				}
			}
		}
	}
	catch (ByteBufferException&)
	{
		sLog.outError(
			"WorldSocket::ProcessIncoming ByteBufferException occured while parsing an instant handled packet (opcode: %u) from client %s, accountid=%s.",
			opcode,
			GetRemoteAddress().c_str(),
			m_Session ? m_Session->GetAccountId().c_str() : "");
		if (sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))
		{
			DEBUG_LOG("Dumping error-causing packet:");
			new_pct.hexlike();
		}

		if (sWorld.GetConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET))
		{
			DETAIL_LOG("Disconnecting session [account id %s / address %s] for badly formatted packet.",
				m_Session ? m_Session->GetAccountId().c_str() : "", GetRemoteAddress().c_str());

			return -1;
		}
	}

	return 0;
}

int WorldSocket::HandleAuthSession(WorldPacket& recvPacket)
{
	std::string username, password;
	recvPacket >> username;
	recvPacket >> password;
	boost::algorithm::to_lower(username);

	DEBUG_LOG("WorldSocket::HandleAuthSession: account %s", username.c_str());

	std::string safe_account = username;
	LoginDatabase.escape_string(safe_account);

	QueryResult* result = LoginDatabase.PQuery("SELECT "
											   "username, "                //0
											   "access_level, "            //1
											   "ip, "                      //2
											   "banned, "                  //3
											   "character_slot "           //4
											   "FROM accounts "
											   "WHERE login = '%s'",
		safe_account.c_str());

	if (!result)
	{
		if (sConfig.GetBoolDefault("L1J.AutoCreateAccounts", false))
		{
			if (AOR_OK == sAccountMgr.CreateAccount(username, password))
			{
				result = LoginDatabase.PQuery("SELECT "
											  "username, "                //0
											  "access_level, "            //1
											  "ip, "                      //2
											  "banned, "                  //3
											  "character_slot "           //4
											  "FROM accounts "
											  "WHERE login = '%s'",
					safe_account.c_str());
			}
		}
	}

	if (!result)
	{
		WorldPacket packet(S_OPCODE_LOGINRESULT, 13);
		packet << uint8(LOR_USER_OR_PASS_WRONG);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		SendPacket(packet);

		sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (unknown account).");
		return -1;
	}

	Field* fields = result->Fetch();
	std::string accid = fields[0].GetCppString();
	AccountTypes sec = AccountTypes(fields[1].GetUInt8());
	bool banned = fields[3].GetBool();
	uint8 characterSlot = fields[4].GetUInt8();
	delete result;

	if (banned)
	{
		WorldPacket packet(S_OPCODE_LOGINRESULT, 13);
		packet << uint8(LOR_USER_OR_PASS_WRONG);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		SendPacket(packet);

		sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (Account banned).");
		return -1;
	}

	AccountTypes allowedAccountType = sWorld.GetPlayerSecurityLimit();
	if (allowedAccountType > SEC_PLAYER && sec < allowedAccountType)
	{
		WorldPacket packet(S_OPCODE_LOGINRESULT, 13);
		packet << uint8(LOR_ACCESS_FAILED);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		SendPacket(packet);

		BASIC_LOG("WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
		return -1;
	}

	if (!sAccountMgr.CheckPassword(accid, password))
	{
		WorldPacket packet(S_OPCODE_LOGINRESULT, 13);
		packet << uint8(LOR_USER_OR_PASS_WRONG);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		SendPacket(packet);

		sLog.outError("WorldSocket::HandleAuthSession: Sent Auth Response (authentification failed).");
		return -1;
	}

	std::string address = GetRemoteAddress();
	DEBUG_LOG("WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.",
		username.c_str(),
		address.c_str());

	static SqlStatementID updAccount;
	SqlStatement stmt =
		LoginDatabase.CreateStatement(updAccount, "UPDATE accounts SET ip = ?, lastactive = NOW() WHERE login = ?");
	stmt.PExecute(address.c_str(), accid.c_str());

	{
		WorldPacket packet(S_OPCODE_LOGINRESULT, 13);
		packet << uint8(LOR_OK);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		packet << uint32(0x00000000);
		SendPacket(packet);
	}

	{
		std::string message = "";
		try
		{
			boost::filesystem::path fpath("announcements.txt");
			boost::filesystem::ifstream fs(fpath);
			std::string announcement;
			while (std::getline(fs, announcement))
			{
				message += announcement + "\n";
			}
		}
		catch (const boost::filesystem::filesystem_error&)
		{
		}

		WorldPacket packet(S_OPCODE_COMMONNEWS, message.size());
		packet << message;
		SendPacket(packet);
	}

	/// 完成登入，创建会话层，后续从数据帧解析出的数据包都交由会话层处理
	m_Session = new WorldSession(accid, this, sec, characterSlot);
	sWorld.AddSession(m_Session);
	return 0;
}

int WorldSocket::HandleClientVersion(WorldPacket& recvPacket)
{
	WorldPacket packet(S_OPCODE_SERVERVERSION, 31);
	packet << uint8(0x00);
	packet << uint8(0x02);
	packet << uint32(0x00a8c732); // server verion 3.5C Taiwan Server
	packet << uint32(0x00a8c6a7); // cache verion 3.5C Taiwan Server
	packet << uint32(0x77cf6eba); // auth verion 3.5C Taiwan Server
	packet << uint32(0x00a8cdad); // npc verion 3.5C Taiwan Server
	//packet << uint32(WorldTimer::getMSTime());
	packet << uint32(0);
	packet << uint8(0x00);
	packet << uint8(0x00);
	packet << uint8(sConfig.GetIntDefault("L1J.ClientLanguage", 5));
	packet << uint32(0x00);
	packet << uint8(0xae);
	packet << uint8(0xb2);
	SendPacket(packet);
	return 0;
}
