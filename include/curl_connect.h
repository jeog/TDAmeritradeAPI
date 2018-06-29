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
#include <memory>

#include "curl/curl.h"

namespace conn{

const long HTTP_RESPONSE_OK = 200;

typedef std::chrono::steady_clock clock_ty;
static_assert( static_cast<double>(clock_ty::period::num)
               / clock_ty::period::den <= .001, "invalid tick size of clock" );


class CurlConnectionImpl_
#ifndef THIS_EXPORTS_INTERFACE
; /* forward declare for the exported CurlConnection interface */
#else
{ /* for the CurlConnection impl */

    static struct Init{
        Init() { curl_global_init(CURL_GLOBAL_ALL); }
        /*
         * IMPORTANT
         *
         * need to be sure we don't cleanup libcurl before accessing streamer
         * or it creates issues when uWebSockets tries to create an SSL context;
         * EVP_get_digestbyname("ssl2-md5") returns null which seg faults in
         * SSL_CTX_ctrl in versions < 1.1.0
         */
        ~Init() { curl_global_cleanup(); }
    }_init;

    friend std::ostream&
    operator<<(std::ostream& out, CurlConnectionImpl_& session);

    struct curl_slist *_header;
    CURL *_handle;
    std::map<CURLoption, std::string> _options;

    /* to string overloads for our different stored option values */
    template<typename T, typename Dummy=void>
    struct to;

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

public:
    CurlConnectionImpl_();

    CurlConnectionImpl_(std::string url);

    CurlConnectionImpl_( CurlConnectionImpl_&& );

    CurlConnectionImpl_&
    operator=( CurlConnectionImpl_&& );

    // NO COPY

    // NO ASSIGN

    virtual
    ~CurlConnectionImpl_();

    bool
    operator==( const CurlConnectionImpl_& connection );

    bool
    operator!=( const CurlConnectionImpl_& connection );

    const std::map<CURLoption, std::string>&
    get_option_strings() const;

    template<typename T>
    void
    set_option(CURLoption option, T param);

    // <status code, data, time>
    std::tuple<long, std::string, clock_ty::time_point>
    execute();

    void
    close();

    bool
    is_closed() const;

    operator bool() const;

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
    has_headers();

    void
    SET_fields(const std::vector<std::pair<std::string,std::string>>& fields);

    void
    RESET_options();
};


template<typename T, typename Dummy>
struct CurlConnectionImpl_::to{
    static std::string str(T t)
    { return std::to_string(t); }
};

template<typename Dummy>
struct CurlConnectionImpl_::to<const char*, Dummy>{
    static std::string str(const char* s)
    { return std::string(s); }
};

template<typename T, typename Dummy>
struct CurlConnectionImpl_::to<T*, Dummy>{
    static std::string str(T* p)
    { return std::to_string(reinterpret_cast<unsigned long long>(p)); }
};
#endif /* THIS_EXPORTS_INTERFACE */


class CurlConnection {
    friend std::ostream&
    operator<<(std::ostream& out, CurlConnection& session);

protected:
    CurlConnectionImpl_ *_pimpl;

public:
    static const std::map<CURLoption, std::string> option_strings;

    CurlConnection();

    CurlConnection(std::string url);

    // NO COPY

    // NO ASSIGN

    CurlConnection( CurlConnection&& connection );

    CurlConnection&
    operator=( CurlConnection&& connection );

    virtual
    ~CurlConnection();

    bool
    operator==( const CurlConnection& connection );

    bool
    operator!=( const CurlConnection& connection );

    const std::map<CURLoption, std::string>&
    get_option_strings() const;

    template<typename T>
    void
    set_option(CURLoption option, T param);

    // <status code, data, time>
    std::tuple<long, std::string, clock_ty::time_point>
    execute();

    void
    close();

    bool
    is_closed() const;

    operator bool() const;

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
    has_headers();

    void
    RESET_options();
};


class HTTPSConnection // TODO check valid HTTPS
        : public CurlConnection{
public:
    static const std::string DEFAULT_ENCODING;
    HTTPSConnection();
    HTTPSConnection(std::string url);
};


class HTTPSGetConnection
        : public HTTPSConnection{
    void _set();
public:
    HTTPSGetConnection();
    HTTPSGetConnection(std::string url);
};


class HTTPSPostConnection
        : public HTTPSConnection{
    void _set();
public:
    HTTPSPostConnection();
    HTTPSPostConnection(std::string url);

    void
    SET_fields(const std::vector<std::pair<std::string,std::string>>& fields);
};


#ifdef THIS_EXPORTS_INTERFACE

class CurlException
        : public std::exception{
    std::string _what;
public:
    CurlException(std::string what);
    const char*
    what() const noexcept;
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
CurlConnectionImpl_::set_option(CURLoption option, T param)
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

#endif /* THIS_EXPORTS_INTERFACE */

} /* conn */

#endif // CONNECT_H
