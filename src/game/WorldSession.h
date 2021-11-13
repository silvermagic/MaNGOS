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

#ifndef __WORLDSESSION_H
#define __WORLDSESSION_H

#include "Utilities/LockedQueue.h"
#include "Common.h"

class Item;
class Player;
class WorldPacket;
class WorldSocket;
class QueryResult;
class WorldSession;
class LoginQueryHolder;

struct OpcodeHandler;

/// ���ݰ���������������ʹ�ù�������ֻ�����Լ�����Ȥ�����ݰ�
class PacketFilter
{
public:
    explicit PacketFilter(WorldSession* pSession) : m_pSession(pSession) {}
    virtual ~PacketFilter() {}

    virtual bool Process(WorldPacket* /*packet*/) { return true; }
    virtual bool ProcessLogout() const { return true; }

protected:
    WorldSession* const m_pSession;
};

/// �������̰߳�ȫ�����ݰ��������ݰ�����Ϸ����ѭ���п��Բ��д���Ŀǰֻ���ƶ����ݰ����̰߳�ȫ�ģ���Ҫע����ǣ�
/// ������д�������Եģ�����ͬ��ͼ����ҵ��ƶ����ݰ����Բ��д������ǵ�ͼ�ڵ���ҵ��ƶ����ݰ�Ҳ���봮�д���
class MapSessionFilter : public PacketFilter
{
public:
    explicit MapSessionFilter(WorldSession* pSession) : PacketFilter(pSession) {}
    ~MapSessionFilter() {}

    /// ��Map::Update()�е���
    virtual bool Process(WorldPacket* packet) override;
    /// ��Map::Update()�����ǲ��ܴ�����ҵǳ�
    virtual bool ProcessLogout() const override { return false; }
};

/// ��������̰߳�ȫ�����ݰ��������ݰ�����Ϸ����ѭ���б��봮�д���
class WorldSessionFilter : public PacketFilter
{
public:
    explicit WorldSessionFilter(WorldSession* pSession) : PacketFilter(pSession) {}
    ~WorldSessionFilter() {}

    /// ��World::UpdateSessions()�е���
    virtual bool Process(WorldPacket* packet) override;
};

/// �Ự�㣬������������Ϸ�����ｻ���Ľӿ�
class MANGOS_DLL_SPEC WorldSession
{
	friend class WorldSocket;
    friend class CharacterHandler;
public:
    WorldSession(std::string id, WorldSocket* ws, AccountTypes sec, uint8 characterSlot);
    ~WorldSession();

    /// ���»Ự�㣬��World::Update��Map::Update��ִ��
	bool Update(PacketFilter& updater);

    /// ���ڴ����ݿ������ҹ�������
    bool PlayerLoading() const { return m_playerLoading; }
    /// ����ѵǳ���Ϸ
    bool PlayerLogout() const { return m_playerLogout; }
    /// ����ѵǳ���Ϸ���������ڽ�������ݱ��浽���ݿ�
    bool PlayerLogoutWithSave() const { return m_playerLogout && m_playerSave; }

    /// �˺�����
    AccountTypes GetSecurity() const { return m_security; }
    /// �˺�ID
    std::string GetAccountId() const { return m_accountId; }
    /// ��ɫ�۸���(һ���˺������ӵ�ж��ٸ���ɫ)
    uint8 GetCharacterSlot() const { return m_characterSlot; }
    /// ��ȡ��ҿ��Ƶ���Ϸ��ɫ����
    Player* GetPlayer() const { return m_player; }
    void SetSecurity(AccountTypes security) { m_security = security; }
    void SetCharacterSlot(uint8 characterSlot) { m_characterSlot = characterSlot; }
    std::string const& GetRemoteAddress() { return m_Address; }
    void SetPlayer(Player* plr);

    /// �Ŷӵ��룬����������������ʱ�����������˺���֤����Ҫ�Ŷӵȴ������ɫѡȡ����
    void SetInQueue(bool state) { m_inQueue = state; }

    /// ��ҵǳ�
    void LogoutPlayer(bool Save);
    void KickPlayer();

    /// �Ự������Player�Ľӿڣ�����һ�����ݰ����ͻ���
	void SendPacket(const WorldPacket* packet);

public:                                                 /// �����봦�����
    void Handle_NULL(WorldPacket& recvPacket);          /// ������Ĭ�ϴ�����򣬱�ʾδ����
    void Handle_EarlyProccess(WorldPacket& recvPacket); /// ָ��������������׽��ֲ㴦��
    void Handle_ServerSide(WorldPacket& recvPacket);    /// �������ǽ��޷�����ڲ�ʹ�ã������ܴӿͻ��˻�ȡ
    void Handle_Deprecated(WorldPacket& recvPacket);    /// �Ѿ������Ĳ����룬��¼��־�������Ų�ͻ�����Щ�����߼�û��ɾ��

    void HandleCommonClick(WorldPacket &recvPacket);     /// ��ҽ�ɫѡȡ���ݰ����������ɫ�����첽��ѯ����
	void HandleCharEnum(QueryResult* result);            /// ��ҽ�ɫѡȡ�ص��������о����ӵ�еĽ�ɫ�б�
    void HandleLoginToServer(WorldPacket &recvPacket);   /// ��ҵ������ݰ�����������������첽��������
    void HandlePlayerLogin(LoginQueryHolder *holder);    /// ��ҵ���ص�����������Player����

	void HandleMovementOpcodes(WorldPacket& recvPacket); /// ����ƶ����ݰ�����

	void HandleCastSpellOpcode(WorldPacket& recvPacket); /// ���ʩ������

	void HandleUseItemOpcode(WorldPacket& recvPacket); /// ����ʹ�ô���
	void HandleEquipItem(Item *pItem);

protected:
	/// �Ự�������׽��ֲ�Ľӿڣ��׽��ֲ㽫����֡���ܳ����ݰ���ͨ���˽ӿڽ����ݰ�·�ɵ��Ự��
	void QueuePacket(WorldPacket* new_packet);

	/// ִ�в����봦�����
    void ExecuteOpcode(OpcodeHandler const& opHandle, WorldPacket* packet);

    /// ��־��������
    void LogUnexpectedOpcode(WorldPacket* packet, const char* reason);

    uint32 m_GUIDLow;
    Player* m_player;        // ��ҵ�ǰ���ƵĽ�ɫ����
    WorldSocket* m_Socket;   // �׽��ֲ�
    std::string m_Address;   // �ͻ��������ַ

    AccountTypes m_security; // �˺�����
    std::string m_accountId; // �˺�ID
    uint8 m_characterSlot;   // �˺������ӵ�еĽ�ɫ����

    bool m_inQueue;          // ������ǰ�Ự�Ƿ��ڵ���ȴ���(�˺���֤��ɣ�������δ�����ɫѡ�Ž���)
    bool m_playerLoading;    // ���ڴ����ݿ��м��ؽ�ɫ����
    bool m_playerLogout;     // ����뿪��Ϸ����
    bool m_playerSave;       // �Ƿ����ڱ���������ݵ����ݿ�
    MaNGOS::LockedQueue<WorldPacket*> m_recvQueue; // ���ݰ����ն��У����׽��ֲ��ܰ�ȫ�Ľ����ݰ�·�ɵ��Ự��
};
#endif
