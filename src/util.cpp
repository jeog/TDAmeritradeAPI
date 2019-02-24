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

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <vector>
#include <cctype>

#include "../include/util.h"

using std::string;
using std::stringstream;

namespace util {

#ifdef USE_SIGNAL_BLOCKER_

#define THROW_RUNTIME_WITH_ERRNO(msg) \
do{ \
    int e = errno; \
    string s = string("SignalBlocker: ") + msg + string(", errno: ") \
        + std::to_string(e); \
    throw std::runtime_error(s); \
}while(0)

SignalBlocker::SignalBlocker( const std::set<int>& signums )
    :
        _mask(),
        _active(false),
        _signums( signums )
    {
        if( !signums.empty() ){
            sigemptyset(&_mask);
            for(auto sig : signums){
                sigaddset(&_mask, sig);
            }
            if( pthread_sigmask(SIG_BLOCK, &_mask, &_mask_old) )
                THROW_RUNTIME_WITH_ERRNO("pthread_sigmask failed");
            _active = true;
        }
    }

void
SignalBlocker::_grab_pending()
{
    /*
     * making two assumptions to be safe:
     *   - can be multiple signals of same type (as per sigwait docs)
     *   - there may be signal types pending that we don't know about,
     *     in which case we only wait on ours
     */

    int dummy, ret, n = 0;
    sigset_t pending, waiton;

    while( true ){
        if( sigemptyset( &pending ) )
            THROW_RUNTIME_WITH_ERRNO("sigemptyset(1) failed");

        if( sigpending( &pending ) )
            THROW_RUNTIME_WITH_ERRNO("sigpending failed");

        if( sigemptyset( &waiton ) )
            THROW_RUNTIME_WITH_ERRNO("sigemptyset(2) failed");

        for( int sig : _signums ){
            ret = sigismember(&pending, sig);
            if( ret == -1 )
                THROW_RUNTIME_WITH_ERRNO("sigismember failed");
            else if( ret == 1 ){
                if( sigaddset( &waiton, sig ) )
                    THROW_RUNTIME_WITH_ERRNO("sigaddset failed");
                ++n;
            }
        }

        if( n < 1 )
            break;

        while( n-- ){
            if( sigwait(&waiton, &dummy) )
                THROW_RUNTIME_WITH_ERRNO("sigwait failed");
        }
    }
}

SignalBlocker::~SignalBlocker()
{
    if( _active ){
        _grab_pending();

        /* reset to original state */
        if( pthread_sigmask(SIG_SETMASK, &_mask_old, 0) == -1 )
            THROW_RUNTIME_WITH_ERRNO("pthread_sigmask failed");
    }
}

#undef THROW_RUNTIME_WITH_ERRNO
#endif /* USE_SIGNAL_BLOCKER_ */

void
debug_out(string tag, string message, std::ostream& out)
{
#ifdef DEBUG_VERBOSE_1_
    static std::mutex mtx;
    std::lock_guard<std::mutex> _(mtx);
    out<< std::setw(20) << std::left << tag << ' '
       << std::setw(20) << std::left << std::this_thread::get_id() << ' '
       << message << std::endl;
#endif /* DEBUG_VERBOSE_1_ */
}


string
url_encode(const string& url)
{
    stringstream ss;
    ss << std::hex;

    for(auto i = url.begin(); i < url.end(); ++i){
        auto c = *i;
        if(isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'){
            ss << c;
        }else{
            ss << std::uppercase << '%' << std::setw(2)
               << (int)(unsigned char)c << std::nouppercase;
        }
    }
    return ss.str();
}


string
build_encoded_query_str(const std::vector<std::pair<string,string>>& params)
{
    if( params.empty() )
        return "";

    stringstream ss;
    for(auto& p : params){
        ss << p.first << "=" << util::url_encode(p.second) << "&";
    }
    string s(ss.str());
    return s.erase(s.size() - 1);
}



bool
is_all_digits(string::const_iterator& p, int n) // note the ref
{
    for(int i = 0; i < n; ++i, ++p){
        if( !isdigit(*p) )
            return false;
    }
    return true;
}

/*
 * yyyy-MM-dd  or yyy-MM-ddTHH:mm:ssz
 * web interface allows some different values for 'T' and 'z' than the spec
 * we'll just check they exist (upper or lower case)
 */
bool
is_valid_iso8601_datetime(const string& dt)
{
    size_t s = dt.size();
    if( s != 10 && s != 20)
        return false;

    auto p = dt.cbegin();
    if( !is_all_digits(p, 4) ) return false;
    if( *p++ != '-' )          return false;
    if( !is_all_digits(p, 2) ) return false;
    if( *p++ != '-' )          return false;
    if( !is_all_digits(p, 2) ) return false;
    if( p == dt.cend() )       return true;

    if( toupper(*p++) != 'T' ) return false;
    if( !is_all_digits(p,2) )  return false;
    if( *p++ != ':' )          return false;
    if( !is_all_digits(p,2) )  return false;
    if( *p++ != ':')           return false;
    if( !is_all_digits(p, 2) ) return false;
    if( toupper(*p) != 'Z' )   return false;

    return true;
}

} /* util */


