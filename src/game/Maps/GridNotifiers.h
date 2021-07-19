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

#ifndef MANGOS_GRIDNOTIFIERS_H
#define MANGOS_GRIDNOTIFIERS_H

#include <iostream>
#include "Entities/Corpse.h"
#include "Entities/Creature.h"
#include "Entities/Object.h"
#include "Entities/DynamicObject.h"
#include "Entities/GameObject.h"
#include "Entities/Player.h"
#include "Entities/Unit.h"

namespace MaNGOS
{
	/**
	 * 魔兽世界对象进入/离开自己的视野，需要向自己发送出现/消失数据包，这也是魔兽世界的UpdateVisibilityOf的逻辑，
	 * 而VisibleNotifier的i_clientGUIDs作用就是用来记录有哪些游戏对象离开了自己的视野，实现逻辑是在Visit前先备份，
	 * 然后在Visit的时候删除当前能看到的游戏对象，剩下的就是以前能看到现在看不到的了，最后向其中玩家类型的游戏对象发送出现/消失数据包，
	 * 天堂视野管理没有魔兽世界这么复杂，对象进入/离开不需要发送特殊的数据包，所以这边简化了VisibleNotifier实现，其使用方法的伪代码如下:
	 *
	 * MaNGOS::Visit(VisibleNotifier notifier)
	 * {
	 * 		for (auto obj : objs)
	 * 		{
	 * 			notifier.Visit(obj);
	 * 		}
	 * }
	 *
	 * 其中objs就是网格内指定类型的对象列表，其类型在VisibleNotifier的template<class T>中指定
	 */

	struct MANGOS_DLL_DECL VisibleNotifier
	{
		Camera& m_camera; // 相机对象(用来查找网格列表的中心点)

		explicit VisibleNotifier(Camera& c) : m_camera(c)
		{
		}
		/**
		 * 更新相机对象的视野内容(实际更新的是相机拥有者的视野内容)
		 */
		template<class T>
		void Visit(GridRefManager<T>& m);
		void Visit(CameraMapType& /*m*/)
		{
		} // 跳过对网格内相机类型对象的处理
	};

	struct MANGOS_DLL_DECL VisibleChangesNotifier
	{
		WorldObject& m_object; // 游戏对象(用来查找网格列表的中心点)

		explicit VisibleChangesNotifier(WorldObject& object) : m_object(object)
		{
		}
		/**
		 * 通知网格内的相机类型对象，让其更新游戏对象在其视野内的情况
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		} // 仅处理相机类型对象，跳过对网格内其他类型对象的处理
	};

	// 向玩家所在网格附近的玩家发送消息
	struct MANGOS_DLL_DECL MessageDeliverer
	{
		Player& m_player;       // 玩家(用来查找网格列表的中心点)
		WorldPacket* m_message; // 消息
		bool m_toSelf;          // 是否需要发送给自己

		MessageDeliverer(Player& player, WorldPacket* msg, bool to_self)
			: m_player(player), m_message(msg), m_toSelf(to_self)
		{
		}
		/**
		 * 通知网格内的相机类型对象，向其拥有者(仅限玩家)发送消息
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		} // 仅处理相机类型对象，跳过对网格内其他类型对象的处理
	};

	// 向玩家所在网格附近的玩家发送消息
	struct MessageDelivererExcept
	{
		uint32 m_phaseMask;               // 团队任务阶段掩码
		WorldObject& m_object;            // 游戏对象(用来查找网格列表的中心点)
		WorldPacket* m_message;           // 消息
		const Player* m_skipped_receiver; // 过滤指定玩家

		MessageDelivererExcept(WorldObject& obj, WorldPacket* msg, const Player* skipped)
			: m_object(obj), m_message(msg), m_skipped_receiver(skipped)
		{
		}
		/**
		 * 通知网格内的相机类型对象，向其拥有者(仅限玩家，且玩家团队任务阶段需满足要求)发送消息
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// 向游戏对象所在网格附近的玩家发送消息
	struct MANGOS_DLL_DECL ObjectMessageDeliverer
	{
		uint32 m_phaseMask;     // 团队任务阶段掩码
		WorldObject& m_object;  // 游戏对象(用来查找网格列表的中心点)
		WorldPacket* m_message; // 消息

		ObjectMessageDeliverer(WorldObject& obj, WorldPacket* msg) : m_object(obj), m_message(msg)
		{
		}
		/**
		 * 通知网格内的相机类型对象，向其拥有者(仅限玩家，且玩家团队任务阶段需满足要求)发送消息
		 */
		void Visit(CameraMapType& m);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// 向玩家所在网格附近的玩家发送消息(检测距离)
	struct MANGOS_DLL_DECL MessageDistDeliverer
	{
		uint32 m_phaseMask;     // 团队任务阶段掩码
		Player& m_player;       // 玩家(用来查找网格列表的中心点)
		WorldPacket* m_message; // 消息
		bool m_toSelf;          // 消息是否要发送给自己
		bool m_ownTeamOnly;     // 消息是否仅发送给团队玩家
		int32 m_dist;           // 距离

		MessageDistDeliverer(Player& player, WorldPacket* msg, int32 dist, bool to_self, bool ownTeamOnly)
			: m_player(player), m_message(msg), m_toSelf(to_self), m_ownTeamOnly(ownTeamOnly), m_dist(dist)
		{
		}
		/**
		 * 通知网格内的相机类型对象(满足距离要求)，向其拥有者(仅限玩家，且玩家团队任务阶段需满足要求)发送消息
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// 向游戏对象所在网格附近的玩家发送消息(检测距离)
	struct MANGOS_DLL_DECL ObjectMessageDistDeliverer
	{
		uint32 m_phaseMask;     // 团队任务阶段掩码
		WorldObject& m_object;  // 游戏对象(用来查找网格列表的中心点)
		WorldPacket* m_message; // 消息
		int32 m_dist;           // 距离

		ObjectMessageDistDeliverer(WorldObject& obj, WorldPacket* msg, int32 dist)
			: m_object(obj), m_message(msg), m_dist(dist)
		{
		}
		/**
		 * 通知网格内的相机类型对象(满足距离要求)，向其拥有者(仅限玩家，且玩家团队任务阶段需满足要求)发送消息
		 */
		void Visit(CameraMapType& m);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	struct MANGOS_DLL_DECL ObjectUpdater
	{
		uint32 m_timeDiff;

		explicit ObjectUpdater(const uint32& diff) : m_timeDiff(diff)
		{
		}
		template<class T>
		void Visit(GridRefManager<T>& m);
		void Visit(CreatureMapType&);
		void Visit(PlayerMapType&)
		{
		}
		void Visit(CorpseMapType&)
		{
		}
		void Visit(CameraMapType&)
		{
		}
	};

	struct MANGOS_DLL_DECL PlayerRelocationNotifier
	{
		Player& m_player;

		PlayerRelocationNotifier(Player& player) : m_player(player)
		{
		}
		void Visit(CreatureMapType&) {}
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	struct MANGOS_DLL_DECL CreatureRelocationNotifier
	{
		Creature& m_creature;
		CreatureRelocationNotifier(Creature& c) : m_creature(c)
		{
		}
		void Visit(PlayerMapType&) {}
		void Visit(CreatureMapType&) {}
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	struct MANGOS_DLL_DECL DynamicObjectUpdater
	{
		DynamicObject& m_dynobject;
		Unit* m_check;
		bool m_positive;
		DynamicObjectUpdater(DynamicObject& dynobject, Unit* caster, bool positive)
			: m_dynobject(dynobject), m_positive(positive)
		{
			m_check = caster;
			Unit* owner = m_check->GetOwner();
			if (owner)
			{
				m_check = owner;
			}
		}

		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
		void Visit(CreatureMapType&);
		void Visit(PlayerMapType&);
		void VisitHelper(Unit* target) {}
	};
}
#endif
