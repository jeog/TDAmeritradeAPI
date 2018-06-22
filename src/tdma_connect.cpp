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

#include "../include/_tdma_api.h"

namespace tdma{

using namespace std;
using namespace conn;
using namespace chrono;


/*
 *  The responses don't appear completely consistent w/ the docs so
 *  we have to define different callbacks. 
 */
void
default_api_on_error_callback( long code,
                                   const string& data,
                                   bool allow_refresh )
{
    /*
     *  500/expired token errors for the data calls require an unescape of the 
     *  leading '{'. (A bug that should be fixed in v2 of API)
     *
     *  This version *seems* to work with:
     *     quotes
     *     historical
     *     options
     *
     *  SUCCESS_OK -> true
     *  500 & "Access Token expired" -> false
     *  everything else throws
     *
     *  NOTE - WE SHOULD ONLY GET OUT OF HERE IF WE CAN REFRESH THE TOKEN
     */
    json data_json;
    string data_uesc, err_msg;

    switch(code){
    case 500:
        //"\n      \{\n     "error":"Access Token expired"\n    \t\t}\n     "
        if( !allow_refresh ){
            throw APIExecutionException("unknow exception", code);
        }
        data_uesc = unescape_returned_post_data(data);
        data_json = json::parse(data_uesc);
        err_msg = data_json["error"];
        if( err_msg != "Access Token expired" ){
            throw APIExecutionException("unknow exception: " + err_msg, code);
        }
        return; /* REFRESH TOKEN */
    case 503:
        throw ServerError("server (temporarily) unavailable", code);
    case 400:
        throw InvalidRequest("bad/malformed request", code);
    case 401:
        throw InvalidRequest("unauthorized", code);
    case 403:
        throw InvalidRequest("forbidden", code);
    case 404:
        throw InvalidRequest("not found", code);
    case 406:
        throw InvalidRequest("invalid regex or excessive requests", code);
    default:
        throw APIExecutionException("unknow exception", code);

    };
}


bool
on_api_return( long code,
               const string& data,
               bool allow_refresh,
               api_on_error_cb_ty on_error_cb )
{
    if( code == HTTP_RESPONSE_OK )
        return true;

    cerr<< "error response: " << code << endl;
    if( data.empty() )
        throw APIExecutionException("no return message", code);

    /* 
     * on_error_cb *should* only return if we need to refresh token
     * otherwise it throws some derivative of APIExecutionException 
     */
    on_error_cb(code, data, allow_refresh);
    cerr<< "access token expired; try to refresh..." << endl;
    return false;
}


tuple<long, string, conn::clock_ty::time_point>
curl_execute(HTTPSConnection& connection)
{
    /*
     * Curl exceptions are not exposed publicly so we catch and wrap
     */
    try{
        return connection.execute();
    }catch( CurlConnectionError& e ){
        cerr<< "CurlConnectionError --> APIExecutionException" << endl;
        throw APIExecutionException(e.what(), e.code);
    }
}


pair<json, conn::clock_ty::time_point>
api_execute( HTTPSConnection& connection,
             Credentials& creds,
             api_on_error_cb_ty on_error_cb )
{
    if( creds.access_token.empty() )
        throw LocalCredentialException("creds.access_token is empty");

    if( !connection.has_headers() ){
        /* 
         * IMPORTANT - don't add the headers if this is a repeat call to avoid a
         *             malformed request that can lead to http error or SIGPIPE 
         */
        connection.ADD_headers(
            { {"Accept", "application/json"},
              {"Authorization", "Bearer " + creds.access_token} }
            );
    }

    long r_code;
    string r_data;
    conn::clock_ty::time_point r_tp;
    tie(r_code, r_data, r_tp) = curl_execute(connection);
    
    if( !on_api_return(r_code, r_data, true, on_error_cb) ){
        /* 
         * if response callback returns false we have an expired token 
         */        
        RefreshAccessToken(creds);

        /* update the header */
        connection.RESET_headers();
        connection.ADD_headers(
            { {"Accept", "application/json"},
              {"Authorization", "Bearer " + creds.access_token} }
            );

        /* try again */
        tie(r_code, r_data, r_tp) = curl_execute(connection);
        on_api_return(r_code, r_data, false, on_error_cb);
    } 

    return make_pair( (r_data.empty() ? json() : json::parse(r_data)), r_tp );
}


json
api_auth_execute(HTTPSPostConnection& connection, std::string fname)
{
    assert(connection); 

    long r_code;
    string r_data;
    conn::clock_ty::time_point r_tp;
    tie(r_code, r_data, r_tp) = curl_execute(connection);

    if( r_code != HTTP_RESPONSE_OK ){
        cerr<< "error response: " << r_code << endl;
        throw AuthenticationException(fname + " failed", r_code);
    }

    string r_esc = unescape_returned_post_data(r_data); // REMOVE ????
    json r_json = json::parse(r_esc);
    return r_json;
}


const milliseconds APIGetter::DEF_WAIT_MSEC(500);

milliseconds APIGetter::wait_msec(APIGetter::DEF_WAIT_MSEC);
milliseconds APIGetter::last_get_msec(util::get_msec_since_epoch<conn::clock_ty>());

mutex APIGetter::get_mtx;


APIGetter::APIGetter(Credentials& creds, api_on_error_cb_ty on_error_callback)
    :       
        _on_error_callback(on_error_callback),
        _credentials(creds),
        _connection()
    {       
    }


json
APIGetter::get()
{
    if( !_connection )
        throw APIException("connection is closed");

    assert( !_connection.is_closed() );
    return APIGetter::throttled_get(*this);
}


json
APIGetter::throttled_get(APIGetter& getter)
{
    using namespace chrono;

    /* 
     * get_mtx allows threaded api execution from different getters in
     * different threads AND the same getter in different threads.
     *
     * IT DOESN'T HANDLE OTHER OTHER SYNC ISSUES INSIDE THE CurlConnection
     * CLASSES.
     */
    lock_guard<mutex> _(get_mtx);

    auto elapsed = util::get_msec_since_epoch<conn::clock_ty>() - last_get_msec;
    assert( elapsed.count() >= 0 );

    auto remaining = wait_msec - elapsed;
    if( remaining.count() > 0 ){
        /*
         * wait_msec and last_get_msec provide a global throttling mechanism
         * for ALL get requests to avoid excessive calls to TDMA servers
         */
        assert( remaining <= wait_msec );
        this_thread::sleep_for( remaining );
    }

    json j;
    conn::clock_ty::time_point tp;
    tie(j, tp) = api_execute( getter._connection, getter._credentials,
                              getter._on_error_callback );

    last_get_msec = duration_cast<milliseconds>(tp.time_since_epoch());
    return j;
}


void
APIGetter::set_wait_msec(milliseconds msec)
{
    lock_guard<mutex> _(get_mtx);
    wait_msec = msec;
}


bool
error_msg_about_token_expiration(const string& msg)
{
    using namespace regex_constants;

    static const regex ACCESS_TOKEN_RX("Access Token", ECMAScript | icase);
    static const regex EXPIRE_RX("Expire", ECMAScript | icase);

    return regex_search(msg, ACCESS_TOKEN_RX) && regex_search(msg, EXPIRE_RX);
}


string
unescape_returned_post_data(const string& s)
{
    stringstream ss;

    for(auto i = s.begin(); i < s.end(); ++i){
        if( *i != '\\'){
            ss << *i ;
        }
    }
    return ss.str();
}


} /* tdma */
