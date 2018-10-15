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

#include <string>

#include "tdma_api_execute.h"

namespace tdma {

class OrderLegImpl {
    OrderAssetType _asset_type;
    std::string _symbol;
    OrderInstruction _instruction;
    size_t _quantity;
    // leg id ?

public:
    typedef OrderLeg ProxyType;
    static const int TYPE_ID_LOW = 1;
    static const int TYPE_ID_HIGH = 1;

    OrderLegImpl( OrderAssetType asset_type,
                   std::string symbol,
                   OrderInstruction instruction,
                   size_t quantity );

    OrderLegImpl();

    bool
    operator!=(const OrderLegImpl& other) const;

    bool
    operator==(const OrderLegImpl& other) const;

    OrderAssetType
    get_asset_type() const;

    std::string
    get_symbol() const;

    OrderInstruction
    get_instruction() const;

    size_t
    get_quantity() const;

    json
    as_json() const;

    std::string
    as_json_string() const;

    typename ProxyType::CType // need to call Destroy when done
    as_ctype() const;
};


class OrderTicketImpl {
    OrderSession _session;
    OrderDuration _duration;
    std::string _cancel_time;
    OrderType _type;
    ComplexOrderStrategyType _complex_strategy_type;
    OrderStrategyType _strategy_type; // DEFAULT to SINGLE
    std::vector<OrderLegImpl> _legs;
    std::vector<OrderTicketImpl> _children;
    double _price;
    double _stop_price;
    // requestedDestination
    // specialInstruction
    // stopPriceLinkBasis
    // stopPriceLinkType
    // stopPriceOffset
    // stopType
    // priceLinkBasis
    // priceLinkType
    // taxLotMethod

public:
    typedef OrderTicket ProxyType;
    static const int TYPE_ID_LOW = 1;
    static const int TYPE_ID_HIGH = 1;

    /*
     * Restrict to default construction so client has to manually add
     * each field...
     */
    OrderTicketImpl();

    virtual
    ~OrderTicketImpl() {}

    bool
    operator!=(const OrderTicketImpl& other) const;

    bool
    operator==(const OrderTicketImpl& other) const;

    json
    as_json() const;

    std::string
    as_json_string() const;

    OrderSession
    get_session() const;

    OrderTicketImpl&
    set_session(OrderSession session);

    OrderDuration
    get_duration() const;

    OrderTicketImpl&
    set_duration(OrderDuration duration);

    std::string
    get_cancel_time() const;

    OrderTicketImpl&
    set_cancel_time(const std::string& cancel_time);

    OrderType
    get_type() const;

    OrderTicketImpl&
    set_type(OrderType order_type);

    ComplexOrderStrategyType
    get_complex_strategy_type() const;

    OrderTicketImpl&
    set_complex_strategy_type(ComplexOrderStrategyType complex_strategy_type);

    OrderStrategyType
    get_strategy_type() const;

    OrderTicketImpl&
    set_strategy_type(OrderStrategyType order_strategy_type);

    std::vector<OrderLegImpl>
    get_legs() const;

    OrderLegImpl
    get_leg(size_t n) const;

    OrderTicketImpl&
    add_leg(const OrderLegImpl& leg);

    OrderTicketImpl&
    add_legs(const std::vector<OrderLegImpl>& legs);

    OrderTicketImpl&
    remove_leg(size_t n);

    OrderTicketImpl&
    replace_leg(size_t n, OrderLegImpl leg);

    OrderTicketImpl&
    clear_legs();

    std::vector<OrderTicketImpl>
    get_children() const;

    OrderTicketImpl&
    add_child(const OrderTicketImpl& child);

    OrderTicketImpl&
    clear_children();

    double
    get_price() const;

    OrderTicketImpl&
    set_price(double price);

    double
    get_stop_price() const;

    OrderTicketImpl&
    set_stop_price(double stop_price);

    typename ProxyType::CType // need to call Destroy when done
    as_ctype() const;
};


template<typename T>
int
order_obj_is_same( typename T::ProxyType::CType *pl,
                      typename T::ProxyType::CType *pr,
                      int *is_same,
                      int allow_exceptions )
{
    int err = proxy_is_callable<T>(pl, allow_exceptions);
    if( err )
        return err;

    err = proxy_is_callable<T>(pr, allow_exceptions);
    if( err )
        return err;

    CHECK_PTR(is_same, "is_same", allow_exceptions);

    static auto meth = +[](void *l, void *r){
        return *reinterpret_cast<T*>(l) == *reinterpret_cast<T*>(r);
    };

    std::tie(*is_same, err) = CallImplFromABI( allow_exceptions, meth, pl->obj,
                                               pr->obj);
    return err;
}


} /* tdma */
