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

#include <sstream>
#include <iostream>
#include <regex>
#include <cctype>
#include <mutex>
#include <string.h>

#include "../include/_tdma_api.h"
#include "../include/curl_connect.h"

using std::string;
using std::vector;
using std::tuple;
using std::pair;
using std::cerr;
using std::endl;

#ifdef USE_SIGNAL_BLOCKER_
namespace{
util::SignalBlocker signal_blocker({SIGPIPE});
};
#endif

namespace {

bool
error_msg_about_token_expiration(const string& msg)
{
    using namespace std::regex_constants;

    static const std::regex ACCESS_TOKEN_RX("Access Token", ECMAScript | icase);
    static const std::regex EXPIRE_RX("Expire", ECMAScript | icase);

    return std::regex_search(msg, ACCESS_TOKEN_RX)
        && std::regex_search(msg, EXPIRE_RX);
}

} /* namespace */


namespace tdma{

bool
base_on_error_callback(long code, const string& data, bool allow_refresh)
{
    /*
     *  ***THE FOLLOWING CHANGED on Jun 30 2018 ***
     *
     *    500/expired token errors for the data calls require an unescape of the
     *    leading '{'. (A bug that should be fixed in v2 of API)
     *
     *    This version *seems* to work with:
     *       quotes
     *       historical
     *       options
     *
     *    SUCCESS_OK -> true
     *    500 & "Access Token expired" -> false
     *    everything else throws
     *
     *  *** THE FOLLOWING IS RELEVANT AFETER Jun 30 2018 ***
     *
     *    For ALL getters the server responds with 401 error to indicate
     *    an expired access token.
     *
     *  RETURN TRUE IF WE CAN REFRESH, FALSE IF WE DONT THROW FROM HERE
     */
    switch(code){
    case 401:
        if( !error_msg_about_token_expiration(data) )
            TDMA_API_THROW( ConnectException,
                            "unknown exception: " + data, code );

        if( !allow_refresh )
            TDMA_API_THROW( ConnectException,
                            "not allowed to refresh token: " + data, code );

        return true ; /* REFRESH TOKEN */
    case 403:
        TDMA_API_THROW( InvalidRequest,
                        "forbidden: " + data, code );
    case 404:
        TDMA_API_THROW( InvalidRequest,
                        "not found: " + data, code );
    case 500:
        TDMA_API_THROW( ServerError,
                        "unexpected server error: " + data, code);
    case 503:
        TDMA_API_THROW( ServerError,
                        "server (temporarily) unavailable: " + data, code );
    case 504:
        TDMA_API_THROW( ServerError,
                        "unknown server error: " + data, code);
    };
    return false;
}

void
data_api_on_error_callback(long code, const string& data)
{
    switch(code){
    case 400:
        TDMA_API_THROW( InvalidRequest,
                        "bad/malformed request: " + data, code );
    case 406:
        TDMA_API_THROW( InvalidRequest,
                        "invalid regex or excessive requests: " + data, code );
    };
}

void
account_api_on_error_callback(long code, const string& data)
{
    if( code == 400 )
        TDMA_API_THROW( InvalidRequest,
                        "validation problem: " + data, code );
}

void
query_api_on_error_callback(long code, const string& data)
{
    switch(code){
    case 400:
        TDMA_API_THROW( InvalidRequest,
                        "validation problem: " + data, code );
    case 406:
        TDMA_API_THROW( InvalidRequest,
                        "invalid regex or excessive requests: " + data, code );
    };
}


tuple<long, string, string, conn::clock_ty::time_point>
curl_execute(conn::HTTPConnectionInterface& connection, bool return_header_data)
{   /*
     * Curl exceptions are not exposed publicly so we catch and wrap
     */
    try{
        return connection.execute(return_header_data);
    }catch( conn::CurlConnectionError& e ){
        cerr<< "CurlConnectionError --> ConnectionException" << endl;
        string msg = e.what() + string("(curl code=")
                   + std::to_string(e.code) + ')';
        TDMA_API_THROW( ConnectException, msg );
    }
}


bool
on_return( long code,
           long success_code,
           const string& data,
           bool allow_refresh,
           api_on_error_cb_ty on_error_cb )
{
    if( code == success_code )
        return true;

    cerr<< "error response: " << code << endl;
    if( data.empty() )
        TDMA_API_THROW(ConnectException, "no return message", code);

    if( !base_on_error_callback(code, data, allow_refresh) ){
        /*
         * base callback will either:
         *   1) throw
         *   2) return true if needs to refresh token
         *   3) return false if it doesn't handle the error and should be
         *      passed off to the appropriate callback
         */
        on_error_cb(code, data);

        /* callback may not handle the error and throw so... */
        TDMA_API_THROW(ConnectException,"unknown exception", code);
    }
    return false;
}


vector<pair<string,string>>
build_auth_headers( const vector<pair<string,string>>& headers,
                    const string& access_token )
{
    vector<pair<string,string>> tmp(headers);
    tmp.emplace_back( "Authorization", "Bearer " + access_token);
    return tmp;
}


tuple<string, string, conn::clock_ty::time_point>
connect( conn::HTTPConnectionInterface& connection,
         Credentials& creds,
         const vector<pair<string,string>>& static_headers,
         api_on_error_cb_ty on_error_cb,
         bool return_headers,
         long success_code )
{
    if( !creds.access_token || !creds.client_id )
        TDMA_API_THROW( LocalCredentialException, "invalid credentials" );

    if( creds.access_token[0] == '\0' )
        TDMA_API_THROW( LocalCredentialException, "empty access_token" );

    if( creds.client_id[0] == '\0' )
        TDMA_API_THROW( LocalCredentialException, "empty client_id");

    if( connection.is_closed() )
        TDMA_API_THROW( APIException, "connection is closed");

    /*
     * cache access tokens across calls by client_id so all cred structs
     * of the same account are linked but different client_ids aren't
     *
     * NOTE - the cached token takes priority to avoid refresh 'thrashing'
     *        between unsynced callers
     */
    static std::unordered_map<string, string> token_cache;
    string& cached_token = token_cache.insert(
        {creds.client_id, creds.access_token}
    ).first->second;

    /* only add headers if we don't already have them */
    if( !connection.has_headers() ){
        auto headers = build_auth_headers(static_headers, cached_token);
        connection.add_headers(headers);
    }

    long r_code;
    string r_data, r_head;
    conn::clock_ty::time_point r_tp;
    tie(r_code, r_data, r_head, r_tp) = curl_execute(connection, return_headers);

    if( !on_return(r_code, success_code, r_data, true, on_error_cb) ){
        /*
         * if 'on_return' returns FALSE initially we have an expired token
         * IN THE HEADER (or in the header AND cache):
         *
         * 1) if current token in the header is different than cache:
         *     a) update the cred struct w/ the cache token (if different)
         *     b) reset auth header w/ the cache token
         *     c) try the call again
         *     d) if 'on_return' returns TRUE, return, else...
         * 2) refresh the token (automatically updates 'creds.access_token')
         * 3) update the cache
         * 4) update the header
         * 5) try again (this should either return true or THROW)
         */

        auto old_headers = connection.get_headers();
        assert( old_headers.back().first == "Authorization");

        /* first check that header token is same as cached version */
        if( old_headers.back().second != ("Bearer " + cached_token) ){

            /* overwrite the token in creds w/ cached */
            if( strcmp(creds.access_token, cached_token.c_str()) ){
                /*
                 * should only get in here if client is using references
                 * to different cred structs (not recommended)
                 */
                delete[] creds.access_token;
                creds.access_token = new char[cached_token.size() + 1];
                creds.access_token[cached_token.size()] = 0;
                strcpy(creds.access_token, cached_token.c_str());
            }

            /* update headers w/ cached */
            connection.reset_headers();
            auto new_headers = build_auth_headers(static_headers, cached_token);
            connection.add_headers(new_headers);

            /* try again */
            tie(r_code, r_data, r_head, r_tp) =
                curl_execute(connection, return_headers);

            /* if still FALSE, expired token IN CACHE, continue to refresh */
            if( on_return(r_code, success_code, r_data, true, on_error_cb) )
                return make_tuple(r_data, r_head, r_tp);
        }

        cerr<< "access token expired; try to refresh..." << endl;
        RefreshAccessToken(creds); // updates creds.access_token

        /* update the cache */
        cached_token = creds.access_token;

        /* update the header */
        connection.reset_headers();
        auto new_headers = build_auth_headers(static_headers, cached_token);
        connection.add_headers(new_headers);

        /* try again */
        tie(r_code, r_data, r_head, r_tp) =
            curl_execute(connection, return_headers);

        bool r = on_return(r_code, success_code, r_data, false, on_error_cb);
        assert(r); /* should either be true or have thrown */
        cerr<< "...successfully refreshed access token" << endl;
    } 

    return make_tuple(r_data, r_head, r_tp);
}


pair<string, conn::clock_ty::time_point>
connect_get( conn::HTTPConnectionInterface& connection,
             Credentials& creds,
             api_on_error_cb_ty on_error_cb )
{
    static const vector<pair<string,string>> STATIC_HEADERS = {
        {"Accept", "application/json"}
    };

    assert( connection.get_method() == conn::HttpMethod::http_get );

    string r_data, r_head;
    conn::clock_ty::time_point r_tp;
    tie(r_data, r_head, r_tp) = connect(connection, creds, STATIC_HEADERS,
                                        on_error_cb, false,
                                        conn::HTTP_RESPONSE_OK);

    return make_pair(r_data, r_tp);
}


pair<string, conn::clock_ty::time_point>
connect_execute( conn::HTTPConnectionInterface& connection,
                 Credentials& creds,
                 long success_code )
{
    static const vector<pair<string,string>> STATIC_HEADERS = {
        {"Accept", "*/*"},
        {"Content-Type", "application/json"}
    };

    assert( connection.get_method() != conn::HttpMethod::http_get );

    string r_data, r_head;
    conn::clock_ty::time_point r_tp;
    tie(r_data, r_head, r_tp) = connect( connection, creds, STATIC_HEADERS,
                                         account_api_on_error_callback,
                                         true, success_code );

    return make_pair(r_head, r_tp);
}


json
connect_auth(conn::HTTPConnectionInterface& connection, std::string fname)
{
    static const vector<pair<string,string>> STATIC_HEADERS = {
        {"Content-Type", "application/x-www-form-urlencoded"}
    };

    assert( connection.get_method() == conn::HttpMethod::http_post );

    connection.add_headers(STATIC_HEADERS);

    long r_code;
    string r_data, h_data;
    conn::clock_ty::time_point r_tp;
    tie(r_code, r_data, h_data, r_tp) = curl_execute(connection, false);

    if( r_code != conn::HTTP_RESPONSE_OK ){
        string e = fname + " failed: " + r_data;
        cerr<< "error response: " << r_code << endl << e << endl;
        TDMA_API_THROW(AuthenticationException, e, r_code);
    }

    return json::parse(r_data);
}

} /* tdma */

