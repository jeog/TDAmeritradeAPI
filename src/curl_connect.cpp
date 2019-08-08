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
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "../include/curl_connect.h"
//#include "../include/util.h"

using std::string;
using std::map;
using std::vector;
using std::pair;
using std::tuple;
using std::ostream;

namespace conn{

/*
 * If empty certificate_bundle_path(default) curl uses the default store.
 * If that works w/ openssl great, otherwise we'll fail w/ CURLE_SSL_CACERT
 */
std::string certificate_bundle_path;

class CurlConnection::CurlConnectionImpl_ { 
    static struct Init {
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

    struct curl_slist *_header;
    CURL *_handle;
    map<CURLoption, string> _options;
 
    /* to string overloads for our different stored option values */
    template<typename T, typename Dummy = void>
    struct to;

    struct WriteCallback {
        std::stringbuf _buf;

        static size_t
        write( char* input, size_t sz, size_t n, void* output )
        {
            std::stringbuf& buf = ((WriteCallback*)output)->_buf;
            std::streamsize ssz = buf.sputn(input, sz*n);
            //assert(ssz >= 0);
            return ssz;
        }

        string
        str()
        { return _buf.str(); }

        void
        clear()
        { _buf.str(""); }
    };

public:
    CurlConnectionImpl_(string url)
        : CurlConnectionImpl_()
        { SET_url(url); }
    
    CurlConnectionImpl_()
        :
            _header(nullptr),
            _handle(curl_easy_init())
        { set_option(CURLOPT_NOSIGNAL, 1L); }
    
    ~CurlConnectionImpl_()
        { close(); }
    
    // NO COPY

    // NO ASSIGN

    CurlConnectionImpl_(CurlConnectionImpl_&& connection)
        :
            _header(connection._header),
            _handle(connection._handle),
            _options(move(connection._options))
        {
            connection._header = nullptr;
            connection._handle = nullptr;
        }
    
    CurlConnectionImpl_&
    operator=(CurlConnectionImpl_&& connection)
    {
        if (*this != connection) {
            if (_header)
                curl_slist_free_all(_header);

            if (_handle)
                curl_easy_cleanup(_handle);

            _header = connection._header;
            _handle = connection._handle;
            _options = move(connection._options);
            connection._header = nullptr;
            connection._handle = nullptr;
        }
        return *this;
    }

    bool
    operator!=(const CurlConnectionImpl_& connection)
    {
        return (_handle != connection._handle ||
                _header != connection._header ||
                _options != connection._options);
    }

    bool
    operator==(const CurlConnectionImpl_& connection)
    { return !operator!=(connection); }

    const map<CURLoption, string>&
    get_option_strings() const
    { return _options; }

    template<typename T>
    void
    set_option(CURLoption option, T param)
    {
        static_assert(!std::is_same<T, string>::value,
            "CurlConnection::set_option doesn't accept string");
        if (!_handle) {
            throw CurlException("connection/handle has been closed");
        }
        if (curl_easy_setopt(_handle, option, param) != CURLE_OK) {
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
    
    // <status code, data, header, time>
    tuple<long, string, string, clock_ty::time_point>
    execute( bool return_header_data )
    {
        if (!_handle)
            throw CurlException("connection/handle has been closed");

        WriteCallback cb_data, cb_header;
        set_option(CURLOPT_WRITEFUNCTION, &WriteCallback::write);
        set_option(CURLOPT_WRITEDATA, &cb_data);

        if( return_header_data ){
            set_option(CURLOPT_HEADERFUNCTION, &WriteCallback::write);
            set_option(CURLOPT_HEADERDATA, &cb_header);
        }

        CURLcode ccode = curl_easy_perform(_handle);
        auto tp = clock_ty::now();
        if (ccode != CURLE_OK)
            throw CurlConnectionError(ccode);

        string res = cb_data.str();
        cb_data.clear();
        long c;
        curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &c);

        string head;
        if( return_header_data ){
            head = cb_header.str();
            cb_data.clear();
        }

        return make_tuple(c, res, head, tp);
    }

    void
    close()
    {
        if (_header) {
            curl_slist_free_all(_header);
            _header = nullptr;
        }
        if (_handle) {
            curl_easy_cleanup(_handle);
            _handle = nullptr;
        }
        _options.clear();
    }

    bool
    is_closed() const
    { return _handle == nullptr; }
    
    operator bool() const
    { return !is_closed(); }
    
    void
    SET_url(string url)
    { set_option(CURLOPT_URL, url.c_str()); }

    void
    SET_ssl_verify()
    {
        set_option(CURLOPT_SSL_VERIFYPEER, 1L);
        set_option(CURLOPT_SSL_VERIFYHOST, 2L);
    }

    void
    SET_ssl_verify_using_ca_bundle(string path)
    {
        SET_ssl_verify();
        set_option(CURLOPT_CAINFO, path.c_str());
    }

    void
    SET_ssl_verify_using_ca_certs(string dir)
    {
        SET_ssl_verify();
        set_option(CURLOPT_CAPATH, dir.c_str());
    }

    void
    SET_encoding(string enc)
    { set_option(CURLOPT_ACCEPT_ENCODING, enc.c_str()); }
    
    void
    SET_keepalive()
    { set_option(CURLOPT_TCP_KEEPALIVE, 1L); }

    void
    ADD_headers(const vector<pair<string, string>>& headers)
    {
        if (!_handle)
            throw CurlException("connection/handle has been closed");

        if (headers.empty())
            return;

        for (auto& h : headers) {
            string s = h.first + ": " + h.second;
            _header = curl_slist_append(_header, s.c_str());
            if (!_header) {
                throw CurlOptionException("curl_slist_append failed trying to "
                    "add header", CURLOPT_HTTPHEADER, s);
            }
        }

        return set_option(CURLOPT_HTTPHEADER, _header);
    }
    
    vector<pair<string,string>>
    GET_headers() const
    {
        struct curl_slist * tmp = _header;

        vector<pair<string,string>> headers;
        while( tmp ){
             string h(tmp->data);
             size_t pos = h.find(": ");
             if( pos == string::npos )
                 throw CurlException("malformed header");
             headers.emplace_back( string(h.begin(), h.begin() + pos),
                                   string(h.begin() + pos + 2, h.end()) );
             tmp = tmp->next;
        }
        return headers;
    }

    void
    RESET_headers()
    {
        curl_slist_free_all(_header);
        _header = nullptr;
        _options.erase(CURLOPT_HTTPHEADER);
    }
    
    bool
    has_headers()
    {
        return _header != nullptr;
    }


    void
    SET_fields(const vector<pair<string, string>>& fields)
    {
        if (is_closed())
            throw CurlException("connection/handle has been closed");

        /* CURLOPT_POST FIELDS DOES NOT COPY STRING */
        if (!fields.empty()) {
            std::stringstream ss;
            for (auto& f : fields) {
                ss << f.first << "=" << f.second << "&";
            }
            string s(ss.str());
            if (!s.empty()) {
                //assert(s.back() == '&');
                s.erase(s.end() - 1, s.end());
            }
            set_option(CURLOPT_COPYPOSTFIELDS, s.c_str());
        }
    }

    void
    SET_fields(const string& fields)
    {
        if (is_closed())
            throw CurlException("connection/handle has been closed");

        /* CURLOPT_POST FIELDS DOES NOT COPY STRING */
        if (!fields.empty()) {
            set_option(CURLOPT_COPYPOSTFIELDS, fields.c_str());
        }
    }
    
    void
    RESET_options()
    {
        RESET_headers();
        if (_handle) {
            curl_easy_reset(_handle);
        }
        _options.clear();
    }

    ostream&
    to_out(ostream& out);
};

CurlConnection::CurlConnectionImpl_::Init CurlConnection::CurlConnectionImpl_::_init;

template<typename T, typename Dummy>
struct CurlConnection::CurlConnectionImpl_::to {
    static string str(T t)
    { return std::to_string(t); }
};

template<typename Dummy>
struct CurlConnection::CurlConnectionImpl_::to<const char*, Dummy> {
    static string str(const char* s)
    { return string(s); }
};

template<typename T, typename Dummy>
struct CurlConnection::CurlConnectionImpl_::to<T*, Dummy> {
    static string str(T* p)
    { return std::to_string(reinterpret_cast<unsigned long long>(p)); }
};


CurlConnection::CurlConnection()
    : _pimpl( new CurlConnectionImpl_() )
    {}

CurlConnection::CurlConnection(string url)
    : _pimpl( new CurlConnectionImpl_(url) )
    {}


CurlConnection::CurlConnection( CurlConnection&& connection )
    : _pimpl( connection._pimpl )
    { connection._pimpl = nullptr; }

CurlConnection&
CurlConnection::operator=( CurlConnection&& connection )
{
    if(*this != connection){
        _pimpl = connection._pimpl;
        connection._pimpl = nullptr;
    }
    return *this;
}

CurlConnection::~CurlConnection()
{ if(_pimpl) delete _pimpl; }

bool
CurlConnection::operator==( const CurlConnection& connection )
{
    if( _pimpl != connection._pimpl ) // different objs
        return false;

    if( !_pimpl ) // both null
        return true;

    return _pimpl->operator==(*connection._pimpl); // logically ==
}

bool
CurlConnection::operator!=( const CurlConnection& connection )
{ return !operator==(connection); }

const map<CURLoption, string>&
CurlConnection::get_option_strings() const
{ return _pimpl->get_option_strings(); }

template<typename T>
void
CurlConnection::set_option(CURLoption option, T param)
{ _pimpl->set_option(option, param); }

// <status code, data, time>
tuple<long, string, string, clock_ty::time_point>
CurlConnection::execute( bool return_header_data )
{ return _pimpl->execute(return_header_data); }

void
CurlConnection::close()
{ _pimpl->close(); }

bool
CurlConnection::is_closed() const
{ return _pimpl->is_closed(); }


CurlConnection::operator bool() const
{ return _pimpl->operator bool(); }

void
CurlConnection::SET_url(string url)
{ _pimpl->SET_url(url); }

void
CurlConnection::CurlConnection::SET_ssl_verify()
{ _pimpl->SET_ssl_verify(); }

void
CurlConnection::SET_ssl_verify_using_ca_bundle(string path)
{ _pimpl->SET_ssl_verify_using_ca_bundle(path); }

void
CurlConnection::SET_ssl_verify_using_ca_certs(string dir)
{ _pimpl->SET_ssl_verify_using_ca_certs(dir); }

void
CurlConnection::SET_encoding(string enc)
{ _pimpl->SET_encoding(enc); }

void
CurlConnection::SET_keepalive()
{ _pimpl->SET_keepalive(); }

void
CurlConnection::ADD_headers(const vector<pair<string,string>>& headers)
{ _pimpl->ADD_headers(headers); }

vector<pair<string,string>>
CurlConnection::GET_headers() const
{ return _pimpl->GET_headers(); }

void
CurlConnection::RESET_headers()
{ _pimpl->RESET_headers(); }

bool
CurlConnection::has_headers()
{ return _pimpl->has_headers(); }

void
CurlConnection::RESET_options()
{ _pimpl->RESET_options(); }

const string HTTPSConnection::DEFAULT_ENCODING("gzip");

HTTPSConnection::HTTPSConnection()
    : CurlConnection()
    { _set(); }

HTTPSConnection::HTTPSConnection(string url)
    : CurlConnection(url)
    { _set(); }

void
HTTPSConnection::_set()
{
    if( certificate_bundle_path.empty() ) {
/*
        cerr << "no 'certificate_bundle_path' w/ non-native SSL support - "
                "CurlConnectionError(CURLE_SSL_CACERT) likely..." << endl;
*/
        SET_ssl_verify();
    } else 
         SET_ssl_verify_using_ca_bundle(certificate_bundle_path);    
}


HTTPSGetConnection::HTTPSGetConnection()
    : HTTPSConnection()
    { _set(); }


HTTPSGetConnection::HTTPSGetConnection(string url)
    : HTTPSConnection(url)
    { _set(); }

void
HTTPSGetConnection::_set()
{
    set_option(CURLOPT_HTTPGET, 1L);
    SET_encoding(DEFAULT_ENCODING);
    SET_keepalive();
}


HTTPSPostConnection::HTTPSPostConnection()
    : HTTPSConnection()
    { _set(); }

HTTPSPostConnection::HTTPSPostConnection(string url)
    : HTTPSConnection(url)
    { _set(); }

void
HTTPSPostConnection::_set()
{
    set_option(CURLOPT_POST, 1L);
    SET_encoding(DEFAULT_ENCODING);
    SET_keepalive();
}

void
HTTPSPostConnection::SET_fields(const vector<pair<string,string>>& fields)
{ _pimpl->SET_fields(fields); }

void
HTTPSPostConnection::SET_fields(const string& fields)
{ _pimpl->SET_fields(fields); }


HTTPSDeleteConnection::HTTPSDeleteConnection()
    : HTTPSConnection()
    { _set(); }


HTTPSDeleteConnection::HTTPSDeleteConnection(string url)
    : HTTPSConnection(url)
    { _set(); }

void
HTTPSDeleteConnection::_set()
{
    set_option(CURLOPT_CUSTOMREQUEST, "DELETE");
    SET_encoding(DEFAULT_ENCODING);
}


CurlException::CurlException(string what)
    : _what(what)
    {}

const char*
CurlException::what() const noexcept
{ return _what.c_str(); }


CurlOptionException::CurlOptionException(CURLoption opt, string val)
    :
        CurlException( "error setting easy curl option(" + std::to_string(opt)
                       + ") with value(" + val + ")"),
        option(opt),
        value(val)
    {}


CurlOptionException::CurlOptionException(string what, CURLoption opt, string val)
    :
        CurlException(what),
        option(opt),
        value(val)
    {}


CurlConnectionError::CurlConnectionError(CURLcode code)
    :
        CurlException("curl connection error"),
        code(code)
    {}


vector<pair<string, string>>
fields_str_to_map(const string& fstr)
{
    static const string C{'&'};

    vector<pair<string, string>> res;
    auto b = fstr.cbegin();
    do{
        auto e = find_first_of(b, fstr.cend(), C.cbegin(), C.cend());
        string s(b,e);
        if( !s.empty() ){
            auto s_b = s.cbegin();
            auto s_e = s.cend();
            auto sep = find(s_b, s_e,'=');
            if( sep != s_e ){
                string k(s_b,sep);
                string v(sep+1, s_e);
                res.emplace_back(k,v);
            }
        }
        if( e == fstr.cend() )
            break; // avoid b going past end
        b = e + 1;
    }while(true);

    return res;
}


vector<pair<string, string>>
header_list_to_map(struct curl_slist *hlist)
{
    vector<pair<string, string>> res;
    while( hlist ){
        string s(hlist->data);
        auto i = find(s.cbegin(), s.cend(),':');
        string k(s.cbegin(),i);
        string v(i+1, s.cend());
        res.emplace_back(k, v);
        hlist = hlist->next;
    }
    return res;
}


ostream&
operator<<(ostream& out, const CurlConnection& session)
{ return session._pimpl->to_out(out); }

ostream&
CurlConnection::CurlConnectionImpl_::to_out(ostream& out)
{
    using std::endl;

    for (auto& opt : get_option_strings()) {

        auto oiter = option_strings.find(opt.first);
        if (oiter == option_strings.end()) {
            out << "\tUNKNOWN" << endl;
            continue;
        }

        switch (opt.first) {
        case CURLOPT_COPYPOSTFIELDS:
            out << "\t" << oiter->second << ":" << endl;
            for (auto p : fields_str_to_map(opt.second)) {
                out << "\t\t" << p.first << "\t" << p.second << endl;
            }
            continue;
        case CURLOPT_HTTPHEADER:
            out << "\t" << oiter->second << ":" << endl;
            for (auto p : header_list_to_map(_header)) {
                out << "\t\t" << p.first << "\t" << p.second << endl;
            }
            continue;
        case CURLOPT_WRITEDATA:
        case CURLOPT_WRITEFUNCTION:
            out << "\t" << oiter->second << "\t" << std::hex
                << stoull(opt.second) << std::dec << endl;
            continue;
        default:
            out << "\t" << oiter->second << "\t" << opt.second << endl;
        }
    }

    return out;
}

const map<CURLoption, string> CurlConnection::option_strings = {
    { CURLOPT_SSL_VERIFYPEER, "CURLOPT_SSL_VERIFYPEER"},
    { CURLOPT_SSL_VERIFYHOST, "CURLOPT_SSL_VERIFYHOST"},
    { CURLOPT_CAINFO, "CURLOPT_CAINFO"},
    { CURLOPT_CAPATH, "CURLOPT_CAPATH"},
    { CURLOPT_URL, "CURLOPT_URL"},
    { CURLOPT_ACCEPT_ENCODING, "CURLOPT_ACCEPT_ENCODING"},
    { CURLOPT_TCP_KEEPALIVE, "CURLOPT_TCP_KEEPALIVE"},
    { CURLOPT_HTTPGET, "CURLOPT_HTTPGET"},
    { CURLOPT_POST, "CURLOPT_POST"},
    { CURLOPT_COPYPOSTFIELDS, "CURLOPT_COPYPOSTFIELDS"},
    { CURLOPT_WRITEFUNCTION, "CURLOPT_WRITEFUNCTION"},
    { CURLOPT_WRITEDATA, "CURLOPT_WRITEDATA"},
    { CURLOPT_HTTPHEADER, "CURLOPT_HTTPHEADER"},
    { CURLOPT_NOSIGNAL, "CURLOPT_NOSIGNAL"}
};


void
set_certificate_bundle_path( const std::string& path )
{ certificate_bundle_path = path; }

std::string
get_certificate_bundle_path()
{ return certificate_bundle_path; }

} /* conn */
