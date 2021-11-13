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
 * ���ฺ����Զ�̿ͻ��˵�ͨ�ţ������������ʧ��ʱ����-1��ʹ��ʱ��Ҫʹ�����ü���(std::shared_ptr<WorldSocket>)
 *
 * �����������ǰ��ʹ����һ������������������͵�����֡����Щʱ��ĳЩ����д����������������׽���û�����̼��������
 * ������ʾ���Ե�ԭ����ô���ǽ��ȴ�10ms�ļ��������(��Ҳ��Update()��д����������)����ô����ԭ������Ϊ����˻����������д������
 * ����ÿ��д���������ֺ�С����������е�����TCP_CORK������TCP_CORKʹ��200ms�ļ���������������̷߳������ݰ��Ŀ���������С�ģ�
 * �����ܹ����̴�����С�ֽ�д�롣
 *
 * Update()�����ĵ�����WorldSocketMgr��ReactorRunnable����
 *
 * �������룬��ǰ����handle_input������ʹ����һ��1024�ֽڵĻ��棬
 * Ȼ�󽫽��յ������ݷַ�����Ҫ�ĵط���Ŀǰ��ͻ������ɵ������ǳ�ƥ�䡣
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
    /// ������Ԫ��
    friend class WorldSocketMgr;
    friend class ReactorRunnable;

    /// ���������׽�������
    typedef boost::asio::ip::tcp::acceptor Acceptor;

    /// ����������
    typedef std::mutex LockType;
    typedef std::lock_guard<LockType> GuardType;

    WorldSocket(boost::asio::io_context &ioContext, tcp::socket socket);
    virtual ~WorldSocket(void);

    /// ����׽����Ƿ��Ѿ��ر�
    bool IsClosed(void) const;

    /// �ر��׽���
    void CloseSocket(void);

    /// ��ȡ���ӵĶԶ˵�ַ
    const std::string& GetRemoteAddress(void) const;

    /// �׽��ֲ������Ự��Ľӿڣ�����һ�����ݰ����ͻ��ˣ��˺������̰߳�ȫ��
    void SendPacket(const WorldPacket& pct);

protected:
    /// ��ȡ�׽��ֲ��е��ֽ���
    virtual void handle_input();

    /// ������֡д���׽��ֲ�
    virtual bool handle_output();

    /// ��WorldSocketMgr/ReactorRunnable����
    void Start();
    bool Update(void);

private:
    /// ���ڸ��������ȡ���ֽ����İ�������
    bool handle_input_header(void);
    bool handle_input_payload(void);
    void handle_input_missing_data(void);

    /// �������ݰ��ķ���(����֡��������)
	void handle_output(MessageBuffer &buffer);
	void handle_output_queue(const boost::system::error_code& error);

    /// �����յ������ݰ�(����֡��������)
    int ProcessIncoming(WorldPacket &new_pct);

    /// ��ProcessIncoming���ã����ڿͻ��˰汾��֤������˺���֤�����ݰ��Ĵ���
    int HandleAuthSession(WorldPacket& recvPacket);
    int HandleClientVersion(WorldPacket& recvPacket);

private:
	/// �׽��ֲ㣬���ڷ��ͺͽ�������֡���׽��ֲ�ɢ���ڲ�ͬ�������߳��У��߳��ڵ��׽��ֲ㴮�У��̼߳���׽��ֲ㲢�У�������������֡�ӽ��ܵ����ܣ�
	/// ��������֡��ͷ�������ݹ��ɣ�ͷ����СΪ2�ֽ�(������)����¼����������֡�ĳ���(�����ײ�2�ֽ�)
    tcp::socket m_Socket;
	/// �׽��ֲ�رձ�־
	std::atomic<bool> m_Closed;

    /// ��¼���һ���յ���������ʱ��
    steady_clock::time_point m_LastPingTime;

    /// ��¼�Զ�(�ͻ���)�������ַ
    std::string m_Address;

    /// ���ڹ����׽��ֲ�����֡�ļӽ��ܣ�������֡ת��Ϊ�ɴ�������ݰ�
    AuthCrypt m_Crypt;
    /// �ӽ�����Կ�����������
	uint32 m_Seed;

    /// ���ڱ����Ự�����Ļ�����
    LockType m_SessionLock;

    /// �Ự�㣬�ͻ����˺���֤���ǰ�����ݰ����׽��ֲ㴦����֤������ݰ�(��Ϸ�߼�)�ڻỰ�㴦��������Ϸ����ĻỰ��֮�䶼�Ǵ��е�
    WorldSession* m_Session;

    /// �׽��ֲ�ʹ�ã����ڽ������Կͻ��˵��ֽ���
    MessageBuffer m_InBuffer;
    /// ���ڴ��ֽ�������ȡ��һ��������֡
    MessageBuffer m_HeaderBuffer, m_PacketBuffer;
	/// ����֡���Ͷ��У��׽��ֲ�ʹ�ã����ڱ�����ܺ������֡
	std::queue<MessageBuffer> m_OutBuffer;
	/// �׽��ֲ�ʹ�ã����ڴ��з�������֡���Ͷ����е�����֡
	boost::asio::io_context::strand m_Tx;

    /// �������ݰ����͵ĸ����ܲ�������(ע��Ự����׽��ֲ������ڲ�ͬ��Boost ASIO��������)���Ự����׽��ֲ㹲ͬʹ�ã�
    /// �Ự�㽫��Ҫ���͵��ͻ��˵����ݰ�������У�Ȼ���׽��ֲ㽫��ȡ������������ܺ��������֡����������֡���Ͷ���
    MPSCQueue<WorldPacket> m_OutPacket;
};

#endif  /* _WORLDSOCKET_H */
