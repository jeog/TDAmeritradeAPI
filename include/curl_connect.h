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

#include "_common.h"
#include "curl/curl.h"

namespace conn{

const long HTTP_RESPONSE_OK = 200;

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
    void _set();
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

} /* conn */

#endif // CONNECT_H
