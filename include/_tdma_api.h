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

/* HEADER FOR ALL (BACKEND) TDMA API LIBRAY/SOURCE */

#include "_common.h"

#include <string>
#include <functional>
#include <sstream>

#include "util.h"
#include "tdma_common.h"
#include "curl_connect.h"

namespace tdma{

const std::string URL_BASE = "https://api.tdameritrade.com/v1/";
const std::string URL_MARKETDATA = URL_BASE + "marketdata/";
const std::string URL_ACCOUNTS = URL_BASE + "accounts/";
const std::string URL_INSTRUMENTS = URL_BASE + "instruments";

typedef std::function<void(long, const std::string&)> api_on_error_cb_ty;

json
connect_auth( conn::HTTPSPostConnection& connection, std::string fname);

std::pair<std::string, conn::clock_ty::time_point>
connect_get( conn::HTTPSConnection& connection,
             Credentials& creds,
             api_on_error_cb_ty on_error_cb );

std::pair<std::string, conn::clock_ty::time_point>
connect_execute( conn::HTTPSConnection& connection,
                   Credentials& creds,
                   long success_code );

json
get_user_principals_for_streaming(Credentials& creds);

void
data_api_on_error_callback(long code, const std::string& data);

void
account_api_on_error_callback(long code, const std::string& data);

void
query_api_on_error_callback(long code, const std::string& data);

int
to_new_char_buffer( const std::string& s,
                    char** buf,
                    size_t* n,
                    bool allow_exceptions );

int
to_new_char_buffers( const std::set<std::string>& strs,
                     char*** bufs,
                     size_t *n,
                     bool allow_exceptions );

void
set_error_state( int code,
                 const std::string&  msg,
                 int fileno,
                 const std::string& filename );

std::tuple<int, std::string, int, std::string>
get_error_state();


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

/*
 * CallImplFromABI provides an exception-safe environment to run implementation
 *                 code on the 'inside' of the library boundary. Can propogate
 *                 exceptions or return error codes with or without return
 *                 value of passed function pointer.
 */
template<typename RetTy, typename... Args, typename... Args2>
typename ImplReturnHelper<RetTy>::type
CallImplFromABI(bool allow_throw, RetTy(*func)(Args...), Args2... args)
{
    if( allow_throw )
        return ImplReturnHelper<RetTy>::from_call(func, args...);

    int err = 0;
    int lineno = 0;
    std::string msg, filename;

    try{
        return ImplReturnHelper<RetTy>::from_call(func, args...);
    }catch(ConnectException& e){
        err = e.error_code();
        msg = e.what();
        lineno = e.lineno();
        filename = e.filename();
        std::cerr<< "ABI :: " << e.name() << " --> error code " << err
                 << std::endl;
    }catch(APIException& e){
        err = e.error_code();
        msg = e.what();
        lineno = e.lineno();
        filename = e.filename();
        std::cerr<< "ABI :: " << e.name() << " --> error code " << err
                 << std::endl;
    }catch(std::exception& e){
        err = TDMA_API_STD_EXCEPTION;
        msg = e.what();
        std::cerr<< "ABI :: std::exception(" << msg << ") --> error code "
                 << err << std::endl;
    }catch(...){
        err = TDMA_API_UNKNOWN_EXCEPTION;
        msg = "unknown exception";
        std::cerr<< "ABI :: unknown exception --> error code" << err
                 << std::endl;
    }

    set_error_state(err, msg, lineno, filename);
    return ImplReturnHelper<RetTy>::from_error(err);
}


/*
 * TDMA_API_THROW should ONLY be used by code that is run by CallImplFromABI
 */
#define TDMA_API_THROW(exc, ...) throw exc(__VA_ARGS__, __LINE__, __FILE__)


template<typename T2>
inline int
return_error(std::pair<T2, int>&& r){ return r.second; }

inline int
return_error(int r){ return r; }


template<typename ProxyTy>
void
kill_proxy( ProxyTy *proxy,
            typename std::enable_if<
                IsValidCProxy<ProxyTy, Getter_C>::value ||
                IsValidCProxy<ProxyTy, StreamingSubscription_C>::value ||
                IsValidCProxy<ProxyTy, OrderLeg_C>::value ||
                IsValidCProxy<ProxyTy, OrderTicket_C>::value
                >::type* _ = nullptr )
{
    proxy->obj = nullptr;
    proxy->type_id = -1;
}

template<typename ProxyTy>
void
kill_proxy( ProxyTy *proxy,
            typename std::enable_if<
                std::is_same<ProxyTy, StreamingSession_C>::value
                >::type* _ = nullptr )
{
    proxy->obj = nullptr;
    proxy->type_id = -1;
    proxy->ctx = nullptr;
}

template<typename ProxyTy>
void
kill_proxy( ProxyTy *proxy,
            typename std::enable_if<
                std::is_same<ProxyTy, std::nullptr_t>::value
                >::type* _ = nullptr )
{}


/*
 * handle_error<> can be used anywhere on the 'inside' of the library boundary
 *                 to throw exceptions, set error state, and kill proxies
 *
 * HANDLE_ERROR/EX are recommended for building exceptions w/ line number/file
 *                 that can be stored globally and passed to the reconstituted
 *                 exceptions on the client side of the lib (see error_to_exc).
 */
template<typename ExcTy, typename ProxyTy=std::nullptr_t>
int
handle_error( const std::string& msg,
              bool exc,
              int lineno,
              const std::string& filename,
              ProxyTy *proxy=nullptr)
{
    if( proxy )
        kill_proxy(proxy);
    int code = ExcTy::ERROR_CODE;
    set_error_state(code, msg, lineno, filename);
    if( exc )
        throw ExcTy(msg, lineno, filename);
    return code;
}

#define HANDLE_ERROR(exc, msg, allow) \
    handle_error<exc>(msg, allow, __LINE__, __FILE__)

#define HANDLE_ERROR_EX(exc, msg, allow, proxy) \
    handle_error<exc>(msg, allow, __LINE__, __FILE__, proxy)



template<typename T>
int
alloc_to_buffer(T** pbuf, size_t n, bool allow_exceptions)
{
    *pbuf = reinterpret_cast<T*>(malloc((n) * sizeof(T)));
    if( !(*pbuf) ){
        return HANDLE_ERROR(
            MemoryError, "failed to allocate buffer memory", allow_exceptions
            );
    }
    return 0;
}


/*
 * CHECK_PTR/KILL check for null pointer and call HANDLE_ERROR/EX
 */
#define CHECK_PTR(ptr, name, exc) \
if( !(ptr) ) \
    return HANDLE_ERROR( tdma::ValueException, "null " name " pointer", exc )

#define CHECK_PTR_KILL_PROXY(ptr, name, exc, proxy) \
if( !(ptr) ) \
    return HANDLE_ERROR_EX( tdma::ValueException, "null " name " pointer", \
                            exc, proxy )


/*
 * CHECK_ENUM/KILL check for invalid enums and call HANDLE_ERROR/EX
 */
#define CHECK_ENUM(name, val, exc) \
if( !(name##_is_valid(val)) ) \
    return HANDLE_ERROR( tdma::ValueException, "invalid " #name " enum value", \
                         exc )

#define CHECK_ENUM_KILL_PROXY(name, val, exc, proxy) \
if( !(name##_is_valid(val)) ) \
    return HANDLE_ERROR_EX( tdma::ValueException, "invalid " #name " enum value", \
                            exc, proxy )


template<typename ImplTy>
int
getter_is_creatable( Credentials *pcreds,
                       typename ImplTy::ProxyType::CType *pgetter,
                       int allow_exceptions )
{
    static_assert( ImplTy::TYPE_ID_LOW > 0 && ImplTy::TYPE_ID_HIGH > 0,
                   "invalid getter type" );

    CHECK_PTR(pgetter, "getter", allow_exceptions);

    CHECK_PTR_KILL_PROXY(pcreds, "credentials", allow_exceptions, pgetter);

    if( !pcreds->access_token | !pcreds->refresh_token | !pcreds->client_id ){
        return HANDLE_ERROR_EX(tdma::LocalCredentialException,
            "invalid credentials struct", allow_exceptions, pgetter
            );
    }

    return 0;
}

template<typename T>
inline int
base_proxy_is_callable( T *proxy, int allow_exceptions )
{
    static_assert( IsValidCProxy<T>::value, "invalid C Proxy");
    CHECK_PTR(proxy, "proxy", allow_exceptions);
    CHECK_PTR(proxy->obj, "proxy->obj", allow_exceptions);
    return 0;
}

template<typename ImplTy>
int
proxy_is_callable( typename ImplTy::ProxyType::CType *proxy,
                   int allow_exceptions )
{
    int err = base_proxy_is_callable(proxy, allow_exceptions);
    if( err )
        return err;

    if( proxy->type_id < ImplTy::TYPE_ID_LOW ||
        proxy->type_id > ImplTy::TYPE_ID_HIGH )
    {
        return HANDLE_ERROR(TypeException,"invalid type id", allow_exceptions);
    }
    return 0;
}


template<typename ImplTy>
int
destroy_proxy(typename ImplTy::ProxyType::CType *proxy, int allow_exceptions)
{
    int err = proxy_is_callable<ImplTy>(proxy, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void* obj){
        delete reinterpret_cast<ImplTy*>(obj);
    };

    err = CallImplFromABI(allow_exceptions, meth, proxy->obj);
    kill_proxy(proxy);
    return err;
}


template<typename T> /* FOR BASIC TYPES i.e. enum <--> int */
struct ImplAccessor{
    // set single statically castable value
    template<typename ImplTy, typename CastToTy=T, typename R=void>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         R(ImplTy::*method)(CastToTy),
         T val,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap = +[](void* obj, R(ImplTy::*meth)(CastToTy), T m){
            return (reinterpret_cast<ImplTy*>(obj)
                ->*meth)(static_cast<CastToTy>(m));
        };

        return return_error(
            CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method, val)
        );
    }


    // set two statically castable values
    template< typename ImplTy, typename CastToTy=T,
              typename T2=T, typename CastToTy2=T2, typename R=void >
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         R(ImplTy::*method)(CastToTy, CastToTy2),
         T val,
         T2 val2,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap =
            +[](void* obj, R(ImplTy::*meth)(CastToTy, CastToTy2), T a, T2 b){
                return (reinterpret_cast<ImplTy*>(obj)
                        ->*meth)(static_cast<CastToTy>(a),
                                 static_cast<CastToTy2>(b));
        };

        return return_error(
            CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method,
                            val, val2)
                            );
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
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        if( !pval){
            return HANDLE_ERROR(tdma::ValueException,
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
struct ImplAccessor<char**>{
    template<typename ImplTy, typename R=void>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         R(ImplTy::*method)(const std::string&),
         const char* val,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap =
            +[](void* obj, R(ImplTy::*meth)(const std::string&), const char* v){
            return (reinterpret_cast<ImplTy*>(obj)->*meth)(v ? v : "");
        };

        return return_error(
            CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method, val)
            );
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::string(ImplTy::*method)(void) const,
         char **pval,
         size_t *n,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        CHECK_PTR(pval, "buf", allow_exceptions);
        CHECK_PTR(n, "n", allow_exceptions);

        static auto mwrap =
            +[](void* obj, std::string(ImplTy::*meth)(void) const){
                return (reinterpret_cast<ImplTy*>(obj)->*meth)();
            };

        std::string r;
        std::tie(r,err) = CallImplFromABI(allow_exceptions, mwrap, pgetter->obj,
                                          method);
        if( err )
            return err;

        return to_new_char_buffer(r, pval, n, allow_exceptions);
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::string(ImplTy::*method)(void),
         char **pval,
         size_t *n,
         int allow_exceptions )
    {
        return get<ImplTy>(
            pgetter,
            reinterpret_cast<std::string(ImplTy::*)(void) const>(method),
            pval, n, allow_exceptions
            );
    }
};


template<> /* FOR ARRAY OF C STR TYPES */
struct ImplAccessor<char***>{
    template<typename ImplTy, typename R=void>
    static int
    set( typename ImplTy::ProxyType::CType* pgetter,
         R(ImplTy::*method)(const std::set<std::string>&),
         const char** val,
         size_t n,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        static auto mwrap =
            +[]( void* obj, R(ImplTy::*meth)(const std::set<std::string>&),
                 const char** s, size_t n ){
                    std::set<std::string> strs;
                    while( n-- )
                        strs.insert(s[n]);
                    return (reinterpret_cast<ImplTy*>(obj)->*meth)(strs);
                };

        return return_error(
            CallImplFromABI(allow_exceptions, mwrap, pgetter->obj, method,
                            val, n)
                            );
    }

    template<typename ImplTy>
    static int
    get( typename ImplTy::ProxyType::CType* pgetter,
         std::set<std::string>(ImplTy::*method)(void) const,
         char ***pval,
         size_t *n,
         int allow_exceptions )
    {
        int err = proxy_is_callable<ImplTy>(pgetter, allow_exceptions);
        if( err )
            return err;

        CHECK_PTR(pval, "buf", allow_exceptions);
        CHECK_PTR(n, "n", allow_exceptions);

        static auto mwrap =
            +[](void* obj, std::set<std::string>(ImplTy::*meth)(void) const){
                return (reinterpret_cast<ImplTy*>(obj)->*meth)();
            };

        std::set<std::string> strs;
        std::tie(strs,err) = CallImplFromABI(allow_exceptions, mwrap,
                                             pgetter->obj, method);
        if( err )
            return err;

        return to_new_char_buffers(strs, pval, n, allow_exceptions);
    }
};

} /* tdma */

#define TDMA_API_TO_STRING_ABI_ARGS \
    int v, \
    char** buf, \
    size_t* n, \
    int allow_exceptions

#endif // TDMA_API_H_
