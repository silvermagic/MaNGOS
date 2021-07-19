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

/// 数据包过滤器，调用者使用过滤器来只处理自己感兴趣的数据包
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

/// 仅处理线程安全的数据包，即数据包在游戏世界循环中可以并行处理，目前只有移动数据包是线程安全的，需要注意的是，
/// 这个并行处理是相对的，即不同地图的玩家的移动数据包可以并行处理，但是地图内的玩家的移动数据包也必须串行处理
class MapSessionFilter : public PacketFilter
{
public:
    explicit MapSessionFilter(WorldSession* pSession) : PacketFilter(pSession) {}
    ~MapSessionFilter() {}

    /// 在Map::Update()中调用
    virtual bool Process(WorldPacket* packet) override;
    /// 在Map::Update()中我们不能处理玩家登出
    virtual bool ProcessLogout() const override { return false; }
};

/// 仅处理非线程安全的数据包，即数据包在游戏世界循环中必须串行处理
class WorldSessionFilter : public PacketFilter
{
public:
    explicit WorldSessionFilter(WorldSession* pSession) : PacketFilter(pSession) {}
    ~WorldSessionFilter() {}

    /// 在World::UpdateSessions()中调用
    virtual bool Process(WorldPacket* packet) override;
};

/// 会话层，玩家与玩家在游戏世界里交互的接口
class MANGOS_DLL_SPEC WorldSession
{
	friend class WorldSocket;
    friend class CharacterHandler;
public:
    WorldSession(std::string id, WorldSocket* ws, AccountTypes sec, uint8 characterSlot);
    ~WorldSession();

    /// 更新会话层，在World::Update和Map::Update中执行
	bool Update(PacketFilter& updater);

    /// 正在从数据库加载玩家关联数据
    bool PlayerLoading() const { return m_playerLoading; }
    /// 玩家已登出游戏
    bool PlayerLogout() const { return m_playerLogout; }
    /// 玩家已登出游戏，并且正在将玩家数据保存到数据库
    bool PlayerLogoutWithSave() const { return m_playerLogout && m_playerSave; }

    /// 账号类型
    AccountTypes GetSecurity() const { return m_security; }
    /// 账号ID
    std::string GetAccountId() const { return m_accountId; }
    /// 角色槽个数(一个账号最多能拥有多少个角色)
    uint8 GetCharacterSlot() const { return m_characterSlot; }
    /// 获取玩家控制的游戏角色对象
    Player* GetPlayer() const { return m_player; }
    void SetSecurity(AccountTypes security) { m_security = security; }
    void SetCharacterSlot(uint8 characterSlot) { m_characterSlot = characterSlot; }
    std::string const& GetRemoteAddress() { return m_Address; }
    void SetPlayer(Player* plr);

    /// 排队登入，当服务器人数过多时，玩家在完成账号验证后，需要排队等待进入角色选取界面
    void SetInQueue(bool state) { m_inQueue = state; }

    /// 玩家登出
    void LogoutPlayer(bool Save);
    void KickPlayer();

    /// 会话层留给Player的接口，发送一个数据包到客户端
	void SendPacket(const WorldPacket* packet);

public:                                                 /// 操作码处理程序
    void Handle_NULL(WorldPacket& recvPacket);          /// 操作码默认处理程序，表示未处理
    void Handle_EarlyProccess(WorldPacket& recvPacket); /// 指明操作码必须在套接字层处理
    void Handle_ServerSide(WorldPacket& recvPacket);    /// 操作码是仅限服务端内部使用，不可能从客户端获取
    void Handle_Deprecated(WorldPacket& recvPacket);    /// 已经废弃的操作码，记录日志，方便排查客户端哪些废弃逻辑没有删掉

    void HandleCommonClick(WorldPacket &recvPacket);     /// 玩家角色选取数据包处理，发起角色数据异步查询请求
	void HandleCharEnum(QueryResult* result);            /// 玩家角色选取回调函数，列举玩家拥有的角色列表
    void HandleLoginToServer(WorldPacket &recvPacket);   /// 玩家登入数据包处理，发起玩家数据异步加载请求
    void HandlePlayerLogin(LoginQueryHolder *holder);    /// 玩家登入回调函数，创建Player对象

	void HandleMovementOpcodes(WorldPacket& recvPacket); /// 玩家移动数据包处理

	void HandleCastSpellOpcode(WorldPacket& recvPacket); /// 玩家施法处理

	void HandleUseItemOpcode(WorldPacket& recvPacket); /// 道具使用处理
	void HandleEquipItem(Item *pItem);

protected:
	/// 会话层留给套接字层的接口，套接字层将数据帧解密成数据包后，通过此接口将数据包路由到会话层
	void QueuePacket(WorldPacket* new_packet);

	/// 执行操作码处理程序
    void ExecuteOpcode(OpcodeHandler const& opHandle, WorldPacket* packet);

    /// 日志辅助函数
    void LogUnexpectedOpcode(WorldPacket* packet, const char* reason);

    uint32 m_GUIDLow;
    Player* m_player;        // 玩家当前控制的角色对象
    WorldSocket* m_Socket;   // 套接字层
    std::string m_Address;   // 客户端网络地址

    AccountTypes m_security; // 账号类型
    std::string m_accountId; // 账号ID
    uint8 m_characterSlot;   // 账号最多能拥有的角色个数

    bool m_inQueue;          // 表明当前会话是否处于登入等待中(账号认证完成，但是尚未进入角色选着界面)
    bool m_playerLoading;    // 正在从数据库中加载角色数据
    bool m_playerLogout;     // 玩家离开游戏处理
    bool m_playerSave;       // 是否正在保存玩家数据到数据库
    MaNGOS::LockedQueue<WorldPacket*> m_recvQueue; // 数据包接收队列，让套接字层能安全的将数据包路由到会话层
};
#endif
