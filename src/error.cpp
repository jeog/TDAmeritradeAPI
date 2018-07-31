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

#ifndef ERROR_H_
#define ERROR_H_

#include <string>
#include <chrono>

#include "../include/_common.h"
#include "../include/tdma_common.h"

namespace {
int last_error_code = 0;
std::string last_error_msg("");
};

int
LastErrorCode_ABI( int *code, int allow_exceptions )
{
    *code = last_error_code;
    return 0;
}

int
LastErrorMsg_ABI( char** buf, size_t *n, int allow_exceptions )
{
    *n = last_error_msg.size() + 1;
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !*buf ){
        std::string err("failed to allocate buffer memory");
        set_error_state(TDMA_API_MEMORY_ERROR, err);
        if( allow_exceptions ){
            throw tdma::MemoryError(err);
        }
        return TDMA_API_MEMORY_ERROR;
    }
    (*buf)[(*n)-1] = 0;
    strncpy(*buf, last_error_msg.c_str(), (*n)-1);
    return 0;
}

void
set_error_state(int code, const std::string&  msg)
{
    last_error_code = code;
    last_error_msg = msg;
}

std::pair<int, std::string>
get_error_state()
{
    return std::make_pair(last_error_code, last_error_msg);
}

#endif /* ERROR_H_ */
