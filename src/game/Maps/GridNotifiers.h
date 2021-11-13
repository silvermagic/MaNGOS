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
	 * ħ������������/�뿪�Լ�����Ұ����Ҫ���Լ����ͳ���/��ʧ���ݰ�����Ҳ��ħ�������UpdateVisibilityOf���߼���
	 * ��VisibleNotifier��i_clientGUIDs���þ���������¼����Щ��Ϸ�����뿪���Լ�����Ұ��ʵ���߼�����Visitǰ�ȱ��ݣ�
	 * Ȼ����Visit��ʱ��ɾ����ǰ�ܿ�������Ϸ����ʣ�µľ�����ǰ�ܿ������ڿ��������ˣ����������������͵���Ϸ�����ͳ���/��ʧ���ݰ���
	 * ������Ұ����û��ħ��������ô���ӣ��������/�뿪����Ҫ������������ݰ���������߼���VisibleNotifierʵ�֣���ʹ�÷�����α��������:
	 *
	 * MaNGOS::Visit(VisibleNotifier notifier)
	 * {
	 * 		for (auto obj : objs)
	 * 		{
	 * 			notifier.Visit(obj);
	 * 		}
	 * }
	 *
	 * ����objs����������ָ�����͵Ķ����б���������VisibleNotifier��template<class T>��ָ��
	 */

	struct MANGOS_DLL_DECL VisibleNotifier
	{
		Camera& m_camera; // �������(�������������б�����ĵ�)

		explicit VisibleNotifier(Camera& c) : m_camera(c)
		{
		}
		/**
		 * ��������������Ұ����(ʵ�ʸ��µ������ӵ���ߵ���Ұ����)
		 */
		template<class T>
		void Visit(GridRefManager<T>& m);
		void Visit(CameraMapType& /*m*/)
		{
		} // ������������������Ͷ���Ĵ���
	};

	struct MANGOS_DLL_DECL VisibleChangesNotifier
	{
		WorldObject& m_object; // ��Ϸ����(�������������б�����ĵ�)

		explicit VisibleChangesNotifier(WorldObject& object) : m_object(object)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ������������Ϸ����������Ұ�ڵ����
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		} // ������������Ͷ����������������������Ͷ���Ĵ���
	};

	// ������������񸽽�����ҷ�����Ϣ
	struct MANGOS_DLL_DECL MessageDeliverer
	{
		Player& m_player;       // ���(�������������б�����ĵ�)
		WorldPacket* m_message; // ��Ϣ
		bool m_toSelf;          // �Ƿ���Ҫ���͸��Լ�

		MessageDeliverer(Player& player, WorldPacket* msg, bool to_self)
			: m_player(player), m_message(msg), m_toSelf(to_self)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ�������ӵ����(�������)������Ϣ
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		} // ������������Ͷ����������������������Ͷ���Ĵ���
	};

	// ������������񸽽�����ҷ�����Ϣ
	struct MessageDelivererExcept
	{
		uint32 m_phaseMask;               // �Ŷ�����׶�����
		WorldObject& m_object;            // ��Ϸ����(�������������б�����ĵ�)
		WorldPacket* m_message;           // ��Ϣ
		const Player* m_skipped_receiver; // ����ָ�����

		MessageDelivererExcept(WorldObject& obj, WorldPacket* msg, const Player* skipped)
			: m_object(obj), m_message(msg), m_skipped_receiver(skipped)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ�������ӵ����(������ң�������Ŷ�����׶�������Ҫ��)������Ϣ
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// ����Ϸ�����������񸽽�����ҷ�����Ϣ
	struct MANGOS_DLL_DECL ObjectMessageDeliverer
	{
		uint32 m_phaseMask;     // �Ŷ�����׶�����
		WorldObject& m_object;  // ��Ϸ����(�������������б�����ĵ�)
		WorldPacket* m_message; // ��Ϣ

		ObjectMessageDeliverer(WorldObject& obj, WorldPacket* msg) : m_object(obj), m_message(msg)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ�������ӵ����(������ң�������Ŷ�����׶�������Ҫ��)������Ϣ
		 */
		void Visit(CameraMapType& m);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// ������������񸽽�����ҷ�����Ϣ(������)
	struct MANGOS_DLL_DECL MessageDistDeliverer
	{
		uint32 m_phaseMask;     // �Ŷ�����׶�����
		Player& m_player;       // ���(�������������б�����ĵ�)
		WorldPacket* m_message; // ��Ϣ
		bool m_toSelf;          // ��Ϣ�Ƿ�Ҫ���͸��Լ�
		bool m_ownTeamOnly;     // ��Ϣ�Ƿ�����͸��Ŷ����
		int32 m_dist;           // ����

		MessageDistDeliverer(Player& player, WorldPacket* msg, int32 dist, bool to_self, bool ownTeamOnly)
			: m_player(player), m_message(msg), m_toSelf(to_self), m_ownTeamOnly(ownTeamOnly), m_dist(dist)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ���(�������Ҫ��)������ӵ����(������ң�������Ŷ�����׶�������Ҫ��)������Ϣ
		 */
		void Visit(CameraMapType&);
		template<class SKIP>
		void Visit(GridRefManager<SKIP>&)
		{
		}
	};

	// ����Ϸ�����������񸽽�����ҷ�����Ϣ(������)
	struct MANGOS_DLL_DECL ObjectMessageDistDeliverer
	{
		uint32 m_phaseMask;     // �Ŷ�����׶�����
		WorldObject& m_object;  // ��Ϸ����(�������������б�����ĵ�)
		WorldPacket* m_message; // ��Ϣ
		int32 m_dist;           // ����

		ObjectMessageDistDeliverer(WorldObject& obj, WorldPacket* msg, int32 dist)
			: m_object(obj), m_message(msg), m_dist(dist)
		{
		}
		/**
		 * ֪ͨ�����ڵ�������Ͷ���(�������Ҫ��)������ӵ����(������ң�������Ŷ�����׶�������Ҫ��)������Ϣ
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
