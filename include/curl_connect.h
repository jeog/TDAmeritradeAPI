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

#ifndef CONNECT_H
#define CONNECT_H

#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <vector>
#include <assert.h>
#include <mutex>
#include <tuple>

#include "curl/curl.h"

namespace conn{

const long HTTP_RESPONSE_OK = 200;

typedef std::chrono::steady_clock clock_ty;
static_assert( static_cast<double>(clock_ty::period::num)
               / clock_ty::period::den <= .001, "invalid tick size of clock" );

class CurlConnection {
    friend std::ostream&
    operator<<(std::ostream& out, CurlConnection& session);

    struct curl_slist *_header;
    CURL *_handle;
    std::map<CURLoption, std::string> _options;

    /* to string overloads for our different stored option values */
    template<typename T, typename Dummy=void>
    struct to;

    static struct Init{
        Init()
        { curl_global_init(CURL_GLOBAL_ALL); }
        /*
         * IMPORTANT
         *
         * need to be sure we don't cleanup libcurl before accessing streamer
         * or it creates issues when uWebSockets tries to create an SSL context;
         * EVP_get_digestbyname("ssl2-md5") returns null which seg faults in
         * SSL_CTX_ctrl in versions < 1.1.0
         */
        ~Init()
        { curl_global_cleanup(); }
    }_init;

public:
    static const std::map<CURLoption, std::string> option_strings;

    struct WriteCallback{
        std::stringbuf _buf;

        static size_t
        write(char* input, size_t sz, size_t n, void* output);

        std::string
        str()
        { return _buf.str(); }

        void
        clear()
        { _buf.str(""); }
    };

    CurlConnection();
    CurlConnection(std::string url);

    virtual
    ~CurlConnection();

    const std::map<CURLoption, std::string>&
    get_option_strings() const
    { return _options; }

    template<typename T>
    void
    set_option(CURLoption option, T param);

    // <status code, data, time>
    std::tuple<long, std::string, clock_ty::time_point>
    execute();

    void
    close();

    bool
    is_closed() const
    { return _handle == nullptr; }

    operator
    bool() const
    { return !is_closed(); }

    void
    SET_url(std::string url);

    void
    SET_ssl_verify();

    void
    SET_ssl_verify_using_ca_bundle(std::string path);

    void
    SET_ssl_verify_using_ca_certs(std::string dir);

    void
    SET_encoding(std::string enc);

    void
    SET_keepalive();

    void
    ADD_headers(const std::vector<std::pair<std::string,std::string>>& headers);

    void
    RESET_headers();

    bool
    has_headers()
    { return _header != nullptr; }

    void
    RESET_options();
};


// TODO check valid HTTPS
class HTTPSConnection
        : public CurlConnection{
public:
    static const std::string DEFAULT_ENCODING;
    HTTPSConnection();
    HTTPSConnection(std::string url);
};


class HTTPSGetConnection
        : public HTTPSConnection{
public:
    HTTPSGetConnection();
    HTTPSGetConnection(std::string url);
};


class HTTPSPostConnection
        : public HTTPSConnection{
public:
    HTTPSPostConnection();
    HTTPSPostConnection(std::string url);

    void
    SET_fields(const std::vector<std::pair<std::string,std::string>>& fields);
};


class CurlException
        : public std::exception{
    std::string _what;
public:
    CurlException(std::string what)
        : _what(what)
    {}
    const char*
    what() const noexcept
    { return _what.c_str(); }
};


class CurlOptionException
        : public CurlException {
public:
    const CURLoption option;
    const std::string value;
    CurlOptionException(CURLoption opt, std::string val);
    CurlOptionException(std::string what, CURLoption opt, std::string val);
};


class CurlConnectionError
        : public CurlException {
public:
    const CURLcode code;
    CurlConnectionError(CURLcode code);
};


template<typename T>
void
CurlConnection::set_option(CURLoption option, T param)
{
    static_assert( !std::is_same<T,std::string>::value,
                   "CurlConnection::set_option doesn't accept string" );
    if( !_handle ){
        throw CurlException("connection/handle has been closed");
    }
    if( curl_easy_setopt(_handle, option, param) != CURLE_OK ){
        throw CurlOptionException(option, to<T>::str(param));
    }
    /*
     * NOTE we don't do anything special with function pointers
     *      (e.g CURLOPT_WRITEFUNCTION) or list poiners
     *      (e.g CURLOPT_HTTPHEADER) except store the address.
     *      See 'to' struct specializations.
     */
    _options[option] = to<T>::str(param);
}

template<typename T, typename Dummy>
struct CurlConnection::to{
    static std::string str(T t)
    { return std::to_string(t); }
};

template<typename Dummy>
struct CurlConnection::to<const char*, Dummy>{
    static std::string str(const char* s)
    { return std::string(s); }
};

template<typename T, typename Dummy>
struct CurlConnection::to<T*, Dummy>{
    static std::string str(T* p)
    { return std::to_string(reinterpret_cast<unsigned long long>(p)); }
};


std::ostream&
operator<<(std::ostream& out, CurlConnection& connection);


} /* conn */

#endif // CONNECT_H
