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
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <regex>

#include "../include/_tdma_api.h"

using std::string;
using std::stringstream;


namespace tdma{

string
BuildOptionSymbolImpl( const string& underlying,
                       unsigned int month,
                       unsigned int day,
                       unsigned int year,
                       bool is_call,
                       double strike )
{
    using std::setw;
    using std::setfill;
    stringstream ss;

    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month (1-12)");

    if( day < 1 || day > 31 )
        TDMA_API_THROW(ValueException,"invalid day (1-31)");

    if( underlying.empty() )
        TDMA_API_THROW(ValueException,"underlying is empty");

    if( underlying.find('_') != string::npos )
        TDMA_API_THROW(ValueException,"invalid character in underlying: '_'" );

    string yy = std::to_string(year);
    if( yy.size() != 4 )
        TDMA_API_THROW(ValueException,"invalid number of digits in year");

    if( strike <= 0.0 )
        TDMA_API_THROW(ValueException,"strike price <= 0.0");

    ss << util::toupper(underlying) << '_'
       << setw(2) << setfill('0') << month
       << setw(2) << setfill('0') << day
       << setw(2) << setfill('0') << yy.substr( yy.size() - 2 )
       << (is_call ? 'C' : 'P');

    string s(util::to_fixedpoint_string(strike));
    if( s.find('.') != string::npos ){
        s.erase( s.find_last_not_of('0') + 1, string::npos);
        s.erase( s.find_last_not_of('.') + 1, string::npos);
    }
    return ss.str() + s;
}

void
CheckOptionSymbolImpl(const string& option) // ABC_MMDDYY[C|P]{STRIKE}
{
    size_t sz = option.size();
    if( sz == 0 )
        TDMA_API_THROW(ValueException,"option string is empty");

    size_t uspos = option.find('_');
    if( uspos == string::npos )
        TDMA_API_THROW(ValueException,"option string is missing underscore");
    if( uspos == 0)
        TDMA_API_THROW(ValueException,"option string is missing underlying");

    size_t cppos = option.find_first_of("CP",uspos+1);
    if( cppos == string::npos )
        TDMA_API_THROW( ValueException,
                        "option string is missing strike type(C/P)" );

    if( (cppos - uspos - 1) != 6 )
        TDMA_API_THROW( ValueException,
                        "option string contains invalid date substr size" );

    static const std::regex DATE_RE("[0-9]{6}");
    if( !std::regex_match(option.substr(uspos+1, 6), DATE_RE) )
        TDMA_API_THROW( ValueException,
                        "option string contains date substr with non-digits" );

    int month = stoi(option.substr(uspos+1, 2));
    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month");

    int day = stoi(option.substr(uspos+3,2));
    if( day < 1 || day > 31 ) // TODO
        TDMA_API_THROW(ValueException,"invalid day");

    int year = stoi(option.substr(uspos+5,2));
    if( year < 0 || year > 99 )
        TDMA_API_THROW(ValueException,"invalid year");

    assert( cppos == uspos + 5 + 2);
    if( cppos + 1 == sz )
        TDMA_API_THROW(ValueException,"option string is missing strike");

    string strk(option.substr(uspos+8, string::npos));

    // TODO careful w/ non-standard strikes i.e split-adjusted
    static const std::regex STRK_RE(
        "^([1-9][0-9]*(\\.[0-9]*[1-9])?)|(\\.[0-9]*[1-9])$");
    if( !std::regex_match(strk, STRK_RE) )
        TDMA_API_THROW(ValueException,"option string contains invalid strike");
}

int
to_new_char_buffer( const string& s,
                    char** buf,
                    size_t* n,
                    bool allow_exceptions )
{
    assert(buf);
    assert(n);

    *n = s.size() + 1;
    int err = alloc_to_buffer(buf, *n, allow_exceptions);
    if( err )
        return err;

    strncpy(*buf, s.c_str(), (*n)-1);
    (*buf)[(*n)-1] = 0;

    return 0;
}

int
to_new_char_buffers( const std::set<string>& strs,
                     char*** bufs,
                     size_t *n,
                     bool allow_exceptions )
{
    assert(bufs);
    assert(n);

    *n = strs.size();
    int err = alloc_to_buffer(bufs, *n, allow_exceptions);
    if( err )
        return err;

    int cnt = 0;
    for(auto& s : strs){
        size_t s_sz = s.size();
        (*bufs)[cnt] = reinterpret_cast<char*>(malloc(s_sz+1));
        if( !(*bufs)[cnt] ){
            // unwind allocations
            while( --cnt >= 0 ){
               free( (*bufs)[cnt] );
            }
            free( *bufs );
            return HANDLE_ERROR(tdma::MemoryError,
                "failed to allocate buffer memory", allow_exceptions
                );
        }
        (*bufs)[cnt][s_sz] = 0;
        strncpy((*bufs)[cnt], s.c_str(), s_sz);
        ++cnt;
    }

    return 0;
}

} /* tdma */



using namespace tdma;

int
LibraryBuildDateTime_ABI(char **buf, size_t *n, int allow_exceptions)
{
    static const string DT = __DATE__ + string(" - ") + __TIME__;

    return to_new_char_buffer(DT, buf, n, allow_exceptions);
}

int
BuildOptionSymbol_ABI( const char* underlying,
                       unsigned int month,
                       unsigned int day,
                       unsigned int year,
                       int is_call,
                       double strike,
                       char **buf,
                       size_t *n,
                       int allow_exceptions )
{
    CHECK_PTR(underlying, "underlying", allow_exceptions);
    CHECK_PTR(buf, "buf", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    int err;
    string r;
    std::tie(r,err) = CallImplFromABI(allow_exceptions, BuildOptionSymbolImpl,
                                      underlying, month, day, year, is_call,
                                      strike);
    if( err )
        return err;

    return to_new_char_buffer(r, buf, n, allow_exceptions);
}

int
CheckOptionSymbol_ABI(const char* symbol, int allow_exceptions)
{
    CHECK_PTR(symbol, "symbol", allow_exceptions);
    return CallImplFromABI(allow_exceptions, CheckOptionSymbolImpl, symbol);
}

int
FreeBuffer_ABI( char* buf, int allow_exceptions )
{
    if( buf )
        free( (void*)buf );
    return 0;
}

int
FreeBuffers_ABI( char** bufs, size_t n, int allow_exceptions )
{
    if( bufs ){
        while(n--){
          char *c = bufs[n];
          assert(c);
          free(c);
        }
        free(bufs);
    }
    return 0;
}

int
FreeFieldsBuffer_ABI( int* fields, int allow_exceptions )
{
    if( fields )
        free( (void*)fields );
    return 0;
}







