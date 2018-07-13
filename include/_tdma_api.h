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

#ifndef TDMA_API_H_
#define TDMA_API_H_

#include "_common.h"

#include <string>
#include <functional>
#include <sstream>
#include <chrono>

#include "util.h"
#include "tdma_api_get.h"
#include "tdma_api_streaming.h"
#include "curl_connect.h"


namespace tdma{

const std::string URL_BASE = "https://api.tdameritrade.com/v1/";

inline json
get_user_principals_for_streaming(Credentials& creds)
{ throw APIException("NOT IMPLEMENTED");
//return UserPrincipalsGetter(creds,true,true,false,false).get();
}

StreamerInfo
get_streamer_info(Credentials& creds);

std::pair<std::string, conn::clock_ty::time_point>
api_execute( conn::HTTPSConnection& connection,
              Credentials& creds,
              api_on_error_cb_ty on_error_cb );//= default_api_on_error_callback );


json
api_auth_execute( conn::HTTPSPostConnection& connection, std::string fname);

bool
error_msg_about_token_expiration(const std::string& msg);

std::string
unescape_returned_post_data(const std::string& s);


class APIGetterImpl{
    static std::chrono::milliseconds wait_msec; // DEF_WAIT_MSEC
    static std::chrono::milliseconds last_get_msec; // 0
    static std::mutex get_mtx;

    static std::string
    throttled_get(APIGetterImpl& getter);

    api_on_error_cb_ty _on_error_callback;
    std::reference_wrapper<Credentials> _credentials;
    conn::HTTPSGetConnection _connection;

protected:
    APIGetterImpl(Credentials& creds, api_on_error_cb_ty on_error_callback);

    /*
     * restrict copy and assign (for now at least):
     *
     *   1) should there ever be more than one of the same exact getter?
     *   2) want to restrict copy/assign of the underlying connection
     *      object to simplify things so if we share refs to it:
     *         a) one ref can close() on another
     *         b) destruction becomes more complicated
     *
     * allow move (consistent w/ underlying connection objects)
     */

    APIGetterImpl( APIGetterImpl&& ) = default;

    APIGetterImpl&
    operator=( APIGetterImpl&& ) = default;

    virtual
    ~APIGetterImpl(){}

    void
    set_url(std::string url);

public:
    static const std::chrono::milliseconds DEF_WAIT_MSEC;

    static std::chrono::milliseconds
    get_wait_msec();

    static void
    set_wait_msec(std::chrono::milliseconds msec);

    std::string
    get();

    void
    close();
};


template<typename RetTy>
struct ImplReturnHelper{
    typedef std::pair<RetTy,int> type;

    template<typename... Args, typename... Args2>
    static type
    from_call(RetTy(*func)(Args...), Args2... args)
    { return type{func(args...), 0}; }

    static type
    from_error(int err)
    {
        static_assert( std::is_default_constructible<RetTy>::value,
                       "'RetTy' not default constructible" );
        return type{RetTy(), err};
    }
};

template<>
struct ImplReturnHelper<void>{
    typedef int type;

    template<typename... Args, typename... Args2>
    static type
    from_call(void(*func)(Args...), Args2... args)
    {
        func(args...);
        return 0;
    }

    static type
    from_error(int err)
    { return err; }
};

template<typename RetTy, typename... Args, typename... Args2>
typename ImplReturnHelper<RetTy>::type
CallImplFromABI(bool allow_throw, RetTy(*func)(Args...), Args2... args)
{
    if( allow_throw )
        return ImplReturnHelper<RetTy>::from_call(func, args...);

    int err = 0;
    try{
        return ImplReturnHelper<RetTy>::from_call(func, args...);
    }catch(StreamingException& e){ err = TDMA_API_STREAM_ERROR;
    }catch(ServerError& e){ err = TDMA_API_SERVER_ERROR;
    }catch(InvalidRequest& e){ err = TDMA_API_REQEST_ERROR;
    }catch(AuthenticationException& e){ err = TDMA_API_AUTH_ERROR;
    }catch(APIExecutionException& e){ err = TDMA_API_EXEC_ERROR;
    }catch(MemoryError& e){ err = TDMA_API_MEMORY_ERROR;
    }catch(TypeException& e) { err = TDMA_API_TYPE_ERROR;
    }catch(ValueException& e){ err = TDMA_API_VALUE_ERROR;
    }catch(LocalCredentialException& e){ err = TDMA_API_CRED_ERROR;
    }catch(APIException& e){ err = TDMA_API_ERROR;
    }

    return ImplReturnHelper<RetTy>::from_error(err);
}

} /* tdma */

#endif // TDMA_API_H_
