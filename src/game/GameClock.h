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
#ifndef MANGOS_GAMECLOCK_H
#define MANGOS_GAMECLOCK_H

#include <chrono>
#include "Common.h"

namespace GameClock
{
	/// ����������ʱ��
	time_t GetStartTime();

	/// ��������ǰʱ�䵥λ��
	time_t GetGameTime();

	/// �Է��������Ѿ������ĺ�����
	uint32 GetGameTimeMS();

	/// ��ǰϵͳʱ��ʱ���
	std::chrono::system_clock::time_point GetGameTimeSystemPoint();

	/// ��ǰ�ȶ�ʱ�ӵ�ʱ���
	std::chrono::steady_clock::time_point GetGameTimeSteadyPoint();

	/// ��������������Ϸ����ĳ���ʱ��
	uint32 GetUptime();

	/// ��������ǰ����
	const tm* GetDateAndTime();

	/// ������Ϸʱ��
	void UpdateGameTimers();

	/// ��Ϸ���統ǰʱ�䵥λ�룬��Ϸ����ʱ���ʱ�����ʵ����ʱ���6������������ʱ�䲻�Ǵ�1970��01��01��0:00:00��ʼ��ģ�
	/// ��һ���ɿ���ʱ�俪ʼ���㣬�����ʱʹ��2021��01��01��0:00:00
	time_t GetGameWorldTime();
}

#endif //MANGOS_GAMECLOCK_H
