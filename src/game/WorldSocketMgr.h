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

/// �������������̺߳���ͻ��˵������׽���
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
    /// ���������׽��֣������������߳�
    int StartNetwork(boost::asio::io_context &ioContext, uint16 port, std::string& address);

    /// ֹͣ�����׽��ֺ����������̣߳����ҵȴ����������߳��˳�
    void StopNetwork();

    /// �ȴ��������紦���߳��˳�
    void Wait();

    /// ��ȡ������ַ�Ͷ˿�
    std::string& GetBindAddress() { return m_Addr; }
    uint16 GetBindPort() { return m_Port; }

protected:
	/// �첽�׽��ּ����ص�������Ϊ�µĿͻ������Ӵ����׽��ֲ㣬�����������̹߳���
    void OnSocketOpen(boost::asio::io_context &ioContext, tcp::socket socket);
    /// �����첽�׽��ּ���
    void StartReactiveIO(boost::asio::io_context &ioContext);

    std::vector<std::shared_ptr<ReactorRunnable> > m_NetThreads; // �����߳�
    size_t m_NetThreadsCount;                                    // �����̸߳���

    int m_SockOutKBuff; // �ں˻�������С
    int m_SockOutUBuff; // �û���������С
    bool m_UseNoDelay;  // ʹ���첽IO

    std::string m_Addr; // �����������ַ
    uint16 m_Port;      // ����������˿�

    std::shared_ptr<tcp::acceptor> m_Acceptor; // �����׽���
};

#define sWorldSocketMgr MaNGOS::Singleton<WorldSocketMgr>::Instance()

#endif
