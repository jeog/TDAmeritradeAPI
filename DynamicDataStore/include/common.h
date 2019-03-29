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


#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <string>
#include <algorithm>
#include <cctype>


bool
log_init(const std::string& path);


void
log_info( const std::string& tag,
          const std::string& msg1,
          const std::string msg2 = "",
          const std::string sep = ":" );


void
log_error( const std::string& tag,
           const std::string& msg1,
           const std::string msg2 = "",
           const std::string sep = ":" );


inline std::string
toupper(std::string str)
{
    std::transform( str.begin(), str.end(), str.begin(),
                    [](unsigned char c){ return std::toupper(c); } );
    return str;
}


template< typename Collection, typename ValTy, typename LessThanPred >
typename Collection::iterator
find_desc( Collection& c, ValTy val, LessThanPred cmp )
{
    auto b = std::lower_bound( c.rbegin(), c.rend(), val, cmp );
    if( (b == c.rend()) || cmp(val, *b) )
        return c.end();

    return (b + 1).base();
}


template<typename T >
inline bool
fits_in_signed(T t)
{
    using L = std::numeric_limits<typename std::make_signed<T>::type>;
    return std::is_signed<T>::value || t <= static_cast<T>(L::max());
}


template<typename T>
inline T
bounded( T val, T low, T high )
{
    return std::max(low, std::min(high, val));
}

#endif /* INCLUDE_COMMON_H_ */
