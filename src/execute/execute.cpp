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
#include <iostream>

#include "../../include/_tdma_api.h"
#include "../../include/_execute.h"

using namespace std;
using namespace conn;

namespace tdma{

std::string
order_id_from_header(const std::string& header)
{
    static const regex ID_RX(
        "Location:[ ]*https://api\\.tdameritrade\\.com/.+/[0-9]+/orders/"
        "([0-9]+)[ ]*[\r\n]+"
    );

    smatch m;
    regex_search(header, m, ID_RX);
    if( m.ready() && m.size() == 2 )
        return m[1];

    cerr<< "failed to find order ID in header" << endl;
    return "";
}

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

    string r_head;
    conn::clock_ty::time_point r_tp;
    tie(r_head, r_tp) =
        connect_execute(connection, creds, HTTP_RESPONSE_CREATED);
    return order_id_from_header(r_head);
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

    // TODO catch exceptions and return fail state ??
    connect_execute(connection, creds, HTTP_RESPONSE_OK);
    return true;
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

    return to_new_char_buffer(r, buf, n, allow_exceptions);
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
        return to_new_char_buffer("NORMAL", buf, n, allow_exceptions);
    case OrderSession::AM:
        return to_new_char_buffer("AM", buf, n, allow_exceptions);
    case OrderSession::PM:
        return to_new_char_buffer("PM", buf, n, allow_exceptions);
    case OrderSession::SEAMLESS:
        return to_new_char_buffer("SEAMLESS", buf, n, allow_exceptions);
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
        return to_new_char_buffer("DAY", buf, n, allow_exceptions);
    case OrderDuration::GOOD_TILL_CANCEL:
        return to_new_char_buffer("GOOD_TILL_CANCEL", buf, n, allow_exceptions);
    case OrderDuration::FILL_OR_KILL:
        return to_new_char_buffer("FILL_OR_KILL", buf, n, allow_exceptions);
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
        return to_new_char_buffer("EQUITY", buf, n, allow_exceptions);
    case OrderAssetType::OPTION:
        return to_new_char_buffer("OPTION", buf, n, allow_exceptions);
    case OrderAssetType::INDEX:
        return to_new_char_buffer("INDEX", buf, n, allow_exceptions);
    case OrderAssetType::MUTUAL_FUND:
        return to_new_char_buffer("MUTUAL_FUND", buf, n, allow_exceptions);
    case OrderAssetType::CASH_EQUIVALENT:
        return to_new_char_buffer("CASH_EQUIVALENT", buf, n, allow_exceptions);
    case OrderAssetType::FIXED_INCOME:
        return to_new_char_buffer("FIXED_INCOME", buf, n, allow_exceptions);
    case OrderAssetType::CURRENCY:
        return to_new_char_buffer("CURRENCY", buf, n, allow_exceptions);
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
        return to_new_char_buffer("BUY", buf, n, allow_exceptions);
    case OrderInstruction::SELL:
        return to_new_char_buffer("SELL", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_COVER:
        return to_new_char_buffer("BUY_TO_COVER", buf, n, allow_exceptions);
    case OrderInstruction::SELL_SHORT:
        return to_new_char_buffer("SELL_SHORT", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_OPEN:
        return to_new_char_buffer("BUY_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::BUY_TO_CLOSE:
        return to_new_char_buffer("BUY_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_OPEN:
        return to_new_char_buffer("SELL_TO_OPEN", buf, n, allow_exceptions);
    case OrderInstruction::SELL_TO_CLOSE:
        return to_new_char_buffer("SELL_TO_CLOSE", buf, n, allow_exceptions);
    case OrderInstruction::EXCHANGE:
        return to_new_char_buffer("EXCHANGE", buf, n, allow_exceptions);
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
        return to_new_char_buffer("MARKET", buf, n, allow_exceptions);
    case OrderType::LIMIT:
        return to_new_char_buffer("LIMIT", buf, n, allow_exceptions);
    case OrderType::STOP:
        return to_new_char_buffer("STOP", buf, n, allow_exceptions);
    case OrderType::STOP_LIMIT:
        return to_new_char_buffer("STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP:
        return to_new_char_buffer("TRAILING_STOP", buf, n, allow_exceptions);
    case OrderType::MARKET_ON_CLOSE:
        return to_new_char_buffer("MARKET_ON_CLOSE", buf, n, allow_exceptions);
    case OrderType::EXERCISE:
        return to_new_char_buffer("EXERCISE", buf, n, allow_exceptions);
    case OrderType::TRAILING_STOP_LIMIT:
        return to_new_char_buffer("TRAILING_STOP_LIMIT", buf, n, allow_exceptions);
    case OrderType::NET_DEBIT:
        return to_new_char_buffer("NET_DEBIT", buf, n, allow_exceptions);
    case OrderType::NET_CREDIT:
        return to_new_char_buffer("NET_CREDIT", buf, n, allow_exceptions);
    case OrderType::NET_ZERO:
        return to_new_char_buffer("NET_ZERO", buf, n, allow_exceptions);
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
        return to_new_char_buffer("NONE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COVERED:
        return to_new_char_buffer("COVERED", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL:
        return to_new_char_buffer("VERTICAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BACK_RATIO:
        return to_new_char_buffer("BACK_RATIO", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CALENDAR:
        return to_new_char_buffer("CALENDAR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DIAGONAL:
        return to_new_char_buffer("DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRADDLE:
        return to_new_char_buffer("STRADDLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::STRANGLE:
        return to_new_char_buffer("STRANGLE", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_SYNTHETIC:
        return to_new_char_buffer("COLLAR_SYNTHETIC", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::BUTTERFLY:
        return to_new_char_buffer("BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CONDOR:
        return to_new_char_buffer("CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::IRON_CONDOR:
        return to_new_char_buffer("IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::VERTICAL_ROLL:
        return to_new_char_buffer("VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::COLLAR_WITH_STOCK:
        return to_new_char_buffer("COLLAR_WITH_STOCK", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::DOUBLE_DIAGONAL:
        return to_new_char_buffer("DOUBLE_DIAGONAL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_BUTTERFLY:
        return to_new_char_buffer("UNBALANCED_BUTTERFLY", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_CONDOR:
        return to_new_char_buffer("UNBALANCED_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_IRON_CONDOR:
        return to_new_char_buffer("UNBALANCED_IRON_CONDOR", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::UNBALANCED_VERTICAL_ROLL:
        return to_new_char_buffer("UNBALANCED_VERTICAL_ROLL", buf, n, allow_exceptions);
    case ComplexOrderStrategyType::CUSTOM:
        return to_new_char_buffer("CUSTOM", buf, n, allow_exceptions);
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
        return to_new_char_buffer("SINGLE", buf, n, allow_exceptions);
    case OrderStrategyType::OCO:
        return to_new_char_buffer("OCO", buf, n, allow_exceptions);
    case OrderStrategyType::TRIGGER:
        return to_new_char_buffer("TRIGGER", buf, n, allow_exceptions);
    default:
        throw std::runtime_error("Invalid OrderStrategyType");
    }
}

