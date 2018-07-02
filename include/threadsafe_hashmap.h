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

#ifndef THREADSAFE_HASHMAP_H
#define THREADSAFE_HASHMAP_H

#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "_common.h"

template<typename K, typename V >
class ThreadSafeHashMap {
    std::unordered_map<K, V> _map;
    mutable std::mutex _mtx;

public:
    typedef std::function<void(const std::unordered_map<K,V>& m)> access_cb_ty;

    ThreadSafeHashMap() = default;

    ThreadSafeHashMap(const ThreadSafeHashMap& m)
    {
        std::lock_guard<std::mutex> _(m._mtx);
        _map = m._map;
    }

    /* restrict for now, deadlock seems possible from reciprocal
       assigns from different threads using both mutexs */
    ThreadSafeHashMap&
    operator=(const ThreadSafeHashMap&) = delete;

    // NO MOVE ASSIGN
    ThreadSafeHashMap&
    operator=(ThreadSafeHashMap&) = delete;

    // NO MOVE
    ThreadSafeHashMap(ThreadSafeHashMap&&) = delete;

    virtual
    ~ThreadSafeHashMap(){}

    void
    insert(const K& k, const V& v)
    {
        std::lock_guard<std::mutex> _(_mtx);
        _map[k] = v;
    }

    std::pair<V, bool>
    get_safe(const K& k)
    {
        std::lock_guard<std::mutex> _(_mtx);
        if( _map.count(k) )
            return std::make_pair(_map[k], true);
        return std::make_pair(V(), false);
    }

    std::pair<V, bool>
    get_and_remove_safe(const K& k)
    {
        std::lock_guard<std::mutex> _(_mtx);
        auto p = _map.find(k);
        if( p != _map.end() ){
            auto ret = std::make_pair(p->second, true);
            _map.erase(p);
            return ret;
        }
        return std::make_pair(V(), false);
    }

    bool
    empty() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _map.empty();
    }

    size_t
    size() const
    {
        std::lock_guard<std::mutex> _(_mtx);
        return _map.size();
    }

    void
    clear()
    {
        std::lock_guard<std::mutex> _(_mtx);
        _map.clear();
    }

    void
    access(access_cb_ty access_cb)
    {
        std::lock_guard<std::mutex> _(_mtx);
        access_cb(_map);
    }
};

#endif // THREADSAFE_HASHMAP_H
