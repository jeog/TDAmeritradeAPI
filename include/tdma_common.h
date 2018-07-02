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

#include <string>

#include "_common.h"
#include "json.hpp"
#include "util.h"

using json = nlohmann::json;

namespace tdma{

struct DLL_SPEC_ Credentials{
    std::string access_token;
    std::string refresh_token;
    long long epoch_sec_token_expiration;
    std::string client_id;
};


DLL_SPEC_ Credentials
LoadCredentials(std::string path, std::string password);

DLL_SPEC_ void
StoreCredentials(std::string path, std::string password, const Credentials& creds);


struct DLL_SPEC_ CredentialsManager{
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


DLL_SPEC_ Credentials
RequestAccesToken( std::string code,
                   std::string client_id,
                   std::string redirect_uri = "https://127.0.0.1");

DLL_SPEC_ void
RefreshAccessToken(Credentials& creds);

DLL_SPEC_ void
SetCertificateBundlePath(const std::string& path);

class DLL_SPEC_ APIException
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


class DLL_SPEC_ LocalCredentialException
        : public APIException{
protected:
    LocalCredentialException() = default;
public:
    LocalCredentialException(std::string what)
        : APIException(what)
    {}
};


class DLL_SPEC_ ValueException
        : public APIException{
public:
    ValueException(std::string what)
        : APIException(what)
    {}
};


class DLL_SPEC_ APIExecutionException
        : public APIException{
public:
    const long code;
    APIExecutionException(std::string what, long code)
        : APIException(what),
          code(code)
    {}
};


class DLL_SPEC_ AuthenticationException
        : public APIExecutionException{
public:
    AuthenticationException(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class DLL_SPEC_ InvalidRequest
        : public APIExecutionException{
public:
    InvalidRequest(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class DLL_SPEC_ ServerError
        : public APIExecutionException{
public:
    ServerError(std::string what, long code)
        : APIExecutionException(what,code)
    {}
};


class DLL_SPEC_ StreamingException
        : public APIException {
public:
    StreamingException()
    {}
    StreamingException(std::string what)
        : APIException(what)
    {}
};


} /* tdma */


#endif /* TDMA_COMMON_H */
