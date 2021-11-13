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

#include <set>
#include <atomic>
#include <memory>
#include <boost/asio/ip/address.hpp>
#include <boost/thread.hpp>
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Utilities/SingletonImpl.h"
#include "Common.h"
#include "Log.h"
#include "Opcodes.h"
#include "World.h"
#include "WorldSocket.h"
#include "WorldSocketMgr.h"

#if BOOST_VERSION >= 106600
#define MaNGOS_MAX_LISTEN_CONNECTIONS boost::asio::socket_base::max_listen_connections
#else
#define MaNGOS_MAX_LISTEN_CONNECTIONS boost::asio::socket_base::max_connections
#endif

INSTANTIATE_SINGLETON_1(WorldSocketMgr);

/**
 * �����߳�
 */
class ReactorRunnable
{
public:
    ReactorRunnable() :
        m_Connections(0),
        m_Stopped(false)
    {
    }

    virtual ~ReactorRunnable()
    {
        Stop();
    }

    void Start()
    {
        if (m_NetThread.joinable())
		{
        	return;
		}

        m_NetThread = boost::thread(boost::bind(&ReactorRunnable::Run, this));
    }

    void Stop()
    {
        m_Stopped = true;
    }

    void Wait()
    {
        if (m_NetThread.joinable())
		{
        	m_NetThread.join();
		}
    }

    int Connections()
    {
        return m_Connections.load();
    }

    /// ���׽��ֲ���ӵ���ʱ���У��ȴ��´θ���ʱ��ʽ���������̹߳���
    int AddSocket(std::shared_ptr<WorldSocket> ws)
    {
        boost::lock_guard<boost::mutex> guard(m_NewSockets_Lock);

        ++m_Connections;
        m_NewSockets.push_back(ws);

        return 0;
    }

protected:
	/// ���׽��ִ���ʱ������ʽ���������̹߳���
    void AddNewSockets()
    {
        boost::lock_guard<boost::mutex> guard(m_NewSockets_Lock);

        if (m_NewSockets.empty())
		{
        	return;
		}

        for (auto iter = m_NewSockets.begin(); iter != m_NewSockets.end(); ++iter)
        {
            std::shared_ptr<WorldSocket> ws = (*iter);

            if (ws->IsClosed())
            {
                --m_Connections;
            }
            else
			{
            	m_Sockets.push_back(ws);
			}
        }

        m_NewSockets.clear();
    }

    void Run()
    {
        DEBUG_LOG("Network Thread Starting");

        WorldDatabase.ThreadStart();

        while (!m_Stopped)
		{
			Update();
			boost::this_thread::sleep_for(boost::chrono::microseconds(10));
		}

        WorldDatabase.ThreadEnd();

        DEBUG_LOG("Network Thread Exitting");

        return;
    }

    /// �������̹߳���������׽��ֲ�ִ��WorldSocket::Update�������������֡����ͷ���
    void Update()
    {
        AddNewSockets();

		m_Sockets.erase(std::remove_if(m_Sockets.begin(), m_Sockets.end(), [this](std::shared_ptr<WorldSocket> iter)
		{
			if (!iter->Update())
			{
				if (!iter->IsClosed())
				{
					iter->CloseSocket();
				}

				--m_Connections;
				return true;
			}

			return false;
		}), m_Sockets.end());
    }

private:
    typedef std::vector<std::shared_ptr<WorldSocket> > SocketContainer;

    std::atomic_int m_Connections;  // ��¼��ǰ������׽��ֲ����
    std::atomic<bool> m_Stopped;    // �Ƿ�ֹͣ

    boost::thread m_NetThread;      // �����߳̾��

	SocketContainer m_Sockets;      // �����̵߳�ǰ����������׽��ֲ�

	SocketContainer m_NewSockets;   // ���׽��ֲ����б����ڰ�ȫ�Ľ������̴߳������׽���������ӵ������߳��е���ʱ�б�
    boost::mutex m_NewSockets_Lock; // �������µ��׽��ֲ��б�ķ���
};

WorldSocketMgr::WorldSocketMgr():
    m_NetThreads(),
    m_NetThreadsCount(0),
    m_SockOutKBuff(-1),
    m_SockOutUBuff(65536),
    m_UseNoDelay(true),
    m_Addr("127.0.0.1"),
    m_Port(0),
    m_Acceptor(nullptr)
{
    InitializeOpcodes();
}

WorldSocketMgr::~WorldSocketMgr()
{
    m_NetThreads.clear();
    m_Acceptor = nullptr;
}

int WorldSocketMgr::StartNetwork(boost::asio::io_context &ioContext, uint16 port, std::string& address)
{
    m_Addr = address;
    m_Port = port;
    m_UseNoDelay = sConfig.GetBoolDefault("Network.TcpNodelay", true);
    // -1 means use default
    m_SockOutKBuff = sConfig.GetIntDefault("Network.OutKBuff", -1);
    m_SockOutUBuff = sConfig.GetIntDefault("Network.OutUBuff", 65536);
    if (m_SockOutUBuff <= 0)
    {
        sLog.outError("Network.OutUBuff is wrong in your config file");
        return -1;
    }

    int num_threads = sConfig.GetIntDefault("Network.Threads", 1);
    if (num_threads <= 0)
    {
        sLog.outError("Network.Threads is wrong in your config file");
        return -1;
    }
    m_NetThreadsCount = static_cast<size_t>(num_threads);
    m_NetThreads.resize(m_NetThreadsCount);
    BASIC_LOG("Max allowed socket connections %d", MaNGOS_MAX_LISTEN_CONNECTIONS);
    for (size_t i = 0; i < m_NetThreadsCount; ++i)
    {
        auto nt = std::make_shared<ReactorRunnable>();
        nt->Start();
        m_NetThreads[i] = nt;
    }

    try
    {
        m_Acceptor = std::make_shared<tcp::acceptor>(ioContext);
        tcp::endpoint ep(boost::asio::ip::make_address(m_Addr), m_Port);
        m_Acceptor->open(ep.protocol());
        m_Acceptor->bind(ep);
        m_Acceptor->listen(MaNGOS_MAX_LISTEN_CONNECTIONS);
        StartReactiveIO(ioContext);
    }
    catch (boost::system::system_error const& err)
    {
        sLog.outError("Failed to initialize client's socket %s", err.what());
        return -1;
    }

    return 0;
}

void WorldSocketMgr::StopNetwork()
{
    if (m_Acceptor)
    {
        boost::system::error_code ec;
        m_Acceptor->close(ec);
        if (ec)
		{
        	sLog.outError("WorldSocketMgr::StopNetwork: close accept: %s", ec.message().c_str());
		}
    }

    if (m_NetThreadsCount != 0)
    {
        for (size_t i = 0; i < m_NetThreadsCount; ++i)
		{
        	m_NetThreads[i]->Stop();
		}
    }

    Wait();
}

void WorldSocketMgr::Wait()
{
    if (m_NetThreadsCount != 0)
    {
        for (size_t i = 0; i < m_NetThreadsCount; ++i)
		{
        	m_NetThreads[i]->Wait();
		}
    }
}

void WorldSocketMgr::OnSocketOpen(boost::asio::io_context &ioContext, tcp::socket socket)
{
    try
    {
        if (m_SockOutKBuff >= 0)
        {
            socket.set_option(boost::asio::socket_base::send_buffer_size(m_SockOutKBuff));
        }

        if (m_UseNoDelay)
        {
            socket.set_option(boost::asio::ip::tcp::no_delay(true));
        }

        /// �ҵ�һ����Կ��е������߳�
        size_t min = 0;
        MANGOS_ASSERT(m_NetThreadsCount >= 0);
        for (size_t i = 1; i < m_NetThreadsCount; ++i)
		{
			if (m_NetThreads[i]->Connections() < m_NetThreads[min]->Connections())
			{
				min = i;
			}
		}

        /// �����µ��׽��ֲ����
        auto ws = std::make_shared<WorldSocket>(ioContext, std::move(socket));
        /// �����׽��ֲ���첽������
        ws->Start();
        /// ���׽��ֲ�����������̹߳���
        m_NetThreads[min]->AddSocket(ws);
    }
    catch (boost::system::system_error const& err)
    {
        sLog.outError("WorldSocketMgr::OnSocketOpen %s", err.what());
    }
}

void WorldSocketMgr::StartReactiveIO(boost::asio::io_context &ioContext)
{
	m_Acceptor->async_accept([this, &ioContext](boost::system::error_code ec, tcp::socket socket)
	{
		if (!ec)
		{
			OnSocketOpen(ioContext, std::move(socket));
		}

		if (!World::IsStopped())
		{
			StartReactiveIO(ioContext);
		}
	});
}
