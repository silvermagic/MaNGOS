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


#include "ObjectAccessor.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "GridNotifiers.h"

using namespace MaNGOS;

void VisibleChangesNotifier::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
	{
        iter->getSource()->UpdateVisibilityOf(&m_object);
    }
}

void MessageDeliverer::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter) {
        Player* owner = iter->getSource()->GetOwner();
        if (m_toSelf || owner != &m_player) {
            // 天堂不存在团队任务，所以玩家所处阶段肯定是相同的
            if (!m_player.InSamePhase(iter->getSource()->GetBody()))
			{
            	continue;
			}

            if (!m_player.IsInScreen(iter->getSource()->GetBody()))
			{
            	continue;
			}

            if (WorldSession* session = owner->GetSession())
			{
            	session->SendPacket(m_message);
			}
        }
    }
}

void MessageDelivererExcept::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter) {
        Player* owner = iter->getSource()->GetOwner();
        if (!owner->InSamePhase(m_phaseMask) || owner == m_skipped_receiver)
		{
        	continue;
		}

		if (!m_object.IsInScreen(iter->getSource()->GetBody()))
		{
			continue;
		}

        if (WorldSession* session = owner->GetSession())
		{
        	session->SendPacket(m_message);
		}
    }
}

void ObjectMessageDeliverer::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter) {
        Player* owner = iter->getSource()->GetOwner();
        if (!iter->getSource()->GetBody()->InSamePhase(m_phaseMask))
		{
        	continue;
		}

		if (!m_object.IsInScreen(iter->getSource()->GetBody()))
		{
			continue;
		}

        if (WorldSession* session = owner->GetSession())
		{
        	session->SendPacket(m_message);
		}
    }
}

void MessageDistDeliverer::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter) {
        Player* owner = iter->getSource()->GetOwner();

        // todo: 团队
        if ((m_toSelf || owner != &m_player) && (!m_dist || iter->getSource()->GetBody()->IsWithinDist(&m_player, m_dist))) {
            if (!m_player.InSamePhase(iter->getSource()->GetBody()))
			{
            	continue;
			}

            if (WorldSession* session = owner->GetSession())
			{
            	session->SendPacket(m_message);
			}
        }
    }
}

void ObjectMessageDistDeliverer::Visit(CameraMapType& m)
{
    for (CameraMapType::iterator iter = m.begin(); iter != m.end(); ++iter) {
        if (!m_dist || iter->getSource()->GetBody()->IsWithinDist(&m_object, m_dist))
        {
            if (!m_object.InSamePhase(iter->getSource()->GetBody()))
			{
            	continue;
			}

            if (WorldSession* session = iter->getSource()->GetOwner()->GetSession())
			{
            	session->SendPacket(m_message);
			}
        }
    }
}

void ObjectUpdater::Visit(CreatureMapType& m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        WorldObject::UpdateHelper helper(iter->getSource());
        helper.Update(m_timeDiff);
    }
}

void DynamicObjectUpdater::Visit(CreatureMapType&  m)
{
    for (auto iter = m.begin(); iter != m.end(); ++iter)
    {
        VisitHelper(iter->getSource());
    }
}

void DynamicObjectUpdater::Visit(PlayerMapType&  m)
{
    for (auto iter = m.begin(); iter != m.end(); ++iter)
    {
        VisitHelper(iter->getSource());
    }
}
