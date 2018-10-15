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

#include "../../include/_tdma_api.h"
#include "../../include/_execute.h"

using namespace std;
using namespace conn;

namespace tdma{

std::string
Execute_SendOrderImpl( Credentials& creds,
                           const std::string& account_id,
                           const OrderTicketImpl& order )
{
    string url = URL_ACCOUNTS + util::url_encode(account_id) + "/orders";
    string body = order.as_json_string();

    if( body.empty() )
        TDMA_API_THROW(ValueException, "order json is empty");

    HTTPSPostConnection connection;
    connection.SET_url(url);
    connection.SET_fields(body);

    string order_id;
    conn::clock_ty::time_point tp;
    tie(order_id, tp) = connect_order_send( connection, creds );
    return order_id;
}

bool
Execute_CancelOrderImpl( Credentials& creds,
                             const std::string& account_id,
                             const std::string& order_id )
{
    string url = URL_ACCOUNTS + util::url_encode(account_id)
               + "/orders/" + util::url_encode(order_id); // encode uncessary

    HTTPSDeleteConnection connection;
    connection.SET_url(url);

    bool success;
    conn::clock_ty::time_point tp;
    tie(success, tp) = connect_order_cancel(connection, creds);
    return success;

}


} /* tdma */

using namespace tdma;


int
Execute_SendOrder_ABI( Credentials *creds,
                           const char* account_id,
                           OrderTicket_C *porder,
                           char** buf,
                           size_t *n,
                           int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
         return err;

    CHECK_PTR(account_id, "account id", allow_exceptions);
    CHECK_PTR(buf, "buf", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth =
        +[]( Credentials *c, const char* id, OrderTicket_C* porder ){
            return Execute_SendOrderImpl(
                *c, id, *reinterpret_cast<OrderTicketImpl*>(porder->obj)
                );
        };

    std::string r;
    std::tie(r,err) = CallImplFromABI( allow_exceptions, meth, creds,
                                       account_id, porder );
    if( err )
        return err;

    *n = r.size() + 1; // NULL TERM
    *buf = reinterpret_cast<char*>(malloc(*n));
    if( !*buf ){
        return HANDLE_ERROR( tdma::MemoryError,
            "failed to allocate buffer memory", allow_exceptions
            );
    }
    (*buf)[(*n)-1] = 0;
    strncpy(*buf, r.c_str(), (*n)-1);
    return 0;
}

int
Execute_CancelOrder_ABI( Credentials *creds,
                             const char* account_id,
                             const char* order_id,
                             int *success,
                             int allow_exceptions )
{
    CHECK_PTR(account_id, "account id", allow_exceptions);
    CHECK_PTR(order_id, "order id", allow_exceptions);

    static auto meth =
        +[]( Credentials *c, const char* aid, const char* oid ){
            return Execute_CancelOrderImpl(*c, aid, oid);
        };

    int err;
    std::tie(*success,err) = CallImplFromABI( allow_exceptions, meth, creds,
                                              account_id, order_id);
    return err;
}


int
OrderSession_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderSession, v, allow_exceptions);

    switch(static_cast<OrderSession>(v)){
    case OrderSession::NORMAL:
        return alloc_C_str("NORMAL", buf, n, allow_exceptions);
    case OrderSession::AM:
        return alloc_C_str("AM", buf, n, allow_exceptions);
    case OrderSession::PM:
        return alloc_C_str("PM", buf, n, allow_exceptions);
    case OrderSession::SEAMLESS:
        return alloc_C_str("SEAMLESS", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderSession");
    }
}

int
OrderDuration_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderDuration, v, allow_exceptions);

    switch(static_cast<OrderDuration>(v)){
    case OrderDuration::DAY:
        return alloc_C_str("DAY", buf, n, allow_exceptions);
    case OrderDuration::GOOD_TILL_CANCEL:
        return alloc_C_str("GOOD_TILL_CANCEL", buf, n, allow_exceptions);
    case OrderDuration::FILL_OR_KILL:
        return alloc_C_str("FILL_OR_KILL", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderDuration");
    }
}


int
OrderAssetType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderAssetType, v, allow_exceptions);

    switch(static_cast<OrderAssetType>(v)){
    case OrderAssetType::EQUITY:
        return alloc_C_str("EQUITY", buf, n, allow_exceptions);
    case OrderAssetType::OPTION:
        return alloc_C_str("OPTION", buf, n, allow_exceptions);
    case OrderAssetType::INDEX:
        return alloc_C_str("INDEX", buf, n, allow_exceptions);
    case OrderAssetType::MUTUAL_FUND:
        return alloc_C_str("MUTUAL_FUND", buf, n, allow_exceptions);
    case OrderAssetType::CASH_EQUIVALENT:
        return alloc_C_str("CASH_EQUIVALENT", buf, n, allow_exceptions);
    case OrderAssetType::FIXED_INCOME:
        return alloc_C_str("FIXED_INCOME", buf, n, allow_exceptions);
    case OrderAssetType::CURRENCY:
        return alloc_C_str("CURRENCY", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderAssetType");
    }
}


int
OrderInstruction_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderInstruction, v, allow_exceptions);

    switch(static_cast<OrderInstruction>(v)){
    case OrderInstruction::BUY:
        return alloc_C_str("BUY", buf, n, allow_exceptions);
    case OrderInstruction::SELL:
        return alloc_C_str("SELL", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_COVER:
        return alloc_C_str("BUY_TO_COVER", buf, n, allow_exceptions);
    case OrderInstruction::SELL_SHORT:
        return alloc_C_str("SELL_SHORT", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_OPEN:
        return alloc_C_str("BUY_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_CLOSE:
        return alloc_C_str("BUY_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_OPEN:
        return alloc_C_str("SELL_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_CLOSE:
        return alloc_C_str("SELL_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::EXCHANGE:
        return alloc_C_str("EXCHANGE", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderInstruction");
    }
}


int
OrderType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderType, v, allow_exceptions);

    switch(static_cast<OrderType>(v)){
    case OrderType::MARKET:
        return alloc_C_str("MARKET", buf, n, allow_exceptions);
    case OrderType::LIMIT:
        return alloc_C_str("LIMIT", buf, n, allow_exceptions);
    case OrderType::STOP:
        return alloc_C_str("STOP", buf, n, allow_exceptions);
    case OrderType::STOP_LIMIT:
        return alloc_C_str("STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP:
        return alloc_C_str("TRAILING_STOP", buf, n, allow_exceptions);
    case OrderType::MARKET_ON_CLOSE:
        return alloc_C_str("MARKET_ON_CLOSE", buf, n, allow_exceptions);
    case OrderType::EXERCISE:
        return alloc_C_str("EXERCISE", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP_LIMIT:
        return alloc_C_str("TRAILING_STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::NET_DEBIT:
        return alloc_C_str("NET_DEBIT", buf, n, allow_exceptions);
    case OrderType::NET_CREDIT:
        return alloc_C_str("NET_CREDIT", buf, n, allow_exceptions);
    case OrderType::NET_ZERO:
        return alloc_C_str("NET_ZERO", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderType");
    }
}


int
ComplexOrderStrategyType_to_string_ABI(
    int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(ComplexOrderStrategyType, v, allow_exceptions);

    switch(static_cast<ComplexOrderStrategyType>(v)){
    case ComplexOrderStrategyType::NONE:
        return alloc_C_str("NONE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COVERED:
        return alloc_C_str("COVERED", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL:
        return alloc_C_str("VERTICAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BACK_RATIO:
        return alloc_C_str("BACK_RATIO", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CALENDAR:
        return alloc_C_str("CALENDAR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DIAGONAL:
        return alloc_C_str("DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRADDLE:
        return alloc_C_str("STRADDLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRANGLE:
        return alloc_C_str("STRANGLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_SYNTHETIC:
        return alloc_C_str("COLLAR_SYNTHETIC", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BUTTERFLY:
        return alloc_C_str("BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CONDOR:
        return alloc_C_str("CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::IRON_CONDOR:
        return alloc_C_str("IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL_ROLL:
        return alloc_C_str("VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_WITH_STOCK:
        return alloc_C_str("COLLAR_WITH_STOCK", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DOUBLE_DIAGONAL:
        return alloc_C_str("DOUBLE_DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_BUTTERFLY:
        return alloc_C_str("UNBALANCED_BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_CONDOR:
        return alloc_C_str("UNBALANCED_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_IRON_CONDOR:
        return alloc_C_str("UNBALANCED_IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_VERTICAL_ROLL:
        return alloc_C_str("UNBALANCED_VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CUSTOM:
        return alloc_C_str("CUSTOM", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid ComplexOrderStrategyType");
    }
}


int
OrderStrategyType_to_string_ABI(int v, char** buf, size_t* n, int allow_exceptions)
{
    CHECK_ENUM(OrderStrategyType, v, allow_exceptions);

    switch(static_cast<OrderStrategyType>(v)){
    case OrderStrategyType::SINGLE:
        return alloc_C_str("SINGLE", buf, n, allow_exceptions);
    case OrderStrategyType::OCO:
        return alloc_C_str("OCO", buf, n, allow_exceptions);
    case OrderStrategyType::TRIGGER:
        return alloc_C_str("TRIGGER", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderStrategyType");
    }
}

