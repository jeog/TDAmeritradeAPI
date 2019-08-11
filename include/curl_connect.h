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
#include <mutex>
#include <tuple>
#include <memory>

#include "curl/curl.h"

namespace conn{

const long HTTP_RESPONSE_OK = 200;
const long HTTP_RESPONSE_CREATED = 201;

typedef std::chrono::steady_clock clock_ty;
static_assert( static_cast<double>(clock_ty::period::num)
               / clock_ty::period::den <= .001, "invalid tick size of clock" );

class CurlConnection {
    friend std::ostream&
    operator<<(std::ostream& out, const CurlConnection& session);       

protected:
    class CurlConnectionImpl_;
    CurlConnectionImpl_ *_pimpl;

public:
    static const std::map<CURLoption, std::string> option_strings;

    CurlConnection();
    CurlConnection(std::string url);
    CurlConnection( CurlConnection&& connection );
    CurlConnection&
    operator=( CurlConnection&& connection );

    CurlConnection( const CurlConnection& ) = delete;

    CurlConnection&
    operator=( const CurlConnection& ) = delete;

    virtual
    ~CurlConnection();

    bool
    operator==( const CurlConnection& connection );

    bool
    operator!=( const CurlConnection& connection );

    // use this to get any options that don't have their own 'GET' method
    const std::map<CURLoption, std::string>&
    get_option_strings() const;

    template<typename T>
    void
    set_option(CURLoption option, T param);

    // <status code, body, header(optional), time>
    std::tuple<long, std::string, std::string, clock_ty::time_point>
    execute(bool return_header_data);

    void
    close();

    bool
    is_closed() const;

    operator bool() const;

    void
    SET_url(std::string url);

    std::string
    GET_url() const;

    void
    SET_ssl_verify(bool on);

    void
    SET_ssl_verify_using_ca_bundle(std::string path);

    void
    SET_ssl_verify_using_ca_certs(std::string dir);

    void
    SET_encoding(std::string enc);

    void
    SET_keepalive(bool on);

    void
    ADD_headers(const std::vector<std::pair<std::string,std::string>>& headers);

    std::vector<std::pair<std::string,std::string>>
    GET_headers() const;

    void
    RESET_headers();

    bool
    has_headers();

    void
    RESET_options();

    void
    SET_fields(const std::vector<std::pair<std::string, std::string>>& fields);

    void
    SET_fields(const std::string& fields);
};

// defaults to 'gzip' content encoding
class HTTPConnection : public CurlConnection {
public:
    static const std::string DEFAULT_ENCODING;
    enum class HttpMethod { http_get, http_post, http_delete, http_put };
                            //, HEAD, CONNECT, OPTIONS, TRACE, PATCH };
private:
    HttpMethod method;
    void init(HttpMethod meth);

public:
    HTTPConnection( const std::string& url, HttpMethod method );
    HTTPConnection( HttpMethod method );

    void /* looks for 'http:://' or 'https://' */
    SET_url(std::string url);

    HttpMethod
    getMethod() const
    { return method; }
};


// defaults to KeepAlive flag set
class HTTPGetConnection : public HTTPConnection{
public:
    static const HttpMethod http_method = HttpMethod::http_get;
    HTTPGetConnection()
        : HTTPConnection( http_method) {}
    HTTPGetConnection(const std::string& url)
        : HTTPConnection(url, http_method) {}
};


class HTTPPostConnection : public HTTPConnection {
public:
    static const HttpMethod http_method = HttpMethod::http_post;
    HTTPPostConnection()
        : HTTPConnection( http_method ) { }
    HTTPPostConnection(const std::string& url)
        : HTTPConnection(url, http_method) { }
};


class HTTPDeleteConnection : public HTTPConnection {
public:
    static const HttpMethod http_method = HttpMethod::http_delete;
    HTTPDeleteConnection()
        : HTTPConnection(http_method) { }
    HTTPDeleteConnection(const std::string& url)
        : HTTPConnection(url, http_method) { }
};

class HTTPPutConnection : public HTTPConnection {
public:
    static const HttpMethod http_method = HttpMethod::http_put;
    HTTPPutConnection()
        : HTTPConnection(http_method) { }
    HTTPPutConnection(const std::string& url)
        : HTTPConnection(url, http_method) { }
};


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
    CurlConnectionError(CURLcode code, const std::string& msg);
};


void
set_certificate_bundle_path( const std::string& path );

std::string
get_certificate_bundle_path();

} /* conn */

#endif // CONNECT_H
