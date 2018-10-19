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

#ifdef __cplusplus

namespace tdma{

using namespace std;

std::string
BuildOptionSymbolImpl( const std::string& underlying,
                         unsigned int month,
                         unsigned int day,
                         unsigned int year,
                         bool is_call,
                         double strike )
{
    std::stringstream ss;

    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month (1-12)");

    if( day < 1 || day > 31 )
        TDMA_API_THROW(ValueException,"invalid day (1-31)");

    if( underlying.empty() )
        TDMA_API_THROW(ValueException,"underlying is empty");

    if( underlying.find('_') != std::string::npos )
        TDMA_API_THROW(ValueException,"invalid character in underlying: '_'" );

    std::string yy = std::to_string(year);
    if( yy.size() != 4 )
        TDMA_API_THROW(ValueException,"invalid number of digits in year");

    if( strike <= 0.0 )
        TDMA_API_THROW(ValueException,"strike price <= 0.0");

    ss << util::toupper(underlying) << '_'
       << std::setw(2) << std::setfill('0') << month
       << std::setw(2) << std::setfill('0') << day
       << std::setw(2) << std::setfill('0') << yy.substr( yy.size() - 2 )
       << (is_call ? 'C' : 'P');

    std::string s(std::to_string(strike));
    if( s.find('.') != std::string::npos ){
        s.erase( s.find_last_not_of('0') + 1, std::string::npos);
        s.erase( s.find_last_not_of('.') + 1, std::string::npos);
    }
    return ss.str() + s;
}

void
CheckOptionSymbolImpl(const std::string& option) // ABC_MMDDYY[C|P]{STRIKE}
{
    size_t sz = option.size();
    if( sz == 0 )
        TDMA_API_THROW(ValueException,"option string is empty");

    size_t uspos = option.find('_');
    if( uspos == std::string::npos )
        TDMA_API_THROW(ValueException,"option string is missing underscore");
    if( uspos == 0)
        TDMA_API_THROW(ValueException,"option string is missing underlying");

    size_t cppos = option.find_first_of("CP",uspos+1);
    if( cppos == std::string::npos )
        TDMA_API_THROW(ValueException,"option string is missing strike type(C/P)");

    if( (cppos - uspos - 1) != 6 )
        TDMA_API_THROW(ValueException,"option string contains invalid date substr size");

    static const std::regex DATE_RE("[0-9]{6}");
    if( !std::regex_match(option.substr(uspos+1, 6), DATE_RE) )
        TDMA_API_THROW(ValueException,"option string contains date substr with non-digits");

    int month = std::stoi(option.substr(uspos+1, 2));
    if( month < 1 || month > 12 )
        TDMA_API_THROW(ValueException,"invalid month");

    int day = std::stoi(option.substr(uspos+3,2));
    if( day < 1 || day > 31 ) // TODO
        TDMA_API_THROW(ValueException,"invalid day");

    int year = std::stoi(option.substr(uspos+5,2));
    if( year < 0 || year > 99 )
        TDMA_API_THROW(ValueException,"invalid year");

    assert( cppos == uspos + 5 + 2);
    if( cppos + 1 == sz )
        TDMA_API_THROW(ValueException,"option string is missing strike");

    std::string strk(option.substr(uspos+8, std::string::npos));

    // TODO careful w/ non-standard strikes i.e split-adjusted
    static const std::regex STRK_RE(
        "^([1-9][0-9]*(\\.[0-9]*[1-9])?)|(\\.[0-9]*[1-9])$");
    if( !std::regex_match(strk, STRK_RE) )
        TDMA_API_THROW(ValueException,"option string contains invalid strike");
}

int
to_new_char_buffer( const std::string& s, char** buf, size_t* n,
                      bool allow_exceptions )
{
    assert(buf);
    assert(n);
    *n = s.size() + 1;
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !(*buf) ){
        return HANDLE_ERROR( MemoryError, "failed to allocate buffer memory",
                             allow_exceptions );
    }
    strncpy(*buf, s.c_str(), (*n)-1);
    (*buf)[(*n)-1] = 0;
    return 0;
}

} /* tdma */


#endif /* __cplusplus */

using namespace tdma;

int
LibraryBuildDateTime_ABI(char **buf, size_t *n, int allow_exceptions)
{
    static const std::string DT = __DATE__ + std::string(" - ") + __TIME__;

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
    std::string r;
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







