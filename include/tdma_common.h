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

#include "json.hpp"
#include "util.h"

using json = nlohmann::json;

namespace tdma{

struct Credentials{
    std::string access_token;
    std::string refresh_token;
    long long epoch_sec_token_expiration;
    std::string client_id;
};


Credentials
LoadCredentials(std::string path, std::string password);

void
StoreCredentials(std::string path, std::string password, const Credentials& creds);


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


Credentials
RequestAccesToken( std::string code,
                   std::string client_id,
                   std::string redirect_uri = "https://127.0.0.1");

void
RefreshAccessToken(Credentials& creds);


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


#endif /* TDMA_COMMON_H */
