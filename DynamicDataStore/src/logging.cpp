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
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <sstream>
#include <mutex>
#include <algorithm>

#include "common.h"

namespace {

const int W = 24;

std::ofstream log_file;
std::mutex log_mtx;

std::string
trim(const std::string& str, int w)
{
    size_t sz = std::min(static_cast<size_t>(w), str.size());
    return str.substr(0, sz);
}

struct OStreamStateReseter{
    std::ostream& stream;
    std::ios old_state;
    OStreamStateReseter(std::ostream& stream)
        : stream(stream), old_state(nullptr)
    { old_state.copyfmt(stream); }
    ~OStreamStateReseter()
    { stream.copyfmt(old_state); }
};


void
log_write(std::ostream& out, const std::string& c1, const std::string& c2,
          const std::string& c3, const std::string& c4)
{
    OStreamStateReseter reseter(out);
    out << std::setw(W) << std::left << trim(c1,W)
        << std::setw(W) << trim(c2,W)
        << std::setw(W) << trim(c3,W)
        << c4 << std::endl;
}


void
log(const std::string& tag, const std::string& msg, std::ostream& out)
{
    using namespace std::chrono;

    static const std::string TIME_FORMAT_STR("%Y-%m-%d %H:%M:%S");

    auto now_t = system_clock::to_time_t( system_clock::now() );
    std::string buf(W, '\0');
    std::strftime( &buf[0], W, TIME_FORMAT_STR.c_str(),
                   localtime(&now_t) );
    buf.erase( buf.find_first_of('\0') );

    std::stringstream ss;
    ss << std::this_thread::get_id();

    log_write(out, buf, ss.str(), tag, msg);
}


void
log(const std::string& tag, const std::string& msg, bool is_error)
{
    if( log_file ){
        if( log_file.tellp() == std::streamoff(0) )
            log_write( log_file, "DATETIME", "THREAD", "TAG", "MESSAGE");
        log(tag, msg, log_file);
    }else if( !is_error )
        log(tag, msg, std::cout);

    if( is_error )
        log(tag, msg, std::cerr);
}

} /* namespace */


bool
log_init(const std::string& path)
{
    std::lock_guard<std::mutex> lock(log_mtx);
    log_file.open( path, std::ios_base::out | std::ios_base::app );
    if( !log_file ){
        std::cerr<< "failed to open log file: " << path << std::endl;
        return false;
    }
    return true;
}

void
log_info( const std::string& tag,
          const std::string& msg1,
          const std::string msg2,
          const std::string sep )
{
    std::string msg = msg2.empty() ? msg1 : (msg1 + ' ' + sep + ' ' + msg2);
    std::lock_guard<std::mutex> lock(log_mtx);
    log(tag, msg, false);
}

void
log_error( const std::string& tag,
           const std::string& msg1,
           const std::string msg2,
           const std::string sep )
{
    std::string msg = msg2.empty() ? msg1 : (msg1 + ' ' + sep + ' ' + msg2);
    std::lock_guard<std::mutex> lock(log_mtx);
    log(tag, msg, true);
}
