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

#ifndef TDMA_COMMON_H
#define TDMA_COMMON_H

/* HEADER COMMON TO ALL TDMA API CLIENT HEADERS */

#include "_common.h"
#include <string.h>

#ifdef __cplusplus

#include "json.hpp"

using json = nlohmann::json;

/*
 * if C++ DECL_C_CPP_TDMA_ENUM expands to:
 *
 * namespace tdma{
 * enum class type: int {
 *    name,
 *    name,
 *    ...
 *    name
 * };
 * }
 *
 * and defines an inline [TypeName]_is_valid for checks in the ABI
 * and declares a stable-ABI to_string: 'type_to_string_ABI(type t)'
 * and declares/defines a '<<' overload
 * and declares/defines an overloaded 'to_string()'
 *
 * if C it expands to:
 *
 *   enum type {
 *      type_name,
 *      type_name,
 *      ...
 *      type_name
 *   }
 *
 * and declares a stable-ABI to_string: 'type_to_string_ABI(type t)'
 */
#define DECL_C_CPP_TDMA_ENUM(type, l, h, ...) \
EXTERN_C_SPEC_ DLL_SPEC_ int \
type##_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions); \
\
inline bool \
type##_is_valid(int v) \
{ return (v >= l && v <= h); } \
\
namespace tdma{ \
enum class type : int { __VA_ARGS__ }; \
\
inline std::string \
to_string(const type& v) \
{ \
    char* buf; \
    size_t n; \
    call_abi( type##_##to_string_ABI, static_cast<int>(v), &buf, &n ); \
    std::string s(buf); \
    if( buf ) free(buf); \
    return s; \
} \
\
inline std::ostream& \
operator<<(std::ostream& out, const type& v) \
{ out << to_string(v); return out; } \
} /* tdma */

#define BUILD_C_CPP_TDMA_ENUM_NAME(type,name) name

#else

#define DECL_C_CPP_TDMA_ENUM(type, l, h, ...) \
typedef enum { __VA_ARGS__ } type; \
EXTERN_C_SPEC_ DLL_SPEC_ int \
type##_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions); \
\
static inline int \
type##_to_string(int v, char** buf, size_t* n) \
{ return type##_to_string_ABI(v, buf, n, 0); }

#define BUILD_C_CPP_TDMA_ENUM_NAME(type,name) type##_##name

#endif /* __cplusplus */


/* C PROXY BASES */
#define DECL_CPROXY_BASE_STRUCT(name) \
    typedef struct {void *obj; int type_id; } name

DECL_CPROXY_BASE_STRUCT(Getter_C);
DECL_CPROXY_BASE_STRUCT(StreamingSubscription_C);
DECL_CPROXY_BASE_STRUCT(OrderLeg_C);
DECL_CPROXY_BASE_STRUCT(OrderTicket_C);

#undef DECL_CPROXY_BASE_STRUCT

typedef struct{
    void *obj;
    int type_id;
    void *ctx; // reserved
} StreamingSession_C;

typedef struct{
    const char* key;
    const char* val;
}KeyValPair;

/* C ERROR CODES */
#define TDMA_API_ERROR 1
#define TDMA_API_CRED_ERROR 2
#define TDMA_API_VALUE_ERROR 3
#define TDMA_API_TYPE_ERROR 4
#define TDMA_API_MEMORY_ERROR 5

#define TDMA_API_CONNECT_ERROR 101
#define TDMA_API_AUTH_ERROR 102
#define TDMA_API_REQUEST_ERROR 103
#define TDMA_API_SERVER_ERROR 104

#define TDMA_API_STREAM_ERROR 201

#define TDMA_API_EXECUTE_ERROR 301

#define TDMA_API_STD_EXCEPTION 501

#define TDMA_API_UNKNOWN_EXCEPTION 1001

struct Credentials;

/*
 * ABI bridge functions
 *
 * How we provide C and C++ interface w/ a stable ABI
 *
 *        Interface    -->      ABI Bridge    -->    Implementation
 *
 *  C     Function()          Function_ABI()
 *  C++   Function()                                 FunctionImpl()
 *
 *  Errors:
 *
 *  C     Error Code   <--     Error Code     <--     Exception
 *  C++   Exception    <--     Error Code     <--     Exception
 */

EXTERN_C_SPEC_ DLL_SPEC_ int
LoadCredentials_ABI( const char* path,
                     const char* password,
                     struct Credentials* pcreds,
                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
StoreCredentials_ABI( const char* path,
                      const char* password,
                      const struct Credentials* pcreds,
                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
RequestAccessToken_ABI( const char* code,
                        const char* client_id,
                        const char* redirect_uri,
                        struct Credentials* pcreds,
                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
RefreshAccessToken_ABI(struct Credentials* creds, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
SetCertificateBundlePath_ABI(const char* path, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
GetCertificateBundlePath_ABI(char **path, size_t *n, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
GetDefaultCertificateBundlePath_ABI( char **path,
                                     size_t *n,
                                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
CreateCredentials_ABI( const char* access_token,
                       const char* refresh_token,
                       long long epoch_sec_token_expiration,
                       const char* client_id,
                       struct Credentials* pcreds,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
CloseCredentials_ABI(struct Credentials* pcreds, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
CopyCredentials_ABI( const struct Credentials* from,
                     struct Credentials *to,
                     int allow_exceptions );


EXTERN_C_SPEC_ DLL_SPEC_ int
FreeBuffer_ABI( char* buf, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
FreeBuffers_ABI( char** bufs, size_t n, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
FreeFieldsBuffer_ABI( int* fields, int allow_exceptions );

// NOTE - DOES NOT DESTROY THE UNDERLYING OBJECTS
EXTERN_C_SPEC_ DLL_SPEC_ int
FreeOrderLegBuffer_ABI( OrderLeg_C *legs, int allow_exceptions );

// NOTE - DOES NOT DESTROY THE UNDERLYING OBJECTS
EXTERN_C_SPEC_ DLL_SPEC_ int
FreeOrderTicketBuffer_ABI( OrderTicket_C *orders, int allow_exceptions );

// NOTE - FREE's THE BUFFER AND THE CHAR*s inside
EXTERN_C_SPEC_ DLL_SPEC_ int
FreeKeyValBuffer_ABI( KeyValPair *pkeyvals, size_t n, int allow_exceptions );

/*
 * 'LastError' calls only return information for the last exc/error to occur
 *  ON THE 'INSIDE' of the library boundary. (Not from header definitions.)
 */
EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorCode_ABI( int *code, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorMsg_ABI( char** buf, size_t *n, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorLineNumber_ABI( int *lineno, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorFilename_ABI( char** buf, size_t *n, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorState_ABI( int *code,
                    char **buf_msg,
                    size_t *n_msg,
                    int *lineno,
                    char **buf_filename,
                    size_t *n_filename,
                    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOptionSymbol_ABI( const char* underlying,
                       unsigned int month,
                       unsigned int day,
                       unsigned int year,
                       int is_call,
                       double strike,
                       char **buf,
                       size_t *n,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
CheckOptionSymbol_ABI(const char* symbol, int allow_exceptions);

EXTERN_C_SPEC_ DLL_SPEC_ int
LibraryBuildDateTime_ABI(char **buf, size_t *n, int allow_exceptions);


#ifndef __cplusplus
/* C interface */

static inline int
LoadCredentials( const char* path,
                 const char* password,
                 struct Credentials* pcreds )
{ return LoadCredentials_ABI(path, password, pcreds,0); }

static inline int
StoreCredentials( const char* path,
                  const char* password,
                  const struct Credentials* pcreds )
{ return StoreCredentials_ABI(path, password, pcreds, 0); }

static inline int
RequestAccessToken( const char* code,
                    const char* client_id,
                    const char* redirect_uri,
                    struct Credentials* pcreds )
{ return RequestAccessToken_ABI(code, client_id, redirect_uri, pcreds, 0); }

static inline int
RefreshAccessToken(struct Credentials* creds )
{ return RefreshAccessToken_ABI(creds, 0); }

static inline int
SetCertificateBundlePath(const char* path)
{ return SetCertificateBundlePath_ABI(path, 0); }

static inline int
GetCertificateBundlePath(char **path, size_t *n)
{ return GetCertificateBundlePath_ABI(path, n, 0); }

static inline int
GetDefaultCertificateBundlePath(char **path, size_t *n )
{ return GetDefaultCertificateBundlePath_ABI(path, n, 0); }

static inline int
CreateCredentials( const char* access_token
                   const char* refresh_token,
                   long long epoch_sec_token_expiration,
                   const char *client_id,
                   struct Credentials *pcreds)
{ return CreateCredentials_ABI(access_token, refresh_token,
                           epoch_sec_token_expiration, client_id, pcreds, 0); }

static inline int
CloseCredentials(struct Credentials* pcreds )
{ return CloseCredentials_ABI(pcreds, 0); }

static inline int
CopyCredentials(const struct Credentials* from, struct Credentials *to)
{ return CopyCredentials_ABI(from, to, 0); }

static inline int
FreeBuffer( char* buf )
{ return FreeBuffer_ABI(buf, 0); }

static inline int
FreeBuffers( char** bufs, size_t n)
{ return FreeBuffers_ABI(bufs, n, 0); }

static inline int
FreeFieldsBuffer( int* fields )
{ return FreeFieldsBuffer_ABI(fields, 0); }

static inline int
FreeOrderLegBuffer( OrderLeg_C *legs )
{ return FreeOrderLegBuffer_ABI(legs, 0); }

static inline int
FreeOrderTicketBuffer( OrderTicket_C *orders )
{ return FreeOrderTicketBuffer_ABI(orders, 0); }

static inline int
FreeKeyValBuffer( KeyValPair *pkeyvals, size_t n )
{ return FreeKeyValBuffer_ABI(pkeyvals, n, 0); }

/*
 * 'LastError' calls only return information for the last exc/error to occur
 *  ON THE 'INSIDE' of the library boundary. (Not from header definitions.)
 */
static inline int
LastErrorCode( int *code )
{ return LastErrorCode_ABI(code, 0); }

static inline int
LastErrorMsg( char** buf, size_t *n)
{ return LastErrorMsg_ABI(buf, n, 0); }

static inline int
LastErrorLineNumber( int *lineno )
{ return LastErrorLineNumber_ABI(lineno, 0); }

static inline int
LastErrorFilename( char** buf, size_t *n )
{ return LastErrorFilename_ABI(buf, n, 0); }

static inline int
BuildOptionSymbol( const char* underlying,
                   unsigned int month,
                   unsigned int day,
                   unsigned int year,
                   int is_call,
                   double strike,
                   char **buf,
                   size_t *n )
{ return BuildOptionSymbol_ABI(underlying, month, day, year, is_call, strike,
                               buf, n, 0); }

static inline int
CheckOptionSymbol(const char* symbol)
{ return CheckOptionSymbol_ABI(symbol, 0); }

#else

namespace tdma{

static void
error_to_exc(int code);

template<typename... Args1, typename... Args2>
inline void
call_abi( int(*abicall)(Args1...), Args2... args)
{
    error_to_exc( abicall(args..., ALLOW_EXCEPTIONS) );
}

}; /* tdma */

#endif /* __cplusplus */


/*
 * if C, client has to call CloseCredentials and CopyCredentials directly
 * a) when done and b) before passing an active instance to LoadCredentials
 * or RequestAccessToken, or the internal char*s will leak memory.
 *
 * if C++ we do it automatically BUT don't define a virtual destructor
 * to avoid any issues with the vptr and the ABI
 *
 * NOTE - using struct w/ public fields C++ client needs to be careful when
 *        when assigning. assignment attempts to Close '*this' which
 *        will seg-fault if it hasn't been properly constructed - USE
 *        CONSTRUCTORS
 */
struct Credentials{
    char *access_token;
    char *refresh_token;
    long long epoch_sec_token_expiration;
    char *client_id;
#ifndef __cplusplus
};
#else
    static const int CRED_FIELD_MAX_STR_LEN = 256 * 16;

private:
    void
    _move_in( Credentials& sc )
    {
        access_token = sc.access_token;
        refresh_token = sc.refresh_token;
        epoch_sec_token_expiration = sc.epoch_sec_token_expiration;
        client_id = sc.client_id;
        sc.access_token = sc.refresh_token = sc.client_id = nullptr;
    }

public:
    Credentials()
        :
            access_token(),
            refresh_token(),
            epoch_sec_token_expiration(),
            client_id()
    {}

    Credentials( const char* access_token,
                 const char* refresh_token,
                 long long epoch_sec_token_expiration,
                 const char *client_id )
    {
        tdma::call_abi( CreateCredentials_ABI, access_token, refresh_token,
                        epoch_sec_token_expiration, client_id, this );
    }

    Credentials( const Credentials& sc )
    { tdma::call_abi( CopyCredentials_ABI, &sc, this ); }

    Credentials&
    operator=( const Credentials& sc )
    {
        // NOTE don't check for logical self, quicker to just overwrite
        tdma::call_abi( CloseCredentials_ABI, this );
        tdma::call_abi( CopyCredentials_ABI, &sc, this );
        return *this;
    }

    Credentials( Credentials&& sc )
        { _move_in(sc); }

    Credentials&
    operator=( Credentials&& sc )
    {
        // NOTE don't check for logical self, quicker to just overwrite
        tdma::call_abi( CloseCredentials_ABI, this );
        _move_in(sc);
        return *this;
    }

    /* NOT VIRTUAL */
    ~Credentials()
    {  tdma::call_abi( CloseCredentials_ABI, this ); }

};

#include <string>
#include "util.h"
#include <functional>

namespace tdma{

template<typename ProxyTy, typename ProxyBaseTy=void>
struct IsValidCProxy{
    static constexpr bool value =
        std::is_class<ProxyTy>::value
        && std::is_trivial<ProxyTy>::value
        && std::is_standard_layout<ProxyTy>::value
        && sizeof(ProxyTy) == sizeof(ProxyBaseTy);
};

template<typename ProxyTy>
struct IsValidCProxy<ProxyTy, void>{
    static constexpr bool value =
        IsValidCProxy<ProxyTy, Getter_C>::value
        || IsValidCProxy<ProxyTy, StreamingSubscription_C>::value
        || IsValidCProxy<ProxyTy, StreamingSession_C>::value
        || IsValidCProxy<ProxyTy, OrderLeg_C>::value
        || IsValidCProxy<ProxyTy, OrderTicket_C>::value;
};

template<typename ProxyTy>
struct IsCProxyBase{
    static constexpr bool value =
        std::is_same<ProxyTy, Getter_C>::value
        || std::is_same<ProxyTy, StreamingSubscription_C>::value
        || std::is_same<ProxyTy, StreamingSession_C>::value
        || std::is_same<ProxyTy, OrderLeg_C>::value
        || std::is_same<ProxyTy, OrderTicket_C>::value;
};

template<typename F, typename... Args>
std::string
str_from_abi_vargs( F abicall, bool allow_exception, Args... args )
{
    char *buf = nullptr;
    size_t n;
    error_to_exc( abicall(args..., &buf, &n, allow_exception) );
    if( !allow_exception && !buf ){
        return {};
    }
    assert(buf);
    std::string s(buf, n-1);
    error_to_exc( FreeBuffer_ABI(buf, allow_exception) );
    return s;
}

template<typename T> /* T - type of base proxy */
class CProxyDestroyer{
    std::function<int(T*, int)> _wrapper;
    static_assert(IsCProxyBase<T>::value, "T not a CProxyBase");
public:
    template<typename T2> /* T2 - type of proxy ABI destroy call needs */
    CProxyDestroyer( int(*abicall)(T2*, int) )
        :
            _wrapper( [=](T* obj, int e){
                          return abicall( reinterpret_cast<T2*>(obj), e);
                      } )
        { static_assert(IsValidCProxy<T2>::value, "T2 not valid CProxy"); }

    void operator()(T* ss){
        if( !ss )
            return;
        if( ss->obj ){
            int err = _wrapper(ss, 0);
            if( err ){
                std::cerr<< "Error destroying C proxy(" << err <<"): "
                         << str_from_abi_vargs(LastErrorMsg_ABI, false)
                         << std::endl;
            }
        }
        delete ss;
    }
};


/* C++ interface */

inline Credentials
LoadCredentials(std::string path, std::string password)
{
    Credentials c;
    call_abi( LoadCredentials_ABI, path.c_str(), password.c_str(), &c );
    return c;
}

inline void
StoreCredentials(std::string path, std::string password, const Credentials& creds)
{
    Credentials c(creds); // copy
    call_abi( StoreCredentials_ABI, path.c_str(), password.c_str(), &c );
}


struct CredentialsManager{
    Credentials credentials;
    std::string path;
    std::string password;
    CredentialsManager(std::string path, std::string password)
        : credentials( LoadCredentials(path, password) ),
          path( path ),
          password( password )
    {}
    virtual ~CredentialsManager()
    { StoreCredentials(path, password, credentials);}
};


inline Credentials
RequestAccessToken( std::string code,
                    std::string client_id,
                    std::string redirect_uri = "https://127.0.0.1")
{
    Credentials c;
    call_abi( RequestAccessToken_ABI, code.c_str(), client_id.c_str(),
              redirect_uri.c_str(), &c );
    return c;
}


inline void
RefreshAccessToken(Credentials& creds)
{ call_abi(RefreshAccessToken_ABI, &creds ); }

inline void
SetCertificateBundlePath(const std::string& path)
{ call_abi( SetCertificateBundlePath_ABI, path.c_str() ); }

inline std::string
GetCertificateBundlePath()
{ return str_from_abi_vargs(GetCertificateBundlePath_ABI, ALLOW_EXCEPTIONS); }

inline std::string
GetDefaultCertificateBundlePath()
{ return str_from_abi_vargs(GetDefaultCertificateBundlePath_ABI, ALLOW_EXCEPTIONS); }

inline int
LastErrorCode()
{
    int code;
    call_abi( LastErrorCode_ABI, &code );
    return code;
}

inline std::string
LastErrorMsg()
{ return str_from_abi_vargs(LastErrorMsg_ABI,ALLOW_EXCEPTIONS); }

inline int
LastErrorLineNumber()
{
    int lineno;
    call_abi( LastErrorLineNumber_ABI, &lineno);
    return lineno;
}

inline std::string
LastErrorFilename()
{ return str_from_abi_vargs(LastErrorFilename_ABI, ALLOW_EXCEPTIONS); }

inline std::string
BuildOptionSymbol( const std::string& underlying,
                   unsigned int month,
                   unsigned int day,
                   unsigned int year,
                   bool is_call,
                   double strike )
{ return str_from_abi_vargs( BuildOptionSymbol_ABI, ALLOW_EXCEPTIONS,
                             underlying.c_str(),
                             month, day, year, static_cast<int>(is_call),
                             strike ); }

// THROWS ValueException if bad symbol
inline void
CheckOptionSymbol(const std::string& symbol)
{ call_abi( CheckOptionSymbol_ABI, symbol.c_str() ); }


class APIException
        : public std::exception{
    std::string _what;
    int _lineno;
    std::string _filename;

public:
    static const int ERROR_CODE = TDMA_API_ERROR;

    APIException( const std::string& what,
                   int lineno=0,
                   const std::string& filename="" )
        :
            _what(what),
            _lineno(lineno),
            _filename(filename)
        {}

    const char*
    what() const noexcept
    { return _what.c_str(); }

    virtual const char*
    name() const noexcept
    { return "APIException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }

    int
    lineno() const noexcept
    { return _lineno; }

    const char*
    filename() const noexcept
    { return _filename.c_str(); }
};


inline std::ostream&
operator<<(std::ostream& out, const APIException& exc)
{
    out<< exc.name() << ": " << exc.what();
    return out;
}


class LocalCredentialException
        : public APIException{
protected:
    LocalCredentialException() = default;
public:
    static const int ERROR_CODE = TDMA_API_CRED_ERROR;

    using APIException::APIException;

    const char*
    name() const noexcept
    { return "LocalCredentialException"; }

    int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class ValueException
        : public APIException{
public:
    static const int ERROR_CODE = TDMA_API_VALUE_ERROR;

    using APIException::APIException;

    const char*
    name() const noexcept
    { return "ValueException"; }

    int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class TypeException
        : public APIException{
public:
    static const int ERROR_CODE = TDMA_API_TYPE_ERROR;

    using APIException::APIException;

    const char*
    name()
    { return "TypeException"; }

    int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class MemoryError
        : public APIException{
public:
    static const int ERROR_CODE = TDMA_API_MEMORY_ERROR;

    using APIException::APIException;

    virtual const char*
    name() const noexcept
    { return "MemoryError"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class ConnectException
        : public APIException{
public:
    static const int ERROR_CODE = TDMA_API_CONNECT_ERROR;

    ConnectException( std::string what,
                      int lineno,
                      const std::string& filename )
        : APIException(what, lineno, filename)
        {}

    ConnectException( std::string what,
                      int status_code,
                      int lineno,
                      const std::string& filename )
        : APIException( what + "(response code =" + std::to_string(status_code)
                        + ')', lineno, filename)
        {}

    virtual const char*
    name() const noexcept
    { return "ConnectException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class AuthenticationException
        : public ConnectException{
public:
    static const int ERROR_CODE = TDMA_API_AUTH_ERROR;

    using ConnectException::ConnectException;

    virtual const char*
    name() const noexcept
    { return "AuthenticationException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class InvalidRequest
        : public ConnectException{
public:
    static const int ERROR_CODE = TDMA_API_REQUEST_ERROR;

    using ConnectException::ConnectException;

    virtual const char*
    name() const noexcept
    { return "InvalidRequest"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class ServerError
        : public ConnectException{
public:
    static const int ERROR_CODE = TDMA_API_SERVER_ERROR;

    using ConnectException::ConnectException;

    virtual const char*
    name() const noexcept
    { return "ServerError"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class StreamingException
        : public APIException {
public:
    static const int ERROR_CODE = TDMA_API_STREAM_ERROR;

    using APIException::APIException;

    virtual const char*
    name() const noexcept
    { return "StreamingException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class ExecuteException
        : public APIException {
public:
    static const int ERROR_CODE = TDMA_API_EXECUTE_ERROR;

    using APIException::APIException;

    virtual const char*
    name() const noexcept
    { return "ExecuteException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class StdException
        : public APIException {
public:
    static const int ERROR_CODE = TDMA_API_STD_EXCEPTION;

    StdException(const std::string& what)
        : APIException(what){}

    virtual const char*
    name() const noexcept
    { return "StdException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class UnknownException
        : public APIException {
public:
    static const int ERROR_CODE = TDMA_API_UNKNOWN_EXCEPTION;

    UnknownException(const std::string& what)
        : APIException(what){}

    virtual const char*
    name() const noexcept
    { return "UnknownException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


static void
error_to_exc(int code)
{
    if( code == 0 )
        return;

    char *bufs[] = {nullptr, nullptr};
    int lineno, c;
    size_t n;

    int err = LastErrorState_ABI(&c, &bufs[0], &n, &lineno, &bufs[1], &n, 0);
    if( err ) {
        throw std::runtime_error("failed to get last error state("
                                 + std::to_string(err) + ")");
    }
    assert(bufs[0]);
    assert(bufs[1]);
    assert(code == c);
    std::string msg(bufs[0]);
    std::string fname(bufs[1]);
    FreeBuffer_ABI(bufs[0], 0);
    FreeBuffer_ABI(bufs[1], 0);

    // move original exc info -> what
    std::stringstream ss;
    ss << msg << " [error code: " << code << ", file: " << fname
       << ", line: " << lineno << ']';
    msg = ss.str();

    switch(code){
    case TDMA_API_ERROR: throw APIException(msg, lineno, fname);
    case TDMA_API_CRED_ERROR: throw LocalCredentialException(msg, lineno, fname);
    case TDMA_API_VALUE_ERROR: throw ValueException(msg, lineno, fname);
    case TDMA_API_TYPE_ERROR: throw TypeException(msg, lineno, fname);
    case TDMA_API_MEMORY_ERROR: throw MemoryError(msg, lineno, fname);
    case TDMA_API_CONNECT_ERROR: throw ConnectException(msg, lineno, fname);
    case TDMA_API_AUTH_ERROR: throw AuthenticationException(msg, lineno, fname);
    case TDMA_API_REQUEST_ERROR: throw InvalidRequest(msg, lineno, fname);
    case TDMA_API_SERVER_ERROR: throw ServerError(msg, lineno, fname);
    case TDMA_API_STREAM_ERROR: throw StreamingException(msg, lineno, fname);
    case TDMA_API_EXECUTE_ERROR: throw ExecuteException(msg, lineno, fname);
    case TDMA_API_STD_EXCEPTION: throw StdException(msg);
    case TDMA_API_UNKNOWN_EXCEPTION: throw UnknownException("unknown exception");
    default:
        throw UnknownException(
            "unknown error code(" + std::to_string(code) + ')'
            );
    };
}


template<typename FromTy, typename ToTy>
ToTy*
set_to_new_array( const std::set<FromTy>& from, ToTy(*trans)(const FromTy&) )
{
    ToTy *tmp = new ToTy[from.size()];
    std::transform( from.begin(), from.end(), tmp, trans );
    return tmp;
}

// NOTE - NOT COPYING STRINGS - DONT EDIT UNDERLYING STRINGS
inline const char**
set_to_new_cstrs(const std::set<std::string>& symbols )
{
    if( symbols.empty() )
        return nullptr;
        //throw ValueException("empty symbols set", __LINE__, __FILE__);
    return set_to_new_array( symbols,
                             +[](const std::string& s){ return s.c_str(); } );
}

template<typename FTy>
int*
set_to_new_int_array(const std::set<FTy>& fields)
{
    if( fields.empty() )
        return nullptr;
        //throw ValueException("empty fields set", __LINE__, __FILE__);
    return set_to_new_array( fields,
                             +[](const FTy& f){ return static_cast<int>(f); } );
}

template<typename F, typename A, typename... Args>
void
new_array_to_abi(F abicall, A *a, Args... args )
{
    try{
        call_abi( abicall, args... );
    }catch(...){
        if(a) delete[] a;
        throw;
    }
    if(a) delete[] a;
}

template<typename CTy>
std::string
str_from_abi( int(*abicall)(CTy*, char**, size_t*, int), CTy *cty )
{ return str_from_abi_vargs(abicall, ALLOW_EXCEPTIONS, cty); }

template<typename CTy>
std::set<std::string>
set_of_strs_from_abi( int(*abicall)(CTy*, char***, size_t*, int), CTy *cty )
{
    char **buf;
    size_t n;
    std::set<std::string> strs;
    call_abi( abicall, cty, &buf, &n );
    if( buf ){
        while(n--){
            char *c = buf[n];
            assert(c);
            strs.insert(c);
            free(c);
        }
        free(buf);
    }
    return strs;
}

template<typename CTy>
void
set_of_strs_to_abi( int(*abicall)(CTy*, const char**, size_t,int),
                    CTy *cty,
                    const std::set<std::string>& strs )
{
    const char** s = nullptr;
    try{
        s = set_to_new_cstrs(strs);
        call_abi( abicall, cty, s, strs.size());
        if(s) delete[] s;
    }catch(...){
        if(s) delete[] s;
        throw;
    }
}

template<typename FTy, typename CTy>
std::set<FTy>
set_of_fields_from_abi( int(*abicall)(CTy*, int**, size_t*,int), CTy *cty )
{
    int *f;
    size_t n;
    call_abi( abicall, cty, &f, &n );
    std::set<FTy> ret;
    while( n-- )
        ret.insert( static_cast<FTy>(f[n]) );
    call_abi( FreeFieldsBuffer_ABI, f );
    return ret;
}



template<typename CTy, typename FTy>
void
set_of_fields_to_abi( int(*abicall)(CTy*, int*, size_t, int),
                      CTy *cty,
                      const std::set<FTy>& fields )
{
    int *i = nullptr;
    try{
        i = set_to_new_int_array(fields);
        call_abi( abicall, cty, i, fields.size());
        if(i) delete[] i;
    }catch(...){
        if(i) delete[] i;
        throw;
    }
}

} /* tdma */

#endif /* __cplusplus */

#endif /* TDMA_COMMON_H */
