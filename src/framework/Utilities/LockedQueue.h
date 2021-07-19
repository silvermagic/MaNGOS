/*
 * Copyright (C) 2009-2012 MaNGOS <http://getmangos.com/>
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

#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <deque>
#include <assert.h>
#include <boost/thread.hpp>
#include "Errors.h"

namespace MaNGOS
{
    template <class T, typename StorageType = std::deque<T> >
    class LockedQueue
    {
            //! Lock access to the queue.
            boost::mutex _lock;

            //! Storage backing the queue.
            StorageType _queue;

            //! Cancellation flag.
            /*volatile*/ bool _canceled;

        public:

            //! Create a LockedQueue.
            LockedQueue()
                : _canceled(false)
            {
            }

            //! Destroy a LockedQueue.
            virtual ~LockedQueue()
            {
            }

            //! Adds an item to the queue.
            void add(const T& item)
            {
                boost::lock_guard<boost::mutex> g(this->_lock);
                _queue.push_back(item);
            }

            //! Gets the next result in the queue, if any.
            bool next(T& result)
            {
                boost::lock_guard<boost::mutex> g(this->_lock);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                _queue.pop_front();

                return true;
            }

            template<class Checker>
            bool next(T& result, Checker& check)
            {
                boost::lock_guard<boost::mutex> g(this->_lock);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                if (!check.Process(result))
                    return false;

                _queue.pop_front();
                return true;
            }

            //! Peeks at the top of the queue. Remember to unlock after use.
            T& peek()
            {
                lock();

                T& result = _queue.front();

                return result;
            }

            //! Cancels the queue.
            void cancel()
            {
                boost::lock_guard<boost::mutex> g(this->_lock);
                _canceled = true;
            }

            //! Checks if the queue is cancelled.
            bool cancelled()
            {
                boost::lock_guard<boost::mutex> g(this->_lock);
                return _canceled;
            }

            //! Locks the queue for access.
            void lock()
            {
                this->_lock.lock();
            }

            //! Unlocks the queue.
            void unlock()
            {
                this->_lock.unlock();
            }

            ///! Checks if we're empty or not with locks held
            bool empty()
            {
                boost::lock_guard<boost::mutex> g(this->_lock);
                return _queue.empty();
            }
    };
}
#endif
