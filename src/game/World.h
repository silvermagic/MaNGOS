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

#ifndef __WORLD_H
#define __WORLD_H

#include <map>
#include <set>
#include <list>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Utilities/Singleton.h"
#include "Utilities/Timer.h"
#include "Common.h"
#include "SharedDefines.h"

class WorldPacket;
class WorldSession;
class Player;
class SqlResultQueue;
class QueryResult;
class WorldSocket;

enum ShutdownMask
{
    SHUTDOWN_MASK_RESTART = 1,
    SHUTDOWN_MASK_IDLE    = 2,
};

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

/// 游戏世界定时器
enum WorldTimers
{
    WUPDATE_WEATHERS    = 0, /// 天气更新定时器
    WUPDATE_COUNT       = 1
};

/// 无符号整数类型配置
enum eConfigUInt32Values
{
    CONFIG_UINT32_MIN_PLAYER_NAME,
    CONFIG_UINT32_STRICT_PLAYER_NAMES,
    CONFIG_UINT32_MIN_CHARTER_NAME,
    CONFIG_UINT32_STRICT_CHARTER_NAMES,
    CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE,
    CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT,
    CONFIG_UINT32_PORT_WORLD,
    CONFIG_UINT32_INTERVAL_CHANGEWEATHER,
	CONFIG_UINT32_CLIENT_LANGUAGE,              /// 客户端语言
    CONFIG_UINT32_VALUE_COUNT
};

/// 有符号整数类型配置
enum eConfigInt32Values
{
    CONFIG_INT32_VALUE_COUNT = 1
};

/// 浮点类型配置
enum eConfigFloatValues
{
    CONFIG_FLOAT_VALUE_COUNT = 1
};

/// 布尔类型配置
enum eConfigBoolValues
{
    CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET,
    CONFIG_BOOL_VALUE_COUNT
};

/// 命令持有者，用于保存待延迟执行的游戏指令
struct CliCommandHolder
{
    typedef void Print(void*, const char*);
    typedef void CommandFinished(void*, bool success);

    /// 如果是服务端控制台输入的命令则为0，如果是远程访问则为登入账号ID
    uint32 m_cliAccountId;
    /// 命令执行权限
    AccountTypes m_cliAccessLevel;
    /// 游戏指令参数
    void* m_callbackArg;
    /// 游戏指令
    char* m_command;
    Print* m_print;
    CommandFinished* m_commandFinished;

    CliCommandHolder(uint32 accountId, AccountTypes cliAccessLevel, void* callbackArg, const char* command, Print* zprint, CommandFinished* commandFinished)
        : m_cliAccountId(accountId), m_cliAccessLevel(cliAccessLevel), m_callbackArg(callbackArg), m_print(zprint), m_commandFinished(commandFinished)
    {
        size_t len = strlen(command) + 1;
        m_command = new char[len];
        memcpy(m_command, command, len);
    }

    ~CliCommandHolder() { delete[] m_command; }
};

/// 游戏世界
class World
{
	friend class MaNGOS::OperatorNew<World>;
protected:
	World();
	~World();
	World(const World&) = delete;
	World& operator=(const World&) = delete;

public:
	/// 游戏世界更新
	void Update(uint32 diff);
	/// 更新玩家会话层，处理来自客户端的数据包
	void UpdateSessions(uint32 diff);

	/// 关闭游戏世界
	bool IsShutdowning() const { return m_ShutdownTimer > 0; }
	void ShutdownServ(uint32 time, uint32 options, uint8 exitcode);
	void ShutdownCancel();
	void ShutdownMsg(bool show = false, Player* player = nullptr);
	static uint8 GetExitCode() { return m_ExitCode; }
	static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }
	static bool IsStopped() { return m_stopEvent; }
	/// 执行游戏停止前的清理动作
    void CleanupsBeforeStop();
	/// 关闭所有会话或关闭低等级的账号会话
	void KickAll();
	void KickAllLess(AccountTypes sec);

	/// 玩家会话管理
    WorldSession* FindSession(std::string id) const;
    void AddSession(WorldSession* s); // 留给套接字层的接口，用于将玩家会话异步加入游戏世界
    bool RemoveSession(std::string id);
	/// 等待登入的会话管理
	typedef std::list<WorldSession*> Queue;
	void AddQueuedSession(WorldSession*);
	bool RemoveQueuedSession(WorldSession* session);
	int32 GetQueuedSessionPos(WorldSession*);
	/// 获取当前等待登入的会话数量和活跃的会话数量
    uint32 GetActiveAndQueuedSessionCount() const { return m_sessions.size(); }
    uint32 GetActiveSessionCount() const { return m_sessions.size() - m_QueuedSessions.size(); }
    uint32 GetQueuedSessionCount() const { return m_QueuedSessions.size(); }
	/// 更新最大等待登入的会话数量和最大活跃的会话数量
	void UpdateMaxSessionCounters();
    /// 获取迄今为止最大等待登入的会话数量和最大活跃的会话数量
    uint32 GetMaxQueuedSessionCount() const { return m_maxQueuedSessionCount; }
    uint32 GetMaxActiveSessionCount() const { return m_maxActiveSessionCount; }

	/// 限制服务器的活跃会话数量，如果limit大于0，则按玩家总数进行限制，如果limit小于0(相当于-AccountTypes)，则按账号等级进行限制
	void SetPlayerLimit(int32 limit, bool needUpdate = false);
	/// 获取服务器的玩家总数限制条件
	uint32 GetPlayerAmountLimit() const { return m_playerLimit >= 0 ? m_playerLimit : 0; }
	/// 获取服务的账号等级限制条件
	AccountTypes GetPlayerSecurityLimit() const { return m_playerLimit <= 0 ? AccountTypes(-m_playerLimit) : SEC_PLAYER; }

    /// 天气系统
	void UpdateWeather();
	WeatherState GetWeatherState() const;

	/// 处理来自服务端控制台或远程访问的命令行处理
	void ProcessCliCommands();
	void QueueCliCommand(CliCommandHolder* commandHolder) { cliCmdQueue.add(commandHolder); }

	/// 数据库异步请求处理
	void UpdateResultQueue();
	void InitResultQueue();

	/// 发送广播消息
	void SendGlobalMessage(WorldPacket* packet);

	/// 获取下次每日任务和每周任务的重置时间
	time_t GetNextDailyQuestsResetTime() const { return m_NextDailyQuestReset; }
	time_t GetNextWeeklyQuestsResetTime() const { return m_NextWeeklyQuestReset; }

	/// 版本信息查询
	void LoadDBVersion();
	const char* GetDBVersion() { return m_DBVersion.c_str(); }
	const char* GetCreatureEventAIVersion() { return m_CreatureEventAIVersion.c_str(); }

	/// 加载并初始化游戏世界配置
	void LoadConfigSettings(bool reload = false);
	void SetInitialWorldSettings();
	/// 获取游戏数据文件(例如地图信息)存储路径
	std::string GetDataPath() const { return m_dataPath; }
	/// 获取客户端语言
	LocaleConstant GetLocaleConstant() const { return m_defaultLocale; }

    /// 浮点数类型配置信息存取
    void SetConfig(eConfigFloatValues index, float value) { m_configFloatValues[index] = value; }
    float GetConfig(eConfigFloatValues rate) const { return m_configFloatValues[rate]; }

    /// 无符号整数类型配置信息存取
    void SetConfig(eConfigUInt32Values index, uint32 value) { m_configUint32Values[index] = value; }
    uint32 GetConfig(eConfigUInt32Values index) const { return m_configUint32Values[index]; }

    /// 有符号整数类型配置信息存取
    void SetConfig(eConfigInt32Values index, int32 value) { m_configInt32Values[index] = value; }
    int32 GetConfig(eConfigInt32Values index) const { return m_configInt32Values[index]; }

    /// 布尔类型配置信息存取
    void SetConfig(eConfigBoolValues index, bool value) { m_configBoolValues[index] = value; }
    bool GetConfig(eConfigBoolValues index) const { return m_configBoolValues[index]; }

protected:
	/// 更新游戏世界，并检查是否
	void UpdateGameTime();

	/// 每日任务和每周任务定时器管理
    void InitDailyQuestResetTime();
    void InitWeeklyQuestResetTime();
    void ResetDailyQuests();
    void ResetWeeklyQuests();

protected:
	/// 尝试将新的玩家会话加入游戏世界的活跃会话，如果游戏世界满了，则加入登入等待队列
	void AddNewSession(WorldSession* s);

	/// 生成新的天气
	bool ReGenerateWeather();

	/// 用于游戏配置初始化设置
    void SetConfig(eConfigUInt32Values index, const char* fieldname, uint32 defvalue);
    void SetConfig(eConfigInt32Values index, const char* fieldname, int32 defvalue);
    void SetConfig(eConfigFloatValues index, const char* fieldname, float defvalue);
    void SetConfig(eConfigBoolValues index, const char* fieldname, bool defvalue);
    void SetConfigPos(eConfigFloatValues index, const char* fieldname, float defvalue);
    void SetConfigMin(eConfigUInt32Values index, const char* fieldname, uint32 defvalue, uint32 minvalue);
    void SetConfigMin(eConfigInt32Values index, const char* fieldname, int32 defvalue, int32 minvalue);
    void SetConfigMin(eConfigFloatValues index, const char* fieldname, float defvalue, float minvalue);
    void SetConfigMinMax(eConfigUInt32Values index, const char* fieldname, uint32 defvalue, uint32 minvalue, uint32 maxvalue);
    void SetConfigMinMax(eConfigInt32Values index, const char* fieldname, int32 defvalue, int32 minvalue, int32 maxvalue);
    void SetConfigMinMax(eConfigFloatValues index, const char* fieldname, float defvalue, float minvalue, float maxvalue);
    bool ConfigNoReload(bool reload, eConfigUInt32Values index, const char* fieldname, uint32 defvalue);
    bool ConfigNoReload(bool reload, eConfigInt32Values index, const char* fieldname, int32 defvalue);
    bool ConfigNoReload(bool reload, eConfigFloatValues index, const char* fieldname, float defvalue);
    bool ConfigNoReload(bool reload, eConfigBoolValues index, const char* fieldname, bool defvalue);

public:
	static volatile uint32 m_worldLoopCounter;
protected:
    static volatile bool m_stopEvent;
    static uint8 m_ExitCode;
    uint32 m_ShutdownTimer;
    uint32 m_ShutdownMask;

    IntervalTimer m_timers[WUPDATE_COUNT]; /// 游戏世界定时器
	WeatherType m_weather;                 /// 游戏世界天气类型
	float m_grade;                         /// 游戏世界天气恶劣程度 小雨 中雨 大雨

    typedef UNORDERED_MAP<std::string, WorldSession*> SessionMap;
    SessionMap m_sessions;                                  /// 玩家活跃会话(玩家已经进入到角色选取界面或已经进入游戏世界了)
	Queue m_QueuedSessions;                                 /// 玩家会话登入等待队列(账号认证完成，但尚未进入到角色选取界面)
	MaNGOS::LockedQueue<WorldSession*> addSessQueue;        /// 玩家会话异步加入队列(账号认证完成后在套接字层刚创建的会话)
	uint32 m_maxActiveSessionCount;                         /// 历史最高活跃度
    uint32 m_maxQueuedSessionCount;                         /// 历史最高拥塞度

	std::string m_DBVersion;                                /// DB版本信息
	std::string m_CreatureEventAIVersion;                   /// 怪物AI版本信息

    uint32 m_configUint32Values[CONFIG_UINT32_VALUE_COUNT]; /// 详见eConfigUInt32Values
    int32 m_configInt32Values[CONFIG_INT32_VALUE_COUNT];    /// 详见eConfigInt32Values
    float m_configFloatValues[CONFIG_FLOAT_VALUE_COUNT];    /// 详见eConfigFloatValues
    bool m_configBoolValues[CONFIG_BOOL_VALUE_COUNT];       /// 详见eConfigBoolValues
    int32 m_playerLimit;                                    /// 服务器人数限制方式，0表示不限制，大于0表示限制玩家个数，小于0表示限制玩家账号等级
    LocaleConstant m_defaultLocale;                         /// 客户端语言
    std::string m_dataPath;                                 /// 游戏数据文件目录，例如地图

    MaNGOS::LockedQueue<CliCommandHolder*> cliCmdQueue;     /// 异步添加来自控制台或远程访问的命令行(线程安全)

    time_t m_NextDailyQuestReset;                           /// 每日任务重置时间
    time_t m_NextWeeklyQuestReset;                          /// 每周任务重置时间
};

extern uint32 realmID; /// 游戏服务区编号，目前天堂就一个大区，未来可以考虑创建一个类似游戏大厅的选区界面

#define sWorld MaNGOS::Singleton<World>::Instance()

#endif
