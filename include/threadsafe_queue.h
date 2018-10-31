/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "_common.h"

template<typename T>
class ThreadSafeQueue {
    std::queue<T> _queue;
    mutable std::mutex _mtx;
    mutable std::mutex _push_mtx;
    std::condition_variable _push_cond;

public:
    typedef T value_type;
    typedef typename std::queue<T>::container_type container_type;
    typedef typename std::queue<T>::size_type size_type;

    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue& q)
    {
        std::lock_guard<std::mutex> _(q._mtx);
        _queue = q._queue;
    }

    /* restrict for now, deadlock seems possible from reciprocal
       assigns from different threads using both mutexs */
    ThreadSafeQueue&
    operator=(const ThreadSafeQueue&) = delete;

    // NO MOVE ASSIGN
    ThreadSafeQueue&
    operator=(ThreadSafeQueue&) = delete;

    // NO MOVE
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;

    virtual
    ~ThreadSafeQueue(){}

    T
    front() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _queue.front();
    }

    std::pair<T, bool>
    front_safe() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _queue.empty() ? std::make_pair(T(), false)
                              : std::make_pair(_queue.front(), true);
    }

    bool
    replace_front_safe(T obj)
    {
        std::lock_guard<std::mutex> _(_mtx);
        if( _queue.empty() )
            return false;
        _queue.front() = obj;
        return true;
    }

    T
    pop_front()
    {
        std::lock_guard<std::mutex> _(_mtx);
        auto tmp = _queue.front();
        _queue.pop();
        return tmp;
    }

    std::pair<T, bool>
    pop_front_safe()
    {
        std::lock_guard<std::mutex> _(_mtx);
        if( _queue.empty() ){
            return {};
        }
        auto tmp = std::make_pair(_queue.front(), true);
        _queue.pop();
        return tmp;
    }

    T
    pop_front_or_wait()
    {
        T val;

        std::unique_lock<std::mutex> lock(_push_mtx);
        _push_cond.wait(
            lock,
            [this, &val]{
                 std::lock_guard<std::mutex> _(_mtx);
                 if( !_queue.empty() ){
                     val = _queue.front();
                     _queue.pop();
                     return true;
                 }
                 return false;
            }
        );
        return val;
    }

    std::pair<T, bool>
    pop_front_or_wait_for(std::chrono::milliseconds timeout)
    {
        std::pair<T, bool> val;

        std::unique_lock<std::mutex> lock(_push_mtx);
        _push_cond.wait_for(
            lock, timeout,
            [this, &val]{
                 std::lock_guard<std::mutex> _(_mtx);
                 if( !_queue.empty() ){
                     val = std::make_pair(_queue.front(), true);
                     _queue.pop();
                     return true;
                 }
                 return false;
            }
        );
        return val;
    }

    const T&
    back() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _queue.back();
    }

    bool
    empty() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _queue.empty();
    }

    typename std::queue<T>::size_type
    size() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _queue.size();
    }

    void
    push(const T& value)
    {
        {
            // ?
            std::lock_guard<std::mutex> _a(_push_mtx);
            std::lock_guard<std::mutex> _b(_mtx);
            _queue.push(value);
        }
        _push_cond.notify_all();
    }

    template<typename... Args>
    void
    emplace(Args... args)
    {
        {
            // ?
            std::lock_guard<std::mutex> _a(_push_mtx);
            std::lock_guard<std::mutex> _b(_mtx);
            _queue.emplace(args...);
        }
        _push_cond.notify_all();
    }

    void
    pop()
    {
        std::lock_guard<std::mutex> _(_mtx);
        _queue.pop();
    }

};

#endif // THREADSAFE_QUEUE_H
