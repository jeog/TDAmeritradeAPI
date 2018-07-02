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

namespace util {

using namespace std;

#ifndef _WIN32
SignalBlocker::SignalBlocker( std::set<int> signums )
    :
        _mask()
    {
        sigemptyset(&_mask);
        for(auto sig : signums){
            sigaddset(&_mask, sig);
        }
        if( pthread_sigmask(SIG_BLOCK, &_mask, &_mask_old) )
            throw std::runtime_error("pthread_sigmask failed");
    }


SignalBlocker::~SignalBlocker()
{
    timespec t{};

    /* grab the pending signals */
    while( sigtimedwait(&_mask, 0, &t) > 0 )
    {}
    if( errno != EAGAIN )
        throw runtime_error("sigtimedwait failed: " + to_string(errno));

    /* reset to original state */
    if( pthread_sigmask(SIG_SETMASK, &_mask_old, 0) == -1 )
        throw runtime_error("pthread_sigmask failed: " + to_string(errno));
}
#endif

void
debug_out(std::string tag, std::string message, std::ostream& out)
{
#ifdef DEBUG
    static std::mutex mtx;
    std::lock_guard<std::mutex> _(mtx);
    out<< std::setw(20) << std::left << tag << ' '
       << std::setw(20) << std::left << this_thread::get_id() << ' '
       << message << endl;
#endif
}


string
url_encode(const string& url)
{
    stringstream ss;
    ss << hex;

    for(auto i = url.begin(); i < url.end(); ++i){
        auto c = *i;
        if(isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'){
            ss << c;
        }else{
            ss << uppercase << '%' << setw(2)
               << (int)(unsigned char)c << nouppercase;
        }
    }
    return ss.str();
}


string
build_encoded_query_str(const vector<pair<string,string>>& params)
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

