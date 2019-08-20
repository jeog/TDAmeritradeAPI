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

    void
    reset_options();

    // <status code, body, header(optional), time>
    std::tuple<long, std::string, std::string, clock_ty::time_point>
    execute(bool return_header_data);

    void
    close();

    bool
    is_closed() const;

    operator bool() const;

    void
    set_url(std::string url);

    std::string
    get_url() const;

    void
    set_ssl_verify(bool on);

    void
    set_ssl_verify_using_ca_bundle(std::string path);

    void
    set_ssl_verify_using_ca_certs(std::string dir);

    void
    set_encoding(std::string enc);

    void
    set_keepalive(bool on);

    void
    add_headers(const std::vector<std::pair<std::string,std::string>>& headers);

    std::vector<std::pair<std::string,std::string>>
    get_headers() const;

    void
    reset_headers();

    bool
    has_headers();

    void
    set_fields(const std::vector<std::pair<std::string, std::string>>& fields);

    void
    set_fields(const std::string& fields);
};


enum class HttpMethod { http_get, http_post, http_delete, http_put };
                        //, HEAD, CONNECT, OPTIONS, TRACE, PATCH };

class HTTPConnectionInterface {
public:
    virtual ~HTTPConnectionInterface(){}

    virtual void close() = 0;

    virtual bool is_closed() const = 0;

    virtual operator bool() const = 0;

    virtual void set_url(const std::string& url) = 0;

    virtual std::string get_url() const = 0;

    virtual HttpMethod get_method() const = 0;

    virtual void set_method(HttpMethod meth) = 0;

    virtual void
    add_headers(const std::vector<std::pair<std::string,std::string>>& headers) = 0;

    virtual std::vector<std::pair<std::string,std::string>>
    get_headers() const = 0;

    virtual void reset_headers() = 0;

    virtual bool has_headers() = 0;

    virtual std::tuple<long, std::string, std::string, clock_ty::time_point>
    execute(bool return_header_data) = 0;

    virtual void
    set_fields(const std::vector<std::pair<std::string, std::string>>& fields) =0;

    virtual void set_fields(const std::string& fields)= 0;
};


// defaults to 'gzip' content encoding
class HTTPConnection : public CurlConnection, public HTTPConnectionInterface {
    enum class Protocol { http, https, none };
    Protocol _proto;
    HttpMethod _meth;

    HttpMethod
    _set_method(HttpMethod meth);

public:
    static const std::string DEFAULT_ENCODING;

    HTTPConnection( const std::string& url, HttpMethod meth );
    HTTPConnection(HttpMethod meth );

    HTTPConnection( HTTPConnection&& connection ) = default;

    HTTPConnection&
    operator=( HTTPConnection&& connection ) = default;

    HTTPConnection( const HTTPConnection& connection ) = delete;

    HTTPConnection&
    operator=( const HTTPConnection& connection ) = delete;

    void
    close()
    { CurlConnection::close(); }

    bool
    is_closed() const
    { return CurlConnection::is_closed(); }

    operator
    bool() const
    { return CurlConnection::operator bool(); }

    void
    set_url(const std::string& url);

    std::string
    get_url() const
    { return CurlConnection::get_url(); }

    HttpMethod
    get_method() const
    { return _meth; }

    void
    set_method(HttpMethod meth)
    { _meth = _set_method(meth); }

    void
    add_headers(const std::vector<std::pair<std::string,std::string>>& headers)
    { CurlConnection::add_headers(headers); }

    std::vector<std::pair<std::string,std::string>>
    get_headers() const
    { return CurlConnection::get_headers(); }

    void
    reset_headers()
    { CurlConnection::reset_headers(); }

    bool
    has_headers()
    { return CurlConnection::has_headers(); }

    std::tuple<long, std::string, std::string, clock_ty::time_point>
    execute(bool return_header_data)
    { return CurlConnection::execute(return_header_data); }

    void
    set_fields(const std::vector<std::pair<std::string, std::string>>& fields)
    { CurlConnection::set_fields(fields); }

    void
    set_fields(const std::string& fields)
    { CurlConnection::set_fields(fields); }
};


std::string
pairs_to_fields_str(const std::vector<std::pair<std::string, std::string>>& fields);


class SharedHTTPConnection : public HTTPConnectionInterface {

    struct Context{
        size_t nref;
        std::shared_ptr<HTTPConnection> conn;
        std::shared_ptr<std::mutex> mtx;

        Context(HTTPConnection * pconn)
            : nref(0), conn(pconn), mtx(new std::mutex())
            {}

        Context() : Context(nullptr) {}
    };

    static std::unordered_map<int, Context> contexts;
    static std::mutex contexts_mtx;

    static int
    nconnections(int context_id);

    static void
    decr_ref(int context_id);

    static void
    incr_ref(int context_id);

    bool _is_open;
    std::string _url;
    HttpMethod _meth;
    std::vector<std::pair<std::string,std::string>> _headers;
    std::string _fields;
    int _id;

    Context&
    _get_context() const;

public:
    SharedHTTPConnection( const std::string& url,
                          HttpMethod meth,
                          int context_id );

    SharedHTTPConnection( HttpMethod meth, int context_id )
        : SharedHTTPConnection("", meth, context_id)
        {}

    SharedHTTPConnection( const SharedHTTPConnection& connection );

    SharedHTTPConnection&
    operator=( const SharedHTTPConnection& connection );

    bool
    operator==( const SharedHTTPConnection& connection );

    bool
    operator!=( const SharedHTTPConnection& connection )
    { return ! operator==(connection); }

    ~SharedHTTPConnection()
    { close(); }

    void
    close();

    bool
    is_closed() const
    { return !_is_open; }

    operator
    bool() const
    { return !is_closed(); }

    std::string
    get_url() const
    { return _url; }

    // doesn't check for a shared proto/host - just overwrites
    void
    set_url(const std::string& url);

    HttpMethod
    get_method() const
    { return _meth;  }

    void
    set_method(HttpMethod meth)
    { _meth = meth; }

    void
    add_headers(const std::vector<std::pair<std::string,std::string>>& headers)
    { _headers.insert( _headers.end(), headers.begin(), headers.end() ); }

    std::vector<std::pair<std::string,std::string>>
    get_headers() const
    { return _headers; }

    void
    reset_headers()
    { _headers.clear(); }

    bool
    has_headers()
    { return !_headers.empty(); }

    std::tuple<long, std::string, std::string, clock_ty::time_point>
    execute(bool return_header_data);

    void
    set_fields(const std::vector<std::pair<std::string, std::string>>& fields)
    { _fields = pairs_to_fields_str(fields); }

    void set_fields(const std::string& fields)
    { _fields = fields; }
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
