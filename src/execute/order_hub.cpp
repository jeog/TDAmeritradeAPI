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

#include "../../include/_tdma_api.h"
#include "../../include/_execute.h"

using namespace std;
using namespace conn;

namespace tdma{

// TODO

/*
json
TestExecuteImpl(std::string addr, std::string fields)
{
    HTTPSPostConnection connection;
    connection.SET_url(addr);
    connection.SET_fields(fields);
    connection.ADD_headers(
          { {"Accept", "application/json"},
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer Test Token AAAAAAAAAAAAAAAAAAAAA"} }
          );

    long r_code;
    string r_data;
    conn::clock_ty::time_point r_tp;
    tie(r_code, r_data, r_tp) = curl_execute(connection);

    if( r_code != HTTP_RESPONSE_OK ){
        cerr<< "error response: " << r_code << endl;
        TDMA_API_THROW(ConnectException,"TestExecute failed", r_code);
    }

    json r_json = json::parse(r_data);
    return r_json;
}
*/


} /* tdma */
