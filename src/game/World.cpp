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

#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Utilities/SingletonImpl.h"
#include "Entities/Object.h"
#include "Entities/Player.h"
#include "Maps/MapManager.h"
#include "Packets/S_SystemMessage.hpp"
#include "Packets/S_Weather.hpp"
#include "SystemConfig.h"
#include "Log.h"
#include "Util.h"
#include "GameClock.h"
#include "Opcodes.h"
#include "ObjectMgr.h"
#include "SQLStorages.h"
#include "World.h"
#include "WorldPacket.h"

INSTANTIATE_SINGLETON_1(World);

volatile bool World::m_stopEvent = false;
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
volatile uint32 World::m_worldLoopCounter = 0;

World::World()
{
    m_playerLimit = 0;
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;
    m_NextDailyQuestReset = 0;
    m_NextWeeklyQuestReset = 0;

    m_defaultLocale = LOCALE_enUS;

    m_weather = WEATHER_TYPE_FINE;

    for (int i = 0; i < CONFIG_UINT32_VALUE_COUNT; ++i)
	{
    	m_configUint32Values[i] = 0;
	}

    for (int i = 0; i < CONFIG_INT32_VALUE_COUNT; ++i)
	{
    	m_configInt32Values[i] = 0;
	}

    for (int i = 0; i < CONFIG_FLOAT_VALUE_COUNT; ++i)
	{
    	m_configFloatValues[i] = 0.0f;
	}

    for (int i = 0; i < CONFIG_BOOL_VALUE_COUNT; ++i)
	{
    	m_configBoolValues[i] = false;
	}
}

World::~World()
{
    while (!m_sessions.empty())
    {
        delete m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
    }

    CliCommandHolder* command = nullptr;
    while (cliCmdQueue.next(command))
	{
    	delete command;
	}

    // TODO free addSessQueue
}

/// Update the World !
void World::Update(uint32 diff)
{
    ///- ������Ϸ���綨ʱ��
    for (int i = 0; i < WUPDATE_COUNT; ++i)
    {
        if (m_timers[i].GetCurrent() >= 0)
		{
        	m_timers[i].Update(diff);
		}
        else
		{
        	m_timers[i].SetCurrent(0);
		}
    }

    ///- ������Ϸʱ�䣬�����������ر�ʱ��
    UpdateGameTime();

    ///- ����ÿ������ʱ��
    if (GameClock::GetGameTime() > m_NextDailyQuestReset)
	{
    	ResetDailyQuests();
	}

    ///- ����ÿ������ʱ��
    if (GameClock::GetGameTime() > m_NextWeeklyQuestReset)
	{
    	ResetWeeklyQuests();
	}

    ///- ������һỰ���������Կͻ��˵����ݰ����������ʹ�á�������ʩ����
    UpdateSessions(diff);

    ///- ��������
    if (m_timers[WUPDATE_WEATHERS].Passed())
    {
        m_timers[WUPDATE_WEATHERS].Reset();

        if (ReGenerateWeather())
		{
			UpdateWeather();
		}
    }

    ///- ���µ�ͼ�ڵ���Ϸ���󣬴�������ƶ�������AI��
	sMapMgr.Update(diff);

    ///- ִ���첽SQL���Ļص�����
    UpdateResultQueue();

	///- ִ����Ϸ������
    ProcessCliCommands();
}

void World::UpdateSessions(uint32 diff)
{
    WorldSession* sess;
    while (addSessQueue.next(sess))
	{
    	AddNewSession(sess);
	}

    for (SessionMap::iterator iter = m_sessions.begin(), next; iter != m_sessions.end(); iter = next)
    {
        next = iter;
        ++next;
        WorldSession* pSession = iter->second;
        WorldSessionFilter updater(pSession);

        if (!pSession->Update(updater))
        {
            RemoveQueuedSession(pSession);
            m_sessions.erase(iter);
            delete pSession;
        }
    }
}

void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode)
{
	if (m_stopEvent)
	{
		return;
	}

	m_ShutdownMask = options;
	m_ExitCode = exitcode;

	if (time == 0)
	{
		if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
		{
			m_stopEvent = true;
		}
		else
		{
			m_ShutdownTimer = 1;
		}
	}
	else
	{
		m_ShutdownTimer = time;
		ShutdownMsg(true);
	}
}

void World::ShutdownCancel()
{
	// ��ǰû�йرղ������Ѿ�̫���޷�ȡ����
	if (!m_ShutdownTimer || m_stopEvent)
	{
		return;
	}

	m_ShutdownMask = 0;
	m_ShutdownTimer = 0;
	m_ExitCode = SHUTDOWN_EXIT_CODE;
	DEBUG_LOG("Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutdown"));
}

void World::ShutdownMsg(bool show /*= false*/, Player* player /*= NULL*/)
{
	if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
	{
		return;
	}

	///- �ڷ������رյ���ʱ�Ĳ�ͬ�׶Σ�ÿ12Сʱ/1Сʱ/5����/1����/15��㲥һ�η������ر���Ϣ
	if (show ||
		(m_ShutdownTimer < 5 * MINUTE && (m_ShutdownTimer % 15) == 0) ||            // <   5 ����; ÿ�� 15 ��㲥һ��
		(m_ShutdownTimer < 15 * MINUTE && (m_ShutdownTimer % MINUTE) == 0) ||       // <  15 ����; ÿ��  1 ���ӹ㲥һ��
		(m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) || // <  30 ����; ÿ��  5 ���ӹ㲥һ��
		(m_ShutdownTimer < 12 * HOUR && (m_ShutdownTimer % HOUR) == 0) ||           // <  12 Сʱ; ÿ��  1 Сʱ�㲥һ��
		(m_ShutdownTimer >= 12 * HOUR && (m_ShutdownTimer % (12 * HOUR)) == 0))     // >= 12 Сʱ; ÿ�� 12 Сʱ�㲥һ��
	{
		std::string str = "����������" + secsToTimeString(m_ShutdownTimer) + "��" + (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "����" : "�ر�");
		S_SystemMessage pkt(Utf8toLocale(str, sWorld.GetLocaleConstant()));
		SendGlobalMessage(&pkt);
		DEBUG_LOG("Server is %s in %s", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutting down"), str.c_str());
	}
}

void World::CleanupsBeforeStop()
{
	KickAll();
	UpdateSessions(1);
}

void World::KickAll()
{
	m_QueuedSessions.clear();

	for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
	{
		iter->second->KickPlayer();
	}
}

void World::KickAllLess(AccountTypes sec)
{
	for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
	{
		if (iter->second->GetSecurity() < sec)
		{
			iter->second->KickPlayer();
		}
	}
}

WorldSession* World::FindSession(std::string id) const
{
	auto iter = m_sessions.find(id);

	if (iter != m_sessions.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

bool World::RemoveSession(std::string id)
{
	/// �ҵ��Ự�����߳���ң��������ǲ����ڴ˴�ɾ���Ự�����������ʧЧ
	auto iter = m_sessions.find(id);

	if (iter != m_sessions.end() && iter->second)
	{
		/// �����Ҵ��ڽ�ɫ����ʱ�ڣ������߳���ң������Ƴ�ʧ��
		if (iter->second->PlayerLoading())
		{
			return false;
		}
		iter->second->KickPlayer();
	}

	/// �ɹ��߳����
	return true;
}

void World::AddSession(WorldSession* s)
{
	addSessQueue.add(s);
}

void World::AddQueuedSession(WorldSession* sess)
{
	sess->SetInQueue(true);
	m_QueuedSessions.push_back(sess);
}

bool World::RemoveQueuedSession(WorldSession* sess)
{
	uint32 sessions = GetActiveSessionCount();
	/// ͳ��iterָ������ǰ�滹�ж��������Ŷӵȴ�
	uint32 position = 1;
	auto iter = m_QueuedSessions.begin();
	bool found = false;
	for (; iter != m_QueuedSessions.end(); ++iter, ++position)
	{
		if (*iter == sess)
		{
			sess->SetInQueue(false);
			/// ɾ����iterָ����һ����ң�������Ҫ֪ͨ�����Ŷӵ���ң����ǵ��Ŷ�˳λ��ǰ�ƽ���һ��
			iter = m_QueuedSessions.erase(iter);
			found = true;
			break;
		}
	}

	if (!found && sessions)
	{
		--sessions;
	}

	if ((!m_playerLimit || (int32)sessions < m_playerLimit) && !m_QueuedSessions.empty())
	{
		WorldSession* s = m_QueuedSessions.front();
		s->SetInQueue(false);
		m_QueuedSessions.pop_front();
		AddNewSession(s);

		iter = m_QueuedSessions.begin();
		position = 1;
	}

	for (; iter != m_QueuedSessions.end(); ++iter, ++position)
	{
		// todo: ֪ͨ�����Ŷӵ������ǰ�����ڻ��ж������
	}

	return found;
}

int32 World::GetQueuedSessionPos(WorldSession* sess)
{
	uint32 position = 1;
	for (auto iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
	{
		if ((*iter) == sess)
		{
			return position;
		}
	}
	return 0;
}

void World::UpdateMaxSessionCounters()
{
	m_maxActiveSessionCount = std::max(m_maxActiveSessionCount, uint32(m_sessions.size() - m_QueuedSessions.size()));
	m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount, uint32(m_QueuedSessions.size()));
}

void World::SetPlayerLimit(int32 limit, bool needUpdate)
{
    if (limit < -SEC_ADMINISTRATOR)
	{
    	limit = -SEC_ADMINISTRATOR;
	}

    m_playerLimit = limit;
}

void World::UpdateWeather()
{
	if (m_grade >= 1)
	{
		m_grade = 0.9999f;
	}
	else if (m_grade < 0)
	{
		m_grade = 0.0001f;
	}
	WeatherState state = GetWeatherState();

	// �㲥����
	S_Weather pkt(state);
	SendGlobalMessage(&pkt);

	char const* wthstr;
	switch (state)
	{
		case WEATHER_STATE_LIGHT_SNOW:
			wthstr = "light snow";
			break;
		case WEATHER_STATE_MEDIUM_SNOW:
			wthstr = "medium snow";
			break;
		case WEATHER_STATE_HEAVY_SNOW:
			wthstr = "heavy snow";
			break;
		case WEATHER_STATE_FINE:
		default:
			wthstr = "fine";
			break;
	}

	DETAIL_FILTER_LOG(LOG_FILTER_WEATHER, "Change the weather to %s.", wthstr);
}

WeatherState World::GetWeatherState() const
{
	if (m_grade < 0.27f)
	{
		return WEATHER_STATE_FINE;
	}

	switch (m_weather)
	{
		case WEATHER_TYPE_SNOW:
			if (m_grade < 0.40f)
				return WEATHER_STATE_LIGHT_SNOW;
			else if (m_grade < 0.70f)
				return WEATHER_STATE_MEDIUM_SNOW;
			else
				return WEATHER_STATE_HEAVY_SNOW;
		case WEATHER_TYPE_FINE:
		default:
			return WEATHER_STATE_FINE;
	}
}

void World::ProcessCliCommands()
{
	CliCommandHolder::Print* zprint = nullptr;
	void* callbackArg = nullptr;
	CliCommandHolder* command;
	while (cliCmdQueue.next(command))
	{
		DEBUG_LOG("CLI command under processing...");
		zprint = command->m_print;
		callbackArg = command->m_callbackArg;
		delete command;
	}
}

void World::InitResultQueue()
{
}

void World::UpdateResultQueue()
{
	CharacterDatabase.ProcessResultQueue();
	WorldDatabase.ProcessResultQueue();
	LoginDatabase.ProcessResultQueue();
}

void World::SendGlobalMessage(WorldPacket* packet)
{
	for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
	{
		if (iter->second && iter->second->GetPlayer() && iter->second->GetPlayer()->IsInWorld())
		{
			iter->second->SendPacket(packet);
		}
	}
}

void World::LoadDBVersion()
{
}

void World::LoadConfigSettings(bool reload)
{
	if (reload)
	{
		if (!sConfig.Reload())
		{
			sLog.outError("World settings reload fail: can't read settings from %s.", sConfig.GetFilename().c_str());
			return;
		}
	}

	///- Read the version of the configuration file and warn the user in case of emptiness or mismatch
	uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
	if (!confVersion)
	{
		sLog.outError("*****************************************************************************");
		sLog.outError(" WARNING: mangosd.conf does not include a ConfVersion variable.");
		sLog.outError("          Your configuration file may be out of date!");
		sLog.outError("*****************************************************************************");
		Log::WaitBeforeContinueIfNeed();
	}
	else
	{
		if (confVersion < _MANGOSDCONFVERSION)
		{
			sLog.outError("*****************************************************************************");
			sLog.outError(" WARNING: Your mangosd.conf version indicates your conf file is out of date!");
			sLog.outError("          Please check for updates, as your current default values may cause");
			sLog.outError("          unexpected behavior.");
			sLog.outError("*****************************************************************************");
			Log::WaitBeforeContinueIfNeed();
		}
	}

	///- ���ÿͻ�������
	SetConfig(CONFIG_UINT32_CLIENT_LANGUAGE, "L1J.ClientLanguage", 5);

	///- Read the player limit and the Message of the day from the config file
	SetPlayerLimit(sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT), true);

	///- Read all rates from the config file

	///- Read other configuration items from the config file
	if (ConfigNoReload(reload, CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT))
		SetConfig(CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT);

	SetConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET, "Network.KickOnBadPacket", false);

	///- Read the "Data" directory from the config file
	std::string dataPath = sConfig.GetStringDefault("DataDir", "./");

	if (dataPath.empty())
	{
		dataPath = "./";
	}
	else if (dataPath.at(dataPath.length() - 1) != '/' && dataPath.at(dataPath.length() - 1) != '\\')
	{
		dataPath.append("/");
	}

	if (reload)
	{
		if (dataPath != m_dataPath)
		{
			sLog.outError("DataDir option can't be changed at mangosd.conf reload, using current value (%s).", m_dataPath.c_str());
		}
	}
	else
	{
		m_dataPath = dataPath;
		sLog.outString("Using DataDir %s", m_dataPath.c_str());
	}
}

void World::SetInitialWorldSettings()
{
	///- ��ʼ���������������
	srand((unsigned int)time(NULL));

	///- ��¼����ʱ��
	uint32 uStartTime = WorldTimer::getMSTime();

	///- ���ط����������ļ�
	LoadConfigSettings();

	sLog.outString("Initialize data stores...");
	LoadSQLStorage();

	///- Init highest guids before any guid using table loading to prevent using not initialized guids in some code.
	sObjectMgr.SetHighestGuids();
	sLog.outString();

	sLog.outString("Loading Terrains...");
	sMapMgr.LoadTerrains();

	sLog.outString("DEBUG:: Initialize game time and timers");
	GameClock::UpdateGameTimers();
	m_timers[WUPDATE_WEATHERS].SetInterval(30 * MINUTE);

	sLog.outString("Calculate next daily quest reset time...");
	InitDailyQuestResetTime();

	sLog.outString("Calculate next weekly quest reset time...");
	InitWeeklyQuestResetTime();

	sLog.outString("WORLD: World initialized");

	uint32 uStartInterval = WorldTimer::getMSTimeDiff(uStartTime, WorldTimer::getMSTime());
	sLog.outString("SERVER STARTUP TIME: %i minutes %i seconds", uStartInterval / 60000, (uStartInterval % 60000) / 1000);
}

void World::UpdateGameTime()
{
	/// ������Ϸʱ��
	time_t thisTime = GameClock::GetGameTime();
	GameClock::UpdateGameTimers();
	uint32 elapsed = uint32(GameClock::GetGameTime() - thisTime);
	/// ����Ƿ������Ϸ�رռ�ʱ��
	if (!m_stopEvent && m_ShutdownTimer > 0 && elapsed > 0)
	{
		/// ��ʱ���Ѿ����ڣ��ر���Ϸ����
		if (m_ShutdownTimer <= elapsed)
		{
			/// ����Ҫ�ȴ���Ϸ����������״̬������Ϸ����ĻỰ��Ϊ0(������ҵ������Ѿ��Ͽ�)
			if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
			{
				m_stopEvent = true;
			}
			else
			{
				/// �ȴ���Ϸ����������״̬(�ȴ�������ҵ������Ѿ��Ͽ�)
				m_ShutdownTimer = 1;
			}
		}
		else
		{
			/// ���ټ�ʱ��ʱ��
			m_ShutdownTimer -= elapsed;
			/// ���û���ʾ�رյ���ʱ
			ShutdownMsg();
		}
	}
}

void World::InitWeeklyQuestResetTime()
{
}

void World::InitDailyQuestResetTime()
{
}

void World::ResetDailyQuests()
{
}

void World::ResetWeeklyQuests()
{
}


void World::AddNewSession(WorldSession* s)
{
	MANGOS_ASSERT(s);

	// NOTE - �˴����׽��ֲ�ʹ�õ�WorldSession*��Ȼ���ھ�������

	/// �������˺Żἷ��ǰһ��������˺ţ��������ǰһ�������˺��Ѿ����ڽ�ɫ����ʱ��(˫��ѡȡ�˽�ɫ�����ǻ�����������ݿ���Ϣ���ؽ׶�)��
	/// ��ô���߳��������˺�
	if (!RemoveSession(s->GetAccountId()))
	{
		s->KickPlayer();
		delete s;
		return;
	}

	/// ���ڲ��������ӵ�����¼��ٻỰ����
	bool decrease_session = true;

	/// ��������Ѿ�����֮ǰ������˺ţ���߾�Ҫ���ɵĵ���Ự�Ƴ�����Ҫע�����һ��Ҫ�ڱ������ĵ���Ự��
	/// RemoveSession()������ִ��KickPlayer()����Ϊ�������Ͼ�Ҫֱ��ɾ���Ự������
	auto iter = m_sessions.find(s->GetAccountId());
	if (iter != m_sessions.end())
	{
		/// ���Ҫ�������˺Ż��ڵȴ�����ʱ��(�Ự����m_sessions����)����ô�����Ự����ͳ��
		if (RemoveQueuedSession(iter->second))
		{
			decrease_session = false;
		}
		/// ֱ��ɾ���Ự����ʹ��m_sessions����Ҳ��Ҫ�������յ�ָ��ᱻ��������������
		delete iter->second;
	}

	/// ���浱ǰ����ĻỰ
	m_sessions[s->GetAccountId()] = s;

	uint32 Sessions = GetActiveAndQueuedSessionCount();
	/// ֻ�м������ǵ�ǰ������Ϸ�еĵ���Ự����
	if (decrease_session)
	{
		--Sessions;
	}
	uint32 pLimit = GetPlayerAmountLimit();
	uint32 QueueSize = GetQueuedSessionCount();

	/// ��������������������ƣ��ҷ�������ǰ�Ѿ���������ô���Ự�������ȴ�����
	if (pLimit > 0 && Sessions >= pLimit && s->GetSecurity() == SEC_PLAYER)
	{
		AddQueuedSession(s);
		UpdateMaxSessionCounters();
		DETAIL_LOG("PlayerQueue: Account id %s is in Queue Position (%u).", s->GetAccountId().c_str(), ++QueueSize);
		return;
	}

	/// ���»Ựͳ����Ϣ
	UpdateMaxSessionCounters();
}

bool World::ReGenerateWeather()
{
	/// �����仯����:
	///- 30% - ��������
	///- 30% - ���������(��������Ǻ�����)���߱��˸��������������������ѩ
	///- 30% - ������ø���(��������Ǻ�����)������Сѩ�����ѩ����ѩ��ɴ�ѩ
	///- 10% - ���������ޱ�(��������Ǻ�����)������Сѩ��ɴ�ѩ
	uint32 u = urand(0, 99);

	if (u < 30)
	{
		return false;
	}

	// ��¼������
	WeatherType old_weather = m_weather;
	float old_grade = m_grade;

	if ((u < 60) && (m_grade < 0.33333334f))
	{
		m_weather = WEATHER_TYPE_FINE;
		m_grade = 0.0f;
	}

	if ((u < 60) && (m_weather != WEATHER_TYPE_FINE))          // ���������Щ
	{
		m_grade -= 0.33333334f;
		return true;
	}

	if ((u < 90) && (m_weather != WEATHER_TYPE_FINE))          // ������ø���
	{
		m_grade += 0.33333334f;
		return true;
	}

	if (m_weather != WEATHER_TYPE_FINE)
	{
		/// ���������仯������ѩΪ��
		///- �����ǰ��Сѩ -> ��ɴ�ѩ
		///- �����ǰ����ѩ -> ���������������
		///- �����ǰ�Ǵ�ѩ -> 50%���Сѩ 50%���������������

		if (m_grade < 0.33333334f)
		{
			m_grade = 0.9999f; // Сѩ -> ��ѩ
			return true;
		}
		else
		{
			if (m_grade > 0.6666667f) // ��ѩ
			{
				uint32 rnd = urand(0, 99);
				if (rnd < 50)
				{
					m_grade -= 0.6666667f;  // 50%���ʱ��Сѩ
					return true;
				}
			}
			m_weather = WEATHER_TYPE_FINE;
			m_grade = 0;
		}
	}

	// 50%���ʱ����������
	uint32 rnd = urand(0, 99);
	m_weather = WeatherType(rnd % MAX_WEATHER_TYPE);

	/// �������Ķ��ӳ̶�(�����������)������ѩΪ��:
	///- 85% Сѩ
	///- 7%  ��ѩ
	///- 7%  ��ѩ
	if (m_weather == WEATHER_TYPE_FINE)
	{
		m_grade = 0.0f;
	}
	else if (u < 90)
	{
		m_grade = rand_norm_f() * 0.3333f;
	}
	else
	{
		rnd = urand(0, 99);
		if (rnd < 50)
		{
			m_grade = rand_norm_f() * 0.3333f + 0.3334f;
		}
		else
		{
			m_grade = rand_norm_f() * 0.3333f + 0.6667f;
		}
	}

	return m_weather != old_weather || m_grade != old_grade;
}

void World::SetConfig(eConfigUInt32Values index, const char* fieldname, uint32 defvalue)
{
    SetConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
    if (int32(GetConfig(index)) < 0)
    {
        sLog.outError("%s (%i) can't be negative. Using %u instead.", fieldname, int32(GetConfig(index)), defvalue);
        SetConfig(index, defvalue);
    }
}

void World::SetConfig(eConfigInt32Values index, const char* fieldname, int32 defvalue)
{
    SetConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
}

void World::SetConfig(eConfigFloatValues index, const char* fieldname, float defvalue)
{
    SetConfig(index, sConfig.GetFloatDefault(fieldname, defvalue));
}

void World::SetConfig(eConfigBoolValues index, const char* fieldname, bool defvalue)
{
    SetConfig(index, sConfig.GetBoolDefault(fieldname, defvalue));
}

void World::SetConfigPos(eConfigFloatValues index, const char* fieldname, float defvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < 0.0f)
    {
        sLog.outError("%s (%f) can't be negative. Using %f instead.", fieldname, GetConfig(index), defvalue);
        SetConfig(index, defvalue);
    }
}

void World::SetConfigMin(eConfigUInt32Values index, const char* fieldname, uint32 defvalue, uint32 minvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be >= %u. Using %u instead.", fieldname, GetConfig(index), minvalue, minvalue);
        SetConfig(index, minvalue);
    }
}

void World::SetConfigMin(eConfigInt32Values index, const char* fieldname, int32 defvalue, int32 minvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be >= %i. Using %i instead.", fieldname, GetConfig(index), minvalue, minvalue);
        SetConfig(index, minvalue);
    }
}

void World::SetConfigMin(eConfigFloatValues index, const char* fieldname, float defvalue, float minvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be >= %f. Using %f instead.", fieldname, GetConfig(index), minvalue, minvalue);
        SetConfig(index, minvalue);
    }
}

void World::SetConfigMinMax(eConfigUInt32Values index, const char* fieldname, uint32 defvalue, uint32 minvalue, uint32 maxvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, GetConfig(index), minvalue, maxvalue, minvalue);
        SetConfig(index, minvalue);
    }
    else if (GetConfig(index) > maxvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, GetConfig(index), minvalue, maxvalue, maxvalue);
        SetConfig(index, maxvalue);
    }
}

void World::SetConfigMinMax(eConfigInt32Values index, const char* fieldname, int32 defvalue, int32 minvalue, int32 maxvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, GetConfig(index), minvalue, maxvalue, minvalue);
        SetConfig(index, minvalue);
    }
    else if (GetConfig(index) > maxvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, GetConfig(index), minvalue, maxvalue, maxvalue);
        SetConfig(index, maxvalue);
    }
}

void World::SetConfigMinMax(eConfigFloatValues index, const char* fieldname, float defvalue, float minvalue, float maxvalue)
{
    SetConfig(index, fieldname, defvalue);
    if (GetConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, GetConfig(index), minvalue, maxvalue, minvalue);
        SetConfig(index, minvalue);
    }
    else if (GetConfig(index) > maxvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, GetConfig(index), minvalue, maxvalue, maxvalue);
        SetConfig(index, maxvalue);
    }
}

bool World::ConfigNoReload(bool reload, eConfigUInt32Values index, const char* fieldname, uint32 defvalue)
{
    if (!reload)
	{
    	return true;
	}

    uint32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != GetConfig(index))
	{
    	sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%u).", fieldname, GetConfig(index));
	}

    return false;
}

bool World::ConfigNoReload(bool reload, eConfigInt32Values index, const char* fieldname, int32 defvalue)
{
    if (!reload)
	{
    	return true;
	}

    int32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != GetConfig(index))
	{
    	sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%i).", fieldname, GetConfig(index));
	}

    return false;
}

bool World::ConfigNoReload(bool reload, eConfigFloatValues index, const char* fieldname, float defvalue)
{
    if (!reload)
	{
    	return true;
	}

    float val = sConfig.GetFloatDefault(fieldname, defvalue);
    if (val != GetConfig(index))
	{
    	sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%f).", fieldname, GetConfig(index));
	}

    return false;
}

bool World::ConfigNoReload(bool reload, eConfigBoolValues index, const char* fieldname, bool defvalue)
{
    if (!reload)
	{
    	return true;
	}

    bool val = sConfig.GetBoolDefault(fieldname, defvalue);
    if (val != GetConfig(index))
	{
    	sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%s).", fieldname, GetConfig(index) ? "'true'" : "'false'");
	}

    return false;
}
