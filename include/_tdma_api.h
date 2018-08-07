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
const std::string URL_MARKETDATA = URL_BASE + "marketdata/";
const std::string URL_ACCOUNT_INFO = URL_BASE + "accounts/";
const std::string URL_INSTRUMENTS = URL_BASE + "instruments";

json
get_user_principals_for_streaming(Credentials& creds);

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

void
data_api_on_error_callback(long code, const std::string& data);

void
account_api_on_error_callback(long code, const std::string& data);

void
query_api_on_error_callback(long code, const std::string& data);

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
    typedef APIGetter ProxyType;

    static const std::chrono::milliseconds DEF_WAIT_MSEC;

    static std::chrono::milliseconds
    get_wait_msec();

    static void
    set_wait_msec(std::chrono::milliseconds msec);

    std::string
    get();

    void
    close();

    bool
    is_closed() const;
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
    using namespace std;

    if( allow_throw )
        return ImplReturnHelper<RetTy>::from_call(func, args...);

    int err = 0;
    string msg;
    try{
        return ImplReturnHelper<RetTy>::from_call(func, args...);
    }catch(APIExecutionException& e){
        err = e.error_code();
        msg = e.what() + string("(") + to_string(e.status_code()) + ")";
        cerr<< "ABI :: " << e.name() << " --> error code " << err << endl;
    }catch(APIException& e){
        err = e.error_code();
        msg = e.what();
        cerr<< "ABI :: " << e.name() << " --> error code " << err << endl;
    }catch(std::exception& e){
        cerr<< "ABI :: caught " << e.what() << "... rethrowing" << endl;
        set_error_state(-1, e.what());
        throw;
    }catch(...){
        cerr<< "ABI :: caught unknown object... rethrowing" << endl;
        set_error_state(-1, "unknown error");
        throw;
    }

    set_error_state(err, msg);
    return ImplReturnHelper<RetTy>::from_error(err);
}

template<typename ExcTy>
int
handle_error(std::string msg, bool exc)
{
    int code = ExcTy::ERROR_CODE;
    set_error_state(code, msg);
    if( exc ){
        throw ExcTy(msg);
    }
    return code;
}

template<typename ImplTy>
int
getter_is_creatable( Credentials *pcreds,
                       typename ImplTy::ProxyType::CType *pgetter,
                       int allow_exceptions )
{
    static_assert( ImplTy::ProxyType::TYPE_ID_LOW > 0 &&
                   ImplTy::ProxyType::TYPE_ID_HIGH > 0,
                   "invalid getter type" );
    if( !pgetter )
        return handle_error<tdma::ValueException>(
            "null getter pointer", allow_exceptions
            );

    if( !pcreds ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return handle_error<tdma::ValueException>(
            "null credentials pointer", allow_exceptions
            );
    }

    if( !pcreds->access_token | !pcreds->refresh_token | !pcreds->client_id ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return handle_error<tdma::LocalCredentialException>(
            "invalid credentials struct", allow_exceptions
            );
    }

    return 0;
}

inline int // TODOD
base_getter_is_callable( Getter_C *pgetter, int allow_exceptions )
{
    if( !pgetter )
        return handle_error<tdma::ValueException>(
            "null getter pointer", allow_exceptions
            );

    if( !pgetter->obj )
        return handle_error<tdma::ValueException>(
            "null getter pointer->obj", allow_exceptions
            );

    return 0;
}

template<typename ImplTy>
int
getter_is_callable( typename ImplTy::ProxyType::CType *pgetter,
                      int allow_exceptions )
{
    int err = base_getter_is_callable(
        reinterpret_cast<Getter_C*>(pgetter), allow_exceptions
        );
    if( err )
        return err;

    if( pgetter->type_id < ImplTy::ProxyType::TYPE_ID_LOW ||
        pgetter->type_id > ImplTy::ProxyType::TYPE_ID_HIGH )
    {
        return handle_error<tdma::TypeException>(
            "getter has invalid type id", allow_exceptions
            );
    }
    return 0;
}


template<typename ImplTy>
int
destroy_getter(typename ImplTy::ProxyType::CType *pgetter, int allow_exceptions)
{
    int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj){
        delete reinterpret_cast<ImplTy*>(obj);
    };

    return CallImplFromABI(allow_exceptions, meth, pgetter->obj);
}

template<typename CTy>
int
check_abi_enum(std::function<bool(int)> func , int val, CTy* p, int allow_exceptions)
{
    if( !func(val) ){
        p->obj = nullptr;
        p->type_id = -1;
        return handle_error<tdma::ValueException>(
            "invalid enum value", allow_exceptions
            );
    }
    return 0;
}

inline int
check_abi_enum(std::function<bool(int)> func , int val, int allow_exceptions)
{
    if( !func(val) ){
        return handle_error<tdma::ValueException>(
            "invalid enum value", allow_exceptions
            );
    }
    return 0;
}


template<typename T> /* FOR BASIC TYPES i.e. enum <--> int */
struct GetterImplAccessor{
    // set single statically castable value
    template<typename ImplTy, typename CastToTy=T>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         void(ImplTy::*method)(CastToTy),
         T val,
         int allow_exceptions)
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap = +[](void* obj, void(ImplTy::*meth)(CastToTy), T m){
            return (reinterpret_cast<ImplTy*>(obj)
                ->*meth)(static_cast<CastToTy>(m));
        };

        return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method, val);
    }

    // set two statically castable values
    template< typename ImplTy, typename CastToTy=T,
              typename T2=T, typename CastToTy2=T2 >
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         void(ImplTy::*method)(CastToTy, CastToTy2),
         T val,
         T2 val2,
         int allow_exceptions)
    {
        int err = getter_is_callable<ImplTy>(
            pgetter, allow_exceptions
            );
        if( err )
            return err;

        static auto mwrap =
            +[](void* obj, void(ImplTy::*meth)(CastToTy, CastToTy2), T a, T2 b){
                return (reinterpret_cast<ImplTy*>(obj)
                        ->*meth)(static_cast<CastToTy>(a),
                                 static_cast<CastToTy2>(b));
        };

        return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method,
                               val, val2);
    }

    // get single statically castable value
    template<typename ImplTy, typename CastFromTy=T>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         CastFromTy(ImplTy::*method)(void) const,
         T *pval,
         std::string val_name,
         int allow_exceptions )
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        if( !pval){
            return handle_error<tdma::ValueException>(
                "null " + val_name + " pointer", allow_exceptions
                );
        }

        static auto mwrap =
            +[](void* obj, CastFromTy(ImplTy::*meth)(void) const){
                return (reinterpret_cast<ImplTy*>(obj)->*meth)();
            };

        CastFromTy val;
        std::tie(val, err) = CallImplFromABI(allow_exceptions, mwrap,
                                             pgetter->obj, method);
        if( err )
            return err;

        *pval = static_cast<T>(val);
        return 0;
    }


};


template<> /* FOR C STR TYPES */
struct GetterImplAccessor<char**>{
    template<typename ImplTy>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         void(ImplTy::*method)(const std::string&),
         const char* val,
         int allow_exceptions)
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap =
            +[](void* obj, void(ImplTy::*meth)(const std::string&), const char* v){
            return (reinterpret_cast<ImplTy*>(obj)->*meth)(v ? v : "");
        };

        return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method, val);
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::string(ImplTy::*method)(void) const,
         char **pval,
         size_t *n,
         int allow_exceptions )
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        if( !pval )
            return handle_error<tdma::ValueException>(
                "null 'buf' pointer", allow_exceptions
                );

        if( !n )
            return handle_error<tdma::ValueException>(
                "null 'n' pointer", allow_exceptions
                );

        static auto mwrap =
            +[](void* obj, std::string(ImplTy::*meth)(void) const){
                return (reinterpret_cast<ImplTy*>(obj)->*meth)();
            };

        std::string r;
        std::tie(r,err) = CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method);
        if( err )
            return err;

        *n = r.size() + 1; // NULL TERM
        *pval = reinterpret_cast<char*>(malloc(*n));
        if( !*pval ){
            return handle_error<tdma::MemoryError>(
                "failed to allocate buffer memory", allow_exceptions
                );
        }
        (*pval)[(*n)-1] = 0;
        strncpy(*pval, r.c_str(), (*n)-1);
        return 0;
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::string(ImplTy::*method)(void),
         char **pval,
         size_t *n,
         int allow_exceptions )
    { return get<ImplTy>(pgetter,
        reinterpret_cast<std::string(ImplTy::*)(void) const>(method),
        pval, n, allow_exceptions); }
};



template<> /* FOR ARRAY OF C STR TYPES */
struct GetterImplAccessor<char***>{
    template<typename ImplTy>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         void(ImplTy::*method)(const std::set<std::string>&),
         const char** val,
         size_t n,
         int allow_exceptions)
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap =
            +[]( void* obj, void(ImplTy::*meth)(const std::set<std::string>&),
                 const char** s, size_t n ){
                    std::set<std::string> strs;
                    while( n-- )
                        strs.insert(s[n]);
                    return (reinterpret_cast<ImplTy*>(obj)->*meth)(strs);
                };

        return CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method,
                               val, n);
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::set<std::string>(ImplTy::*method)(void) const,
         char ***pval,
         size_t *n,
         int allow_exceptions )
    {
        int err = getter_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        if( !pval )
            return handle_error<tdma::ValueException>(
                "null 'buf' pointer", allow_exceptions
                );

        if( !n )
            return handle_error<tdma::ValueException>(
                "null 'n' pointer", allow_exceptions
                );

        static auto mwrap =
            +[](void* obj, std::set<std::string>(ImplTy::*meth)(void) const){
                return (reinterpret_cast<ImplTy*>(obj)->*meth)();
            };

        std::set<std::string> strs;
        std::tie(strs,err) = CallImplFromABI(allow_exceptions, mwrap,
                                             pgetter->obj, method);
        if( err )
            return err;

        *n = strs.size();
        *pval = reinterpret_cast<char**>(malloc((*n) * sizeof(char*)));
        if( !*pval ){
            return handle_error<tdma::MemoryError>(
                "failed to allocate buffer memory", allow_exceptions
                );
        }

        int cnt = 0;
        for(auto& s : strs){
            size_t s_sz = s.size();
            (*pval)[cnt] = reinterpret_cast<char*>(malloc(s_sz+1));
            if( !(*pval)[cnt] ){
                return handle_error<tdma::MemoryError>(
                    "failed to allocate buffer memory", allow_exceptions
                    );
            }
            (*pval)[cnt][s_sz] = 0;
            strncpy((*pval)[cnt], s.c_str(), s_sz);
            ++cnt;
        }

        return 0;
    }
};

} /* tdma */

#endif // TDMA_API_H_
