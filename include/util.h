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
#include <algorithm>
#include <cctype>

#include "_common.h"

namespace util{

namespace func{

template<typename R, typename... Args>
struct traits_base{
    typedef R return_type;
    typedef R signature_type(Args...);
    typedef std::tuple<Args...> args_tuple_type;

    static constexpr size_t nargs = std::tuple_size<args_tuple_type>::value;

    template<size_t i>
    struct arg{
        typedef typename std::tuple_element<i, args_tuple_type>::type type;
    };
};

template<typename T>
struct traits;

template<typename R, typename... Args>
struct traits< std::function<R(Args...)> >
        : public traits_base<R, Args...>{
};

template<typename R, typename... Args>
struct traits< R(*)(Args...) >
        : public traits_base<R, Args...>{
};

template<typename T>
struct signature;

template<typename R, typename... Args>
struct signature< R(Args...) >
        : public traits_base<R, Args...> {
    typedef typename traits_base<R, Args...>::signature_type type;

    template<typename F>
    static constexpr bool
    matches(F func)
    { return std::is_same<type, typename traits<F>::signature_type>::value; }
};

} /* func */

#ifdef USE_SIGNAL_BLOCKER_
class SignalBlocker {
    sigset_t _mask, _mask_old;
    bool _active;
    std::set<int> _signums;

    void
    _grab_pending();

public:
    SignalBlocker( const std::set<int>& signums );
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
#ifdef DEBUG_VERBOSE_1_
    static std::mutex mtx;
    std::lock_guard<std::mutex> _(mtx);
    out<< std::setw(40) << std::left << tag << ' '
       << std::setw(20) << std::left << std::hex
                        << reinterpret_cast<size_t>(obj) << std::dec << ' '
       << std::setw(20) << std::left << std::this_thread::get_id() << ' '
       << message << std::endl;
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
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        ClockTy::now().time_since_epoch()
        );
}

inline std::string
toupper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

template< template<typename T, typename... V> class C, typename A, typename...B >
C<A,B...>
toupper(const C<A, B...>& container)
{
    C<A,B...> ret;
    std::transform(container.begin(), container.end(),
                   std::inserter(ret, ret.begin()),
                   [](const A& a){ return toupper(a); });
    return ret;
}

using std::toupper;

template<typename ToTy, typename FromTy>
std::set<ToTy> // not safe
buffers_to_set(FromTy *buffers, size_t n)
{
    std::set<ToTy> s;
    while( n-- )
        s.emplace(static_cast<ToTy>(buffers[n]));
    return s;
}

template<>
inline std::set<std::string> // not safe
buffers_to_set(const char **buffers, size_t n)
{
    std::set<std::string> s;
    while( n-- ){
        const char* c = buffers[n];
        assert(c);
        s.emplace(c);
    }
    return s;
}

class IOStreamFormatGuard{
    std::iostream& _stream;
    std::ios _state;
public:
    explicit IOStreamFormatGuard(std::iostream& stream)
        :
            _stream(stream),
            _state(nullptr)
        { _state.copyfmt(_stream); }

    ~IOStreamFormatGuard()
    { _stream.copyfmt(_state); }
};


class FixedPrecisionConverter{
    std::stringstream _ss;
public:
    FixedPrecisionConverter(unsigned int decimal_places){
        _ss.precision(decimal_places);
        std::fixed(_ss);
    }
    template<typename T>
    std::string
    to_string(T val,
        typename std::enable_if<std::is_floating_point<T>::value,T>::type* _=0){
        _ss.str({});
        _ss.clear();
        _ss << val;
        return _ss.str();
    }
};

template<typename T>
std::string
to_fixedpoint_string(T val, unsigned int decimal_places = 4,
    typename std::enable_if<std::is_floating_point<T>::value, T>::type* _ = 0)
{
    static FixedPrecisionConverter fp4(4);

    switch(decimal_places){
    case 4: return fp4.to_string(val);
    /* ... */
    default:
        std::stringstream ss;
        ss << std::fixed << std::setprecision(decimal_places) << val;
        return ss.str();
    }
}

} /* util */

#endif /* UTIL_H */
