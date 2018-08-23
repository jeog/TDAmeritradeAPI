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

using namespace std;
using namespace chrono;


#ifdef USE_SIGNAL_BLOCKER_
namespace{
util::SignalBlocker signal_blocker({SIGPIPE});
};
#endif


namespace tdma{

using namespace conn;

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
    json data_json;
    string data_uesc, err_msg;

    switch(code){
    case 401:
        if( !allow_refresh ){
            throw APIExecutionException("not allowed to refresh token", code);
        }
        data_json = json::parse(data);
        err_msg = data_json["error"];
        if( !error_msg_about_token_expiration(err_msg) ){
            throw APIExecutionException("unknow exception: " + err_msg, code);
        }
        return true ; /* REFRESH TOKEN */
    case 403:
        throw InvalidRequest("forbidden", code);
    case 404:
        throw InvalidRequest("not found", code);
    case 500:
        // do we still need to unescape this ??
        data_uesc = unescape_returned_post_data(data);
        data_json = json::parse(data_uesc);
        err_msg = data_json["error"];
        throw ServerError("unexpected server error: " + err_msg, code);
    case 503:
        throw ServerError("server (temporarily) unavailable", code);
    case 504:
        throw ServerError("unknown server error: " + data, code);
    };
    return false;
}

void
data_api_on_error_callback(long code, const string& data)
{
    /*
     *  codes 500, 503, 401, 403, 404 handled by base callback
     */
    switch(code){
    case 400:
        throw InvalidRequest("bad/malformed request", code);
    case 406:
        throw InvalidRequest("invalid regex or excessive requests", code);
    };
}

void
account_api_on_error_callback(long code, const string& data)
{
    /*
     *  codes 500, 503, 401, 403, 404 handled by base callback
     */
    if( code == 400 )
        throw InvalidRequest("validation problem", code);
}

void
query_api_on_error_callback(long code, const string& data)
{
    /*
     *  codes 500, 503, 401, 403, 404 handled by base callback
     */
    switch(code){
    case 400:
        throw InvalidRequest("validation problem", code);
    case 406:
        throw InvalidRequest("invalid regex or excessive requests", code);
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
        throw APIExecutionException("unknown exception", code);
    }

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


pair<string, conn::clock_ty::time_point>
api_execute( HTTPSConnection& connection,
              Credentials& creds,
              api_on_error_cb_ty on_error_cb )
{
    if( !creds.access_token || string(creds.access_token).empty() )
        throw LocalCredentialException("creds.access_token is empty");

    if( !connection.has_headers() ){
        /* 
         * IMPORTANT - don't add the headers if this is a repeat call to avoid
         *             a malformed request
         */
        connection.ADD_headers(
            { {"Accept", "application/json"},
              {"Authorization", "Bearer " + string(creds.access_token)} }
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
              {"Authorization", "Bearer " + string(creds.access_token)} }
            );

        /* try again */
        tie(r_code, r_data, r_tp) = curl_execute(connection);
        on_api_return(r_code, r_data, false, on_error_cb);
    } 

    return make_pair( r_data, r_tp );
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
        throw AuthenticationException(fname + " failed " + r_data, r_code);
    }

    string r_esc = unescape_returned_post_data(r_data); // REMOVE ????
    json r_json = json::parse(r_esc);
    return r_json;
}


const milliseconds APIGetterImpl::DEF_WAIT_MSEC(500);

milliseconds APIGetterImpl::wait_msec(APIGetterImpl::DEF_WAIT_MSEC);
milliseconds APIGetterImpl::last_get_msec(util::get_msec_since_epoch<conn::clock_ty>());

mutex APIGetterImpl::get_mtx;


APIGetterImpl::APIGetterImpl(Credentials& creds, api_on_error_cb_ty on_error_callback)
    :       
        _on_error_callback(on_error_callback),
        _credentials(creds),
        _connection()
    {}

void
APIGetterImpl::set_url(string url)
{ _connection.SET_url(url); }

string
APIGetterImpl::get()
{
    if( is_closed() )
        throw APIException("connection is closed");

    assert( !_connection.is_closed() );
    return APIGetterImpl::throttled_get(*this);
}

void
APIGetterImpl::close()
{ _connection.close(); }

bool
APIGetterImpl::is_closed() const
{ return !_connection; }

string
APIGetterImpl::throttled_get(APIGetterImpl& getter)
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
    auto remaining = throttled_wait_remaining();
    if( remaining.count() > 0 ){
        /*
         * wait_msec and last_get_msec provide a global throttling mechanism
         * for ALL get requests to avoid excessive calls to TDMA servers
         */
        assert( remaining <= wait_msec );
        this_thread::sleep_for( remaining );
    }

    string s;
    conn::clock_ty::time_point tp;
    tie(s, tp) = api_execute( getter._connection, getter._credentials,
                              getter._on_error_callback );

    last_get_msec = duration_cast<milliseconds>(tp.time_since_epoch());
    return s;
}

std::chrono::milliseconds
APIGetterImpl::throttled_wait_remaining()
{
    auto elapsed = util::get_msec_since_epoch<conn::clock_ty>() - last_get_msec;
    assert( elapsed.count() >= 0 );
    return wait_msec - elapsed;
}

std::chrono::milliseconds
APIGetterImpl::wait_remaining()
{
    static const std::chrono::milliseconds ms0(0);
    return std::max(throttled_wait_remaining(), ms0);
}

void
APIGetterImpl::set_wait_msec(milliseconds msec)
{
    lock_guard<mutex> _(get_mtx);
    wait_msec = msec;
}

milliseconds
APIGetterImpl::get_wait_msec()
{ return wait_msec; }


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

using namespace tdma;

int
APIGetter_Get_ABI( Getter_C *pgetter,
                     char **buf,
                     size_t *n,
                     int allow_exceptions )
{
    return GetterImplAccessor<char**>::template
        get<APIGetterImpl>(
            pgetter, &APIGetterImpl::get, buf, n, allow_exceptions
        );
}

int
APIGetter_Close_ABI(Getter_C *pgetter, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj){
        reinterpret_cast<APIGetterImpl*>(obj)->close();
    };

    return CallImplFromABI(allow_exceptions, meth, pgetter->obj);
}

int
APIGetter_IsClosed_ABI(Getter_C *pgetter, int*b, int allow_exceptions)
{
    int err = proxy_is_callable<APIGetterImpl>(pgetter, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(b, "b", allow_exceptions);

    static auto meth = +[](void* obj){
        return reinterpret_cast<APIGetterImpl*>(obj)->is_closed();
    };

    tie(*b, err) = CallImplFromABI(allow_exceptions, meth, pgetter->obj);
    return err ? err : 0;
}

int
APIGetter_SetWaitMSec_ABI(unsigned long long msec, int allow_exceptions)
{
    return CallImplFromABI( allow_exceptions, APIGetterImpl::set_wait_msec,
                            milliseconds(msec) );
}

int
APIGetter_GetWaitMSec_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    milliseconds ms;
    int err;
    tie(ms, err) = CallImplFromABI( allow_exceptions,
                                          APIGetterImpl::get_wait_msec );
    if(err)
        return err;

    *msec = static_cast<unsigned long long>(ms.count());
    return 0;
}

int
APIGetter_GetDefWaitMSec_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    *msec = static_cast<unsigned long long>(
        APIGetterImpl::DEF_WAIT_MSEC.count()
        );
    return 0;
}

int
APIGetter_WaitRemaining_ABI(unsigned long long *msec, int allow_exceptions)
{
    CHECK_PTR(msec, "msec", allow_exceptions);

    *msec = static_cast<unsigned long long>(
        APIGetterImpl::wait_remaining().count()
        );
    return 0;
}

