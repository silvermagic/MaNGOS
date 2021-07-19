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

#ifndef _WORLDSOCKET_H
#define _WORLDSOCKET_H

#include "Common.h"

class WorldPacket;
class WorldSession;

/**
 * WorldSocket.
 *
 * 该类负责与远程客户端的通信，大多数方法在失败时返回-1，使用时需要使用引用计数(std::shared_ptr<WorldSocket>)
 *
 * 对于输出，当前类使用了一个队列用来保存待发送的数据帧。有些时候当某些内容写入输出缓冲区，但套接字没有立刻激活输出，
 * 例如提示重试等原因，那么我们将等待10ms的间隔再重试(这也是Update()重写方法做的事)，这么做的原因是因为服务端会产生大量的写操作，
 * 但是每次写的数据量又很小，这个概念有点类似TCP_CORK，但是TCP_CORK使用200ms的间隔，这样从网络线程发送数据包的开销将是最小的，
 * 并且能够容忍大量的小字节写入。
 *
 * Update()函数的调用由WorldSocketMgr和ReactorRunnable管理。
 *
 * 对于输入，当前类在handle_input调用中使用了一个1024字节的缓存，
 * 然后将接收到的数据分发到需要的地方，目前与客户端生成的流量非常匹配。
 *
 */

#include <memory>
#include <boost/chrono/chrono.hpp>
#include <boost/asio.hpp>
#include "Utilities/MPSCQueue.h"
#include "Utilities/MessageBuffer.h"
#include "Auth/AuthCrypt.h"

using boost::chrono::steady_clock;
using boost::asio::ip::tcp;

class WorldSocket
{
public:
    /// 声明有元类
    friend class WorldSocketMgr;
    friend class ReactorRunnable;

    /// 声明接收套接字类型
    typedef boost::asio::ip::tcp::acceptor Acceptor;

    /// 互斥锁类型
    typedef std::mutex LockType;
    typedef std::lock_guard<LockType> GuardType;

    WorldSocket(boost::asio::io_context &ioContext, tcp::socket socket);
    virtual ~WorldSocket(void);

    /// 检查套接字是否已经关闭
    bool IsClosed(void) const;

    /// 关闭套接字
    void CloseSocket(void);

    /// 获取连接的对端地址
    const std::string& GetRemoteAddress(void) const;

    /// 套接字层留给会话层的接口，发送一个数据包到客户端，此函数是线程安全的
    void SendPacket(const WorldPacket& pct);

protected:
    /// 读取套接字层中的字节流
    virtual void handle_input();

    /// 将数据帧写入套接字层
    virtual bool handle_output();

    /// 由WorldSocketMgr/ReactorRunnable调用
    void Start();
    bool Update(void);

private:
    /// 用于辅助处理读取的字节流的帮助函数
    bool handle_input_header(void);
    bool handle_input_payload(void);
    void handle_input_missing_data(void);

    /// 处理数据包的发送(数据帧经过加密)
	void handle_output(MessageBuffer &buffer);
	void handle_output_queue(const boost::system::error_code& error);

    /// 处理收到的数据包(数据帧经过解密)
    int ProcessIncoming(WorldPacket &new_pct);

    /// 由ProcessIncoming调用，用于客户端版本认证和玩家账号认证的数据包的处理
    int HandleAuthSession(WorldPacket& recvPacket);
    int HandleClientVersion(WorldPacket& recvPacket);

private:
	/// 套接字层，用于发送和接收数据帧，套接字层散列在不同的网络线程中，线程内的套接字层串行，线程间的套接字层并行，用于提升数据帧加解密的性能，
	/// 天堂数据帧由头部和数据构成，头部大小为2字节(不加密)，记录了整个数据帧的长度(包括首部2字节)
    tcp::socket m_Socket;
	/// 套接字层关闭标志
	std::atomic<bool> m_Closed;

    /// 记录最近一次收到心跳包的时间
    steady_clock::time_point m_LastPingTime;

    /// 记录对端(客户端)的网络地址
    std::string m_Address;

    /// 用于管理套接字层数据帧的加解密，将数据帧转换为可处理的数据包
    AuthCrypt m_Crypt;
    /// 加解密密钥的随机种子数
	uint32 m_Seed;

    /// 用于保护会话层句柄的互斥锁
    LockType m_SessionLock;

    /// 会话层，客户端账号认证完成前的数据包在套接字层处理，认证后的数据包(游戏逻辑)在会话层处理，整个游戏世界的会话层之间都是串行的
    WorldSession* m_Session;

    /// 套接字层使用，用于接收来自客户端的字节流
    MessageBuffer m_InBuffer;
    /// 用于从字节流中提取出一个个数据帧
    MessageBuffer m_HeaderBuffer, m_PacketBuffer;
	/// 数据帧发送队列，套接字层使用，用于保存加密后的数据帧
	std::queue<MessageBuffer> m_OutBuffer;
	/// 套接字层使用，用于串行发送数据帧发送队列中的数据帧
	boost::asio::io_context::strand m_Tx;

    /// 用于数据包发送的高性能并发队列(注意会话层和套接字层运行在不同的Boost ASIO上下文中)，会话层和套接字层共同使用，
    /// 会话层将需要发送到客户端的数据包存入队列，然后套接字层将其取出，并将其加密后组成数据帧，存入数据帧发送队列
    MPSCQueue<WorldPacket> m_OutPacket;
};

#endif  /* _WORLDSOCKET_H */
