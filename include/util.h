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

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>
#include <chrono>
#include <mutex>
#include <thread>
#include <signal.h>

#include "_common.h"

namespace util{

#ifdef USE_SIGNAL_BLOCKER_
class SignalBlocker {
    sigset_t _mask, _mask_old;
    bool _active;
public:
    SignalBlocker( std::set<int> signums );
    SignalBlocker( SignalBlocker& sb ) = delete;
    SignalBlocker&
    operator=( SignalBlocker& sb ) = delete;
    virtual ~SignalBlocker();
};
#endif /* USE_SIGNAL_BLOCKER_ */

void
debug_out(std::string tag, std::string message, std::ostream& out=std::cout);

template<typename T>
void
debug_out( std::string tag,
            std::string message,
            T *obj,
            std::ostream& out )
{
    using namespace std;
#ifdef DEBUG_VERBOSE_1_
    static mutex mtx;
    lock_guard<mutex> _(mtx);
    out<< setw(20) << left << tag << ' '
       << setw(20) << left << hex << reinterpret_cast<size_t>(obj) << dec << ' '
       << setw(20) << left << this_thread::get_id() << ' '
       << message << endl;
#endif /* DEBUG_VERBOSE_1_ */
}

std::string
url_encode(const std::string& url);

std::string
build_encoded_query_str(
        const std::vector<std::pair<std::string, std::string>>& params
        );

bool
is_valid_iso8601_datetime(const std::string& dt);

template< template<typename T, typename... V> class C, typename A, typename...B >
std::string
join(const C<A, B...>& container, char j)
{
    if( container.empty() ){
        return "";
    }
    std::stringstream ss;
    for(auto& e : container){
        ss << e << j;
    }
    std::string s(ss.str());
    return s.erase(s.size() - 1);
}


template<typename ClockTy>
std::chrono::milliseconds
get_msec_since_epoch()
{
    using namespace std::chrono;

    auto now = ClockTy::now();
    return duration_cast<milliseconds>( now.time_since_epoch() );
}

} /* util */

#endif /* UTIL_H */
