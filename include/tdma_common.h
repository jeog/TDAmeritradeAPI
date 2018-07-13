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


/* C interface */

inline int
LoadCredentials( const char* path,
                   const char* password,
                   struct Credentials* pcreds )
{ return LoadCredentials_ABI(path, password, pcreds,false); }

inline int
StoreCredentials( const char* path,
                    const char* password,
                    const struct Credentials* pcreds )
{ return StoreCredentials_ABI(path, password, pcreds, false); }

inline int
RequestAccessToken( const char* code,
                      const char* client_id,
                      const char* redirect_uri,
                      struct Credentials* pcreds )
{ return RequestAccessToken_ABI(code, client_id, redirect_uri, pcreds, false); }

inline int
RefreshAccessToken(struct Credentials* creds )
{ return RefreshAccessToken_ABI(creds, false); }

inline int
SetCertificateBundlePath(const char* path)
{ return SetCertificateBundlePath_ABI(path, false); }

inline int
GetCertificateBundlePath(char **path, size_t *n)
{ return GetCertificateBundlePath_ABI(path, n, false); }

inline int
GetDefaultCertificateBundlePath(char **path, size_t *n )
{ return GetDefaultCertificateBundlePath_ABI(path, n, false); }

inline int
CloseCredentials(struct Credentials* pcreds )
{ return CloseCredentials_ABI(pcreds, false); }

inline int
CopyCredentials(const struct Credentials* from, struct Credentials *to)
{ return CopyCredentials_ABI(from, to, false); }


#define TDMA_API_ERROR 1
#define TDMA_API_CRED_ERROR 2
#define TDMA_API_VALUE_ERROR 3
#define TDMA_API_TYPE_ERROR 4
#define TDMA_API_MEMORY_ERROR 5

#define TDMA_API_EXEC_ERROR 101
#define TDMA_API_AUTH_ERROR 102
#define TDMA_API_REQEST_ERROR 103
#define TDMA_API_SERVER_ERROR 104

#define TDMA_API_STREAM_ERROR 201


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

namespace tdma{

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
    APIException()
    {}
    APIException(std::string what)
        : _what(what)
    {}
    const char*
    what() const noexcept
    { return _what.c_str(); }
};


class LocalCredentialException
        : public APIException{
protected:
    LocalCredentialException() = default;
public:
    LocalCredentialException(std::string what)
        : APIException(what)
    {}
};


class ValueException
        : public APIException{
public:
    ValueException(std::string what)
        : APIException(what)
    {}
};


class TypeException
        : public APIException{
public:
    TypeException(std::string what)
        : APIException(what)
    {}
};


class MemoryError
        : public APIException{
public:
    MemoryError(std::string what)
        : APIException(what)
    {}
};


class APIExecutionException
        : public APIException{
public:
    const long code;
    APIExecutionException(std::string what, long code)
        : APIException(what),
          code(code)
    {}
};


class AuthenticationException
        : public APIExecutionException{
public:
    AuthenticationException(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class InvalidRequest
        : public APIExecutionException{
public:
    InvalidRequest(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class ServerError
        : public APIExecutionException{
public:
    ServerError(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class StreamingException
        : public APIException {
public:
    StreamingException()
    {}
    StreamingException(std::string what)
        : APIException(what)
    {}
};


} /* tdma */

#endif /* __cplusplus */

#endif /* TDMA_COMMON_H */
