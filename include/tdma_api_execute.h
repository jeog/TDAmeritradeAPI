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

#ifndef TDMA_API_EXECUTE_H
#define TDMA_API_EXECUTE_H

#include "_common.h"
#include "tdma_common.h"

#ifdef __cplusplus

#include <regex>
#include "json.hpp"

using json = nlohmann::json;
#endif /* __cplusplus */

// TODO check Builders for correct input
//      - positive price and quantity
//      - valid option input

DECL_C_CPP_TDMA_ENUM(OrderSession, 1, 4,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderSession, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderSession, NORMAL),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderSession, AM),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderSession, PM),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderSession, SEAMLESS)
    );

DECL_C_CPP_TDMA_ENUM(OrderDuration, 1, 3,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderDuration, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderDuration, DAY),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderDuration, GOOD_TILL_CANCEL),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderDuration, FILL_OR_KILL)
    );

DECL_C_CPP_TDMA_ENUM(OrderAssetType, 1, 7,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, EQUITY),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, OPTION),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, INDEX),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, MUTUAL_FUND),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, CASH_EQUIVALENT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, FIXED_INCOME),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderAssetType, CURRENCY)
    );

DECL_C_CPP_TDMA_ENUM(OrderInstruction, 1, 9,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, BUY),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, SELL),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, BUY_TO_COVER),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, SELL_SHORT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, BUY_TO_OPEN),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, BUY_TO_CLOSE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, SELL_TO_OPEN),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, SELL_TO_CLOSE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderInstruction, EXCHANGE)
    );

DECL_C_CPP_TDMA_ENUM(OrderType, 1, 11,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, MARKET),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, LIMIT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, STOP),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, STOP_LIMIT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, TRAILING_STOP),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, MARKET_ON_CLOSE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, EXERCISE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, TRAILING_STOP_LIMIT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, NET_DEBIT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, NET_CREDIT),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderType, NET_ZERO)
    );

DECL_C_CPP_TDMA_ENUM(ComplexOrderStrategyType, 0, 19,
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, NONE),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, COVERED),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, VERTICAL),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, BACK_RATIO),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, CALENDAR),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, DIAGONAL),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, STRADDLE),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, STRANGLE),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, COLLAR_SYNTHETIC),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, BUTTERFLY),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, CONDOR),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, IRON_CONDOR),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, VERTICAL_ROLL),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, COLLAR_WITH_STOCK),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, DOUBLE_DIAGONAL),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, UNBALANCED_BUTTERFLY),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, UNBALANCED_CONDOR),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, UNBALANCED_IRON_CONDOR),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, UNBALANCED_VERTICAL_ROLL),
    BUILD_C_CPP_TDMA_ENUM_NAME(ComplexOrderStrategyType, CUSTOM)
    );

DECL_C_CPP_TDMA_ENUM(OrderStrategyType, 0, 2,
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStrategyType, SINGLE),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStrategyType, OCO),
    BUILD_C_CPP_TDMA_ENUM_NAME(OrderStrategyType, TRIGGER)
    );

#define THROW_VALUE_EXCEPTION(m) throw ValueException(m, __LINE__, __FILE__)

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_Create_ABI(  int asset_type,
                       const char* symbol,
                       int instruction,
                       size_t quantity,
                       OrderLeg_C *pleg,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_Destroy_ABI( OrderLeg_C *pleg, int allow_exceptions );

/*
 * COPY CONSTRUCT
 * NOTE - if 'from' is invalid this fails silently (to support C++ copy/assign)
 *        check 'to->obj' manually for non-null
 */
EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_Copy_ABI( OrderLeg_C *from, OrderLeg_C *to, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_IsSame_ABI( OrderLeg_C* pl,
                     OrderLeg_C* pr,
                     int *is_same,
                     int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_GetAssetType_ABI( OrderLeg_C* instrument,
                           int *asset_type,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_GetSymbol_ABI( OrderLeg_C* instrument,
                        char **buf,
                        size_t *n,
                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_GetInstruction_ABI( OrderLeg_C* pleg,
                             int *instruction,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_GetQuantity_ABI( OrderLeg_C* pleg,
                          size_t *quantity,
                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderLeg_AsJsonString_ABI( OrderLeg_C* pleg,
                           char **buf,
                           size_t *n,
                           int allow_exceptions );

#ifndef __cplusplus
/* C interface */

static inline int
OrderLeg_Create( OrderAssetType asset_type,
                 const char* symbol,
                 OrderInstruction instruction,
                 size_t quantity,
                 OrderLeg_C *pleg )
{ return OrderLeg_Create_ABI( (int)asset_type, symbol, (int)instruction,
                              quantity, pleg, 0); }

static inline int
OrderLeg_Destroy( OrderLeg_C *pleg )
{ return OrderLeg_Destroy_ABI( pleg, 0 ); }

/*
 * COPY CONSTRUCT
 * NOTE - if 'from' is invalid this fails silently (to support C++ copy/assign)
 *        check 'to->obj' manually for non-null
 */
static inline int
OrderLeg_Copy( OrderLeg_C *from, OrderLeg_C *to )
{ return OrderLeg_Copy_ABI(from, to, 0); }

static inline int
OrderLeg_IsSame( OrderLeg_C* pl, OrderLeg_C* pr, int *is_same )
{ return OrderLeg_IsSame_ABI( pl, pr, is_same, 0 ); }

static inline int
OrderLeg_GetAssetType( OrderLeg_C* pleg, OrderAssetType *asset_type )
{ return OrderLeg_GetAssetType_ABI( pleg, (int*)asset_type, 0 ); }

static inline int
OrderLeg_GetSymbol( OrderLeg_C* pleg, char **buf, size_t *n )
{ return OrderLeg_GetSymbol_ABI( pleg, buf, n, 0); }

static inline int
OrderLeg_GetInstruction( OrderLeg_C* pleg, OrderInstruction *instruction )
{ return OrderLeg_GetInstruction_ABI( pleg, (int*)instruction, 0 ); }

static inline int
OrderLeg_GetQuantity( OrderLeg_C* pleg, size_t *quantity )
{ return OrderLeg_GetQuantity_ABI( pleg, quantity, 0 ); }

static inline int
OrderLeg_AsJsonString( OrderLeg_C* pleg, char **buf, size_t *n )
{ return OrderLeg_AsJsonString_ABI( pleg, buf, n, 0 ); }

#else
/* C++ interface */

namespace tdma {

// NON-VIRTUAL destructor
template< typename CTy,
          int(*destroy_func)(CTy*, int),
          int(*copy_func)(CTy*, CTy*, int),
          int(*cmp_func)(CTy*, CTy*, int*, int),
          int(*json_func)(CTy*, char**, size_t*, int)>
class OrderObjectProxy {
    std::unique_ptr<CTy, CProxyDestroyer<CTy>> _cproxy;

public:
    typedef CTy CType;

    OrderObjectProxy()
        : _cproxy( {new CTy{0,0}, destroy_func} )
        {}

    OrderObjectProxy( const OrderObjectProxy& ob )
        :
            _cproxy( {new CTy{0,0}, destroy_func} )
        {
            call_abi( copy_func, ob.get_cproxy(), get_cproxy() );
        }

    OrderObjectProxy( OrderObjectProxy&& ob)
        : _cproxy( std::move(ob._cproxy) )
        {}

    OrderObjectProxy( CType& ob )
        :
            _cproxy( {new CTy{0,0}, destroy_func} )
        {
            call_abi( copy_func, &ob, get_cproxy() );
        }

    OrderObjectProxy( CType&& ob )
        :
            _cproxy( {new CTy{ob.obj, ob.type_id}, destroy_func} )
        {
            ob.obj = nullptr;
            ob.type_id = 0;
        }

    OrderObjectProxy&
    operator=( const OrderObjectProxy& ob )
    {
        if( *this != ob ){
            _cproxy.reset( new CType{0,0} );
            call_abi( copy_func, ob.get_cproxy(), get_cproxy() );
        }
        return *this;
    }

    OrderObjectProxy&
    operator=( OrderObjectProxy&& ob)
    {
        if( *this != ob )
            _cproxy = std::move(ob._cproxy);
        return *this;
    }

    bool
    operator==(const OrderObjectProxy& other) const
    {
        int b;
        call_abi( cmp_func, get_cproxy(), other.get_cproxy(), &b );
        return static_cast<bool>(b);
    }

    bool
    operator!=(const OrderObjectProxy& other) const
    { return !(*this == other); }

    CType
    release_cproxy()
    {
        CType *tmp = _cproxy.release();
        CType copy = *tmp;
        delete tmp;
        return copy;
    }

    CType*
    get_cproxy() const
    { return _cproxy.get(); }

    json
    as_json() const
    { return json::parse( str_from_abi(json_func, get_cproxy()) ); }

};

// NON-VIRTUAL destructor
class OrderLeg
    : public OrderObjectProxy< OrderLeg_C,
                               OrderLeg_Destroy_ABI,
                               OrderLeg_Copy_ABI,
                               OrderLeg_IsSame_ABI,
                               OrderLeg_AsJsonString_ABI >{
public:
    using OrderObjectProxy::OrderObjectProxy;

    OrderLeg( OrderAssetType asset_type,
               std::string symbol,
               OrderInstruction instruction,
               size_t quantity )
        :
            OrderObjectProxy()
        {
            call_abi( OrderLeg_Create_ABI, static_cast<int>(asset_type),
                      symbol.c_str(), static_cast<int>(instruction), quantity,
                      get_cproxy() );
        }

    OrderAssetType
    get_asset_type() const
    {
        int a;
        call_abi( OrderLeg_GetAssetType_ABI, get_cproxy(), &a );
        return static_cast<OrderAssetType>(a);
    }

    std::string
    get_symbol() const
    { return str_from_abi(OrderLeg_GetSymbol_ABI, get_cproxy()); }

    OrderInstruction
    get_instruction() const
    {
        int oi;
        call_abi( OrderLeg_GetInstruction_ABI, get_cproxy(), &oi );
        return static_cast<OrderInstruction>(oi);
    }

    size_t
    get_quantity() const
    {
        size_t q;
        call_abi( OrderLeg_GetQuantity_ABI, get_cproxy(), &q );
        return q;
    }
};

} /* tdma */

#endif /* __cplusplus */

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_Create_ABI( OrderTicket_C *porder, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_Destroy_ABI( OrderTicket_C *porder, int allow_exceptions );

/*
 * COPY CONSTRUCT
 * NOTE - if 'from' is invalid this fails silently (to support C++ copy/assign)
 *        check 'to->obj' manually for non-null
 */
EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_Copy_ABI( OrderTicket_C *from,
                      OrderTicket_C *to,
                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_IsSame_ABI( OrderTicket_C* pl,
                        OrderTicket_C* pr,
                        int *is_same,
                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetSession_ABI( OrderTicket_C *porder,
                            int *session,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetSession_ABI( OrderTicket_C *porder,
                            int session,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetDuration_ABI( OrderTicket_C *porder,
                             int *duration,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetDuration_ABI( OrderTicket_C *porder,
                             int duration,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetCancelTime_ABI( OrderTicket_C *porder,
                               char **buf,
                               size_t* n,
                               int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetCancelTime_ABI( OrderTicket_C *porder,
                               const char* cancel_time,
                               int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetType_ABI( OrderTicket_C *porder,
                         int *order_type,
                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetType_ABI( OrderTicket_C *porder,
                         int order_type,
                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetComplexStrategyType_ABI( OrderTicket_C *porder,
                                        int *complex_strategy_type,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetComplexStrategyType_ABI( OrderTicket_C *porder,
                                        int complex_strategy_type,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetStrategyType_ABI( OrderTicket_C *porder,
                                 int *strategy_type,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetStrategyType_ABI( OrderTicket_C *porder,
                                 int strategy_type,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetPrice_ABI( OrderTicket_C *porder,
                          double *price,
                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetPrice_ABI( OrderTicket_C *porder,
                          double price,
                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetStopPrice_ABI( OrderTicket_C *porder,
                              double *stop_price,
                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_SetStopPrice_ABI( OrderTicket_C *porder,
                              double stop_price,
                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetLegs_ABI( OrderTicket_C *porder,
                         OrderLeg_C** plegs,
                         size_t* n,
                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetLeg_ABI( OrderTicket_C *porder,
                        size_t pos,
                        OrderLeg_C* pleg,
                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_AddLegs_ABI( OrderTicket_C *porder,
                         OrderLeg_C* plegs,
                         size_t n,
                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_RemoveLeg_ABI( OrderTicket_C *porder,
                           size_t pos,
                           int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_ReplaceLeg_ABI( OrderTicket_C *porder,
                            size_t pos,
                            OrderLeg_C* pleg,
                            int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_ClearLegs_ABI( OrderTicket_C *porder, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_GetChildren_ABI( OrderTicket_C *porder,
                             OrderTicket_C** pchildren,
                             size_t* n,
                             int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_AddChild_ABI( OrderTicket_C *porder,
                          OrderTicket_C* pchild,
                          int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_ClearChildren_ABI( OrderTicket_C *porder, int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
OrderTicket_AsJsonString_ABI( OrderTicket_C* porder,
                              char **buf,
                              size_t *n,
                              int allow_exceptions );

#ifndef __cplusplus
/* C interface */

static inline int
OrderTicket_Create( OrderTicket_C *porder )
{ return OrderTicket_Create_ABI( porder, 0 ); }

static inline int
OrderTicket_Destroy( OrderTicket_C *porder)
{ return OrderTicket_Destroy_ABI( porder, 0 ); }

/*
 * COPY CONSTRUCT
 * NOTE - if 'from' is invalid this fails silently (to support C++ copy/assign)
 *        check 'to->obj' manually for non-null
 */
static inline int
OrderTicket_Copy( OrderTicket_C *from, OrderTicket_C *to )
{ return OrderTicket_Copy_ABI( from, to, 0 ); }

static inline int
OrderTicket_IsSame( OrderTicket_C* pl, OrderTicket_C* pr, int *is_same)
{ return OrderTicket_IsSame_ABI( pl, pr, is_same, 0 ); }

static inline int
OrderTicket_GetSession( OrderTicket_C *porder, OrderSession *session)
{ return OrderTicket_GetSession_ABI( porder, (int*)session, 0 ); }

static inline int
OrderTicket_SetSession( OrderTicket_C *porder, OrderSession session)
{ return OrderTicket_SetSession_ABI( porder, (int)session, 0 ); }

static inline int
OrderTicket_GetDuration( OrderTicket_C *porder, OrderDuration *duration)
{ return OrderTicket_GetDuration_ABI( porder, (int*)duration, 0 ); }

static inline int
OrderTicket_SetDuration( OrderTicket_C *porder, OrderDuration duration)
{ return OrderTicket_SetDuration_ABI( porder, (int)duration, 0 ); }

static inline int
OrderTicket_GetCancelTime( OrderTicket_C *porder, char **buf, size_t* n)
{ return OrderTicket_GetCancelTime_ABI( porder, buf, n, 0 ); }

static inline int
OrderTicket_SetCancelTime( OrderTicket_C *porder, const char* cancel_time)
{ return OrderTicket_SetCancelTime_ABI( porder, cancel_time, 0 ); }

static inline int
OrderTicket_GetType( OrderTicket_C *porder, OrderType *order_type)
{ return OrderTicket_GetType_ABI( porder, (int*)order_type, 0 ); }

static inline int
OrderTicket_SetType( OrderTicket_C *porder, OrderType order_type)
{ return OrderTicket_SetType_ABI( porder, (int)order_type, 0 ); }

static inline int
OrderTicket_GetComplexStrategyType(
    OrderTicket_C *porder,
    ComplexOrderStrategyType *complex_strategy_type
    )
{
    return OrderTicket_GetComplexStrategyType_ABI(
        porder, (int*)complex_strategy_type, 0
        );
}

static inline int
OrderTicket_SetComplexStrategyType(
    OrderTicket_C *porder,
    ComplexOrderStrategyType complex_strategy_type
    )
{
    return OrderTicket_SetComplexStrategyType_ABI(
        porder, (int)complex_strategy_type, 0
        );
}

static inline int
OrderTicket_GetStrategyType( OrderTicket_C *porder,
                             OrderStrategyType *strategy_type )
{ return OrderTicket_GetStrategyType_ABI( porder, (int*)strategy_type, 0 ); }

static inline int
OrderTicket_SetStrategyType( OrderTicket_C *porder,
                             OrderStrategyType strategy_type )
{ return OrderTicket_SetStrategyType_ABI( porder, (int)strategy_type, 0 ); }

static inline int
OrderTicket_GetPrice( OrderTicket_C *porder, double *price)
{ return OrderTicket_GetPrice_ABI( porder, price, 0 ); }

static inline int
OrderTicket_SetPrice( OrderTicket_C *porder, double price)
{ return OrderTicket_SetPrice_ABI( porder, price, 0 ); }

static inline int
OrderTicket_GetStopPrice( OrderTicket_C *porder, double *stop_price)
{ return OrderTicket_GetStopPrice_ABI( porder, stop_price, 0 ); }

static inline int
OrderTicket_SetStopPrice( OrderTicket_C *porder, double stop_price)
{ return OrderTicket_SetStopPrice_ABI( porder, stop_price, 0 ); }

static inline int
OrderTicket_GetLegs( OrderTicket_C *porder, OrderLeg_C** plegs, size_t* n )
{ return OrderTicket_GetLegs_ABI( porder, plegs, n, 0 ); }

static inline int
OrderTicket_GetLeg( OrderTicket_C *porder, size_t pos, OrderLeg_C* pleg )
{ return OrderTicket_GetLeg_ABI( porder, pos, pleg, 0 ); }

static inline int
OrderTicket_AddLegs( OrderTicket_C *porder, OrderLeg_C* plegs, size_t n )
{ return OrderTicket_AddLegs_ABI( porder, plegs, n, 0 ); }

static inline int
OrderTicket_RemoveLeg( OrderTicket_C *porder, size_t pos)
{ return OrderTicket_RemoveLeg_ABI( porder, pos, 0 ); }

static inline int
OrderTicket_ReplaceLeg( OrderTicket_C *porder, size_t pos,  OrderLeg_C* pleg )
{ return OrderTicket_ReplaceLeg_ABI( porder, pos, pleg, 0 ); }

static inline int
OrderTicket_ClearLegs( OrderTicket_C *porder)
{ return OrderTicket_ClearLegs_ABI( porder, 0 ); }

static inline int
OrderTicket_GetChildren( OrderTicket_C *porder,
                         OrderTicket_C** pchildren,
                         size_t* n  )
{ return OrderTicket_GetChildren_ABI( porder, pchildren, n, 0 ); }

static inline int
OrderTicket_AddChild( OrderTicket_C *porder, OrderTicket_C* pchild)
{ return OrderTicket_AddChild_ABI( porder, pchild, 0 ); }

// TODO Remove/ReplaceChild

static inline int
OrderTicket_ClearChildren( OrderTicket_C *porder)
{ return OrderTicket_ClearChildren_ABI( porder, 0 ); }

static inline int
OrderTicket_AsJsonString( OrderTicket_C* porder, char **buf, size_t *n)
{ return OrderTicket_AsJsonString_ABI( porder, buf, n, 0 ); }


#else
/* C++ interface */

namespace tdma{

// NON-VIRTUAL destructor
class OrderTicket
    : public OrderObjectProxy< OrderTicket_C,
                               OrderTicket_Destroy_ABI,
                               OrderTicket_Copy_ABI,
                               OrderTicket_IsSame_ABI,
                               OrderTicket_AsJsonString_ABI > {
public:
    typedef OrderTicket_C CType;
    using OrderObjectProxy::OrderObjectProxy;

    OrderTicket( )
        :
            OrderObjectProxy()
        {
            call_abi( OrderTicket_Create_ABI, get_cproxy() );
        }

    OrderSession
    get_session() const
    {
        int v;
        call_abi( OrderTicket_GetSession_ABI, get_cproxy(), &v );
        return static_cast<OrderSession>(v);
    }

    OrderTicket&
    set_session(OrderSession session)
    {
        call_abi( OrderTicket_SetSession_ABI, get_cproxy(),
                  static_cast<int>(session) );
        return *this;
    }

    OrderDuration
    get_duration() const
    {
        int v;
        call_abi( OrderTicket_GetDuration_ABI, get_cproxy(), &v );
        return static_cast<OrderDuration>(v);
    }

    OrderTicket&
    set_duration(OrderDuration duration)
    {
        call_abi( OrderTicket_SetDuration_ABI, get_cproxy(),
                  static_cast<int>(duration) );
        return *this;
    }

    std::string
    get_cancel_time() const
    { return str_from_abi(OrderTicket_GetCancelTime_ABI, get_cproxy()); }

    OrderTicket&
    set_cancel_time(const std::string& cancel_time)
    {
        call_abi( OrderTicket_SetCancelTime_ABI, get_cproxy(),
                  cancel_time.c_str() );
        return *this;
    }

    OrderType
    get_type() const
    {
        int v;
        call_abi( OrderTicket_GetType_ABI, get_cproxy(), &v );
        return static_cast<OrderType>(v);
    }

    OrderTicket&
    set_type(OrderType order_type)
    {
        call_abi( OrderTicket_SetType_ABI, get_cproxy(),
                   static_cast<int>(order_type) );
        return *this;
    }

    ComplexOrderStrategyType
    get_complex_strategy_type() const
    {
        int v;
        call_abi( OrderTicket_GetComplexStrategyType_ABI, get_cproxy(), &v );
        return static_cast<ComplexOrderStrategyType>(v);
    }

    OrderTicket&
    set_complex_strategy_type(ComplexOrderStrategyType complex_strategy_type)
    {
        call_abi( OrderTicket_SetComplexStrategyType_ABI, get_cproxy(),
                  static_cast<int>(complex_strategy_type) );
        return *this;
    }

    OrderStrategyType
    get_strategy_type() const
    {
        int v;
        call_abi( OrderTicket_GetStrategyType_ABI, get_cproxy(), &v );
        return static_cast<OrderStrategyType>(v);
    }

    OrderTicket&
    set_strategy_type(OrderStrategyType strategy_type)
    {
        call_abi( OrderTicket_SetStrategyType_ABI, get_cproxy(),
                  static_cast<int>(strategy_type) );
        return *this;
    }

    double
    get_price() const
    {
        double p;
        call_abi( OrderTicket_GetPrice_ABI, get_cproxy(), &p );
        return p;
    }

    OrderTicket&
    set_price(double price)
    {
        call_abi( OrderTicket_SetPrice_ABI, get_cproxy(), price );
        return *this;
    }

    double
    get_stop_price() const
    {
        double p;
        call_abi( OrderTicket_GetStopPrice_ABI, get_cproxy(), &p );
        return p;
    }

    OrderTicket&
    set_stop_price(double stop_price)
    {
        call_abi( OrderTicket_SetStopPrice_ABI, get_cproxy(), stop_price );
        return *this;
    }

    std::vector<OrderLeg>
    get_legs() const
    {
        OrderLeg_C *lbuf;
        size_t n;
        call_abi( OrderTicket_GetLegs_ABI, get_cproxy(), &lbuf, &n );
        std::vector<OrderLeg> legs;
        for(size_t i = 0; i < n; ++i){
            legs.emplace_back( std::move(lbuf[i]) );
        }
        call_abi( FreeOrderLegBuffer_ABI, lbuf );
        return legs;
    }

    OrderLeg
    get_leg(size_t pos) const
    {
        OrderLeg_C leg;
        call_abi( OrderTicket_GetLeg_ABI, get_cproxy(), pos, &leg );
        return std::move(leg);
    }

    OrderTicket&
    add_leg(const OrderLeg& leg)
    {
        call_abi( OrderTicket_AddLegs_ABI, get_cproxy(), leg.get_cproxy(), 1 );
        return *this;
    }

    OrderTicket&
    add_legs(const std::vector<OrderLeg>& legs)
    {
        if( !legs.empty() ){
            std::vector<OrderLeg_C> plegs;
            for(auto& l : legs){
                plegs.push_back( *l.get_cproxy() );
            }
            call_abi( OrderTicket_AddLegs_ABI, get_cproxy(), &plegs[0],
                      legs.size() );
        }
        return *this;
    }

    OrderTicket&
    remove_leg(size_t pos)
    {
        call_abi( OrderTicket_RemoveLeg_ABI, get_cproxy(), pos );
        return *this;
    }

    OrderTicket&
    replace_leg(size_t pos, const OrderLeg& leg)
    {
        call_abi( OrderTicket_ReplaceLeg_ABI, get_cproxy(), pos,
                  leg.get_cproxy() );
        return *this;
    }

    OrderTicket&
    clear_legs()
    {
        call_abi( OrderTicket_ClearLegs_ABI, get_cproxy() );
        return *this;
    }

    std::vector<OrderTicket>
    get_children() const
    {
        OrderTicket_C *cbuf;
        size_t n;
        call_abi( OrderTicket_GetChildren_ABI, get_cproxy(), &cbuf, &n );
        std::vector<OrderTicket> kids(n);
        for(size_t i = 0; i < n; ++i){
            kids[i] = std::move(cbuf[i]);
        }
        call_abi( FreeOrderTicketBuffer_ABI, cbuf );
        return kids;
    }

    OrderTicket&
    add_child(const OrderTicket& child)
    {
        call_abi( OrderTicket_AddChild_ABI, get_cproxy(), child.get_cproxy() );
        return *this;
    }

    OrderTicket&
    clear_children()
    {
        call_abi( OrderTicket_ClearChildren_ABI, get_cproxy() );
        return *this;
    }

};

} /* tdma */

#endif /* __cplusplus */


EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Simple_ABI( int order_type,
                       int asset_type,
                       const char* symbol,
                       int instruction,
                       size_t quantity,
                       double limit_price,
                       double stop_price,
                       OrderTicket_C *porder,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Equity_ABI( int order_type,
                       const char* symbol,
                       int instruction,
                       size_t quantity,
                       double limit_price,
                       double stop_price,
                       OrderTicket_C *porder,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Option_ABI( int order_type,
                       const char* symbol,
                       size_t quantity,
                       int is_buy,
                       int to_open,
                       double price,
                       OrderTicket_C *porder,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_OptionEx_ABI( int order_type,
                         const char* underlying,
                         unsigned int month,
                         unsigned int day,
                         unsigned int year,
                         int is_call,
                         double strike,
                         size_t quantity,
                         int is_buy,
                         int to_open,
                         double price,
                         OrderTicket_C *porder,
                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_ABI( int complex_strategy_type,
                       OrderLeg_C *plegs,
                       size_t n,
                       int is_market,
                       double price,
                       OrderTicket_C *porder,
                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Vertical_ABI( const char* symbol_buy,
                                const char* symbol_sell,
                                size_t quantity,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_VerticalEx_ABI( const char* underlying,
                                  unsigned int month,
                                  unsigned int day,
                                  unsigned int year,
                                  int are_calls,
                                  double strike_buy,
                                  double strike_sell,
                                  size_t quantity,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );


EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_VerticalRoll_ABI( const char* symbol_close_buy,
                                    const char* symbol_close_sell,
                                    const char* symbol_open_buy,
                                    const char* symbol_open_sell,
                                    size_t quantity_close,
                                    size_t quantity_open,
                                    int is_market,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_VerticalRollEx_ABI( const char* underlying,
                                      unsigned int month_close,
                                      unsigned int day_close,
                                      unsigned int year_close,
                                      unsigned int month_open,
                                      unsigned int day_open,
                                      unsigned int year_open,
                                      int are_calls,
                                      double strike_close_buy,
                                      double strike_close_sell,
                                      double strike_open_buy,
                                      double strike_open_sell,
                                      size_t quantity_close,
                                      size_t quantity_open,
                                      int is_market,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Butterfly_ABI( const char* symbol_outer1,
                                 const char* symbol_inner1,
                                 const char* symbol_outer2,
                                 size_t quantity_outer1,
                                 size_t quantity_outer2,
                                 int is_buy,
                                 int to_open,
                                 int is_market,
                                 double price,
                                 OrderTicket_C *porder,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_ButterflyEx_ABI( const char* underlying,
                                   unsigned int month,
                                   unsigned int day,
                                   unsigned int year,
                                   int are_calls,
                                   double strike_outer1,
                                   double strike_inner1,
                                   double strike_outer2,
                                   size_t quantity_outer1,
                                   size_t quantity_outer2,
                                   int is_buy,
                                   int to_open,
                                   int is_market,
                                   double price,
                                   OrderTicket_C *porder,
                                   int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_BackRatio_ABI( const char* symbol_buy,
                                 const char* symbol_sell,
                                 size_t quantity_buy,
                                 size_t quantity_sell,
                                 int to_open,
                                 int is_market,
                                 double price,
                                 OrderTicket_C *porder,
                                 int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_BackRatioEx_ABI( const char* underlying,
                                   unsigned int month,
                                   unsigned int day,
                                   unsigned int year,
                                   int are_calls,
                                   double strike_buy,
                                   double strike_sell,
                                   size_t quantity_buy,
                                   size_t quantity_sell,
                                   int to_open,
                                   int is_market,
                                   double price,
                                   OrderTicket_C *porder,
                                   int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Calendar_ABI( const char* symbol_buy,
                                const char* symbol_sell,
                                size_t quantity,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CalendarEx_ABI( const char* underlying,
                                  unsigned int month_buy,
                                  unsigned int day_buy,
                                  unsigned int year_buy,
                                  unsigned int month_sell,
                                  unsigned int day_sell,
                                  unsigned int year_sell,
                                  int are_calls,
                                  double strike,
                                  size_t quantity,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Diagonal_ABI( const char* symbol_buy,
                                const char* symbol_sell,
                                size_t quantity,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_DiagonalEx_ABI( const char* underlying,
                                  unsigned int month_buy,
                                  unsigned int day_buy,
                                  unsigned int year_buy,
                                  unsigned int month_sell,
                                  unsigned int day_sell,
                                  unsigned int year_sell,
                                  int are_calls,
                                  double strike_buy,
                                  double strike_sell,
                                  size_t quantity,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Straddle_ABI( const char* symbol_call,
                                const char* symbol_put,
                                size_t quantity,
                                int is_buy,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_StraddleEx_ABI( const char* underlying,
                                  unsigned int month,
                                  unsigned int day,
                                  unsigned int year,
                                  double strike,
                                  size_t quantity,
                                  int is_buy,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Strangle_ABI( const char* symbol_call,
                                const char* symbol_put,
                                size_t quantity,
                                int is_buy,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_StrangleEx_ABI( const char* underlying,
                                  unsigned int month,
                                  unsigned int day,
                                  unsigned int year,
                                  double strike_call,
                                  double strike_put,
                                  size_t quantity,
                                  int is_buy,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CollarSynthetic_ABI( const char* symbol_buy,
                                       const char* symbol_sell,
                                       size_t quantity,
                                       int to_open,
                                       int is_market,
                                       double price,
                                       OrderTicket_C *porder,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CollarSyntheticEx_ABI( const char* underlying,
                                         unsigned int month,
                                         unsigned int day,
                                         unsigned int year,
                                         double strike_call,
                                         double strike_put,
                                         size_t quantity,
                                         int is_buy,
                                         int to_open,
                                         int is_market,
                                         double price,
                                         OrderTicket_C *porder,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CollarWithStock_ABI( const char* symbol_buy,
                                       const char* symbol_sell,
                                       const char* symbol_stock,
                                       size_t quantity,
                                       int is_buy,
                                       int to_open,
                                       int is_market,
                                       double price,
                                       OrderTicket_C *porder,
                                       int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CollarWithStockEx_ABI( const char* underlying,
                                         unsigned int month,
                                         unsigned int day,
                                         unsigned int year,
                                         double strike_call,
                                         double strike_put,
                                         size_t quantity,
                                         int is_buy,
                                         int to_open,
                                         int is_market,
                                         double price,
                                         OrderTicket_C *porder,
                                         int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_Condor_ABI( const char* symbol_outer1,
                              const char* symbol_inner1,
                              const char* symbol_inner2,
                              const char* symbol_outer2,
                              size_t quantity1,
                              size_t quantity2,
                              int is_buy,
                              int to_open,
                              int is_market,
                              double price,
                              OrderTicket_C *porder,
                              int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_CondorEx_ABI( const char* underlying,
                                unsigned int month,
                                unsigned int day,
                                unsigned int year,
                                double strike_outer1,
                                double strike_inner1,
                                double strike_inner2,
                                double strike_outer2,
                                int are_calls,
                                size_t quantity1,
                                size_t quantity2,
                                int is_buy,
                                int to_open,
                                int is_market,
                                double price,
                                OrderTicket_C *porder,
                                int allow_exceptions );


EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_IronCondor_ABI( const char* symbol_call_buy,
                                  const char* symbol_call_sell,
                                  const char* symbol_put_buy,
                                  const char* symbol_put_sell,
                                  size_t quantity_call,
                                  size_t quantity_put,
                                  int to_open,
                                  int is_market,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_IronCondorEx_ABI( const char* underlying,
                                    unsigned int month,
                                    unsigned int day,
                                    unsigned int year,
                                    double strike_call_buy,
                                    double strike_call_sell,
                                    double strike_put_buy,
                                    double strike_put_sell,
                                    size_t quantity_call,
                                    size_t quantity_put,
                                    int to_open,
                                    int is_market,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_DoubleDiagonal_ABI( const char* symbol_call_buy,
                                      const char* symbol_call_sell,
                                      const char* symbol_put_buy,
                                      const char* symbol_put_sell,
                                      size_t quantity,
                                      int to_open,
                                      int is_market,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_DoubleDiagonalEx_ABI( const char* underlying,
                                        unsigned int month_buy,
                                        unsigned int day_buy,
                                        unsigned int year_buy,
                                        unsigned int month_sell,
                                        unsigned int day_sell,
                                        unsigned int year_sell,
                                        double strike_call_buy,
                                        double strike_call_sell,
                                        double strike_put_buy,
                                        double strike_put_sell,
                                        size_t quantity,
                                        int to_open,
                                        int is_market,
                                        double price,
                                        OrderTicket_C *porder,
                                        int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_OneCancelsOther_ABI( OrderTicket_C *porder1,
                                OrderTicket_C *porder2,
                                OrderTicket_C *porder,
                                int allow_exceptions );

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_OneTriggersOther_ABI( OrderTicket_C *porder_primary,
                                 OrderTicket_C *porder_conditional,
                                 OrderTicket_C *porder,
                                 int allow_exceptions );

#ifndef __cplusplus

static inline int
BuildOrder_Simple( OrderType order_type,
                  OrderAssetType asset_type,
                  const char* symbol,
                  OrderInstruction instruction,
                  size_t quantity,
                  double limit_price,
                  double stop_price,
                  OrderTicket_C *porder )
{ return BuildOrder_Simple_ABI( (int)order_type, (int)asset_type, symbol,
                               (int)instruction, quantity, limit_price,
                               stop_price, porder, 0); }

static inline int
BuildOrder_Equity( OrderType order_type,
                  const char* symbol,
                  OrderInstruction instruction,
                  size_t quantity,
                  double limit_price,
                  double stop_price,
                  OrderTicket_C *porder )
{ return BuildOrder_Equity_ABI( (int)order_type, symbol, (int)instruction,
                                quantity, limit_price, stop_price, porder, 0); }

static inline int
BuildOrder_Equity_Market_Buy( const char* symbol,
                              size_t quantity,
                              OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_MARKET, symbol, OrderInstruction_BUY,
                            quantity, 0., 0., porder ); }

static inline int
BuildOrder_Equity_Market_Sell( const char* symbol,
                               size_t quantity,
                               OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_MARKET, symbol,
                           OrderInstruction_SELL, quantity, 0., 0., porder ); }

static inline int
BuildOrder_Equity_Market_Short( const char* symbol,
                                size_t quantity,
                                OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_MARKET, symbol,
                           OrderInstruction_SELL_SHORT, quantity, 0., 0.,
                           porder ); }

static inline int
BuildOrder_Equity_Market_Cover( const char* symbol,
                                size_t quantity,
                                OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_MARKET, symbol,
                           OrderInstruction_BUY_TO_COVER, quantity, 0., 0.,
                           porder ); }

static inline int
BuildOrder_Equity_Limit_Buy( const char* symbol,
                             size_t quantity,
                             double price,
                             OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_LIMIT, symbol,
                           OrderInstruction_BUY, quantity, price, 0., porder); }

static inline int
BuildOrder_Equity_Limit_Sell( const char* symbol,
                                size_t quantity,
                                double price,
                                OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_LIMIT, symbol,
                           OrderInstruction_SELL, quantity, price, 0.,
                           porder ); }

static inline int
BuildOrder_Equity_Limit_Short( const char* symbol,
                                size_t quantity,
                                double price,
                                OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_LIMIT, symbol,
                           OrderInstruction_SELL_SHORT, quantity, price, 0.,
                           porder ); }

static inline int
BuildOrder_Equity_Limit_Cover( const char* symbol,
                                size_t quantity,
                                double price,
                                OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_LIMIT, symbol,
                           OrderInstruction_BUY_TO_COVER, quantity, price, 0.,
                           porder ); }

static inline int
BuildOrder_Equity_Stop_Buy( const char* symbol,
                             size_t quantity,
                             double price,
                             OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_STOP, symbol,
                           OrderInstruction_BUY, quantity, 0., price,  porder); }

static inline int
BuildOrder_Equity_Stop_Sell( const char* symbol,
                              size_t quantity,
                              double price,
                              OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_STOP, symbol,
                           OrderInstruction_SELL, quantity, 0., price,
                           porder ); }

static inline int
BuildOrder_Equity_Stop_Short( const char* symbol,
                              size_t quantity,
                               double price,
                               OrderTicket_C *porder)
{ return BuildOrder_Equity( OrderType_STOP, symbol,
                           OrderInstruction_SELL_SHORT, quantity, 0., price,
                           porder ); }

static inline int
BuildOrder_Equity_Stop_Cover( const char* symbol,
                              size_t quantity,
                               double price,
                               OrderTicket_C *porder)
{ return BuildOrder_Equity( OrderType_STOP, symbol,
                           OrderInstruction_BUY_TO_COVER, quantity, 0., price,
                           porder ); }

static inline int
BuildOrder_Equity_StopLimit_Buy( const char* symbol,
                                 size_t quantity,
                                 double stop_price,
                                 double limit_price,
                                 OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_STOP_LIMIT, symbol,
                           OrderInstruction_BUY, quantity, limit_price,
                           stop_price, porder); }

static inline int
BuildOrder_Equity_StopLimit_Sell( const char* symbol,
                                  size_t quantity,
                                  double stop_price,
                                  double limit_price,
                                  OrderTicket_C *porder )
{ return BuildOrder_Equity( OrderType_STOP_LIMIT, symbol,
                           OrderInstruction_SELL, quantity, limit_price,
                           stop_price, porder ); }

static inline int
BuildOrder_Equity_StopLimit_Short( const char* symbol,
                                   size_t quantity,
                                   double stop_price,
                                   double limit_price,
                                   OrderTicket_C *porder)
{ return BuildOrder_Equity( OrderType_STOP_LIMIT,symbol,
                           OrderInstruction_SELL_SHORT, quantity, limit_price,
                           stop_price, porder ); }

static inline int
BuildOrder_Equity_StopLimit_Cover( const char* symbol,
                                   size_t quantity,
                                   double stop_price,
                                   double limit_price,
                                   OrderTicket_C *porder)
{ return BuildOrder_Equity( OrderType_STOP_LIMIT, symbol,
                           OrderInstruction_BUY_TO_COVER, quantity, limit_price,
                           stop_price, porder ); }

/* OPTION */

static inline int
BuildOrder_Option_Market( const char* symbol,
                          size_t quantity,
                          int is_buy,
                          int to_open,
                          OrderTicket_C *porder )
{ return BuildOrder_Option_ABI( OrderType_MARKET, symbol, quantity, is_buy,
                                to_open, 0.0, porder, 0 ); }

static inline int
BuildOrder_Option_MarketEx( const char* underlying,
                            unsigned int month,
                            unsigned int day,
                            unsigned int year,
                            int is_call,
                            double strike,
                            size_t quantity,
                            int is_buy,
                            int to_open,
                            OrderTicket_C *porder )
{ return BuildOrder_OptionEx_ABI( OrderType_MARKET, underlying, month, day, year,
                                is_call, strike, quantity, is_buy, to_open,
                                0.0, porder, 0 ); }


static inline int
BuildOrder_Option_Limit( const char* symbol,
                         size_t quantity,
                         int is_buy,
                         int to_open,
                         double price,
                         OrderTicket_C *porder )
{ return BuildOrder_Option_ABI( OrderType_LIMIT, symbol, quantity, is_buy,
                                to_open, price, porder, 0 ); }

static inline int
BuildOrder_Option_LimitEx( const char* underlying,
                           unsigned int month,
                           unsigned int day,
                           unsigned int year,
                           int is_call,
                           double strike,
                           size_t quantity,
                           int is_buy,
                           int to_open,
                           double price,
                           OrderTicket_C *porder )
{ return BuildOrder_OptionEx_ABI( OrderType_LIMIT, underlying, month, day, year,
                                is_call, strike, quantity, is_buy, to_open,
                                price, porder, 0 ); }

/* OPTION SPREAD (MANUAL) */

static inline int
BuildOrder_Spread( ComplexOrderStrategyType complex_strategy_type,
                   OrderLeg_C *plegs,
                   size_t n,
                   int is_market,
                   double price,
                   OrderTicket_C *porder)
{ return BuildOrder_Spread_ABI( complex_strategy_type, plegs, n, is_market, price,
                           porder, 0 ); }

/* VERTICAL SPREAD */

static inline int
BuildOrder_Spread_Vertical_Market( const char* symbol_buy,
                                   const char* symbol_sell,
                                   size_t quantity,
                                   int to_open,
                                   OrderTicket_C *porder )
{ return BuildOrder_Spread_Vertical_ABI( symbol_buy, symbol_sell, quantity,
                                       to_open, 1, 0, porder, 0 ); }

static inline int
BuildOrder_Spread_Vertical_MarketEx( const char* underlying,
                                     unsigned int month,
                                     unsigned int day,
                                     unsigned int year,
                                     int are_calls,
                                     double strike_buy,
                                     double strike_sell,
                                     size_t quantity,
                                     int to_open,
                                     OrderTicket_C *porder )
{ return BuildOrder_Spread_VerticalEx_ABI( underlying, month, day, year, are_calls,
                                          strike_buy, strike_sell, quantity,
                                          to_open, 1, 0, porder, 0 ); }

static inline int
BuildOrder_Spread_Vertical_Limit( const char* symbol_buy,
                                  const char* symbol_sell,
                                  size_t quantity,
                                  int to_open,
                                  double price,
                                  OrderTicket_C *porder )
{ return BuildOrder_Spread_Vertical_ABI( symbol_buy, symbol_sell, quantity,
                                       to_open, 0, price, porder, 0 ); }

static inline int
BuildOrder_Spread_Vertical_LimitEx( const char* underlying,
                                    unsigned int month,
                                    unsigned int day,
                                    unsigned int year,
                                    int are_calls,
                                    double strike_buy,
                                    double strike_sell,
                                    size_t quantity,
                                    int to_open,
                                    double price,
                                    OrderTicket_C *porder )
{ return BuildOrder_Spread_VerticalEx_ABI( underlying, month, day, year, are_calls,
                                          strike_buy, strike_sell, quantity,
                                          to_open, 0, price, porder, 0 ); }

/* VERTICAL ROLL SPREAD */

static inline int
BuildOrder_Spread_VerticalRoll_Market(
    const char* symbol_close_buy,
    const char* symbol_close_sell,
    const char* symbol_open_buy,
    const char* symbol_open_sell,
    size_t quantity,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRoll_ABI( symbol_close_buy, symbol_close_sell,
                                             symbol_open_buy, symbol_open_sell,
                                             quantity, quantity, 1, 0, porder,
                                             0 ); }

static inline int
BuildOrder_Spread_VerticalRoll_MarketEx(
    const char* underlying,
    unsigned int month_close,
    unsigned int day_close,
    unsigned int year_close,
    unsigned int month_open,
    unsigned int day_open,
    unsigned int year_open,
    int are_calls,
    double strike_close_buy,
    double strike_close_sell,
    double strike_open_buy,
    double strike_open_sell,
    size_t quantity,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRollEx_ABI( underlying, month_close, day_close,
                                               year_close, month_open, day_open,
                                               year_open, are_calls,
                                               strike_close_buy, strike_close_sell,
                                               strike_open_buy, strike_open_sell,
                                               quantity,
                                               quantity, 1, 0, porder, 0 ); }

static inline int
BuildOrder_Spread_VerticalRoll_Limit(
    const char* symbol_close_buy,
    const char* symbol_close_sell,
    const char* symbol_open_buy,
    const char* symbol_open_sell,
    size_t quantity,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRoll_ABI( symbol_close_buy, symbol_close_sell,
                                             symbol_open_buy, symbol_open_sell,
                                             quantity, quantity, 0, price,
                                             porder, 0 ); }

static inline int
BuildOrder_Spread_VerticalRoll_LimitEx(
    const char* underlying,
    unsigned int month_close,
    unsigned int day_close,
    unsigned int year_close,
    unsigned int month_open,
    unsigned int day_open,
    unsigned int year_open,
    int are_calls,
    double strike_close_buy,
    double strike_close_sell,
    double strike_open_buy,
    double strike_open_sell,
    size_t quantity,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRollEx_ABI( underlying, month_close, day_close,
                                              year_close, month_open, day_open,
                                              year_open, are_calls,
                                              strike_close_buy, strike_close_sell,
                                              strike_open_buy, strike_open_sell,
                                              quantity,
                                              quantity, 0, price, porder, 0 ); }

/* UNABALANCED VERTICAL ROLL SPREAD */
// NOTE - no check that quantity_close != quantity_open
static inline int
BuildOrder_Spread_VerticalRollUnbalanced_Market(
    const char* symbol_close_buy,
    const char* symbol_close_sell,
    const char* symbol_open_buy,
    const char* symbol_open_sell,
    size_t quantity_close,
    size_t quantity_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRoll_ABI( symbol_close_buy, symbol_close_sell,
                                             symbol_open_buy, symbol_open_sell,
                                             quantity_close, quantity_open, 1,
                                             0, porder, 0 ); }

static inline int
BuildOrder_Spread_VerticalRollUnbalanced_MarketEx(
    const char* underlying,
    unsigned int month_close,
    unsigned int day_close,
    unsigned int year_close,
    unsigned int month_open,
    unsigned int day_open,
    unsigned int year_open,
    int are_calls,
    double strike_close_buy,
    double strike_close_sell,
    double strike_open_buy,
    double strike_open_sell,
    size_t quantity_close,
    size_t quantity_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRollEx_ABI( underlying, month_close, day_close,
                                               year_close, month_open, day_open,
                                               year_open, are_calls,
                                               strike_close_buy, strike_close_sell,
                                               strike_open_buy, strike_open_sell,
                                               quantity_close, quantity_open, 1,
                                               0, porder, 0 ); }

static int
BuildOrder_Spread_VerticalRollUnbalanced_Limit(
    const char* symbol_close_buy,
    const char* symbol_close_sell,
    const char* symbol_open_buy,
    const char* symbol_open_sell,
    size_t quantity_close,
    size_t quantity_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRoll_ABI( symbol_close_buy, symbol_close_sell,
                                             symbol_open_buy, symbol_open_sell,
                                             quantity_close, quantity_open, 0,
                                             price, porder, 0 ); }

static inline int
BuildOrder_Spread_VerticalRollUnbalanced_LimitEx(
    const char* underlying,
    unsigned int month_close,
    unsigned int day_close,
    unsigned int year_close,
    unsigned int month_open,
    unsigned int day_open,
    unsigned int year_open,
    int are_calls,
    double strike_close_buy,
    double strike_close_sell,
    double strike_open_buy,
    double strike_open_sell,
    size_t quantity_close,
    size_t quantity_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_VerticalRollEx_ABI( underlying, month_close, day_close,
                                              year_close, month_open, day_open,
                                              year_open, are_calls,
                                              strike_close_buy, strike_close_sell,
                                              strike_open_buy, strike_open_sell,
                                              quantity_close, quantity_open, 0,
                                              price, porder, 0 ); }

/* BUTTERFLY */
static inline int
BuildOrder_Spread_Butterfly_Market(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Butterfly_ABI( symbol_outer1, symbol_inner1,
                                          symbol_outer2, quantity, quantity,
                                          is_buy, to_open, 1, .0,
                                          porder, 0); }

static inline int
BuildOrder_Spread_Butterfly_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_outer1,
    double strike_inner1,
    double strike_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_ButterflyEx_ABI( underlying, month, day, year,
                                            are_calls, strike_outer1,
                                            strike_inner1, strike_outer2,
                                            quantity, quantity, is_buy, to_open,
                                            1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_Butterfly_Limit(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Butterfly_ABI( symbol_outer1, symbol_inner1,
                                          symbol_outer2, quantity, quantity,
                                          is_buy, to_open, 0, price,
                                          porder, 0); }

static inline int
BuildOrder_Spread_Butterfly_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_outer1,
    double strike_inner1,
    double strike_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_ButterflyEx_ABI( underlying, month, day, year,
                                            are_calls, strike_outer1,
                                            strike_inner1, strike_outer2,
                                            quantity, quantity, is_buy, to_open,
                                            0, price, porder, 0 ); }

/* BUTTERFLY UNBALANCED*/
// NOTE - no check that quantity_outer1 != quantity_outer1
static inline int
BuildOrder_Spread_ButterflyUnbalanced_Market(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_outer2,
    size_t quantity_outer1,
    size_t quantity_outer2,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Butterfly_ABI( symbol_outer1, symbol_inner1,
                                          symbol_outer2, quantity_outer1,
                                          quantity_outer2, is_buy, to_open, 1,
                                          .0, porder, 0); }

static inline int
BuildOrder_Spread_ButterflyUnbalanced_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_outer1,
    double strike_inner1,
    double strike_outer2,
    size_t quantity_outer1,
    size_t quantity_outer2,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_ButterflyEx_ABI( underlying, month, day, year,
                                            are_calls, strike_outer1,
                                            strike_inner1, strike_outer2,
                                            quantity_outer1, quantity_outer2,
                                            is_buy, to_open, 1, 0., porder,
                                            0 ); }

static inline int
BuildOrder_Spread_ButterflyUnbalanced_Limit(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_outer2,
    size_t quantity_outer1,
    size_t quantity_outer2,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Butterfly_ABI( symbol_outer1, symbol_inner1,
                                          symbol_outer2, quantity_outer1,
                                          quantity_outer2, is_buy, to_open, 0,
                                          price, porder, 0); }

static inline int
BuildOrder_Spread_ButterflyUnbalanced_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_outer1,
    double strike_inner1,
    double strike_outer2,
    size_t quantity_outer1,
    size_t quantity_outer2,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_ButterflyEx_ABI( underlying, month, day, year,
                                            are_calls, strike_outer1,
                                            strike_inner1, strike_outer2,
                                            quantity_outer1, quantity_outer2,
                                            is_buy, to_open, 0, price, porder,
                                            0 ); }

/* BACK_RATIO */
static inline int
BuildOrder_Spread_BackRatio_Market(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity_buy,
    size_t quantity_sell,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_BackRatio_ABI( symbol_buy, symbol_sell, quantity_buy,
                                          quantity_sell, to_open, 1, .0,
                                          porder, 0); }

static inline int
BuildOrder_Spread_BackRatio_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_buy,
    double strike_sell,
    size_t quantity_buy,
    size_t quantity_sell,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_BackRatioEx_ABI( underlying, month, day, year,
                                            are_calls, strike_buy, strike_sell,
                                            quantity_buy, quantity_sell,
                                            to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_BackRatio_Limit(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity_buy,
    size_t quantity_sell,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_BackRatio_ABI( symbol_buy, symbol_sell, quantity_buy,
                                          quantity_sell, to_open, 0, price,
                                          porder, 0); }

static inline int
BuildOrder_Spread_BackRatio_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    int are_calls,
    double strike_buy,
    double strike_sell,
    size_t quantity_buy,
    size_t quantity_sell,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_BackRatioEx_ABI( underlying, month, day, year,
                                            are_calls, strike_buy, strike_sell,
                                            quantity_buy, quantity_sell,
                                            to_open, 0, price, porder, 0 ); }

/* CALENDAR */
static inline int
BuildOrder_Spread_Calendar_Market(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Calendar_ABI( symbol_buy, symbol_sell, quantity,
                                         to_open, 1, .0, porder, 0); }

static inline int
BuildOrder_Spread_Calendar_MarketEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    int are_calls,
    double strike,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CalendarEx_ABI( underlying, month_buy, day_buy,
                                           year_buy,  month_sell, day_sell,
                                           year_sell, are_calls, strike, quantity,
                                           to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_Calendar_Limit(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Calendar_ABI( symbol_buy, symbol_sell, quantity,
                                         to_open, 0, price, porder, 0); }

static inline int
BuildOrder_Spread_Calendar_LimitEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    int are_calls,
    double strike,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CalendarEx_ABI( underlying, month_buy, day_buy,
                                           year_buy,  month_sell, day_sell,
                                           year_sell, are_calls, strike, quantity,
                                           to_open, 0, price, porder, 0 ); }

/* DIAGONAL */
static inline int
BuildOrder_Spread_Diagonal_Market(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Diagonal_ABI( symbol_buy, symbol_sell, quantity,
                                         to_open, 1, .0, porder, 0); }

static inline int
BuildOrder_Spread_Diagonal_MarketEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    int are_calls,
    double strike_buy,
    double strike_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DiagonalEx_ABI( underlying, month_buy, day_buy,
                                           year_buy,  month_sell, day_sell,
                                           year_sell, are_calls, strike_buy,
                                           strike_sell, quantity, to_open, 1,
                                           0., porder, 0 ); }

static inline int
BuildOrder_Spread_Diagonal_Limit(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Diagonal_ABI( symbol_buy, symbol_sell, quantity,
                                         to_open, 0, price, porder, 0); }

static inline int
BuildOrder_Spread_Diagonal_LimitEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    int are_calls,
    double strike_buy,
    double strike_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DiagonalEx_ABI( underlying, month_buy, day_buy,
                                           year_buy,  month_sell, day_sell,
                                           year_sell, are_calls, strike_buy,
                                           strike_sell, quantity, to_open, 0,
                                           price, porder, 0 ); }

/* STRADDLE */
static inline int
BuildOrder_Spread_Straddle_Market(
    const char* symbol_call,
    const char* symbol_put,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Straddle_ABI( symbol_call, symbol_put, quantity,
                                         is_buy, to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_Straddle_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_StraddleEx_ABI( underlying, month, day, year, strike,
                                           quantity, is_buy, to_open, 1, 0.,
                                           porder, 0 ); }

static inline int
BuildOrder_Spread_Straddle_Limit(
    const char* symbol_call,
    const char* symbol_put,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Straddle_ABI( symbol_call, symbol_put, quantity,
                                         is_buy, to_open, 0, price, porder, 0 ); }

static inline int
BuildOrder_Spread_Straddle_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_StraddleEx_ABI( underlying, month, day, year, strike,
                                           quantity, is_buy, to_open, 0, price,
                                           porder, 0 ); }

/* STRANGLE */
static inline int
BuildOrder_Spread_Strangle_Market(
    const char* symbol_call,
    const char* symbol_put,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Strangle_ABI( symbol_call, symbol_put, quantity,
                                         is_buy, to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_Strangle_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_StrangleEx_ABI( underlying, month, day, year,
                                           strike_call, strike_put, quantity,
                                           is_buy, to_open, 1, 0., porder,
                                           0 ); }

static inline int
BuildOrder_Spread_Strangle_Limit(
    const char* symbol_call,
    const char* symbol_put,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Strangle_ABI( symbol_call, symbol_put, quantity,
                                         is_buy, to_open, 0, price, porder,
                                         0 ); }

static inline int
BuildOrder_Spread_Strangle_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_StrangleEx_ABI( underlying, month, day, year,
                                           strike_call, strike_put, quantity,
                                           is_buy, to_open, 0, price, porder,
                                           0 ); }

/* COLLAR SYNTHETIC */
static inline int
BuildOrder_Spread_CollarSynthetic_Market(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarSynthetic_ABI( symbol_buy, symbol_sell,
                                                quantity, to_open, 1, 0.,
                                                porder, 0 ); }

static inline int
BuildOrder_Spread_CollarSynthetic_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarSyntheticEx_ABI( underlying, month, day, year,
                                                  strike_call, strike_put,
                                                  quantity, is_buy, to_open, 1,
                                                  0., porder, 0 ); }

static inline int
BuildOrder_Spread_CollarSynthetic_Limit(
    const char* symbol_buy,
    const char* symbol_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarSynthetic_ABI( symbol_buy, symbol_sell,
                                                quantity, to_open, 0, price,
                                                porder, 0 ); }

static inline int
BuildOrder_Spread_CollarSynthetic_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarSyntheticEx_ABI( underlying, month, day, year,
                                                  strike_call, strike_put,
                                                  quantity, is_buy, to_open, 0,
                                                  price, porder, 0 ); }


/* COLLAR WITH STOCK */
static inline int
BuildOrder_Spread_CollarWithStock_Market(
    const char* symbol_buy,
    const char* symbol_sell,
    const char* symbol_stock,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarWithStock_ABI( symbol_buy, symbol_sell,
                                                symbol_stock, quantity, is_buy,
                                                to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_CollarWithStock_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarWithStockEx_ABI( underlying, month, day, year,
                                                  strike_call, strike_put,
                                                  quantity, is_buy, to_open, 1,
                                                  0., porder, 0 ); }

static inline int
BuildOrder_Spread_CollarWithStock_Limit(
    const char* symbol_buy,
    const char* symbol_sell,
    const char* symbol_stock,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarWithStock_ABI( symbol_buy, symbol_sell,
                                                symbol_stock, quantity, is_buy,
                                                to_open, 0, price, porder, 0 ); }

static inline int
BuildOrder_Spread_CollarWithStock_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call,
    double strike_put,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CollarWithStockEx_ABI( underlying, month, day, year,
                                                  strike_call, strike_put,
                                                  quantity, is_buy, to_open, 0,
                                                  price, porder, 0 ); }

/* CONDOR */
static inline int
BuildOrder_Spread_Condor_Market(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_inner2,
    const char* symbol_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Condor_ABI( symbol_outer1, symbol_inner1,
                                       symbol_inner2, symbol_outer2, quantity,
                                       quantity, is_buy, to_open, 1, 0.,
                                       porder, 0 ); }

static inline int
BuildOrder_Spread_Condor_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_outer1,
    double strike_inner1,
    double strike_inner2,
    double strike_outer2,
    int are_calls,
    size_t quantity,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CondorEx_ABI( underlying, month, day, year,
                                       strike_outer1, strike_inner1,
                                       strike_inner2, strike_outer2, are_calls,
                                       quantity, quantity, is_buy, to_open, 1,
                                       0., porder, 0 ); }

static inline int
BuildOrder_Spread_Condor_Limit(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_inner2,
    const char* symbol_outer2,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Condor_ABI( symbol_outer1, symbol_inner1,
                                       symbol_inner2, symbol_outer2, quantity,
                                       quantity, is_buy, to_open, 0, price,
                                       porder, 0 ); }

static inline int
BuildOrder_Spread_Condor_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_outer1,
    double strike_inner1,
    double strike_inner2,
    double strike_outer2,
    int are_calls,
    size_t quantity,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CondorEx_ABI( underlying, month, day, year,
                                       strike_outer1, strike_inner1,
                                       strike_inner2, strike_outer2, are_calls,
                                       quantity, quantity, is_buy, to_open, 0,
                                       price, porder, 0 ); }

/* CONDOR UNBALANCED */
// NOTE we don't check quantity1 != quantity2
static inline int
BuildOrder_Spread_CondorUnbalanced_Market(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_inner2,
    const char* symbol_outer2,
    size_t quantity1,
    size_t quantity2,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Condor_ABI( symbol_outer1, symbol_inner1,
                                       symbol_inner2, symbol_outer2, quantity1,
                                       quantity2, is_buy, to_open, 1, 0.,
                                       porder, 0 ); }

static inline int
BuildOrder_Spread_CondorUnbalanced_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_outer1,
    double strike_inner1,
    double strike_inner2,
    double strike_outer2,
    int are_calls,
    size_t quantity1,
    size_t quantity2,
    int is_buy,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CondorEx_ABI( underlying, month, day, year,
                                       strike_outer1, strike_inner1,
                                       strike_inner2, strike_outer2, are_calls,
                                       quantity1, quantity2, is_buy, to_open,
                                       1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_CondorUnbalanced_Limit(
    const char* symbol_outer1,
    const char* symbol_inner1,
    const char* symbol_inner2,
    const char* symbol_outer2,
    size_t quantity1,
    size_t quantity2,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_Condor_ABI( symbol_outer1, symbol_inner1,
                                       symbol_inner2, symbol_outer2, quantity1,
                                       quantity2, is_buy, to_open, 0, price,
                                       porder, 0 ); }

static inline int
BuildOrder_Spread_CondorUnbalanced_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_outer1,
    double strike_inner1,
    double strike_inner2,
    double strike_outer2,
    int are_calls,
    size_t quantity1,
    size_t quantity2,
    int is_buy,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_CondorEx_ABI( underlying, month, day, year,
                                       strike_outer1, strike_inner1,
                                       strike_inner2, strike_outer2, are_calls,
                                       quantity1, quantity2, is_buy, to_open,
                                       0, price, porder, 0 ); }

/* IRON CONDOR */

static inline int
BuildOrder_Spread_IronCondor_Market(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondor_ABI( symbol_call_buy, symbol_call_sell,
                                           symbol_put_buy, symbol_put_sell,
                                           quantity, quantity, to_open, 1, 0.,
                                           porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondor_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondorEx_ABI( underlying, month, day, year,
                                             strike_call_buy, strike_call_sell,
                                             strike_put_buy, strike_put_sell,
                                             quantity, quantity, to_open, 1,
                                             0., porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondor_Limit(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondor_ABI( symbol_call_buy, symbol_call_sell,
                                           symbol_put_buy, symbol_put_sell,
                                           quantity, quantity, to_open, 0,
                                           price, porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondor_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondorEx_ABI( underlying, month, day, year,
                                             strike_call_buy, strike_call_sell,
                                             strike_put_buy, strike_put_sell,
                                             quantity, quantity, to_open, 0,
                                             price, porder, 0 ); }

/* IRON CONDOR UNBALANCED*/
// NOTE - we don't check quantity_call != quantity_put

static inline int
BuildOrder_Spread_IronCondorUnbalanced_Market(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity_call,
    size_t quantity_put,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondor_ABI( symbol_call_buy, symbol_call_sell,
                                           symbol_put_buy, symbol_put_sell,
                                           quantity_call, quantity_put, to_open,
                                           1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondorUnbalanced_MarketEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity_call,
    size_t quantity_put,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondorEx_ABI( underlying, month, day, year,
                                             strike_call_buy, strike_call_sell,
                                             strike_put_buy, strike_put_sell,
                                             quantity_call, quantity_put,
                                             to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondorUnbalanced_Limit(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity_call,
    size_t quantity_put,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondor_ABI( symbol_call_buy, symbol_call_sell,
                                           symbol_put_buy, symbol_put_sell,
                                           quantity_call, quantity_put, to_open,
                                           0, price, porder, 0 ); }

static inline int
BuildOrder_Spread_IronCondorUnbalanced_LimitEx(
    const char* underlying,
    unsigned int month,
    unsigned int day,
    unsigned int year,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity_call,
    size_t quantity_put,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_IronCondorEx_ABI( underlying, month, day, year,
                                             strike_call_buy, strike_call_sell,
                                             strike_put_buy, strike_put_sell,
                                             quantity_call, quantity_put,
                                             to_open, 0, price, porder, 0 ); }

/* DOUBLE DIAGONAL */

static inline int
BuildOrder_Spread_DoubleDiagonal_Market(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DoubleDiagonal_ABI( symbol_call_buy, symbol_call_sell,
                                               symbol_put_buy, symbol_put_sell,
                                               quantity, to_open, 1, 0.,
                                               porder, 0 ); }

static inline int
BuildOrder_Spread_DoubleDiagonal_MarketEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity,
    int to_open,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DoubleDiagonalEx_ABI( underlying, month_buy, day_buy,
                                                 year_buy, month_sell, day_sell,
                                                 year_sell, strike_call_buy,
                                                 strike_call_sell,
                                                 strike_put_buy,
                                                 strike_put_sell, quantity,
                                                 to_open, 1, 0., porder, 0 ); }

static inline int
BuildOrder_Spread_DoubleDiagonal_Limit(
    const char* symbol_call_buy,
    const char* symbol_call_sell,
    const char* symbol_put_buy,
    const char* symbol_put_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DoubleDiagonal_ABI( symbol_call_buy, symbol_call_sell,
                                               symbol_put_buy, symbol_put_sell,
                                               quantity, to_open, 0, price,
                                               porder, 0 ); }

static inline int
BuildOrder_Spread_DoubleDiagonal_LimitEx(
    const char* underlying,
    unsigned int month_buy,
    unsigned int day_buy,
    unsigned int year_buy,
    unsigned int month_sell,
    unsigned int day_sell,
    unsigned int year_sell,
    double strike_call_buy,
    double strike_call_sell,
    double strike_put_buy,
    double strike_put_sell,
    size_t quantity,
    int to_open,
    double price,
    OrderTicket_C *porder
    )
{ return BuildOrder_Spread_DoubleDiagonalEx_ABI( underlying, month_buy, day_buy,
                                                 year_buy, month_sell, day_sell,
                                                 year_sell, strike_call_buy,
                                                 strike_call_sell,
                                                 strike_put_buy,
                                                 strike_put_sell, quantity,
                                                 to_open, 0, price, porder, 0 ); }

/* OCO / OTO */

static inline int
BuildOrder_OneCancelsOther( OrderTicket_C *porder1,
                            OrderTicket_C *porder2,
                            OrderTicket_C *porder )
{ return BuildOrder_OneCancelsOther_ABI( porder1, porder2, porder, 0 ); }

static inline int
BuildOrder_OneTriggersOther( OrderTicket_C *porder_primary,
                             OrderTicket_C *porder_conditional,
                             OrderTicket_C *porder )
{ return BuildOrder_OneTriggersOther_ABI( porder_primary, porder_conditional,
                                          porder, 0 ); }


#else
/* C++ interface */

namespace tdma {

class OrderBuilderBase {
public:
    OrderBuilderBase() = delete;
    /* aliases */
    static constexpr OrderAssetType EQUITY = OrderAssetType::EQUITY;
    static constexpr OrderAssetType OPTION = OrderAssetType::OPTION;
    static constexpr OrderType MARKET = OrderType::MARKET;
    static constexpr OrderType LIMIT = OrderType::LIMIT;
    static constexpr OrderType STOP = OrderType::STOP;
    static constexpr OrderType STOP_LIMIT = OrderType::STOP_LIMIT;
    static constexpr OrderType NET_CREDIT = OrderType::NET_CREDIT;
    static constexpr OrderType NET_DEBIT = OrderType::NET_DEBIT;
    static constexpr OrderType NET_ZERO = OrderType::NET_ZERO;
    static constexpr OrderInstruction BUY = OrderInstruction::BUY;
    static constexpr OrderInstruction SELL = OrderInstruction::SELL;
    static constexpr OrderInstruction SELL_SHORT =
        OrderInstruction::SELL_SHORT;
    static constexpr OrderInstruction BUY_TO_COVER =
        OrderInstruction::BUY_TO_COVER;
    static constexpr OrderInstruction BUY_TO_OPEN =
        OrderInstruction::BUY_TO_OPEN;
    static constexpr OrderInstruction SELL_TO_OPEN =
        OrderInstruction::SELL_TO_OPEN;
    static constexpr OrderInstruction BUY_TO_CLOSE =
        OrderInstruction::BUY_TO_CLOSE;
    static constexpr OrderInstruction SELL_TO_CLOSE =
        OrderInstruction::SELL_TO_CLOSE;
protected:
    static constexpr auto O = tdma::BuildOptionSymbol;
};


class SimpleOrderBuilder
        : public OrderBuilderBase {
public:
    SimpleOrderBuilder() = delete;

    typedef OrderTicket(*build_meth_ty)(OrderType, OrderAssetType,
        const std::string&, OrderInstruction, size_t, double, double);

    static OrderTicket
    build( OrderType order_type,
            OrderAssetType asset_type,
            const std::string& symbol,
            OrderInstruction instruction,
            size_t quantity,
            double limit_price = 0.0,
            double stop_price = 0.0)
    {
        OrderLeg leg(asset_type, symbol, instruction, quantity);
        return OrderTicket().set_type( order_type )
                      .set_duration(OrderDuration::DAY)
                      .set_session(OrderSession::NORMAL)
                      .add_leg(leg)
                      .set_price(limit_price)
                      .set_stop_price(stop_price);
    }

    class Equity{
    public:
        typedef OrderTicket(*build_meth_ty)(OrderType, const std::string&,
            OrderInstruction, size_t, double, double);

        static OrderTicket
        build( OrderType order_type,
                const std::string& symbol,
                OrderInstruction instruction,
                size_t quantity,
                double limit_price = 0.0,
                double stop_price = 0.0)
        {
            if( (order_type == LIMIT || order_type == STOP_LIMIT)
                && limit_price <= 0.0 ){
                    THROW_VALUE_EXCEPTION("limit price <= 0.0");
            }
            if( (order_type == STOP || order_type == STOP_LIMIT)
                && stop_price <= 0.0 ){
                THROW_VALUE_EXCEPTION("stop price <= 0.0");
            }
            return SimpleOrderBuilder::build( order_type, EQUITY, symbol,
                                              instruction, quantity,
                                              limit_price, stop_price );
        }

        class Market{
        public:
            Market() = delete;

            static OrderTicket
            Buy(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, BUY, quantity); }

            static OrderTicket
            Sell(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, SELL, quantity); }

            static OrderTicket
            Short(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, SELL_SHORT, quantity); }

            static OrderTicket
            Cover(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, BUY_TO_COVER, quantity); }
        };

        class Limit{
        public:
            Limit() = delete;

            static OrderTicket
            Buy(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, BUY, quantity, price); }

            static OrderTicket
            Sell(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, SELL, quantity, price); }

            static OrderTicket
            Short(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, SELL_SHORT, quantity, price); }

            static OrderTicket
            Cover(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, BUY_TO_COVER, quantity, price); }
        };

        class Stop{
        public:
            Stop() = delete;

            static OrderTicket
            Buy(const std::string& symbol, size_t quantity, double stop_price)
            { return build(STOP, symbol, BUY, quantity, 0., stop_price); }

            static OrderTicket
            Buy( const std::string& symbol,
                 size_t quantity,
                 double stop_price,
                 double limit_price )
            { return build( STOP_LIMIT, symbol, BUY, quantity, limit_price,
                            stop_price ); }

            static OrderTicket
            Sell(const std::string& symbol, size_t quantity, double stop_price)
            { return build(STOP, symbol, SELL, quantity, 0., stop_price); }

            static OrderTicket
            Sell( const std::string& symbol,
                   size_t quantity,
                   double stop_price,
                   double limit_price )
            { return build( STOP_LIMIT, symbol, SELL, quantity, limit_price,
                            stop_price ); }

            static OrderTicket
            Short(const std::string& symbol, size_t quantity, double stop_price)
            { return build(STOP, symbol, SELL_SHORT, quantity, 0., stop_price); }

            static OrderTicket
            Short( const std::string& symbol,
                    size_t quantity,
                    double stop_price,
                    double limit_price )
            { return build( STOP_LIMIT, symbol, SELL_SHORT, quantity,
                            limit_price, stop_price ); }

            static OrderTicket
            Cover(const std::string& symbol, size_t quantity, double stop_price)
            { return build(STOP, symbol, BUY_TO_COVER, quantity, 0., stop_price); }

            static OrderTicket
            Cover( const std::string& symbol,
                    size_t quantity,
                    double stop_price,
                    double limit_price )
            { return build( STOP_LIMIT, symbol, BUY_TO_COVER, quantity,
                            limit_price, stop_price ); }
        };

    }; /* Equity */

    class Option{
    public:
        Option() = delete;

        typedef OrderTicket(*raw_build_meth_ty)(OrderType, const std::string&,
            OrderInstruction, size_t, double);

        typedef OrderTicket(*ex_build_meth_ty)(OrderType, const std::string&,
            unsigned int, unsigned int, unsigned int, bool, double,
            OrderInstruction, size_t, double);

        static OrderTicket
        build( OrderType order_type,
                const std::string& symbol,
                OrderInstruction instruction,
                size_t quantity,
                double price = 0)
        {
            if( order_type == LIMIT && price <= 0.0 )
                THROW_VALUE_EXCEPTION("limit price <= 0.0");
            return SimpleOrderBuilder::build( order_type, OPTION, symbol,
                                            instruction, quantity, price );
        }

        static OrderTicket
        build( OrderType order_type,
                const std::string& underlying,
                unsigned int month,
                unsigned int day,
                unsigned int year,
                bool is_call,
                double strike,
                OrderInstruction instruction,
                size_t quantity,
                double price = 0 )
        { return build( order_type, O(underlying,month,day,year,is_call,strike),
                        instruction, quantity, price ); }


#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        bool is_call, \
        double strike, \
        size_t quantity

        class Market{
        public:
            Market() = delete;

            static OrderTicket
            BuyToOpen(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, BUY_TO_OPEN, quantity); }

            static OrderTicket
            BuyToOpen( ARGS )
            { return build( MARKET, underlying, month, day, year, is_call,
                            strike, BUY_TO_OPEN, quantity); }

            static OrderTicket
            SellToOpen(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, SELL_TO_OPEN, quantity); }

            static OrderTicket
            SellToOpen( ARGS )
            { return build( MARKET, underlying, month, day, year, is_call,
                            strike, SELL_TO_OPEN, quantity); }

            static OrderTicket
            BuyToClose(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, BUY_TO_CLOSE, quantity); }

            static OrderTicket
            BuyToClose( ARGS )
            { return build( MARKET, underlying, month, day, year, is_call,
                            strike, BUY_TO_CLOSE, quantity); }

            static OrderTicket
            SellToClose(const std::string& symbol, size_t quantity)
            { return build(MARKET, symbol, SELL_TO_CLOSE, quantity); }

            static OrderTicket
            SellToClose( ARGS )
            { return build( MARKET, underlying, month, day, year, is_call,
                            strike, SELL_TO_CLOSE, quantity); }
        };

        class Limit{
        public:
            Limit() = delete;

            static OrderTicket
            BuyToOpen(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, BUY_TO_OPEN, quantity, price); }

            static OrderTicket
            BuyToOpen( ARGS, double price )
            { return build( LIMIT, underlying, month, day, year, is_call,
                            strike, BUY_TO_OPEN, quantity, price); }

            static OrderTicket
            SellToOpen(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, SELL_TO_OPEN, quantity, price); }

            static OrderTicket
            SellToOpen( ARGS, double price )
            { return build( LIMIT, underlying, month, day, year, is_call,
                            strike, SELL_TO_OPEN, quantity, price); }

            static OrderTicket
            BuyToClose(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, BUY_TO_CLOSE, quantity, price); }

            static OrderTicket
            BuyToClose( ARGS, double price )
            { return build( LIMIT, underlying, month, day, year, is_call,
                            strike, BUY_TO_CLOSE, quantity, price); }

            static OrderTicket
            SellToClose(const std::string& symbol, size_t quantity, double price)
            { return build(LIMIT, symbol, SELL_TO_CLOSE, quantity, price); }

            static OrderTicket
            SellToClose( ARGS, double price )
            { return build( LIMIT, underlying, month, day, year, is_call,
                strike, SELL_TO_CLOSE, quantity, price); }
        };

#undef ARGS
    }; /* Option */

}; /* SimpleOrderBuilder */


class SpreadOrderBuilder
        : public OrderBuilderBase {

    static OrderLeg
    make_leg(const std::string& s, OrderInstruction instr, size_t quantity)
    { return {OPTION, s, instr, quantity}; }

public:
    SpreadOrderBuilder() = delete;

    using OrderBuilderBase::O;

    typedef OrderTicket(*build_meth_ty)(ComplexOrderStrategyType,
        const std::vector<OrderLeg>&, bool, double);

    static OrderTicket
    build( ComplexOrderStrategyType complex_strategy_type,
            const std::vector<OrderLeg>& legs,
            bool market_order = true,
            double price = 0.0 )
    {
        OrderTicket o;
        o.set_type(MARKET)
         .set_duration(OrderDuration::DAY)
         .set_session(OrderSession::NORMAL)
         .set_complex_strategy_type(complex_strategy_type)
         .add_legs(legs);

        if( market_order ){
            if( price )
                THROW_VALUE_EXCEPTION("market order contains price");
            return o;
        }else{ // overides MARKET
            if( price > 0 )
                o.set_type(NET_DEBIT).set_price(price);
            else if( price < 0 )
                o.set_type(NET_CREDIT).set_price(-price);
            else
                o.set_type(NET_ZERO);
        }
        return o;
    }

    class Vertical {
    public:
        Vertical() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::VERTICAL;

#define ARGS_RAW \
        const std::string& symbol_buy, \
        const std::string& symbol_sell, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        bool are_calls, \
        double strike_buy, \
        double strike_sell, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, bool, double, double, size_t, bool,
            bool, double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                {make_leg(symbol_buy, oi1, quantity),
                 make_leg(symbol_sell, oi2, quantity)},
                market_order, price );
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            return build( O(underlying, month, day, year, are_calls, strike_buy),
                          O(underlying, month, day, year, are_calls, strike_sell),
                          quantity, to_open, market_order, price );
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, true); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, true, false, price); }

        static OrderTicket
        Open( ARGS )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                              strike_sell, quantity, true); }

        static OrderTicket
        Open( ARGS, double price )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                              strike_sell, quantity, true, false,price); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, false); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, false,false, price); }

        static OrderTicket
        Close( ARGS )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                              strike_sell, quantity, false); }

        static OrderTicket
        Close( ARGS, double price )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                              strike_sell, quantity, false, false, price); }
#undef ARGS_RAW
#undef ARGS

        class Roll {
        public:
            Roll() = delete;
            static constexpr ComplexOrderStrategyType strategy
                = ComplexOrderStrategyType::VERTICAL_ROLL;

#define ARGS_RAW \
            const std::string& symbol_close_buy, \
            const std::string& symbol_close_sell, \
            const std::string& symbol_open_buy, \
            const std::string& symbol_open_sell

#define ARGS_NEW_EXP \
            const std::string& underlying, \
            unsigned int month_close, \
            unsigned int day_close, \
            unsigned int year_close, \
            unsigned int month_open, \
            unsigned int day_open, \
            unsigned int year_open, \
            bool are_calls, \
            double strike_close_buy, \
            double strike_close_sell, \
            double strike_open_buy, \
            double strike_open_sell

#define ARGS_SAME_EXP \
            const std::string& underlying, \
            unsigned int month, \
            unsigned int day, \
            unsigned int year, \
            bool are_calls, \
            double strike_close_buy, \
            double strike_close_sell, \
            double strike_open_buy, \
            double strike_open_sell

            typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                 const std::string&, const std::string&, const std::string&,
                 size_t, size_t, bool, double);

             typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
                 unsigned int, unsigned int, unsigned int, unsigned int,
                 unsigned int, bool, double, double, double, double, size_t,
                 size_t, bool, double);

            static OrderTicket
            build( ARGS_RAW,
                   size_t quantity_close,
                   size_t quantity_open,
                   bool market_order = true,
                   double price = 0 )
            {
                OrderTicket o = Open(symbol_open_buy, symbol_open_sell, quantity_open);
                return Vertical::build(
                    symbol_close_buy, symbol_close_sell, quantity_close, false,
                    market_order, price)
                          .add_legs( o.get_legs())
                          .set_complex_strategy_type(strategy);
            }

            static OrderTicket
            build( ARGS_NEW_EXP,
                    size_t quantity_close,
                    size_t quantity_open,
                    bool market_order = true,
                    double price = 0 )
            {
                OrderTicket o = Open(underlying, month_open, day_open, year_open,
                               are_calls, strike_open_buy, strike_open_sell,
                               quantity_open);

                return Vertical::build(
                    underlying, month_close, day_close, year_close, are_calls,
                    strike_close_buy, strike_close_sell, quantity_close, false,
                    market_order, price)
                        .add_legs( o.get_legs())
                        .set_complex_strategy_type(strategy);
            }

            class Unbalanced {
            public:
                Unbalanced() = delete;
                static constexpr ComplexOrderStrategyType strategy
                    = ComplexOrderStrategyType::UNBALANCED_VERTICAL_ROLL;

                template< typename... T >
                static OrderTicket
                build( T... args )
                { return Roll::build(args...)
                            .set_complex_strategy_type(strategy); }

                static OrderTicket
                Raw( ARGS_RAW, size_t quantity_close, size_t quantity_open )
                { return build( symbol_close_buy, symbol_close_sell,
                                       symbol_open_buy, symbol_open_sell,
                                       quantity_close, quantity_open ); }

                static OrderTicket
                Raw( ARGS_RAW,
                     size_t quantity_close,
                     size_t quantity_open,
                     double price )
                { return build( symbol_close_buy, symbol_close_sell,
                                      symbol_open_buy, symbol_open_sell,
                                      quantity_close, quantity_open,
                                      false, price ); }

                static OrderTicket
                NewExp( ARGS_NEW_EXP,
                         size_t quantity_close,
                         size_t quantity_open )
                { return build( underlying, month_close, day_close,
                                       year_close, month_open, day_open,
                                       year_open, are_calls, strike_close_buy,
                                       strike_close_sell, strike_open_buy,
                                       strike_open_sell, quantity_close,
                                       quantity_open); }

                static OrderTicket
                NewExp( ARGS_NEW_EXP,
                        size_t quantity_close,
                        size_t quantity_open,
                        double price )
                { return build( underlying, month_close, day_close,
                                      year_close, month_open, day_open,
                                      year_open, are_calls, strike_close_buy,
                                      strike_close_sell, strike_open_buy,
                                      strike_open_sell, quantity_close,
                                      quantity_open, false, price); }

                static OrderTicket
                SameExp( ARGS_SAME_EXP,
                          size_t quantity_close,
                          size_t quantity_open )
                { return build( underlying, month, day, year, month, day,
                                       year, are_calls, strike_close_buy,
                                       strike_close_sell, strike_open_buy,
                                       strike_open_sell, quantity_close,
                                       quantity_open ); }

                static OrderTicket
                SameExp( ARGS_SAME_EXP,
                          size_t quantity_close,
                          size_t quantity_open,
                          double price )
                { return build( underlying, month, day, year, month, day,
                                      year, are_calls, strike_close_buy,
                                      strike_close_sell, strike_open_buy,
                                      strike_open_sell, quantity_close,
                                      quantity_open, false, price ); }

            }; /* Unbalanced */


            static OrderTicket
            Raw( ARGS_RAW, size_t quantity )
            { return build( symbol_close_buy, symbol_close_sell,
                                   symbol_open_buy, symbol_open_sell,
                                   quantity, quantity ); }

            static OrderTicket
            Raw( ARGS_RAW, size_t quantity, double price )
            { return build( symbol_close_buy, symbol_close_sell,
                                  symbol_open_buy, symbol_open_sell,
                                  quantity, quantity, false, price ); }

            static OrderTicket
            NewExp( ARGS_NEW_EXP, size_t quantity )
            { return build( underlying, month_close, day_close,
                                   year_close, month_open, day_open, year_open,
                                   are_calls, strike_close_buy, strike_close_sell,
                                   strike_open_buy, strike_open_sell,
                                   quantity, quantity); }

            static OrderTicket
            NewExp( ARGS_NEW_EXP, size_t quantity, double price )
            { return build( underlying, month_close, day_close,
                                   year_close, month_open, day_open, year_open,
                                   are_calls, strike_close_buy, strike_close_sell,
                                   strike_open_buy, strike_open_sell,
                                   quantity, quantity, false, price); }

            static OrderTicket
            SameExp( ARGS_SAME_EXP, size_t quantity )
            { return build( underlying, month, day, year, month, day,
                                   year, are_calls, strike_close_buy,
                                   strike_close_sell, strike_open_buy,
                                   strike_open_sell, quantity, quantity ); }

            static OrderTicket
            SameExp( ARGS_SAME_EXP, size_t quantity, double price )
            { return build( underlying, month, day, year, month, day, year,
                                  are_calls, strike_close_buy, strike_close_sell,
                                  strike_open_buy, strike_open_sell, quantity,
                                  quantity, false, price ); }
#undef ARGS_RAW
#undef ARGS_NEW_EXP
#undef ARGS_SAME_EXP
        }; /* Roll */

    }; /* Vertical */


    class Butterfly {
    public:
        Butterfly() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::BUTTERFLY;

#define ARGS_RAW(outer,inner) \
        const std::string& symbol_##outer##1, \
        const std::string& symbol_##inner##1, \
        const std::string& symbol_##outer##2

#define ARGS(outer,inner) \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        bool are_calls, \
        double strike_##outer##1, \
        double strike_##inner##1, \
        double strike_##outer##2

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, const std::string&, OrderInstruction,
                OrderInstruction, size_t, size_t, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, bool, double, double, double,
            OrderInstruction, OrderInstruction, size_t, size_t, bool, double);

        static OrderTicket
        build( ARGS_RAW(outer,inner),
                OrderInstruction instr_outer,
                OrderInstruction instr_inner,
                size_t quantity_outer1,
                size_t quantity_outer2,
                bool market_order = true,
                double price = 0.0 )
        {
            size_t quantity_inner = quantity_outer1 + quantity_outer2;
            return SpreadOrderBuilder::build( strategy,
                {make_leg(symbol_outer1, instr_outer, quantity_outer1),
                 make_leg(symbol_inner1, instr_inner, quantity_inner),
                 make_leg(symbol_outer2, instr_outer, quantity_outer2)},
                 market_order, price );
        }

        static OrderTicket
        build( ARGS(outer,inner),
                OrderInstruction instr_outer,
                OrderInstruction instr_inner,
                size_t quantity_outer1,
                size_t quantity_outer2,
                bool market_order = true,
                double price = 0.0 )
        {
            return build(
                O(underlying, month, day, year, are_calls, strike_outer1),
                O(underlying, month, day, year, are_calls, strike_inner1),
                O(underlying, month, day, year, are_calls, strike_outer2),
                instr_outer, instr_inner, quantity_outer1, quantity_outer2,
                market_order, price
                );
        }

    public:
        class Unbalanced {
        public:
            Unbalanced() = delete;
            static constexpr ComplexOrderStrategyType strategy
                = ComplexOrderStrategyType::UNBALANCED_BUTTERFLY;

            template< typename... T >
            static OrderTicket
            build( T... args )
            { return Butterfly::build(args...)
                        .set_complex_strategy_type(strategy); }

            static OrderTicket
            BuyToOpen( ARGS_RAW(buy,sell),
                        size_t quantity_buy1,
                        size_t quantity_buy2 )
            { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                                  BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity_buy1, quantity_buy2); }

            static OrderTicket
            BuyToOpen( ARGS_RAW(buy,sell),
                        size_t quantity_buy1,
                        size_t quantity_buy2,
                        double price)
            { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                                  BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity_buy1, quantity_buy2, false, price); }

            static OrderTicket
            BuyToOpen( ARGS(buy,sell),
                        size_t quantity_buy1,
                        size_t quantity_buy2 )
            { return build(underlying, month, day, year, are_calls,
                                  strike_buy1, strike_sell1, strike_buy2,
                                  BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity_buy1, quantity_buy2 ); }

            static OrderTicket
            BuyToOpen( ARGS(buy,sell),
                        size_t quantity_buy1,
                        size_t quantity_buy2,
                        double price )
            { return build(underlying, month, day, year, are_calls,
                                  strike_buy1, strike_sell1, strike_buy2,
                                  BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity_buy1, quantity_buy2, false, price ); }

            static OrderTicket
            SellToOpen( ARGS_RAW(sell,buy),
                         size_t quantity_sell1,
                         size_t quantity_sell2 )
            { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                                  SELL_TO_OPEN, BUY_TO_OPEN,
                                  quantity_sell1, quantity_sell2); }

            static OrderTicket
            SellToOpen( ARGS_RAW(sell,buy),
                         size_t quantity_sell1,
                         size_t quantity_sell2,
                         double price )
            { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                                  SELL_TO_OPEN, BUY_TO_OPEN,
                                  quantity_sell1, quantity_sell2, false, price); }

            static OrderTicket
            SellToOpen( ARGS(sell,buy),
                         size_t quantity_sell1,
                         size_t quantity_sell2 )
            { return build(underlying, month, day, year, are_calls,
                                  strike_sell1, strike_buy1, strike_sell2,
                                  SELL_TO_OPEN, BUY_TO_OPEN,
                                  quantity_sell1, quantity_sell2 ); }

            static OrderTicket
            SellToOpen( ARGS(sell,buy),
                         size_t quantity_sell1,
                         size_t quantity_sell2,
                         double price )
            { return build(underlying, month, day, year, are_calls,
                                 strike_sell1, strike_buy1, strike_sell2,
                                 SELL_TO_OPEN, BUY_TO_OPEN,
                                 quantity_sell1, quantity_sell2, false, price ); }

            static OrderTicket
            BuyToClose( ARGS_RAW(buy,sell),
                         size_t quantity_buy1,
                         size_t quantity_buy2 )
            { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                                  BUY_TO_CLOSE, SELL_TO_CLOSE,
                                  quantity_buy1, quantity_buy2); }

            static OrderTicket
            BuyToClose( ARGS_RAW(buy,sell),
                        size_t quantity_buy1,
                        size_t quantity_buy2,
                        double price)
            { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                                  BUY_TO_CLOSE, SELL_TO_CLOSE,
                                  quantity_buy1, quantity_buy2, false, price); }

            static OrderTicket
            BuyToClose( ARGS(buy,sell),
                         size_t quantity_buy1,
                         size_t quantity_buy2 )
            { return build(underlying, month, day, year, are_calls,
                                  strike_buy1, strike_sell1, strike_buy2,
                                  BUY_TO_CLOSE, SELL_TO_CLOSE,
                                  quantity_buy1, quantity_buy2 ); }

            static OrderTicket
            BuyToClose( ARGS(buy,sell),
                         size_t quantity_buy1,
                         size_t quantity_buy2,
                         double price )
            { return build(underlying, month, day, year, are_calls,
                                  strike_buy1, strike_sell1, strike_buy2,
                                  BUY_TO_CLOSE, SELL_TO_CLOSE,
                                  quantity_buy1, quantity_buy2, false, price ); }

            static OrderTicket
            SellToClose( ARGS_RAW(sell,buy),
                          size_t quantity_sell1,
                          size_t quantity_sell2 )
            { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                                  SELL_TO_CLOSE, BUY_TO_CLOSE,
                                  quantity_sell1, quantity_sell2); }

            static OrderTicket
            SellToClose( ARGS_RAW(sell,buy),
                          size_t quantity_sell1,
                          size_t quantity_sell2,
                          double price )
            { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                                  SELL_TO_CLOSE, BUY_TO_CLOSE,
                                  quantity_sell1, quantity_sell2, false, price); }

            static OrderTicket
            SellToClose( ARGS(sell,buy),
                          size_t quantity_sell1,
                          size_t quantity_sell2 )
            { return build(underlying, month, day, year, are_calls,
                                  strike_sell1, strike_buy1, strike_sell2,
                                  SELL_TO_CLOSE, BUY_TO_CLOSE,
                                  quantity_sell1, quantity_sell2 ); }

            static OrderTicket
            SellToClose( ARGS(sell,buy),
                          size_t quantity_sell1,
                          size_t quantity_sell2,
                          double price )
            { return build(underlying, month, day, year, are_calls,
                                 strike_sell1, strike_buy1, strike_sell2,
                                 SELL_TO_CLOSE, BUY_TO_CLOSE,
                                 quantity_sell1, quantity_sell2, false, price ); }
        }; /* Unbalanced */

        static OrderTicket
        BuyToOpen( ARGS_RAW(buy,sell), size_t quantity )
        { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                              BUY_TO_OPEN, SELL_TO_OPEN,
                              quantity, quantity); }

        static OrderTicket
        BuyToOpen( ARGS_RAW(buy,sell), size_t quantity, double price)
        { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                              BUY_TO_OPEN, SELL_TO_OPEN,
                              quantity, quantity, false, price); }

        static OrderTicket
        BuyToOpen( ARGS(buy,sell), size_t quantity )
        { return build(underlying, month, day, year, are_calls,
                              strike_buy1, strike_sell1, strike_buy2,
                              BUY_TO_OPEN, SELL_TO_OPEN,
                              quantity, quantity ); }

        static OrderTicket
        BuyToOpen( ARGS(buy,sell), size_t quantity, double price )
        { return build(underlying, month, day, year, are_calls,
                              strike_buy1, strike_sell1, strike_buy2,
                              BUY_TO_OPEN, SELL_TO_OPEN,
                              quantity, quantity, false, price ); }

        static OrderTicket
        SellToOpen( ARGS_RAW(sell,buy), size_t quantity )
        { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                              SELL_TO_OPEN, BUY_TO_OPEN,
                              quantity, quantity); }

        static OrderTicket
        SellToOpen( ARGS_RAW(sell,buy), size_t quantity, double price )
        { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                              SELL_TO_OPEN, BUY_TO_OPEN,
                              quantity, quantity, false, price); }

        static OrderTicket
        SellToOpen( ARGS(sell,buy), size_t quantity )
        { return build(underlying, month, day, year, are_calls,
                              strike_sell1, strike_buy1, strike_sell2,
                              SELL_TO_OPEN, BUY_TO_OPEN,
                              quantity, quantity ); }

        static OrderTicket
        SellToOpen( ARGS(sell,buy), size_t quantity, double price )
        { return build(underlying, month, day, year, are_calls,
                             strike_sell1, strike_buy1, strike_sell2,
                             SELL_TO_OPEN, BUY_TO_OPEN,
                             quantity, quantity, false, price ); }

        static OrderTicket
        BuyToClose( ARGS_RAW(buy,sell), size_t quantity )
        { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                              BUY_TO_CLOSE, SELL_TO_CLOSE,
                              quantity, quantity); }

        static OrderTicket
        BuyToClose( ARGS_RAW(buy,sell), size_t quantity, double price )
        { return build(symbol_buy1, symbol_sell1, symbol_buy2,
                              BUY_TO_CLOSE, SELL_TO_CLOSE,
                              quantity, quantity, false, price); }

        static OrderTicket
        BuyToClose( ARGS(buy,sell), size_t quantity )
        { return build(underlying, month, day, year, are_calls,
                              strike_buy1, strike_sell1, strike_buy2,
                              BUY_TO_CLOSE, SELL_TO_CLOSE,
                              quantity, quantity ); }

        static OrderTicket
        BuyToClose( ARGS(buy,sell), size_t quantity, double price )
        { return build(underlying, month, day, year, are_calls,
                              strike_buy1, strike_sell1, strike_buy2,
                              BUY_TO_CLOSE, SELL_TO_CLOSE,
                              quantity, quantity, false, price ); }

        static OrderTicket
        SellToClose( ARGS_RAW(sell,buy), size_t quantity)
        { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                              SELL_TO_CLOSE, BUY_TO_CLOSE,
                              quantity, quantity); }

        static OrderTicket
        SellToClose( ARGS_RAW(sell,buy), size_t quantity, double price )
        { return build(symbol_sell1, symbol_buy1, symbol_sell2,
                              SELL_TO_CLOSE, BUY_TO_CLOSE,
                              quantity, quantity, false, price); }

        static OrderTicket
        SellToClose( ARGS(sell,buy), size_t quantity)
        { return build(underlying, month, day, year, are_calls,
                              strike_sell1, strike_buy1, strike_sell2,
                              SELL_TO_CLOSE, BUY_TO_CLOSE,
                              quantity, quantity ); }

        static OrderTicket
        SellToClose( ARGS(sell,buy), size_t quantity, double price )
        { return build(underlying, month, day, year, are_calls,
                             strike_sell1, strike_buy1, strike_sell2,
                             SELL_TO_CLOSE, BUY_TO_CLOSE,
                             quantity, quantity, false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* Butterfly */


    class BackRatio{ // TODO consider checking quantities !=
    public:
        BackRatio() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::BACK_RATIO;

#define ARGS_RAW \
        const std::string& symbol_buy, \
        const std::string& symbol_sell, \
        size_t quantity_buy, \
        size_t quantity_sell

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        bool are_calls, \
        double strike_buy, \
        double strike_sell, \
        size_t quantity_buy, \
        size_t quantity_sell

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, size_t, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, bool, double, double, size_t, size_t,
            bool, bool, double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                {make_leg(symbol_buy, oi1, quantity_buy),
                 make_leg(symbol_sell, oi2, quantity_sell)},
                 market_order, price);
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            return build( O(underlying, month, day, year, are_calls, strike_buy),
                          O(underlying, month, day, year, are_calls, strike_sell),
                          quantity_buy, quantity_sell, to_open, market_order,
                          price );
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build( symbol_buy, symbol_sell, quantity_buy,
                               quantity_sell, true ); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build( symbol_buy, symbol_sell, quantity_buy,
                              quantity_sell, true, false, price ); }

        static OrderTicket
        Open( ARGS )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                              strike_sell, quantity_buy, quantity_sell, true); }

        static OrderTicket
        Open( ARGS, double price )
        { return build(underlying, month, day, year, are_calls, strike_buy,
                             strike_sell, quantity_buy, quantity_sell, true,
                             false, price); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity_buy,
                               quantity_sell, false); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build( symbol_buy, symbol_sell, quantity_buy,
                              quantity_sell, false, false, price ); }

        static OrderTicket
        Close( ARGS )
        { return build( underlying, month, day, year, are_calls, strike_buy,
                               strike_sell, quantity_buy, quantity_sell,
                               false ); }

        static OrderTicket
        Close( ARGS, double price )
        { return build( underlying, month, day, year, are_calls, strike_buy,
                              strike_sell,  quantity_buy, quantity_sell, false,
                              false, price ); }
#undef ARGS_RAW
#undef ARGS
    };


    class Calendar {
    public:
        Calendar() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::CALENDAR;

#define ARGS_RAW \
        const std::string& symbol_buy, \
        const std::string& symbol_sell, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month_buy, \
        unsigned int day_buy, \
        unsigned int year_buy, \
        unsigned int month_sell, \
        unsigned int day_sell, \
        unsigned int year_sell, \
        bool are_calls, \
        double strike, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, unsigned int, unsigned int,
            unsigned int, bool, double, size_t, bool, bool, double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                {make_leg(symbol_buy, oi1, quantity),
                 make_leg(symbol_sell, oi2, quantity)},
                market_order, price );
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            return build(
                O(underlying, month_buy, day_buy, year_buy, are_calls, strike),
                O(underlying, month_sell, day_sell, year_sell, are_calls, strike),
                quantity, to_open, market_order, price );
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, true); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, true, false, price); }

        static OrderTicket
        Open( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike, quantity, true ); }

        static OrderTicket
        Open( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike, quantity, true, false, price ); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, false); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, false, false, price); }

        static OrderTicket
        Close( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike, quantity, false ); }

        static OrderTicket
        Close( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike, quantity, false, false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* Calendar */


    class Diagonal {
    public:
        Diagonal() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::DIAGONAL;

#define ARGS_RAW \
        const std::string& symbol_buy, \
        const std::string& symbol_sell, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month_buy, \
        unsigned int day_buy, \
        unsigned int year_buy, \
        unsigned int month_sell, \
        unsigned int day_sell, \
        unsigned int year_sell, \
        bool are_calls, \
        double strike_buy, \
        double strike_sell, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, unsigned int, unsigned int,
            unsigned int, bool, double, double, size_t, bool, bool, double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                {make_leg(symbol_buy, oi1, quantity),
                 make_leg(symbol_sell, oi2, quantity)},
                market_order, price );
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            return build(
                O(underlying, month_buy, day_buy, year_buy, are_calls,
                    strike_buy),
                O(underlying, month_sell, day_sell, year_sell, are_calls,
                    strike_sell),
                quantity, to_open, market_order, price );
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, true); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, true, false, price); }

        static OrderTicket
        Open( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike_buy, strike_sell, quantity, true ); }

        static OrderTicket
        Open( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike_buy, strike_sell, quantity, true,
                               false, price ); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, false); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build(symbol_buy, symbol_sell, quantity, false, false, price); }

        static OrderTicket
        Close( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike_buy, strike_sell, quantity, false ); }

        static OrderTicket
        Close( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, are_calls,
                               strike_buy, strike_sell, quantity, false,
                               false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* Diagonal */


    class Straddle {
    public:
        Straddle() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::STRADDLE;

#define ARGS_RAW \
        const std::string& symbol_call, \
        const std::string& symbol_put, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, size_t, OrderInstruction, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, double, size_t, OrderInstruction,
            bool, double);

        static OrderTicket
        build( ARGS_RAW,
                OrderInstruction instruction,
                bool market_order = true,
                double price = 0.0 )
        { return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_call, instruction, quantity),
                     make_leg(symbol_put, instruction, quantity)},
                     market_order, price ); }

        static OrderTicket
        build( ARGS,
               OrderInstruction instruction,
               bool market_order = true,
               double price = 0.0 )
        { return build( O(underlying, month, day, year, true, strike),
                                 O(underlying, month, day, year, false, strike),
                                 quantity, instruction, market_order, price ); }

        static OrderTicket
        BuyToOpen( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                              BUY_TO_OPEN ); }

        static OrderTicket
        BuyToOpen( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              BUY_TO_OPEN, false, price ); }

        static OrderTicket
        BuyToOpen( ARGS )
        { return build( underlying, month, day, year, strike, quantity,
                               BUY_TO_OPEN ); }

        static OrderTicket
        BuyToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike, quantity,
                              BUY_TO_OPEN, false, price ); }

        static OrderTicket
        SellToOpen( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                               SELL_TO_OPEN ); }

        static OrderTicket
        SellToOpen( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              SELL_TO_OPEN, false, price ); }

        static OrderTicket
        SellToOpen( ARGS )
        { return build( underlying, month, day, year, strike, quantity,
                               SELL_TO_OPEN ); }

        static OrderTicket
        SellToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike, quantity,
                              SELL_TO_OPEN, false, price ); }

        static OrderTicket
        BuyToClose( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                              BUY_TO_CLOSE ); }

        static OrderTicket
        BuyToClose( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                             BUY_TO_CLOSE, false, price ); }

        static OrderTicket
        BuyToClose( ARGS )
        { return build( underlying, month, day, year, strike, quantity,
                               BUY_TO_CLOSE ); }

        static OrderTicket
        BuyToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike, quantity,
                              BUY_TO_CLOSE, false, price ); }

        static OrderTicket
        SellToClose( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                              SELL_TO_CLOSE ); }

        static OrderTicket
        SellToClose( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                             SELL_TO_CLOSE, false, price ); }

        static OrderTicket
        SellToClose( ARGS )
        { return build( underlying, month, day, year, strike, quantity,
                               SELL_TO_CLOSE ); }

        static OrderTicket
        SellToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike, quantity,
                              SELL_TO_CLOSE, false, price); }
#undef ARGS_RAW
#undef ARGS
    }; /* STRADDLE */


    class Strangle {
    public:
        Strangle() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::STRANGLE;

#define ARGS_RAW \
        const std::string& symbol_call, \
        const std::string& symbol_put, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike_call, \
        double strike_put, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
                const std::string&, size_t, OrderInstruction, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, double, double, size_t,
            OrderInstruction, bool, double);

        static OrderTicket
        build( ARGS_RAW,
                OrderInstruction instruction,
                bool market_order = true,
                double price = 0.0 )
        {
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_call, instruction, quantity),
                     make_leg(symbol_put, instruction, quantity)},
                     market_order, price );
        }

        static OrderTicket
        build( ARGS,
                OrderInstruction instruction,
                bool market_order = true,
                double price = 0.0 )
        {
            return build( O(underlying, month, day, year, true, strike_call),
                          O(underlying, month, day, year, false, strike_put),
                          quantity, instruction, market_order, price );
        }

        static OrderTicket
        BuyToOpen( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                               BUY_TO_OPEN ); }

        static OrderTicket
        BuyToOpen( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              BUY_TO_OPEN, false, price ); }

        static OrderTicket
        BuyToOpen( ARGS )
        { return build( underlying, month, day, year, strike_call,
                               strike_put, quantity, BUY_TO_OPEN ); }

        static OrderTicket
        BuyToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike_call,
                              strike_put, quantity, BUY_TO_OPEN, false, price ); }

        static OrderTicket
        SellToOpen( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                               SELL_TO_OPEN ); }

        static OrderTicket
        SellToOpen( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              SELL_TO_OPEN, false, price ); }

        static OrderTicket
        SellToOpen( ARGS )
        { return build( underlying, month, day, year, strike_call,
                               strike_put, quantity, SELL_TO_OPEN ); }

        static OrderTicket
        SellToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike_call,
                              strike_put, quantity, SELL_TO_OPEN, false, price ); }

        static OrderTicket
        BuyToClose( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                               BUY_TO_CLOSE ); }

        static OrderTicket
        BuyToClose( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              BUY_TO_CLOSE, false, price ); }

        static OrderTicket
        BuyToClose( ARGS )
        { return build( underlying, month, day, year, strike_call,
                               strike_put, quantity, BUY_TO_CLOSE ); }

        static OrderTicket
        BuyToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike_call,
                              strike_put, quantity, BUY_TO_CLOSE, false, price ); }
        static OrderTicket
        SellToClose( ARGS_RAW )
        { return build( symbol_call, symbol_put, quantity,
                               SELL_TO_CLOSE ); }

        static OrderTicket
        SellToClose( ARGS_RAW, double price )
        { return build( symbol_call, symbol_put, quantity,
                              SELL_TO_CLOSE, false, price ); }

        static OrderTicket
        SellToClose( ARGS )
        { return build( underlying, month, day, year, strike_call,
                               strike_put, quantity, SELL_TO_CLOSE ); }

        static OrderTicket
        SellToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike_call,
                              strike_put, quantity, SELL_TO_CLOSE, false, price); }
#undef ARGS_RAW
#undef ARGS
    }; /* STRANGLE */


    class CollarSynthetic {
    public:
        CollarSynthetic() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::COLLAR_SYNTHETIC;

#define ARGS_RAW \
        const std::string& symbol_buy, \
        const std::string& symbol_sell, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike_call, \
        double strike_put, \
        size_t quantity, \
        bool is_buy

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&,
            unsigned int, unsigned int, unsigned int, double, double,
            size_t, bool, bool, bool, double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_buy, oi1, quantity),
                     make_leg(symbol_sell, oi2, quantity)},
                    market_order, price );
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            auto c = O(underlying, month, day, year, true, strike_call);
            auto p = O(underlying, month, day, year, false, strike_put);
            return build( is_buy ? c : p, is_buy ? p : c,
                          quantity, to_open, market_order, price );
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build(symbol_buy, symbol_sell, quantity, true); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build( symbol_buy, symbol_sell, quantity, true, false, price); }

        static OrderTicket
        Open( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, is_buy, true ); }

        static OrderTicket
        Open( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                              quantity, is_buy, true, false, price); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build( symbol_buy, symbol_sell, quantity, false ); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build( symbol_buy, symbol_sell, quantity, false, false, price); }

        static OrderTicket
        Close( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, is_buy, false ); }

        static OrderTicket
        Close( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, is_buy, false, false, price); }
#undef ARGS_RAW
#undef ARGS
    }; /* CollarSynthetic */


    // NOTE - 'Buy' refers to the stock side (buy put, sell call)
    class CollarWithStock {
    public:
        CollarWithStock() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::COLLAR_WITH_STOCK;

#define ARGS_RAW \
        const std::string& symbol_option_buy, \
        const std::string& symbol_option_sell, \
        const std::string& symbol_stock, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike_call, \
        double strike_put, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, const std::string&, size_t, bool, bool, bool,
            double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&,
            unsigned int, unsigned int, unsigned int, double, double, size_t,
            bool, bool, bool, double);

        static OrderTicket
        build( ARGS_RAW,
                bool is_buy,
                bool to_open,
                bool market_order = true,
                double price = 0.0 )
        {
            OrderInstruction oi1 = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction oi2 = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            OrderInstruction oi3 = to_open ? (is_buy ? BUY : SELL_SHORT )
                                           : (is_buy ? BUY_TO_COVER : SELL );
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_option_buy, oi1, quantity),
                     make_leg(symbol_option_sell, oi2, quantity)},
                    market_order, price )
                .add_leg({EQUITY, symbol_stock, oi3, quantity * 100});
        }

        static OrderTicket
        build( ARGS,
                bool is_buy,
                bool to_open,
                bool market_order = true,
                double price = 0.0 )
        {
            auto c = O(underlying, month, day, year, true, strike_call);
            auto p = O(underlying, month, day, year, false, strike_put);
            return build( is_buy ? p : c, is_buy ? c : p,
                                   underlying, quantity, is_buy, to_open,
                                   market_order, price );
        }

        static OrderTicket
        BuyToOpen( ARGS_RAW )
        { return build( symbol_option_buy, symbol_option_sell,
                               symbol_stock, quantity, true, true ); }

        static OrderTicket
        BuyToOpen( ARGS_RAW, double price)
        { return build( symbol_option_buy, symbol_option_sell,
                              symbol_stock, quantity, true, true, false, price ); }

        static OrderTicket
        BuyToOpen( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, true, true ); }

        static OrderTicket
        BuyToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, true, true, false, price); }

        static OrderTicket
        SellToOpen( ARGS_RAW )
        { return build( symbol_option_buy, symbol_option_sell,
                               symbol_stock, quantity, false, true ); }

        static OrderTicket
        SellToOpen( ARGS_RAW, double price)
        { return build( symbol_option_buy, symbol_option_sell,
                              symbol_stock, quantity, false, true, false, price ); }

        static OrderTicket
        SellToOpen( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, false, true ); }

        static OrderTicket
        SellToOpen( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, false, true, false, price ); }

        static OrderTicket
        BuyToClose( ARGS_RAW )
        { return build( symbol_option_buy, symbol_option_sell,
                               symbol_stock, quantity, true, false ); }

        static OrderTicket
        BuyToClose( ARGS_RAW, double price)
        { return build( symbol_option_buy, symbol_option_sell,
                              symbol_stock, quantity, true, false, false, price ); }

        static OrderTicket
        BuyToClose( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, true, false ); }

        static OrderTicket
        BuyToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, true, false, false, price ); }

        static OrderTicket
        SellToClose( ARGS_RAW )
        { return build( symbol_option_buy, symbol_option_sell,
                               symbol_stock, quantity, false, false ); }

        static OrderTicket
        SellToClose( ARGS_RAW, double price)
        { return build( symbol_option_buy, symbol_option_sell,
                              symbol_stock, quantity, false, false, false, price ); }

        static OrderTicket
        SellToClose( ARGS )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, false, false ); }

        static OrderTicket
        SellToClose( ARGS, double price )
        { return build( underlying, month, day, year, strike_call, strike_put,
                        quantity, false, false, false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* CollarWithStock */


    class Condor {
    public:
        Condor() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::CONDOR;

#define ARGS_RAW(outer, inner) \
        const std::string& symbol_##outer##1,\
        const std::string& symbol_##inner##1, \
        const std::string& symbol_##inner##2, \
        const std::string& symbol_##outer##2

#define ARGS(outer, inner) \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike_##outer##1, \
        double strike_##inner##1, \
        double strike_##inner##2, \
        double strike_##outer##2, \
        bool are_calls

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, const std::string&, const std::string&,
            OrderInstruction, OrderInstruction, size_t, size_t, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, double, double, double, double, bool,
            OrderInstruction, OrderInstruction, size_t, size_t, bool, double);

        static OrderTicket
        build( ARGS_RAW(outer, inner),
                OrderInstruction instr_outer,
                OrderInstruction instr_inner,
                size_t quantity1,
                size_t quantity2,
                bool market_order = true,
                double price = 0.0 )
        {
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_outer1, instr_outer, quantity1),
                     make_leg(symbol_inner1, instr_inner, quantity1),
                     make_leg(symbol_inner2, instr_inner, quantity2),
                     make_leg(symbol_outer2, instr_outer, quantity2)},
                    market_order, price );
        }

        static OrderTicket
        build( ARGS(outer, inner),
                OrderInstruction instr_outer,
                OrderInstruction instr_inner,
                size_t quantity1,
                size_t quantity2,
                bool market_order = true,
                double price = 0.0 )
        {
            return build(
                O(underlying, month, day, year, are_calls, strike_outer1),
                O(underlying, month, day, year, are_calls, strike_inner1),
                O(underlying, month, day, year, are_calls, strike_inner2),
                O(underlying, month, day, year, are_calls, strike_outer2),
                instr_outer, instr_inner, quantity1, quantity2, market_order,
                price);
        }

        class Unbalanced{
        public:
            Unbalanced() = delete;
            static constexpr ComplexOrderStrategyType strategy
                = ComplexOrderStrategyType::UNBALANCED_CONDOR;

            template< typename... T >
            static OrderTicket
            build( T... args )
            { return Condor::build(args...)
                        .set_complex_strategy_type(strategy); }

            static OrderTicket
            BuyToOpen( ARGS_RAW(buy,sell),
                        size_t quantity1,
                        size_t quantity2 )
            { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                                   symbol_buy2, BUY_TO_OPEN,
                                   SELL_TO_OPEN, quantity1, quantity2); }

            static OrderTicket
            BuyToOpen( ARGS_RAW(buy,sell),
                        size_t quantity1,
                        size_t quantity2,
                        double price )
            { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                                  symbol_buy2, BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity1, quantity2, false, price); }

            static OrderTicket
            BuyToOpen( ARGS(buy,sell),
                        size_t quantity1,
                        size_t quantity2 )
            { return build( underlying, month, day, year, strike_buy1,
                                   strike_sell1, strike_sell2, strike_buy2,
                                   are_calls, BUY_TO_OPEN, SELL_TO_OPEN,
                                   quantity1, quantity2 ); }

            static OrderTicket
            BuyToOpen( ARGS(buy, sell),
                        size_t quantity1,
                        size_t quantity2,
                        double price )
            { return build( underlying, month, day, year, strike_buy1,
                                  strike_sell1, strike_sell2, strike_buy2,
                                  are_calls, BUY_TO_OPEN, SELL_TO_OPEN,
                                  quantity1, quantity2, false, price ); }

            static OrderTicket
            SellToOpen( ARGS_RAW(sell,buy),
                         size_t quantity1,
                         size_t quantity2 )
            { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                                   symbol_sell2, SELL_TO_OPEN,
                                   BUY_TO_OPEN, quantity1, quantity2); }

            static OrderTicket
            SellToOpen( ARGS_RAW(sell,buy),
                        size_t quantity1,
                        size_t quantity2,
                        double price )
            { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                                  symbol_sell2, SELL_TO_OPEN,
                                  BUY_TO_OPEN, quantity1, quantity2,
                                  false, price ); }

            static OrderTicket
            SellToOpen( ARGS(sell,buy),
                         size_t quantity1,
                         size_t quantity2 )
            { return build( underlying, month, day, year, strike_sell1,
                                   strike_buy1, strike_buy2, strike_sell2,
                                   are_calls, SELL_TO_OPEN, BUY_TO_OPEN,
                                   quantity1, quantity2 ); }

            static OrderTicket
            SellToOpen( ARGS(sell,buy),
                        size_t quantity1,
                        size_t quantity2,
                        double price )
            { return build( underlying, month, day, year, strike_sell1,
                                  strike_buy1, strike_buy2, strike_sell2,
                                  are_calls, SELL_TO_OPEN, BUY_TO_OPEN,
                                  quantity1, quantity2, false, price ); }

            static OrderTicket
            BuyToClose( ARGS_RAW(buy,sell),
                         size_t quantity1,
                         size_t quantity2 )
            { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                                   symbol_buy2, BUY_TO_CLOSE,
                                   SELL_TO_CLOSE, quantity1, quantity2 ); }

            static OrderTicket
            BuyToClose( ARGS_RAW(buy,sell),
                         size_t quantity1,
                         size_t quantity2,
                         double price )
            { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                                  symbol_buy2, BUY_TO_CLOSE,
                                  SELL_TO_CLOSE, quantity1, quantity2,
                                  false, price ); }

            static OrderTicket
            BuyToClose( ARGS(buy,sell),
                         size_t quantity1,
                         size_t quantity2 )
            { return build( underlying, month, day, year, strike_buy1,
                                   strike_sell1, strike_sell2, strike_buy2,
                                   are_calls, BUY_TO_CLOSE,
                                   SELL_TO_CLOSE, quantity1, quantity2 ); }

            static OrderTicket
            BuyToClose( ARGS(buy,sell),
                        size_t quantity1,
                        size_t quantity2,
                        double price )
            { return build( underlying, month, day, year,
                                   strike_buy1, strike_sell1, strike_sell2,
                                   strike_buy2, are_calls, BUY_TO_CLOSE,
                                   SELL_TO_CLOSE, quantity1, quantity2,
                                   false, price ); }

            static OrderTicket
            SellToClose( ARGS_RAW(sell,buy),
                          size_t quantity1,
                          size_t quantity2 )
            { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                                   symbol_sell2, SELL_TO_CLOSE,
                                   BUY_TO_CLOSE, quantity1, quantity2 ); }

            static OrderTicket
            SellToClose( ARGS_RAW(sell,buy),
                          size_t quantity1,
                          size_t quantity2,
                          double price )
            { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                                  symbol_sell2, SELL_TO_CLOSE,
                                  BUY_TO_CLOSE, quantity1, quantity2,
                                  false, price ); }

            static OrderTicket
            SellToClose( ARGS(sell,buy),
                        size_t quantity1,
                        size_t quantity2 )
            { return build( underlying, month, day, year,
                                   strike_sell1, strike_buy1, strike_buy2,
                                   strike_sell2, are_calls, SELL_TO_CLOSE,
                                   BUY_TO_CLOSE, quantity1, quantity2 ); }

            static OrderTicket
            SellToClose( ARGS(sell,buy),
                          size_t quantity1,
                          size_t quantity2,
                          double price )
            { return build( underlying, month, day, year,
                                  strike_sell1, strike_buy1, strike_buy2,
                                  strike_sell2, are_calls, SELL_TO_CLOSE,
                                  BUY_TO_CLOSE, quantity1, quantity2,
                                  false, price ); }
        }; /* Unbalanced */


        static OrderTicket
        BuyToOpen( ARGS_RAW(buy,sell), size_t quantity )
        { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                               symbol_buy2, BUY_TO_OPEN,
                               SELL_TO_OPEN, quantity, quantity); }

        static OrderTicket
        BuyToOpen( ARGS_RAW(buy,sell), size_t quantity, double price )
        { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                               symbol_buy2, BUY_TO_OPEN,
                               SELL_TO_OPEN, quantity, quantity, false, price); }

        static OrderTicket
        BuyToOpen( ARGS(buy,sell), size_t quantity )
        { return build( underlying, month, day, year,
                               strike_buy1, strike_sell1, strike_sell2,
                               strike_buy2, are_calls, BUY_TO_OPEN,
                               SELL_TO_OPEN, quantity, quantity); }

        static OrderTicket
        BuyToOpen( ARGS(buy,sell), size_t quantity, double price )
        { return build( underlying, month, day, year,
                               strike_buy1, strike_sell1, strike_sell2,
                               strike_buy2, are_calls, BUY_TO_OPEN,
                               SELL_TO_OPEN, quantity, quantity, false, price); }

        static OrderTicket
        SellToOpen( ARGS_RAW(sell,buy), size_t quantity )
        { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                               symbol_sell2, SELL_TO_OPEN,
                               BUY_TO_OPEN, quantity, quantity); }

        static OrderTicket
        SellToOpen( ARGS_RAW(sell,buy), size_t quantity, double price )
        { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                               symbol_sell2, SELL_TO_OPEN,
                               BUY_TO_OPEN, quantity, quantity, false, price); }

        static OrderTicket
        SellToOpen( ARGS(sell,buy), size_t quantity )
        { return build( underlying, month, day, year,
                               strike_sell1, strike_buy1, strike_buy2,
                               strike_sell2, are_calls, SELL_TO_OPEN,
                               BUY_TO_OPEN, quantity, quantity); }

        static OrderTicket
        SellToOpen( ARGS(sell,buy), size_t quantity, double price )
        { return build( underlying, month, day, year,
                               strike_sell1, strike_buy1, strike_buy2,
                               strike_sell2, are_calls, SELL_TO_OPEN,
                               BUY_TO_OPEN, quantity, quantity, false, price); }

        static OrderTicket
        BuyToClose( ARGS_RAW(buy,sell), size_t quantity )
        { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                               symbol_buy2, BUY_TO_CLOSE,
                               SELL_TO_CLOSE, quantity, quantity); }

        static OrderTicket
        BuyToClose( ARGS_RAW(buy,sell), size_t quantity, double price )
        { return build( symbol_buy1, symbol_sell1, symbol_sell2,
                               symbol_buy2, BUY_TO_CLOSE,
                               SELL_TO_CLOSE, quantity, quantity, false, price); }

        static OrderTicket
        BuyToClose( ARGS(buy,sell), size_t quantity )
        { return build( underlying, month, day, year,
                               strike_buy1, strike_sell1, strike_sell2,
                               strike_buy2, are_calls, BUY_TO_CLOSE,
                               SELL_TO_CLOSE, quantity, quantity); }

        static OrderTicket
        BuyToClose( ARGS(buy,sell), size_t quantity, double price )
        { return build( underlying, month, day, year,
                               strike_buy1, strike_sell1, strike_sell2,
                               strike_buy2, are_calls, BUY_TO_CLOSE,
                               SELL_TO_CLOSE, quantity, quantity, false, price); }

        static OrderTicket
        SellToClose( ARGS_RAW(sell,buy), size_t quantity )
        { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                               symbol_sell2, SELL_TO_CLOSE,
                               BUY_TO_CLOSE, quantity, quantity); }

        static OrderTicket
        SellToClose( ARGS_RAW(sell,buy), size_t quantity, double price )
        { return build( symbol_sell1, symbol_buy1, symbol_buy2,
                               symbol_sell2, SELL_TO_CLOSE,
                               BUY_TO_CLOSE, quantity, quantity, false, price); }

        static OrderTicket
        SellToClose( ARGS(sell,buy), size_t quantity )
        { return build( underlying, month, day, year,
                               strike_sell1, strike_buy1, strike_buy2,
                               strike_sell2, are_calls, SELL_TO_CLOSE,
                               BUY_TO_CLOSE, quantity, quantity); }

        static OrderTicket
        SellToClose( ARGS(sell,buy), size_t quantity, double price )
        { return build( underlying, month, day, year,
                               strike_sell1, strike_buy1, strike_buy2,
                               strike_sell2, are_calls, SELL_TO_CLOSE,
                               BUY_TO_CLOSE, quantity, quantity, false, price); }
#undef ARGS_RAW
#undef ARGS
    }; /* Condor */


    class IronCondor {
    public:
        IronCondor() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::IRON_CONDOR;

#define ARGS_RAW \
        const std::string& symbol_call_buy, \
        const std::string& symbol_call_sell, \
        const std::string& symbol_put_buy, \
        const std::string& symbol_put_sell

#define ARGS \
        const std::string& underlying, \
        unsigned int month, \
        unsigned int day, \
        unsigned int year, \
        double strike_call_buy, \
        double strike_call_sell, \
        double strike_put_buy, \
        double strike_put_sell

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, const std::string&, const std::string&,
            size_t, size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, double, double, double, double,
            size_t, size_t, bool, bool, double);

        static OrderTicket
        build( ARGS_RAW,
                size_t quantity_call,
                size_t quantity_put,
                bool to_open,
                bool market_order = true,
                double price = 0.0 )
        {
            OrderInstruction b = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction s = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_call_buy, b, quantity_call),
                     make_leg(symbol_call_sell, s, quantity_call),
                     make_leg(symbol_put_buy, b, quantity_put),
                     make_leg(symbol_put_sell, s, quantity_put)},
                    market_order, price );
        }

        static OrderTicket
        build( ARGS,
                size_t quantity_call,
                size_t quantity_put,
                bool to_open,
                bool market_order = true,
                double price = 0.0 )
        {
            return build(
                O(underlying, month, day, year, true, strike_call_buy),
                O(underlying, month, day, year, true, strike_call_sell),
                O(underlying, month, day, year, false, strike_put_buy),
                O(underlying, month, day, year, false, strike_put_sell),
                quantity_call, quantity_put, to_open, market_order, price);
        }

        class Unbalanced{
        public:
            Unbalanced() = delete;
            static constexpr ComplexOrderStrategyType strategy
                = ComplexOrderStrategyType::UNBALANCED_IRON_CONDOR;

            template< typename... T >
            static OrderTicket
            build( T... args )
            { return IronCondor::build(args...)
                        .set_complex_strategy_type(strategy); }

            static OrderTicket
            Open( ARGS_RAW, size_t quantity_call, size_t quantity_put )
            { return build( symbol_call_buy, symbol_call_sell,
                                   symbol_put_buy, symbol_put_sell,
                                   quantity_call, quantity_put, true ); }

            static OrderTicket
            Open( ARGS_RAW,
                   size_t quantity_call,
                   size_t quantity_put,
                   double price )
            { return build( symbol_call_buy, symbol_call_sell,
                                  symbol_put_buy, symbol_put_sell,
                                  quantity_call, quantity_put, true, false, price ); }

            static OrderTicket
            Open( ARGS, size_t quantity_call, size_t quantity_put )
            { return build( underlying, month, day, year,
                                   strike_call_buy, strike_call_sell,
                                   strike_put_buy, strike_put_sell,
                                   quantity_call, quantity_put, true); }

            static OrderTicket
            Open( ARGS,
                  size_t quantity_call,
                  size_t quantity_put,
                  double price )
            { return build( underlying, month, day, year, strike_call_buy,
                                  strike_call_sell, strike_put_buy,
                                  strike_put_sell, quantity_call,
                                  quantity_put, true, false, price ); }

            static OrderTicket
            Close( ARGS_RAW, size_t quantity_call, size_t quantity_put )
            { return build( symbol_call_buy, symbol_call_sell,
                                   symbol_put_buy, symbol_put_sell,
                                   quantity_call, quantity_put, false ); }

            static OrderTicket
            Close( ARGS_RAW,
                    size_t quantity_call,
                    size_t quantity_put,
                    double price )
            { return build( symbol_call_buy, symbol_call_sell,
                                  symbol_put_buy, symbol_put_sell,
                                  quantity_call, quantity_put, false, false, price ); }

            static OrderTicket
            Close( ARGS, size_t quantity_call, size_t quantity_put )
            { return build( underlying, month, day, year,
                                   strike_call_buy, strike_call_sell,
                                   strike_put_buy, strike_put_sell,
                                   quantity_call, quantity_put, false ); }

            static OrderTicket
            Close( ARGS,
                    size_t quantity_call,
                    size_t quantity_put,
                    double price )
            { return build( underlying, month, day, year, strike_call_buy,
                                  strike_call_sell, strike_put_buy,
                                  strike_put_sell, quantity_call,
                                  quantity_put, false, false, price ); }
        }; /* Unbalanced */


        static OrderTicket
        Open( ARGS_RAW, size_t quantity )
        { return build( symbol_call_buy, symbol_call_sell,
                               symbol_put_buy, symbol_put_sell, quantity,
                               quantity, true ); }

        static OrderTicket
        Open( ARGS_RAW, size_t quantity, double price )
        { return build( symbol_call_buy, symbol_call_sell,
                              symbol_put_buy, symbol_put_sell, quantity,
                              quantity, true, false, price ); }

        static OrderTicket
        Open( ARGS, size_t quantity )
        { return build( underlying, month, day, year, strike_call_buy,
                               strike_call_sell, strike_put_buy,
                               strike_put_sell, quantity, quantity, true); }

        static OrderTicket
        Open( ARGS, size_t quantity, double price )
        { return build( underlying, month, day, year, strike_call_buy,
                              strike_call_sell, strike_put_buy, strike_put_sell,
                              quantity, quantity, true, false, price ); }

        static OrderTicket
        Close( ARGS_RAW, size_t quantity )
        { return build( symbol_call_buy, symbol_call_sell,
                               symbol_put_buy, symbol_put_sell, quantity,
                               quantity, false ); }

        static OrderTicket
        Close( ARGS_RAW, size_t quantity, double price )
        { return build( symbol_call_buy, symbol_call_sell, symbol_put_buy,
                              symbol_put_sell, quantity, quantity, false,
                              false, price ); }

        static OrderTicket
        Close( ARGS, size_t quantity )
        { return build( underlying, month, day, year, strike_call_buy,
                               strike_call_sell, strike_put_buy,
                               strike_put_sell, quantity, quantity, false); }

        static OrderTicket
        Close( ARGS, size_t quantity, double price )
        { return build( underlying, month, day, year, strike_call_buy,
                              strike_call_sell, strike_put_buy, strike_put_sell,
                              quantity, quantity, false, false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* IronCondor */


    class DoubleDiagonal {
    public:
        DoubleDiagonal() = delete;
        static constexpr ComplexOrderStrategyType strategy
            = ComplexOrderStrategyType::DOUBLE_DIAGONAL;

#define ARGS_RAW \
        const std::string& symbol_call_buy, \
        const std::string& symbol_call_sell, \
        const std::string& symbol_put_buy, \
        const std::string& symbol_put_sell, \
        size_t quantity

#define ARGS \
        const std::string& underlying, \
        unsigned int month_buy, \
        unsigned int day_buy, \
        unsigned int year_buy, \
        unsigned int month_sell, \
        unsigned int day_sell, \
        unsigned int year_sell, \
        double strike_call_buy, \
        double strike_call_sell, \
        double strike_put_buy, \
        double strike_put_sell, \
        size_t quantity

        typedef OrderTicket(*raw_build_meth_ty)(const std::string&,
            const std::string&, const std::string&, const std::string&,
            size_t, bool, bool, double);

        typedef OrderTicket(*ex_build_meth_ty)(const std::string&, unsigned int,
            unsigned int, unsigned int, unsigned int, unsigned int,
            unsigned int, double, double, double, double, size_t, bool, bool,
            double);

        static OrderTicket
        build( ARGS_RAW, bool to_open, bool market_order = true, double price = 0.0 )
        {
            OrderInstruction b = to_open ? BUY_TO_OPEN : BUY_TO_CLOSE;
            OrderInstruction s = to_open ? SELL_TO_OPEN : SELL_TO_CLOSE;
            return SpreadOrderBuilder::build( strategy,
                    {make_leg(symbol_call_buy, b, quantity),
                     make_leg(symbol_call_sell, s, quantity),
                     make_leg(symbol_put_buy, b, quantity),
                     make_leg(symbol_put_sell, s, quantity)},
                    market_order, price );
        }

        static OrderTicket
        build( ARGS, bool to_open, bool market_order = true, double price = 0.0 )
        {
            return build(
                O(underlying, month_buy, day_buy, year_buy, true,
                    strike_call_buy),
                O(underlying, month_sell, day_sell, year_sell, true,
                    strike_call_sell),
                O(underlying, month_buy, day_buy, year_buy, false,
                    strike_put_buy),
                O(underlying, month_sell, day_sell, year_sell, false,
                    strike_put_sell),
                quantity, to_open, market_order, price);
        }

        static OrderTicket
        Open( ARGS_RAW )
        { return build( symbol_call_buy, symbol_call_sell,
                               symbol_put_buy, symbol_put_sell, quantity,
                               true ); }

        static OrderTicket
        Open( ARGS_RAW, double price )
        { return build( symbol_call_buy, symbol_call_sell, symbol_put_buy,
                              symbol_put_sell, quantity, true, false, price); }

        static OrderTicket
        Open( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, strike_call_buy,
                               strike_call_sell, strike_put_buy,
                               strike_put_sell, quantity, true); }

        static OrderTicket
        Open( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                              month_sell, day_sell, year_sell, strike_call_buy,
                              strike_call_sell, strike_put_buy, strike_put_sell,
                              quantity, true, false, price ); }

        static OrderTicket
        Close( ARGS_RAW )
        { return build( symbol_call_buy, symbol_call_sell,
                               symbol_put_buy, symbol_put_sell, quantity,
                               false ); }

        static OrderTicket
        Close( ARGS_RAW, double price )
        { return build( symbol_call_buy, symbol_call_sell, symbol_put_buy,
                              symbol_put_sell, quantity, false, false, price ); }

        static OrderTicket
        Close( ARGS )
        { return build( underlying, month_buy, day_buy, year_buy,
                               month_sell, day_sell, year_sell, strike_call_buy,
                               strike_call_sell, strike_put_buy,
                               strike_put_sell, quantity, false ); }

        static OrderTicket
        Close( ARGS, double price )
        { return build( underlying, month_buy, day_buy, year_buy,
                              month_sell, day_sell, year_sell, strike_call_buy,
                              strike_call_sell, strike_put_buy, strike_put_sell,
                              quantity, false, false, price ); }
#undef ARGS_RAW
#undef ARGS
    }; /* DoubleDiagonal */

}; /* OptionSpreadBuilder */


class ConditionalOrderBuilder
        : public OrderBuilderBase {
public:
    ConditionalOrderBuilder() = delete;

    static OrderTicket
    OTO(const OrderTicket& primary, const OrderTicket& conditional)
    {
        return OrderTicket(primary).set_strategy_type(OrderStrategyType::TRIGGER)
                             .add_child(conditional);
    }

    static OrderTicket
    OCO(const OrderTicket& order1, const OrderTicket& order2)
    {
        return OrderTicket().set_strategy_type(OrderStrategyType::OCO)
                      .add_child(order1).add_child(order2);
    }
};

/*
{
    "session": "'NORMAL' or 'AM' or 'PM' or 'SEAMLESS'",
    "duration": "'DAY' or 'GOOD_TILL_CANCEL' or 'FILL_OR_KILL'",
    "orderType": "'MARKET' or 'LIMIT' or 'STOP' or 'STOP_LIMIT' or 'TRAILING_STOP' or 'MARKET_ON_CLOSE' or 'EXERCISE' or 'TRAILING_STOP_LIMIT' or 'NET_DEBIT' or 'NET_CREDIT' or 'NET_ZERO'",
    "cancelTime": {
        "date": "string",
        "shortFormat": false
    },
    "complexOrderStrategyType": "'NONE' or 'COVERED' or 'VERTICAL' or 'BACK_RATIO' or 'CALENDAR' or 'DIAGONAL' or 'STRADDLE' or 'STRANGLE' or 'COLLAR_SYNTHETIC' or 'BUTTERFLY' or 'CONDOR' or 'IRON_CONDOR' or 'VERTICAL_ROLL' or 'COLLAR_WITH_STOCK' or 'DOUBLE_DIAGONAL' or 'UNBALANCED_BUTTERFLY' or 'UNBALANCED_CONDOR' or 'UNBALANCED_IRON_CONDOR' or 'UNBALANCED_VERTICAL_ROLL' or 'CUSTOM'",
    "quantity": 0,
    "filledQuantity": 0,
    "remainingQuantity": 0,
    "requestedDestination": "'INET' or 'ECN_ARCA' or 'CBOE' or 'AMEX' or 'PHLX' or 'ISE' or 'BOX' or 'NYSE' or 'NASDAQ' or 'BATS' or 'C2' or 'AUTO'",
    "destinationLinkName": "string",
    "releaseTime": "string",
    "stopPrice": 0,
    "stopPriceLinkBasis": "'MANUAL' or 'BASE' or 'TRIGGER' or 'LAST' or 'BID' or 'ASK' or 'ASK_BID' or 'MARK' or 'AVERAGE'",
    "stopPriceLinkType": "'VALUE' or 'PERCENT' or 'TICK'",
    "stopPriceOffset": 0,
    "stopType": "'STANDARD' or 'BID' or 'ASK' or 'LAST' or 'MARK'",
    "priceLinkBasis": "'MANUAL' or 'BASE' or 'TRIGGER' or 'LAST' or 'BID' or 'ASK' or 'ASK_BID' or 'MARK' or 'AVERAGE'",
    "priceLinkType": "'VALUE' or 'PERCENT' or 'TICK'",
    "price": 0,
    "taxLotMethod": "'FIFO' or 'LIFO' or 'HIGH_COST' or 'LOW_COST' or 'AVERAGE_COST' or 'SPECIFIC_LOT'",
    "orderLegCollection": [
        {
            "orderLegType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
            "legId": 0,
            "instrument": "The type <Instrument> has the following subclasses [Option, MutualFund, CashEquivalent, Equity, FixedIncome] descriptions are listed below\"",
            "instruction": "'BUY' or 'SELL' or 'BUY_TO_COVER' or 'SELL_SHORT' or 'BUY_TO_OPEN' or 'BUY_TO_CLOSE' or 'SELL_TO_OPEN' or 'SELL_TO_CLOSE' or 'EXCHANGE'",
            "positionEffect": "'OPENING' or 'CLOSING' or 'AUTOMATIC'",
            "quantity": 0,
            "quantityType": "'ALL_SHARES' or 'DOLLARS' or 'SHARES'"
        }
    ],
    "activationPrice": 0,
    "specialInstruction": "'ALL_OR_NONE' or 'DO_NOT_REDUCE' or 'ALL_OR_NONE_DO_NOT_REDUCE'",
    "orderStrategyType": "'SINGLE' or 'OCO' or 'TRIGGER'",
    "orderId": 0,
    "cancelable": false,
    "editable": false,
    "status": "'AWAITING_PARENT_ORDER' or 'AWAITING_CONDITION' or 'AWAITING_MANUAL_REVIEW' or 'ACCEPTED' or 'AWAITING_UR_OUT' or 'PENDING_ACTIVATION' or 'QUEUED' or 'WORKING' or 'REJECTED' or 'PENDING_CANCEL' or 'CANCELED' or 'PENDING_REPLACE' or 'REPLACED' or 'FILLED' or 'EXPIRED'",
    "enteredTime": "string",
    "closeTime": "string",
    "tag": "string",
    "accountId": 0,
    "orderActivityCollection": [
        "\"The type <OrderActivity> has the following subclasses [Execution] descriptions are listed below\""
    ],
    "replacingOrderCollection": [
        {}
    ],
    "childOrderStrategies": [
        {}
    ],
    "statusDescription": "string"
}

//The class <Instrument> has the
//following subclasses:
//-Option
//-MutualFund
//-CashEquivalent
//-Equity
//-FixedIncome
//JSON for each are listed below:

//Option:
{
  "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
  "cusip": "string",
  "symbol": "string",
  "description": "string",
  "type": "'VANILLA' or 'BINARY' or 'BARRIER'",
  "putCall": "'PUT' or 'CALL'",
  "underlyingSymbol": "string",
  "optionMultiplier": 0,
  "optionDeliverables": [
    {
      "symbol": "string",
      "deliverableUnits": 0,
      "currencyType": "'USD' or 'CAD' or 'EUR' or 'JPY'",
      "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'"
    }
  ]
}

//OR

//MutualFund:
{
  "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
  "cusip": "string",
  "symbol": "string",
  "description": "string",
  "type": "'NOT_APPLICABLE' or 'OPEN_END_NON_TAXABLE' or 'OPEN_END_TAXABLE' or 'NO_LOAD_NON_TAXABLE' or 'NO_LOAD_TAXABLE'"
}

//OR

//CashEquivalent:
{
  "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
  "cusip": "string",
  "symbol": "string",
  "description": "string",
  "type": "'SAVINGS' or 'MONEY_MARKET_FUND'"
}

//OR

//Equity:
{
  "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
  "cusip": "string",
  "symbol": "string",
  "description": "string"
}

//OR

//FixedIncome:
{
  "assetType": "'EQUITY' or 'OPTION' or 'INDEX' or 'MUTUAL_FUND' or 'CASH_EQUIVALENT' or 'FIXED_INCOME' or 'CURRENCY'",
  "cusip": "string",
  "symbol": "string",
  "description": "string",
  "maturityDate": "string",
  "variableRate": 0,
  "factor": 0
}

//The class <OrderActivity> has the
//following subclasses:
//-Execution
//JSON for each are listed below:

//Execution:
{
  "activityType": "'EXECUTION' or 'ORDER_ACTION'",
  "executionType": "'FILL'",
  "quantity": 0,
  "orderRemainingQuantity": 0,
  "executionLegs": [
    {
      "legId": 0,
      "quantity": 0,
      "mismarkedQuantity": 0,
      "price": 0,
      "time": "string"
    }
  ]
}
 */

} /* tdma */

#undef THROW_VALUE_EXCEPTION

#endif /* __cplusplus */

#endif /* TDMA_API_EXECUTE_H */


