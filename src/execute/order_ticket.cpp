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

using std::string;
using std::vector;

namespace {

template<typename BTy, typename... Args>
int
build( int allow_exceptions,
        BTy build_method,
        OrderTicket_C *porder,
        Args... args )
{
    using namespace tdma;
    CHECK_PTR(porder, "order", allow_exceptions);

    OrderTicket o;
    int err;
    std::tie(o, err) = CallImplFromABI( allow_exceptions, build_method, args...);
    if( err ){
        kill_proxy(porder);
        return err;
    }

    *porder = o.release_cproxy();
    return 0;
}

} /* namespace */


namespace tdma{

OrderTicketImpl::OrderTicketImpl()
    :
        _session( OrderSession::NONE ),
        _duration( OrderDuration::NONE ),
        _cancel_time( "" ),
        _type( OrderType::NONE ),
        _complex_strategy_type( ComplexOrderStrategyType::NONE ),
        _strategy_type( OrderStrategyType::SINGLE ),
        _legs(),
        _children(),
        _price( 0.0 ),
        _stop_price( 0.0 )
    {
    }

bool
OrderTicketImpl::operator!=(const OrderTicketImpl& other) const
{
    return _session != other._session
        || _duration != other._duration
        || _cancel_time != other._cancel_time
        || _type != other._type
        || _complex_strategy_type != other._complex_strategy_type
        || _strategy_type != other._strategy_type
        || _legs != other._legs
        || _children != other._children
        || _price != other._price
        || _stop_price != other._stop_price;
}

bool
OrderTicketImpl::operator==(const OrderTicketImpl& other) const
{ return !(operator!=(other)); }

json
OrderTicketImpl::as_json() const
{
    json j = { {"orderStrategyType", to_string(_strategy_type)} };

    if( _session != OrderSession::NONE )
        j["session"] = to_string(_session);

    if( _duration != OrderDuration::NONE )
        j["duration"] = to_string(_duration);

    // TODO DOCS SHOW PLAIN OPTION WITH ::NONE
    if( _complex_strategy_type != ComplexOrderStrategyType::NONE )
        j["complexOrderStrategyType"] = to_string(_complex_strategy_type);

    if( _duration == OrderDuration::GOOD_TILL_CANCEL )
        j["cancelTime"] = _cancel_time;

    if( _type != OrderType::NONE )// ASSERT
        j["orderType"] = to_string(_type);

    // this assumes a 0.0 price is always invalid
    if( _price )
        j["price"] = std::to_string(_price);

    if( _stop_price ) // ASSERT _type == stop type
        j["stopPrice"] = std::to_string(_stop_price);

    if( !_legs.empty() ){
        json j_legs;
        for(auto& l : _legs)
            j_legs.push_back(l.as_json());
        j["orderLegCollection"] = j_legs;
    }

    if( !_children.empty() ){
        json j_kids;
        for(auto& c : _children)
            j_kids.push_back(c.as_json());
        j["childOrderStrategies"] = j_kids;
    }

    return j;
}

string
OrderTicketImpl::as_json_string() const
{ return as_json().dump(); }

OrderSession
OrderTicketImpl::get_session() const
{ return _session; }

OrderTicketImpl&
OrderTicketImpl::set_session(OrderSession session)
{ _session = session; return *this; }

OrderDuration
OrderTicketImpl::get_duration() const
{return _duration; }

OrderTicketImpl&
OrderTicketImpl::set_duration(OrderDuration duration)
{ _duration = duration; return *this; }

string
OrderTicketImpl::get_cancel_time() const
{ return _cancel_time; }

OrderTicketImpl&
OrderTicketImpl::set_cancel_time(const string& cancel_time)
{ _cancel_time = cancel_time; return *this; }

OrderType
OrderTicketImpl::get_type() const
{ return _type; }

OrderTicketImpl&
OrderTicketImpl::set_type(OrderType order_type)
{ _type = order_type; return *this; }

ComplexOrderStrategyType
OrderTicketImpl::get_complex_strategy_type() const
{ return _complex_strategy_type; }

OrderTicketImpl&
OrderTicketImpl::set_complex_strategy_type(
    ComplexOrderStrategyType complex_strategy_type)
{ _complex_strategy_type = complex_strategy_type; return *this; }

OrderStrategyType
OrderTicketImpl::get_strategy_type() const
{ return _strategy_type; }

OrderTicketImpl&
OrderTicketImpl::set_strategy_type(OrderStrategyType order_strategy_type)
{ _strategy_type = order_strategy_type; return *this; }

vector<OrderLegImpl>
OrderTicketImpl::get_legs() const
{ return _legs; }

OrderLegImpl
OrderTicketImpl::get_leg(size_t n) const
{
    if( n >= _legs.size() )
        TDMA_API_THROW(ValueException, "invalid leg position");
    return _legs[n];
}

OrderTicketImpl&
OrderTicketImpl::add_leg(const OrderLegImpl& leg)
{ _legs.emplace_back(leg); return *this; }

OrderTicketImpl&
OrderTicketImpl::add_legs(const vector<OrderLegImpl>& legs)
{
    for(auto& l : legs)
        _legs.emplace_back(l);
    return *this;
}

OrderTicketImpl&
OrderTicketImpl::remove_leg(size_t n)
{
    if( n >= _legs.size() )
        TDMA_API_THROW(ValueException, "invalid leg position");
    _legs.erase( _legs.cbegin() + n );
    return *this;
}

OrderTicketImpl&
OrderTicketImpl::replace_leg(size_t n, OrderLegImpl leg)
{
    if( n >= _legs.size() )
        TDMA_API_THROW(ValueException, "invalid leg position");
    _legs[n] = leg;
    return *this;
}

OrderTicketImpl&
OrderTicketImpl::clear_legs()
{ _legs.clear(); return *this; }

vector<OrderTicketImpl>
OrderTicketImpl::get_children() const
{ return _children; }

OrderTicketImpl&
OrderTicketImpl::add_child(const OrderTicketImpl& child)
{ _children.emplace_back(child); return *this; }

OrderTicketImpl&
OrderTicketImpl::clear_children()
{ _children.clear(); return *this; }

double
OrderTicketImpl::get_price() const
{ return _price; }

OrderTicketImpl&
OrderTicketImpl::set_price(double price)
{ _price = price; return *this; }

double
OrderTicketImpl::get_stop_price() const
{ return _stop_price; }

OrderTicketImpl&
OrderTicketImpl::set_stop_price(double stop_price)
{ _stop_price = stop_price; return *this; }

typename OrderTicketImpl::ProxyType::CType // need to call Destroy when done
OrderTicketImpl::as_ctype() const
{
    ProxyType::CType p;
    OrderTicket_Create_ABI( &p, 1 );
    *reinterpret_cast<OrderTicketImpl*>(p.obj) = *this;
    return p;
}

} /* tdma */


using namespace tdma;

int
OrderTicket_Create_ABI( OrderTicket_C *porder, int allow_exceptions )
{
    CHECK_PTR( porder, "order", allow_exceptions);

    static auto meth = +[]( ){ return new OrderTicketImpl(); };

    int err;
    OrderTicketImpl *obj;
    std::tie(obj, err) = CallImplFromABI( allow_exceptions, meth);
    if( err ){
        kill_proxy(porder);
        return err;
    }

    porder->obj = reinterpret_cast<void*>(obj);
    porder->type_id = OrderTicketImpl::TYPE_ID_LOW;
    return 0;
}

int
OrderTicket_Destroy_ABI( OrderTicket_C *porder, int allow_exceptions )
{ return destroy_proxy<OrderTicketImpl>(porder, allow_exceptions); }

int
OrderTicket_Copy_ABI( OrderTicket_C *from,
                         OrderTicket_C *to,
                         int allow_exceptions )
{
    CHECK_PTR(to, "to order", allow_exceptions);

    // bad 'from' fails silently, so allow client to check for non null ->obj
    kill_proxy(to);

    if( proxy_is_callable<OrderTicketImpl>(from, 0) == 0 ){
        int err = OrderTicket_Create_ABI(to, allow_exceptions);
        if( err ){
            return err;
        }
        static auto meth = +[](void *f, void *t ){
            *reinterpret_cast<OrderTicketImpl*>(t) =
                *reinterpret_cast<OrderTicketImpl*>(f);
        };

        err = CallImplFromABI( allow_exceptions, meth, from->obj, to->obj);
        if( err )
            return err;
    }
    return 0;
}

int
OrderTicket_IsSame_ABI( OrderTicket_C* pl,
                           OrderTicket_C* pr,
                           int *is_same,
                           int allow_exceptions )
{ return order_obj_is_same<OrderTicketImpl>(pl, pr, is_same, allow_exceptions); }

int
OrderTicket_GetSession_ABI( OrderTicket_C *porder,
                               int *session,
                               int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderTicketImpl, OrderSession>(
            porder, &OrderTicketImpl::get_session, session, "session",
            allow_exceptions
        );
}

int
OrderTicket_SetSession_ABI( OrderTicket_C *porder,
                               int session,
                               int allow_exceptions )
{
    CHECK_ENUM(OrderSession, session, allow_exceptions);

    return ImplAccessor<int>::template
        set<OrderTicketImpl, OrderSession>(
            porder, &OrderTicketImpl::set_session, session, allow_exceptions
        );
}

int
OrderTicket_GetDuration_ABI( OrderTicket_C *porder,
                                 int *duration,
                                 int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderTicketImpl, OrderDuration>(
            porder, &OrderTicketImpl::get_duration, duration, "duration",
            allow_exceptions
        );
}

int
OrderTicket_SetDuration_ABI( OrderTicket_C *porder,
                                int duration,
                                int allow_exceptions )
{
    CHECK_ENUM(OrderDuration, duration, allow_exceptions);

    return ImplAccessor<int>::template
        set<OrderTicketImpl, OrderDuration>(
            porder, &OrderTicketImpl::set_duration, duration, allow_exceptions
        );
}

int
OrderTicket_GetCancelTime_ABI( OrderTicket_C *porder,
                                   char **buf,
                                   size_t* n,
                                   int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<OrderTicketImpl>(
            porder, &OrderTicketImpl::get_cancel_time, buf, n, allow_exceptions
        );
}

int
OrderTicket_SetCancelTime_ABI( OrderTicket_C *porder,
                                   const char* cancel_time,
                                   int allow_exceptions )
{
    return ImplAccessor<char**>::template
        set<OrderTicketImpl>(
            porder, &OrderTicketImpl::set_cancel_time, cancel_time, allow_exceptions
        );

}

int
OrderTicket_GetType_ABI( OrderTicket_C *porder,
                            int *order_type,
                            int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderTicketImpl, OrderType>(
            porder, &OrderTicketImpl::get_type, order_type, "order_type",
            allow_exceptions
        );
}

int
OrderTicket_SetType_ABI( OrderTicket_C *porder,
                            int order_type,
                            int allow_exceptions )
{
    CHECK_ENUM(OrderType, order_type, allow_exceptions);

    return ImplAccessor<int>::template
        set<OrderTicketImpl, OrderType>(
            porder, &OrderTicketImpl::set_type, order_type, allow_exceptions
        );
}

int
OrderTicket_GetComplexStrategyType_ABI( OrderTicket_C *porder,
                                             int *complex_strategy_type,
                                             int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderTicketImpl, ComplexOrderStrategyType>(
            porder, &OrderTicketImpl::get_complex_strategy_type, complex_strategy_type,
            "complex_strategy_type", allow_exceptions
        );
}

int
OrderTicket_SetComplexStrategyType_ABI( OrderTicket_C *porder,
                                             int complex_strategy_type,
                                             int allow_exceptions )
{
    CHECK_ENUM(ComplexOrderStrategyType, complex_strategy_type,
               allow_exceptions);

    return ImplAccessor<int>::template
        set<OrderTicketImpl, ComplexOrderStrategyType>(
            porder, &OrderTicketImpl::set_complex_strategy_type,
            complex_strategy_type, allow_exceptions
        );
}

int
OrderTicket_GetStrategyType_ABI( OrderTicket_C *porder,
                                     int *strategy_type,
                                     int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderTicketImpl, OrderStrategyType>(
            porder, &OrderTicketImpl::get_strategy_type, strategy_type,
            "strategy_type", allow_exceptions
        );
}

int
OrderTicket_SetStrategyType_ABI( OrderTicket_C *porder,
                                     int strategy_type,
                                     int allow_exceptions )
{
    CHECK_ENUM(OrderStrategyType, strategy_type, allow_exceptions);

    return ImplAccessor<int>::template
        set<OrderTicketImpl, OrderStrategyType>(
            porder, &OrderTicketImpl::set_strategy_type, strategy_type,
            allow_exceptions
        );
}

int
OrderTicket_GetPrice_ABI( OrderTicket_C *porder,
                             double *price,
                             int allow_exceptions )
{
    return ImplAccessor<double>::template
        get<OrderTicketImpl>(
            porder, &OrderTicketImpl::get_price, price, "price",
            allow_exceptions
        );
}

int
OrderTicket_SetPrice_ABI( OrderTicket_C *porder,
                             double price,
                             int allow_exceptions )
{
    return ImplAccessor<double>::template
        set<OrderTicketImpl>(porder, &OrderTicketImpl::set_price, price,
            allow_exceptions
            );
}

int
OrderTicket_GetStopPrice_ABI( OrderTicket_C *porder,
                                 double *stop_price,
                                 int allow_exceptions )
{
    return ImplAccessor<double>::template
        get<OrderTicketImpl>(
            porder, &OrderTicketImpl::get_stop_price, stop_price, "stop_price",
            allow_exceptions
        );
}

int
OrderTicket_SetStopPrice_ABI( OrderTicket_C *porder,
                                 double stop_price,
                                 int allow_exceptions )
{
    return ImplAccessor<double>::template
        set<OrderTicketImpl>(
            porder, &OrderTicketImpl::set_stop_price, stop_price,
            allow_exceptions
            );
}


int
OrderTicket_GetLegs_ABI( OrderTicket_C *porder,
                            OrderLeg_C** plegs,
                            size_t* n,
                            int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(plegs, "order legs", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth = +[](void* o){
        return reinterpret_cast<OrderTicketImpl*>(o)->get_legs();
    };

    vector<OrderLegImpl> legs;
    tie(legs, err) = CallImplFromABI( allow_exceptions, meth, porder->obj );
    if( err )
        return err;

    *n = legs.size();
    err = alloc_to_buffer(plegs, *n, allow_exceptions);
    if( err )
        return err;

    static auto meth2 = +[](OrderLeg_C* l, size_t n, vector<OrderLegImpl>& L){
        try{
            for(size_t i = 0; i < n; ++i)
                l[i] = L[i].as_ctype();
        }catch(...){
            FreeOrderLegBuffer_ABI(l, 0);
            throw;
        }
    };

    return CallImplFromABI(allow_exceptions, meth2, *plegs, *n, legs);
}

int
OrderTicket_GetLeg_ABI( OrderTicket_C *porder,
                           size_t pos,
                           OrderLeg_C* pleg,
                           int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(pleg, "order leg", allow_exceptions);

    static auto meth = +[](void* o, size_t p){
        return reinterpret_cast<OrderTicketImpl*>(o)->get_leg(p).as_ctype();
    };

    std::tie(*pleg, err) = CallImplFromABI( allow_exceptions, meth,
                                            porder->obj, pos );
    return err;
}

int
OrderTicket_AddLegs_ABI( OrderTicket_C *porder,
                            OrderLeg_C* plegs,
                            size_t n,
                            int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(plegs, "order legs", allow_exceptions);

    static auto meth = +[](void* o, OrderLeg_C *l, size_t n){
        vector<OrderLegImpl> legs(n);
        for(size_t i = 0; i < n; ++i){
            legs[i] = *reinterpret_cast<OrderLegImpl*>(l[i].obj);
        }
        reinterpret_cast<OrderTicketImpl*>(o)->add_legs(legs);
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj, plegs, n);
}

int
OrderTicket_RemoveLeg_ABI( OrderTicket_C *porder,
                              size_t pos,
                              int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void *o, size_t p){
        reinterpret_cast<OrderTicketImpl*>(o)->remove_leg(p);
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj, pos);
}


int
OrderTicket_ReplaceLeg_ABI( OrderTicket_C *porder,
                               size_t pos,
                               OrderLeg_C* pleg,
                               int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(pleg, "order leg", allow_exceptions);

    static auto meth = +[](void *o, size_t p, OrderLeg_C* l){
        reinterpret_cast<OrderTicketImpl*>(o)->replace_leg(
            p, *reinterpret_cast<OrderLegImpl*>(l->obj)
            );
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj, pos, pleg);
}

int
OrderTicket_ClearLegs_ABI( OrderTicket_C *porder, int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void *o){
        reinterpret_cast<OrderTicketImpl*>(o)->clear_legs();
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj);
}

int
OrderTicket_GetChildren_ABI( OrderTicket_C *porder,
                                OrderTicket_C** pchildren,
                                size_t* n,
                                int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(pchildren, "children", allow_exceptions);
    CHECK_PTR(n, "n", allow_exceptions);

    static auto meth = +[](void* o){
        return reinterpret_cast<OrderTicketImpl*>(o)->get_children();
    };

    vector<OrderTicketImpl> kids;
    tie(kids, err) = CallImplFromABI( allow_exceptions, meth, porder->obj );
    if( err )
        return err;

    *n = kids.size();
    err = alloc_to_buffer(pchildren, *n, allow_exceptions);
    if( err )
        return err;

    static auto meth2 =
        +[]( OrderTicket_C* l, size_t n, vector<OrderTicketImpl>& L ){
            try{
                for(size_t i = 0; i < n; ++i)
                    l[i] = L[i].as_ctype();
            }catch(...){
                FreeOrderTicketBuffer_ABI(l, 0);
                throw;
            }
         };

    return CallImplFromABI(allow_exceptions, meth2, *pchildren, *n, kids);
}

int
OrderTicket_AddChild_ABI( OrderTicket_C *porder,
                             OrderTicket_C* pchild,
                             int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(pchild, "child", allow_exceptions);

    static auto meth = +[](void *o, OrderTicket_C* c){
        reinterpret_cast<OrderTicketImpl*>(o)->add_child(
            *reinterpret_cast<OrderTicketImpl*>(c->obj)
            );
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj, pchild);
}

int
OrderTicket_ClearChildren_ABI( OrderTicket_C *porder, int allow_exceptions )
{
    int err = proxy_is_callable<OrderTicketImpl>(porder, allow_exceptions);
    if( err )
        return err;

    static auto meth = +[](void *o){
        reinterpret_cast<OrderTicketImpl*>(o)->clear_children();
    };

    return CallImplFromABI(allow_exceptions, meth, porder->obj);
}

int
OrderTicket_AsJsonString_ABI( OrderTicket_C* porder,
                                  char **buf,
                                  size_t *n,
                                  int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<OrderTicketImpl>(porder, &OrderTicketImpl::as_json_string, buf, n,
            allow_exceptions
        );
}

int
FreeOrderLegBuffer_ABI( OrderLeg_C *legs, int allow_exceptions )
{
    if( legs )
        free( (void*)legs );
    return 0;
}

int
FreeOrderTicketBuffer_ABI( OrderTicket_C *orders, int allow_exceptions )
{
    if( orders )
        free( (void*)orders );
    return 0;
}


int
BuildOrder_Simple_ABI( int asset_type,
                          const char* symbol,
                          size_t quantity,
                          int instruction,
                          int order_type,
                          double limit_price,
                          double stop_price,
                          OrderTicket_C *porder,
                          int allow_exceptions  )
{
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, porder);
    CHECK_ENUM_KILL_PROXY(OrderType, order_type, allow_exceptions, porder);
    CHECK_ENUM_KILL_PROXY(OrderAssetType, asset_type, allow_exceptions, porder);
    CHECK_ENUM_KILL_PROXY(OrderInstruction, instruction, allow_exceptions,
                          porder);

    using B = SimpleOrderBuilder;

    return build( allow_exceptions, PrivateBuildAccessor<B, false>::raw, porder,
                  static_cast<OrderAssetType>(asset_type), symbol, quantity,
                  static_cast<OrderInstruction>(instruction),
                  static_cast<OrderType>(order_type),
                  limit_price, stop_price );
}


int
SimpleOrder_CheckPrices_ABI( int order_type,
                                 double limit_price,
                                 double stop_price,
                                 int allow_exceptions )
{
    using EXC = ValueException;
    switch( static_cast<OrderType>(order_type) ){
    case OrderType::MARKET:
        if( limit_price != 0.0 )
            return HANDLE_ERROR(EXC, "limit_price != 0.0", allow_exceptions);
        if( stop_price != 0.0 )
            return HANDLE_ERROR(EXC, "stop_price != 0.0", allow_exceptions);
        break;
    case OrderType::LIMIT:
        if( limit_price <= 0.0 )
            return HANDLE_ERROR(EXC, "limit_price <= 0.0", allow_exceptions);
        if( stop_price != 0.0 )
            return HANDLE_ERROR(EXC, "stop_price != 0.0", allow_exceptions);
        break;
    case OrderType::STOP:
        if( limit_price != 0.0 )
            return HANDLE_ERROR(EXC, "limit_price != 0.0", allow_exceptions);
        if( stop_price <= 0.0 )
            return HANDLE_ERROR(EXC, "stop_price <= 0.0", allow_exceptions);
        break;
    case OrderType::STOP_LIMIT:
        if( limit_price <= 0.0 )
            return HANDLE_ERROR(EXC, "limit_price <= 0.0", allow_exceptions);
        if( stop_price <= 0.0 )
            return HANDLE_ERROR(EXC, "stop_price <= 0.0", allow_exceptions);
        break;
    default:
        throw std::runtime_error("invalid order type");
    };
    return 0;
}



int
BuildOrder_Equity_ABI( const char* symbol,
                          size_t quantity,
                          int is_buy,
                          int to_open,
                          int order_type,
                          double limit_price,
                          double stop_price,
                          OrderTicket_C *porder,
                          int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, porder);
    CHECK_ENUM_KILL_PROXY(OrderType, order_type, allow_exceptions, porder);

    using B = SimpleOrderBuilder::Equity;

    return build( allow_exceptions, PrivateBuildAccessor<B,false>::raw, porder,
                  symbol, quantity, is_buy, to_open,
                  static_cast<OrderType>(order_type), limit_price, stop_price );
}


int
BuildOrder_Option_ABI( const char* symbol,
                          size_t quantity,
                          int is_buy,
                          int to_open,
                          int is_market_order,
                          double price,
                          OrderTicket_C *porder,
                          int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, porder);

    using B = SimpleOrderBuilder::Option;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol, quantity, is_buy, to_open, is_market_order, price);
}

int
BuildOrder_OptionEx_ABI( const char* underlying,
                            unsigned int month,
                            unsigned int day,
                            unsigned int year,
                            int is_call,
                            double strike,
                            size_t quantity,
                            int is_buy,
                            int to_open,
                            int is_market_order,
                            double price,
                            OrderTicket_C *porder,
                            int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(underlying, "underlying", allow_exceptions, porder);

    using B = SimpleOrderBuilder::Option;

    return build( allow_exceptions,  PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, is_call, strike, quantity,
                  is_buy, to_open, is_market_order, price);
}

int
BuildOrder_Spread_ABI( int complex_strategy_type,
                          OrderLeg_C *plegs,
                          size_t n,
                          int is_market_order,
                          double price,
                          OrderTicket_C *porder,
                          int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(plegs, "legs", allow_exceptions, porder);
    CHECK_ENUM_KILL_PROXY(ComplexOrderStrategyType, complex_strategy_type,
                          allow_exceptions, porder);

    using B = SpreadOrderBuilder;

    vector<OrderLeg> legs;
    for(size_t i = 0; i < n; ++i){
        legs.emplace_back( plegs[i] );
    }

    return build( allow_exceptions, PrivateBuildAccessor<B, false>::raw, porder,
                  static_cast<ComplexOrderStrategyType>(complex_strategy_type),
                  move(legs), is_market_order, price );
}

int
BuildOrder_Spread_Vertical_ABI( const char* symbol_buy,
                                    const char* symbol_sell,
                                    size_t quantity,
                                    int to_open,
                                    int is_market_order,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(symbol_buy, "buy symbol", allow_exceptions, porder);
    CHECK_PTR_KILL_PROXY(symbol_sell, "sell symbol", allow_exceptions, porder);

    using B = SpreadOrderBuilder::Vertical;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_buy, symbol_sell, quantity, to_open, is_market_order,
                  price);
}

int
BuildOrder_Spread_VerticalEx_ABI( const char* underlying,
                                      unsigned int month,
                                      unsigned int day,
                                      unsigned int year,
                                      int are_calls,
                                      double strike_buy,
                                      double strike_sell,
                                      size_t quantity,
                                      int to_open,
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(underlying, "underlying", allow_exceptions, porder);

    using B = SpreadOrderBuilder::Vertical;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, are_calls, strike_buy,
                  strike_sell, quantity, to_open, is_market_order, price );

}

int
BuildOrder_Spread_VerticalRoll_ABI( const char* symbol_close_buy,
                                        const char* symbol_close_sell,
                                        const char* symbol_open_buy,
                                        const char* symbol_open_sell,
                                        size_t quantity_close,
                                        size_t quantity_open,
                                        int is_market_order,
                                        double price,
                                        OrderTicket_C *porder,
                                        int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_close_buy, "symbol close buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_close_sell, "symbol close sell",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_open_buy, "symbol open buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_open_sell, "symbol open sell",
                          allow_exceptions, porder);

    using B = SpreadOrderBuilder::Vertical::Roll;
    auto b = (quantity_close == quantity_open )
           ? (PrivateBuildAccessor<B>::raw)
           : (PrivateBuildAccessor<B::Unbalanced>::raw);

    return build( allow_exceptions, b, porder, symbol_close_buy,
                  symbol_close_sell, symbol_open_buy, symbol_open_sell,
                  quantity_close, quantity_open, is_market_order, price );
}

int
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
                                           int is_market_order,
                                           double price,
                                           OrderTicket_C *porder,
                                           int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying", allow_exceptions, porder );

    using B = SpreadOrderBuilder::Vertical::Roll;
    auto b = (quantity_close == quantity_open )
           ? (PrivateBuildAccessor<B>::ex)
           : (PrivateBuildAccessor<B::Unbalanced>::ex);

    return build( allow_exceptions, b, porder, underlying, month_close,
                  day_close, year_close, month_open, day_open, year_open,
                  are_calls, strike_close_buy, strike_close_sell,
                  strike_open_buy, strike_open_sell, quantity_close,
                  quantity_open, is_market_order, price );
}

int
BuildOrder_Spread_Butterfly_ABI( const char* symbol_outer1,
                                     const char* symbol_inner1,
                                     const char* symbol_outer2,
                                     size_t quantity_outer1,
                                     size_t quantity_outer2,
                                     int is_buy,
                                     int to_open,
                                     int is_market_order,
                                     double price,
                                     OrderTicket_C *porder,
                                     int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_outer1, "symbol_outer1",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_inner1, "symbol_inner1",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_outer2, "symbol_outer2",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Butterfly;
    auto b = (quantity_outer1 == quantity_outer2 )
           ? (PrivateBuildAccessor<B>::raw)
           : (PrivateBuildAccessor<B::Unbalanced>::raw);

    return build( allow_exceptions, b, porder, symbol_outer1, symbol_inner1,
                  symbol_outer2, quantity_outer1, quantity_outer2, is_buy,
                  to_open, is_market_order, price );
}

int
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
                                       int is_market_order,
                                       double price,
                                       OrderTicket_C *porder,
                                       int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Butterfly;
    auto b = (quantity_outer1 == quantity_outer2 )
           ? (PrivateBuildAccessor<B>::ex)
           : (PrivateBuildAccessor<B::Unbalanced>::ex);

    return build( allow_exceptions, b, porder, underlying, month, day, year,
                  are_calls, strike_outer1, strike_inner1, strike_outer2,
                  quantity_outer1, quantity_outer2, is_buy, to_open,
                  is_market_order, price );
}


EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_BackRatio_ABI( const char* symbol_buy,
                                     const char* symbol_sell,
                                     size_t quantity_buy,
                                     size_t quantity_sell,
                                     int to_open,
                                     int is_market_order,
                                     double price,
                                     OrderTicket_C *porder,
                                     int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_buy, "symbol_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_sell, "symbol_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::BackRatio;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw , porder,
                  symbol_buy, symbol_sell, quantity_buy, quantity_sell, to_open,
                  is_market_order, price );

}

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
                                       int is_market_order,
                                       double price,
                                       OrderTicket_C *porder,
                                       int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::BackRatio;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, are_calls, strike_buy,
                  strike_sell, quantity_buy, quantity_sell, to_open,
                  is_market_order, price );
}

int
BuildOrder_Spread_Calendar_ABI( const char* symbol_buy,
                                    const char* symbol_sell,
                                    size_t quantity,
                                    int to_open,
                                    int is_market_order,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_buy, "symbol_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_sell, "symbol_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Calendar;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw , porder,
                  symbol_buy, symbol_sell, quantity, to_open, is_market_order,
                  price );
}

int
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
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Calendar;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month_buy, day_buy, year_buy, month_sell,
                  day_sell, year_sell, are_calls, strike, quantity, to_open,
                  is_market_order, price );
}

int
BuildOrder_Spread_Diagonal_ABI( const char* symbol_buy,
                                    const char* symbol_sell,
                                    size_t quantity,
                                    int to_open,
                                    int is_market_order,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_buy, "symbol_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_sell, "symbol_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Diagonal;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_buy, symbol_sell, quantity, to_open, is_market_order,
                  price );
}

int
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
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Diagonal;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month_buy, day_buy, year_buy, month_sell,
                  day_sell, year_sell, are_calls, strike_buy, strike_sell,
                  quantity, to_open, is_market_order, price );
}

int
BuildOrder_Spread_Straddle_ABI( const char* symbol_call,
                                    const char* symbol_put,
                                    size_t quantity,
                                    int is_buy,
                                    int to_open,
                                    int is_market_order,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_call, "symbol_call",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put, "symbol_put",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Straddle;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_call, symbol_put, quantity, is_buy, to_open,
                  is_market_order, price );
}

int
BuildOrder_Spread_StraddleEx_ABI( const char* underlying,
                                      unsigned int month,
                                      unsigned int day,
                                      unsigned int year,
                                      double strike,
                                      size_t quantity,
                                      int is_buy,
                                      int to_open,
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Straddle;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, strike, quantity,
                  is_buy, to_open, is_market_order, price );
}

int
BuildOrder_Spread_Strangle_ABI( const char* symbol_call,
                                     const char* symbol_put,
                                     size_t quantity,
                                     int is_buy,
                                     int to_open,
                                     int is_market_order,
                                     double price,
                                     OrderTicket_C *porder,
                                     int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_call, "symbol_call",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put, "symbol_put",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Strangle;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_call, symbol_put, quantity, is_buy, to_open,
                  is_market_order, price );
}

int
BuildOrder_Spread_StrangleEx_ABI( const char* underlying,
                                      unsigned int month,
                                      unsigned int day,
                                      unsigned int year,
                                      double strike_call,
                                      double strike_put,
                                      size_t quantity,
                                      int is_buy,
                                      int to_open,
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Strangle;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, strike_call, strike_put,
                  quantity, is_buy, to_open, is_market_order, price );
}

int
BuildOrder_Spread_CollarSynthetic_ABI( const char* symbol_buy,
                                            const char* symbol_sell,
                                            size_t quantity,
                                            int to_open,
                                            int is_market_order,
                                            double price,
                                            OrderTicket_C *porder,
                                            int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_buy, "symbol_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_sell, "symbol_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::CollarSynthetic;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_buy, symbol_sell, quantity, to_open, is_market_order,
                  price);
}

int
BuildOrder_Spread_CollarSyntheticEx_ABI( const char* underlying,
                                              unsigned int month,
                                              unsigned int day,
                                              unsigned int year,
                                              double strike_call,
                                              double strike_put,
                                              size_t quantity,
                                              int is_buy,
                                              int to_open,
                                              int is_market_order,
                                              double price,
                                              OrderTicket_C *porder,
                                              int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::CollarSynthetic;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, strike_call, strike_put,
                  quantity, is_buy, to_open, is_market_order, price );
}

int
BuildOrder_Spread_CollarWithStock_ABI( const char* symbol_buy,
                                            const char* symbol_sell,
                                            const char* symbol_stock,
                                            size_t quantity,
                                            int is_buy,
                                            int to_open,
                                            int is_market_order,
                                            double price,
                                            OrderTicket_C *porder,
                                            int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_buy, "symbol_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_sell, "symbol_sell",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_stock, "symbol_stock",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::CollarWithStock;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_buy, symbol_sell, symbol_stock, quantity, is_buy,
                  to_open, is_market_order, price);
}

int
BuildOrder_Spread_CollarWithStockEx_ABI( const char* underlying,
                                              unsigned int month,
                                              unsigned int day,
                                              unsigned int year,
                                              double strike_call,
                                              double strike_put,
                                              size_t quantity,
                                              int is_buy,
                                              int to_open,
                                              int is_market_order,
                                              double price,
                                              OrderTicket_C *porder,
                                              int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::CollarWithStock;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month, day, year, strike_call, strike_put,
                  quantity, is_buy, to_open, is_market_order, price );
}

int
BuildOrder_Spread_Condor_ABI( const char* symbol_outer1,
                                  const char* symbol_inner1,
                                  const char* symbol_inner2,
                                  const char* symbol_outer2,
                                  size_t quantity1,
                                  size_t quantity2,
                                  int is_buy,
                                  int to_open,
                                  int is_market_order,
                                  double price,
                                  OrderTicket_C *porder,
                                  int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_outer1, "symbol_outer1",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_inner1, "symbol_inner1",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_inner2, "symbol_inner2",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_outer2, "symbol_outer2",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Condor;
    auto b = (quantity1 == quantity2 )
           ? PrivateBuildAccessor<B>::raw
           : PrivateBuildAccessor<B::Unbalanced>::raw;

    return build( allow_exceptions, b, porder, symbol_outer1, symbol_inner1,
                  symbol_inner2, symbol_outer2, quantity1, quantity2,
                  is_buy, to_open, is_market_order, price );
}

// position or are_calls not consistent w/ rest of ABI
int
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
                                    int is_market_order,
                                    double price,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::Condor;
    auto b = (quantity1 == quantity2 )
           ? PrivateBuildAccessor<B>::ex
           : PrivateBuildAccessor<B::Unbalanced>::ex;

    return build( allow_exceptions, b, porder, underlying, month, day, year,
                  strike_outer1, strike_inner1, strike_inner2, strike_outer2,
                  are_calls, quantity1, quantity2, is_buy, to_open,
                  is_market_order, price );
}

EXTERN_C_SPEC_ DLL_SPEC_ int
BuildOrder_Spread_IronCondor_ABI( const char* symbol_call_buy,
                                      const char* symbol_call_sell,
                                      const char* symbol_put_buy,
                                      const char* symbol_put_sell,
                                      size_t quantity_call,
                                      size_t quantity_put,
                                      int to_open,
                                      int is_market_order,
                                      double price,
                                      OrderTicket_C *porder,
                                      int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_call_buy, "symbol_call_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_call_sell, "symbol_call_sell",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put_buy, "symbol_put_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put_sell, "symbol_put_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::IronCondor;
    auto b = (quantity_call == quantity_put )
           ? PrivateBuildAccessor<B>::raw
           : PrivateBuildAccessor<B::Unbalanced>::raw;

    return build( allow_exceptions, b, porder, symbol_call_buy, symbol_call_sell,
                  symbol_put_buy, symbol_put_sell, quantity_call, quantity_put,
                  to_open, is_market_order, price );
}

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
                                        int is_market_order,
                                        double price,
                                        OrderTicket_C *porder,
                                        int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::IronCondor;
    auto b = (quantity_call == quantity_put )
           ? PrivateBuildAccessor<B>::ex
           : PrivateBuildAccessor<B::Unbalanced>::ex;

    return build( allow_exceptions, b, porder, underlying, month, day, year,
                  strike_call_buy, strike_call_sell, strike_put_buy,
                  strike_put_sell, quantity_call, quantity_put, to_open,
                  is_market_order, price );
}

int
BuildOrder_Spread_DoubleDiagonal_ABI( const char* symbol_call_buy,
                                           const char* symbol_call_sell,
                                           const char* symbol_put_buy,
                                           const char* symbol_put_sell,
                                           size_t quantity,
                                           int to_open,
                                           int is_market_order,
                                           double price,
                                           OrderTicket_C *porder,
                                           int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( symbol_call_buy, "symbol_call_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_call_sell, "symbol_call_sell",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put_buy, "symbol_put_buy",
                          allow_exceptions, porder );
    CHECK_PTR_KILL_PROXY( symbol_put_sell, "symbol_put_sell",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::DoubleDiagonal;

    return build( allow_exceptions, PrivateBuildAccessor<B>::raw, porder,
                  symbol_call_buy, symbol_call_sell, symbol_put_buy,
                  symbol_put_sell, quantity, to_open, is_market_order, price );
}

int
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
                                             int is_market_order,
                                             double price,
                                             OrderTicket_C *porder,
                                             int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY( underlying, "underlying",
                          allow_exceptions, porder );

    using B = SpreadOrderBuilder::DoubleDiagonal;

    return build( allow_exceptions, PrivateBuildAccessor<B>::ex, porder,
                  underlying, month_buy, day_buy, year_buy, month_sell,
                  day_sell, year_sell, strike_call_buy, strike_call_sell,
                  strike_put_buy, strike_put_sell, quantity, to_open,
                  is_market_order, price );
}

int
BuildOrder_OneCancelsOther_ABI( OrderTicket_C *porder1,
                                    OrderTicket_C *porder2,
                                    OrderTicket_C *porder,
                                    int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(porder1, "order1", allow_exceptions, porder);
    CHECK_PTR_KILL_PROXY(porder2, "order2", allow_exceptions, porder);

    return build( allow_exceptions, ConditionalOrderBuilder::OCO, porder,
                  OrderTicket(*porder1), OrderTicket(*porder2) );
}

int
BuildOrder_OneTriggersOther_ABI( OrderTicket_C *porder_primary,
                                     OrderTicket_C *porder_conditional,
                                     OrderTicket_C *porder,
                                     int allow_exceptions )
{
    CHECK_PTR_KILL_PROXY(porder_primary, "primary order",
                         allow_exceptions, porder);
    CHECK_PTR_KILL_PROXY(porder_conditional, "primary conditional",
                         allow_exceptions, porder);

    return build( allow_exceptions, ConditionalOrderBuilder::OTO, porder,
                  OrderTicket(*porder_primary),
                  OrderTicket(*porder_conditional) );
}
