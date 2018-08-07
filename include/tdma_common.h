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

#include "_common.h"
#include <string.h>

#ifdef __cplusplus
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
    type##_##to_string_ABI(static_cast<int>(v), &buf, &n, 1); \
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
type##_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions);

#define BUILD_C_CPP_TDMA_ENUM_NAME(type,name) type##_##name
#endif /* __cplusplus */

struct Credentials;

/*
 * ABI bridge functions
 *
 *
 * How we provide C and C++ interface w/ a stable ABI
 *
 *           Interface   -->     ABI Bridge   -->    Implementation
 *
 *  C    LoadCredentials()   LoadCredentials_ABI()
 *  C++  LoadCredentials()                           LoadCredentialsImpl()
 *
 *  Errors:
 *
 *  C       Error Code    <--   Error Code   <--    Exception
 *  C++     Exception     <--   Exception    <--    Exception
 *
 *
 * INLINE interface calss  so they stay on client-side of ABI
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

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorCode_ABI( int *code, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
LastErrorMsg_ABI( char** buf, size_t *n, int allow_exceptions );



/* C interface */

inline int
LoadCredentials( const char* path,
                   const char* password,
                   struct Credentials* pcreds )
{ return LoadCredentials_ABI(path, password, pcreds,0); }

inline int
StoreCredentials( const char* path,
                    const char* password,
                    const struct Credentials* pcreds )
{ return StoreCredentials_ABI(path, password, pcreds, 0); }

inline int
RequestAccessToken( const char* code,
                      const char* client_id,
                      const char* redirect_uri,
                      struct Credentials* pcreds )
{ return RequestAccessToken_ABI(code, client_id, redirect_uri, pcreds, 0); }

inline int
RefreshAccessToken(struct Credentials* creds )
{ return RefreshAccessToken_ABI(creds, 0); }

inline int
SetCertificateBundlePath(const char* path)
{ return SetCertificateBundlePath_ABI(path, 0); }

inline int
GetCertificateBundlePath(char **path, size_t *n)
{ return GetCertificateBundlePath_ABI(path, n, 0); }

inline int
GetDefaultCertificateBundlePath(char **path, size_t *n )
{ return GetDefaultCertificateBundlePath_ABI(path, n, 0); }

inline int
CloseCredentials(struct Credentials* pcreds )
{ return CloseCredentials_ABI(pcreds, 0); }

inline int
CopyCredentials(const struct Credentials* from, struct Credentials *to)
{ return CopyCredentials_ABI(from, to, 0); }

inline int
FreeBuffer( char* buf )
{ return FreeBuffer_ABI(buf, 0); }

inline int
FreeBuffers( char** bufs, size_t n)
{ return FreeBuffers_ABI(bufs, n, 0); }

inline int
FreeFieldsBuffer( int* fields )
{ return FreeFieldsBuffer_ABI(fields, 0); }


#define TDMA_API_ERROR 1
#define TDMA_API_CRED_ERROR 2
#define TDMA_API_VALUE_ERROR 3
#define TDMA_API_TYPE_ERROR 4
#define TDMA_API_MEMORY_ERROR 5

#define TDMA_API_EXEC_ERROR 101
#define TDMA_API_AUTH_ERROR 102
#define TDMA_API_REQUEST_ERROR 103
#define TDMA_API_SERVER_ERROR 104

#define TDMA_API_STREAM_ERROR 201

inline int
LastErrorCode( int *code )
{ return LastErrorCode_ABI(code, 0); }

inline int
LastErrorMsg( char** buf, size_t *n)
{ return LastErrorMsg_ABI(buf, n, 0); }


/*
 * if C, client has to call CloseCredentials and CopyCredentials directly
 * a) when done and b) before passing an active instance to LoadCredentials
 * or RequestAccessToken or the internal char*s will leak memory.
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

    Credentials( const Credentials& sc )
    { CopyCredentials_ABI(&sc, this, true); }

    Credentials&
    operator=( Credentials& sc )
    {
        // NOTE don't check for logical self, quicker to just overwrite
        CloseCredentials_ABI(this, true);
        CopyCredentials_ABI(&sc, this, true);
        return *this;
    }

    Credentials( Credentials&& sc )
        { _move_in(sc); }

    Credentials&
    operator=( Credentials&& sc )
    {
        // NOTE don't check for logical self, quicker to just overwrite
        CloseCredentials_ABI(this, true);
        _move_in(sc);
        return *this;
    }

    /* NOT VIRTUAL */
    ~Credentials()
    { CloseCredentials_ABI(this, true); }

};
#endif /* __cplusplus */

/* C++ interface */

#ifdef __cplusplus

#include <string>
#include "util.h"
#include <functional>

namespace tdma{

template<typename CTy, typename CBaseTy>
struct IsValidCProxy{
    static constexpr bool value = std::is_class<CTy>::value
        && std::is_trivial<CTy>::value
        && std::is_standard_layout<CTy>::value
        && sizeof(CTy) == sizeof(CBaseTy);
};

template<typename T>
class CProxyDestroyer{
    std::function<int(T*, int)> _abicall;
public:
    CProxyDestroyer( std::function<int(T*, int)> abicall)
        : _abicall(abicall) {}

    void operator()(T* ss){
        if( !ss )
            return;
        if( ss->obj ){
            int err = _abicall(ss, 0);
            if( err ){
                char *buf;
                size_t n;
                LastErrorMsg_ABI(&buf, &n, 0);
                std::cerr<< "Error destroying C proxy(" << err <<"): "
                         << buf << std::endl;
                FreeBuffer_ABI(buf, 0);
            }
        }
        delete ss;
    }
};

inline Credentials
LoadCredentials(std::string path, std::string password)
{
    Credentials c;
    LoadCredentials_ABI(path.c_str(), password.c_str(), &c, true);
    return c;
}

inline void
StoreCredentials(std::string path, std::string password, const Credentials& creds)
{
    Credentials c(creds); // copy
    StoreCredentials_ABI(path.c_str(), password.c_str(), &c, true);
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
RequestAccesToken( std::string code,
                     std::string client_id,
                     std::string redirect_uri = "https://127.0.0.1")
{
    Credentials c;
    RequestAccessToken_ABI( code.c_str(), client_id.c_str(),
                            redirect_uri.c_str(), &c, true);
    return c;
}


inline void
RefreshAccessToken(Credentials& creds)
{ RefreshAccessToken_ABI( &creds, true ); }

inline void
SetCertificateBundlePath(const std::string& path)
{ SetCertificateBundlePath_ABI(path.c_str(), true); }

inline std::string
GetCertificateBundlePath()
{
    char* path;
    size_t n;
    GetCertificateBundlePath_ABI( &path, &n, true );
    std::string s(path, n-1);
    if(path) free(path);
    return s;
}

inline std::string
GetDefaultCertificateBundlePath()
{
    char* path;
    size_t n;
    GetDefaultCertificateBundlePath_ABI( &path, &n, true );
    std::string s(path, n-1);
    if(path) free(path);
    return s;
}

class APIException
        : public std::exception{
    std::string _what;
public:
    static const int ERROR_CODE = TDMA_API_ERROR;

    APIException()
    {}

    APIException(std::string what)
        : _what(what)
    {}

    virtual const char*
    what() const noexcept
    { return _what.c_str(); }

    virtual const char*
    name() const noexcept
    { return "APIException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class LocalCredentialException
        : public APIException{
protected:
    LocalCredentialException() = default;
public:
    static const int ERROR_CODE = TDMA_API_CRED_ERROR;

    LocalCredentialException(std::string what)
        : APIException(what)
    {}

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

    ValueException(std::string what)
        : APIException(what)
    {}

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

    TypeException(std::string what)
        : APIException(what)
    {}

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

    MemoryError(std::string what)
        : APIException(what)
    {}

    virtual const char*
    name() const noexcept
    { return "MemoryError"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class APIExecutionException
        : public APIException{
    int _status_code;
public:
    static const int ERROR_CODE = TDMA_API_EXEC_ERROR;

    APIExecutionException(std::string what, int status_code)
        : APIException(what),
          _status_code(status_code)
    {}

    virtual const char*
    name() const noexcept
    { return "APIExecutionException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }

    int
    status_code() const noexcept
    { return _status_code; }
};


class AuthenticationException
        : public APIExecutionException{
public:
    static const int ERROR_CODE = TDMA_API_AUTH_ERROR;

    AuthenticationException(std::string what, int status_code)
        : APIExecutionException(what, status_code)
    {}

    virtual const char*
    name() const noexcept
    { return "AuthenticationException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class InvalidRequest
        : public APIExecutionException{
public:
    static const int ERROR_CODE = TDMA_API_REQUEST_ERROR;

    InvalidRequest(std::string what, int status_code)
        : APIExecutionException(what, status_code)
    {}

    virtual const char*
    name() const noexcept
    { return "InvalidRequest"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};


class ServerError
        : public APIExecutionException{
public:
    static const int ERROR_CODE = TDMA_API_SERVER_ERROR;
    ServerError(std::string what, int status_code)
        : APIExecutionException(what, status_code)
    {}

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

    StreamingException()
    {}

    StreamingException(std::string what)
        : APIException(what)
    {}

    virtual const char*
    name() const noexcept
    { return "StreamingException"; }

    virtual int
    error_code() const noexcept
    { return ERROR_CODE; }
};

// TODO move to utils
inline const char*
CPP_to_C(const std::string& in)
{ return in.c_str(); }

inline Credentials*
CPP_to_C(Credentials& in)
{ return &in; }



} /* tdma */

#endif /* __cplusplus */

#endif /* TDMA_COMMON_H */
