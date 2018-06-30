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

#ifndef TDMA_API_H_
#define TDMA_API_H_

#include <string>
#include <functional>
#include <sstream>
#include <chrono>

#include "util.h"
#include "tdma_api_get.h"
#include "tdma_api_streaming.h"


namespace tdma{

const std::string URL_BASE = "https://api.tdameritrade.com/v1/";

inline json
get_user_principals_for_streaming(Credentials& creds)
{ return UserPrincipalsGetter(creds,true,true,false,false).get(); }

StreamerInfo
get_streamer_info(Credentials& creds);

/*
void
default_api_on_error_callback( long code,
                               const std::string& data,
                               bool allow_refresh );
*/

std::pair<json, conn::clock_ty::time_point>
api_execute( conn::HTTPSConnection& connection,
              Credentials& creds,
              api_on_error_cb_ty on_error_cb );//= default_api_on_error_callback );


json
api_auth_execute( conn::HTTPSPostConnection& connection, std::string fname);

bool
error_msg_about_token_expiration(const std::string& msg);


std::string
unescape_returned_post_data(const std::string& s);

} /* amtd */

#endif // TDMA_API_H_
