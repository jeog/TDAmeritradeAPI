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

#include "../include/_tdma_api.h"
#include "../include/_common.h"
#include "../include/tdma_common.h"

using std::string;

namespace {

int last_error_code = 0;
string last_error_msg("");
int last_error_lineno = 0;
string last_error_filename("");

} /* namespace */


namespace tdma{

void
set_error_state( int code,
                   const string& msg,
                   int lineno,
                   const string& filename )
{
    last_error_code = code;
    last_error_msg = msg;
    last_error_lineno = lineno;
    last_error_filename = filename;
}

std::tuple<int, string, int, string>
get_error_state()
{
    return make_tuple(last_error_code, last_error_msg, last_error_lineno,
                      last_error_filename);
}

} /* tdma */


using namespace tdma;

int
LastErrorCode_ABI( int *code, int allow_exceptions )
{
    *code = last_error_code;
    return 0;
}

int
LastErrorMsg_ABI( char** buf, size_t *n, int allow_exceptions )
{ return to_new_char_buffer(last_error_msg, buf, n, allow_exceptions); }

int
LastErrorLineNumber_ABI( int *lineno, int allow_exceptions )
{
    *lineno = last_error_lineno;
    return 0;
}

int
LastErrorFilename_ABI( char** buf, size_t *n, int allow_exceptions )
{ return to_new_char_buffer(last_error_filename, buf, n, allow_exceptions); }

int
LastErrorState_ABI( int *code,
                      char **buf_msg,
                      size_t *n_msg,
                      int *lineno,
                      char **buf_filename,
                      size_t *n_filename,
                      int allow_exceptions )
{
    *code = last_error_code;
    *lineno = last_error_lineno;

    int err = LastErrorMsg_ABI(buf_msg, n_msg, allow_exceptions);
    if( err )
        return err;

    err = LastErrorFilename_ABI(buf_filename, n_filename, allow_exceptions);
    if( err )
        FreeBuffer_ABI(*buf_msg, 0);

    return err;
}


#endif /* ERROR_H_ */
