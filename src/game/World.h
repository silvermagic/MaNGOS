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

/// ��Ϸ���綨ʱ��
enum WorldTimers
{
    WUPDATE_WEATHERS    = 0, /// �������¶�ʱ��
    WUPDATE_COUNT       = 1
};

/// �޷���������������
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
	CONFIG_UINT32_CLIENT_LANGUAGE,              /// �ͻ�������
    CONFIG_UINT32_VALUE_COUNT
};

/// �з���������������
enum eConfigInt32Values
{
    CONFIG_INT32_VALUE_COUNT = 1
};

/// ������������
enum eConfigFloatValues
{
    CONFIG_FLOAT_VALUE_COUNT = 1
};

/// ������������
enum eConfigBoolValues
{
    CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET,
    CONFIG_BOOL_VALUE_COUNT
};

/// ��������ߣ����ڱ�����ӳ�ִ�е���Ϸָ��
struct CliCommandHolder
{
    typedef void Print(void*, const char*);
    typedef void CommandFinished(void*, bool success);

    /// ����Ƿ���˿���̨�����������Ϊ0�������Զ�̷�����Ϊ�����˺�ID
    uint32 m_cliAccountId;
    /// ����ִ��Ȩ��
    AccountTypes m_cliAccessLevel;
    /// ��Ϸָ�����
    void* m_callbackArg;
    /// ��Ϸָ��
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

/// ��Ϸ����
class World
{
	friend class MaNGOS::OperatorNew<World>;
protected:
	World();
	~World();
	World(const World&) = delete;
	World& operator=(const World&) = delete;

public:
	/// ��Ϸ�������
	void Update(uint32 diff);
	/// ������һỰ�㣬�������Կͻ��˵����ݰ�
	void UpdateSessions(uint32 diff);

	/// �ر���Ϸ����
	bool IsShutdowning() const { return m_ShutdownTimer > 0; }
	void ShutdownServ(uint32 time, uint32 options, uint8 exitcode);
	void ShutdownCancel();
	void ShutdownMsg(bool show = false, Player* player = nullptr);
	static uint8 GetExitCode() { return m_ExitCode; }
	static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }
	static bool IsStopped() { return m_stopEvent; }
	/// ִ����Ϸֹͣǰ��������
    void CleanupsBeforeStop();
	/// �ر����лỰ��رյ͵ȼ����˺ŻỰ
	void KickAll();
	void KickAllLess(AccountTypes sec);

	/// ��һỰ����
    WorldSession* FindSession(std::string id) const;
    void AddSession(WorldSession* s); // �����׽��ֲ�Ľӿڣ����ڽ���һỰ�첽������Ϸ����
    bool RemoveSession(std::string id);
	/// �ȴ�����ĻỰ����
	typedef std::list<WorldSession*> Queue;
	void AddQueuedSession(WorldSession*);
	bool RemoveQueuedSession(WorldSession* session);
	int32 GetQueuedSessionPos(WorldSession*);
	/// ��ȡ��ǰ�ȴ�����ĻỰ�����ͻ�Ծ�ĻỰ����
    uint32 GetActiveAndQueuedSessionCount() const { return m_sessions.size(); }
    uint32 GetActiveSessionCount() const { return m_sessions.size() - m_QueuedSessions.size(); }
    uint32 GetQueuedSessionCount() const { return m_QueuedSessions.size(); }
	/// �������ȴ�����ĻỰ����������Ծ�ĻỰ����
	void UpdateMaxSessionCounters();
    /// ��ȡ����Ϊֹ���ȴ�����ĻỰ����������Ծ�ĻỰ����
    uint32 GetMaxQueuedSessionCount() const { return m_maxQueuedSessionCount; }
    uint32 GetMaxActiveSessionCount() const { return m_maxActiveSessionCount; }

	/// ���Ʒ������Ļ�Ծ�Ự���������limit����0������������������ƣ����limitС��0(�൱��-AccountTypes)�����˺ŵȼ���������
	void SetPlayerLimit(int32 limit, bool needUpdate = false);
	/// ��ȡ�����������������������
	uint32 GetPlayerAmountLimit() const { return m_playerLimit >= 0 ? m_playerLimit : 0; }
	/// ��ȡ������˺ŵȼ���������
	AccountTypes GetPlayerSecurityLimit() const { return m_playerLimit <= 0 ? AccountTypes(-m_playerLimit) : SEC_PLAYER; }

    /// ����ϵͳ
	void UpdateWeather();
	WeatherState GetWeatherState() const;

	/// �������Է���˿���̨��Զ�̷��ʵ������д���
	void ProcessCliCommands();
	void QueueCliCommand(CliCommandHolder* commandHolder) { cliCmdQueue.add(commandHolder); }

	/// ���ݿ��첽������
	void UpdateResultQueue();
	void InitResultQueue();

	/// ���͹㲥��Ϣ
	void SendGlobalMessage(WorldPacket* packet);

	/// ��ȡ�´�ÿ�������ÿ�����������ʱ��
	time_t GetNextDailyQuestsResetTime() const { return m_NextDailyQuestReset; }
	time_t GetNextWeeklyQuestsResetTime() const { return m_NextWeeklyQuestReset; }

	/// �汾��Ϣ��ѯ
	void LoadDBVersion();
	const char* GetDBVersion() { return m_DBVersion.c_str(); }
	const char* GetCreatureEventAIVersion() { return m_CreatureEventAIVersion.c_str(); }

	/// ���ز���ʼ����Ϸ��������
	void LoadConfigSettings(bool reload = false);
	void SetInitialWorldSettings();
	/// ��ȡ��Ϸ�����ļ�(�����ͼ��Ϣ)�洢·��
	std::string GetDataPath() const { return m_dataPath; }
	/// ��ȡ�ͻ�������
	LocaleConstant GetLocaleConstant() const { return m_defaultLocale; }

    /// ����������������Ϣ��ȡ
    void SetConfig(eConfigFloatValues index, float value) { m_configFloatValues[index] = value; }
    float GetConfig(eConfigFloatValues rate) const { return m_configFloatValues[rate]; }

    /// �޷�����������������Ϣ��ȡ
    void SetConfig(eConfigUInt32Values index, uint32 value) { m_configUint32Values[index] = value; }
    uint32 GetConfig(eConfigUInt32Values index) const { return m_configUint32Values[index]; }

    /// �з�����������������Ϣ��ȡ
    void SetConfig(eConfigInt32Values index, int32 value) { m_configInt32Values[index] = value; }
    int32 GetConfig(eConfigInt32Values index) const { return m_configInt32Values[index]; }

    /// ��������������Ϣ��ȡ
    void SetConfig(eConfigBoolValues index, bool value) { m_configBoolValues[index] = value; }
    bool GetConfig(eConfigBoolValues index) const { return m_configBoolValues[index]; }

protected:
	/// ������Ϸ���磬������Ƿ�
	void UpdateGameTime();

	/// ÿ�������ÿ������ʱ������
    void InitDailyQuestResetTime();
    void InitWeeklyQuestResetTime();
    void ResetDailyQuests();
    void ResetWeeklyQuests();

protected:
	/// ���Խ��µ���һỰ������Ϸ����Ļ�Ծ�Ự�������Ϸ�������ˣ���������ȴ�����
	void AddNewSession(WorldSession* s);

	/// �����µ�����
	bool ReGenerateWeather();

	/// ������Ϸ���ó�ʼ������
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

    IntervalTimer m_timers[WUPDATE_COUNT]; /// ��Ϸ���綨ʱ��
	WeatherType m_weather;                 /// ��Ϸ������������
	float m_grade;                         /// ��Ϸ�����������ӳ̶� С�� ���� ����

    typedef UNORDERED_MAP<std::string, WorldSession*> SessionMap;
    SessionMap m_sessions;                                  /// ��һ�Ծ�Ự(����Ѿ����뵽��ɫѡȡ������Ѿ�������Ϸ������)
	Queue m_QueuedSessions;                                 /// ��һỰ����ȴ�����(�˺���֤��ɣ�����δ���뵽��ɫѡȡ����)
	MaNGOS::LockedQueue<WorldSession*> addSessQueue;        /// ��һỰ�첽�������(�˺���֤��ɺ����׽��ֲ�մ����ĻỰ)
	uint32 m_maxActiveSessionCount;                         /// ��ʷ��߻�Ծ��
    uint32 m_maxQueuedSessionCount;                         /// ��ʷ���ӵ����

	std::string m_DBVersion;                                /// DB�汾��Ϣ
	std::string m_CreatureEventAIVersion;                   /// ����AI�汾��Ϣ

    uint32 m_configUint32Values[CONFIG_UINT32_VALUE_COUNT]; /// ���eConfigUInt32Values
    int32 m_configInt32Values[CONFIG_INT32_VALUE_COUNT];    /// ���eConfigInt32Values
    float m_configFloatValues[CONFIG_FLOAT_VALUE_COUNT];    /// ���eConfigFloatValues
    bool m_configBoolValues[CONFIG_BOOL_VALUE_COUNT];       /// ���eConfigBoolValues
    int32 m_playerLimit;                                    /// �������������Ʒ�ʽ��0��ʾ�����ƣ�����0��ʾ������Ҹ�����С��0��ʾ��������˺ŵȼ�
    LocaleConstant m_defaultLocale;                         /// �ͻ�������
    std::string m_dataPath;                                 /// ��Ϸ�����ļ�Ŀ¼�������ͼ

    MaNGOS::LockedQueue<CliCommandHolder*> cliCmdQueue;     /// �첽������Կ���̨��Զ�̷��ʵ�������(�̰߳�ȫ)

    time_t m_NextDailyQuestReset;                           /// ÿ����������ʱ��
    time_t m_NextWeeklyQuestReset;                          /// ÿ����������ʱ��
};

extern uint32 realmID; /// ��Ϸ��������ţ�Ŀǰ���þ�һ��������δ�����Կ��Ǵ���һ��������Ϸ������ѡ������

#define sWorld MaNGOS::Singleton<World>::Instance()

#endif
