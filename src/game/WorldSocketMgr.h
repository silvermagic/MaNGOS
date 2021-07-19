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

#ifndef __WORLDSOCKETMGR_H
#define __WORLDSOCKETMGR_H

#include <string>
#include <mutex>
#include <atomic>
#include <boost/asio.hpp>
#include "Utilities/Singleton.h"

using boost::asio::ip::tcp;

class WorldSocket;
class ReactorRunnable;

/// 管理所有网络线程和与客户端的连接套接字
class WorldSocketMgr
{
    friend class WorldSocket;
    friend class MaNGOS::OperatorNew<WorldSocketMgr>;
protected:
	WorldSocketMgr();
	virtual ~WorldSocketMgr();
	WorldSocketMgr(const WorldSocketMgr&) = delete;
	WorldSocketMgr& operator=(const WorldSocketMgr&) = delete;

public:
    /// 启动监听套接字，并启动网络线程
    int StartNetwork(boost::asio::io_context &ioContext, uint16 port, std::string& address);

    /// 停止监听套接字和所有网络线程，并且等待所有网络线程退出
    void StopNetwork();

    /// 等待所有网络处理线程退出
    void Wait();

    /// 获取监听地址和端口
    std::string& GetBindAddress() { return m_Addr; }
    uint16 GetBindPort() { return m_Port; }

protected:
	/// 异步套接字监听回调函数，为新的客户端连接创建套接字层，并交由网络线程管理
    void OnSocketOpen(boost::asio::io_context &ioContext, tcp::socket socket);
    /// 启动异步套接字监听
    void StartReactiveIO(boost::asio::io_context &ioContext);

    std::vector<std::shared_ptr<ReactorRunnable> > m_NetThreads; // 网络线程
    size_t m_NetThreadsCount;                                    // 网络线程个数

    int m_SockOutKBuff; // 内核缓冲区大小
    int m_SockOutUBuff; // 用户缓冲区大小
    bool m_UseNoDelay;  // 使用异步IO

    std::string m_Addr; // 监听的网络地址
    uint16 m_Port;      // 监听的网络端口

    std::shared_ptr<tcp::acceptor> m_Acceptor; // 监听套接字
};

#define sWorldSocketMgr MaNGOS::Singleton<WorldSocketMgr>::Instance()

#endif
