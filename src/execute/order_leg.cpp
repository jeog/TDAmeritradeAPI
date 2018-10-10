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

namespace tdma{

OrderLegImpl::OrderLegImpl( OrderAssetType asset_type,
                               string symbol,
                               OrderInstruction instruction,
                               size_t quantity )
    :
        _asset_type(asset_type),
        _symbol( util::toupper(symbol) ),
        _instruction(instruction),
        _quantity(quantity)
    {
        if( quantity == 0 )
            TDMA_API_THROW(ValueException, "0 quantity");
        if( symbol.empty() )
            TDMA_API_THROW(ValueException, "empty symbol");
    }

OrderLegImpl::OrderLegImpl()
    :
        _asset_type(OrderAssetType::NONE),
        _symbol(),
        _instruction(OrderInstruction::NONE),
        _quantity(0)
    {
    }

bool
OrderLegImpl::operator!=(const OrderLegImpl& other) const
{
    return _asset_type != other._asset_type
        || _symbol != other._symbol
        || _instruction != other._instruction
        || _quantity != other._quantity;
}

bool
OrderLegImpl::operator==(const OrderLegImpl& other) const
{ return !(operator!=(other)); }

OrderAssetType
OrderLegImpl::get_asset_type() const
{ return _asset_type; }

string
OrderLegImpl::get_symbol() const
{ return _symbol; }

OrderInstruction
OrderLegImpl::get_instruction() const
{ return _instruction; }

size_t
OrderLegImpl::get_quantity() const
{ return _quantity; }

json
OrderLegImpl::as_json() const
{
    return json{
        {"instruction", to_string(_instruction)},
        {"quantity", _quantity},
        {"instrument", {
            {"symbol",_symbol},
            {"assetType", to_string(_asset_type)}
        }}
    };
}

string
OrderLegImpl::as_json_string() const
{ return as_json().dump(); }

typename OrderLegImpl::ProxyType::CType // need to call Destroy when done
OrderLegImpl::as_ctype() const
{
    ProxyType::CType p;
    OrderLeg_Create_ABI( static_cast<int>(_asset_type),_symbol.c_str(),
                         static_cast<int>(_instruction), _quantity, &p, 1 );
    return p;
}

} /* tdma */

using namespace tdma;

int
OrderLeg_Create_ABI( int asset_type,
                        const char* symbol,
                        int instruction,
                        size_t quantity,
                        OrderLeg_C *pleg,
                        int allow_exceptions )
{
    CHECK_PTR(pleg, "order leg", allow_exceptions);
    CHECK_PTR_KILL_PROXY(symbol, "symbol", allow_exceptions, pleg);
    CHECK_ENUM_KILL_PROXY(OrderAssetType, asset_type, allow_exceptions, pleg);
    CHECK_ENUM_KILL_PROXY(OrderInstruction, instruction, allow_exceptions, pleg);

    static auto meth = +[]( int a, const char* s, int i, size_t q ){
        return new OrderLegImpl( static_cast<OrderAssetType>(a), s,
            static_cast<OrderInstruction>(i), q);
    };

    int err;
    OrderLegImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, asset_type, symbol,
                                     instruction, quantity );
    if( err ){
        kill_proxy(pleg);
        return err;
    }

    pleg->obj = reinterpret_cast<void*>(obj);
    pleg->type_id = OrderLegImpl::TYPE_ID_LOW;
    return 0;
}

int
OrderLeg_Destroy_ABI( OrderLeg_C *pleg, int allow_exceptions  )
{ return destroy_proxy<OrderLegImpl>(pleg, allow_exceptions); }

int
OrderLeg_Copy_ABI( OrderLeg_C *from, OrderLeg_C *to, int allow_exceptions  )
{
    CHECK_PTR(to, "to leg", allow_exceptions);

    // bad 'from' fails silently, so allow client to check for non null ->obj
    kill_proxy(to);

    if( proxy_is_callable<OrderLegImpl>(from, 0) == 0 ){
        OrderLegImpl* obj = reinterpret_cast<OrderLegImpl*>(from->obj);
        int err = OrderLeg_Create_ABI( static_cast<int>(obj->get_asset_type()),
                                       obj->get_symbol().c_str(),
                                       static_cast<int>(obj->get_instruction()),
                                       obj->get_quantity(), to,
                                       allow_exceptions );
        if( err )
            return err;
    }
    return 0;
}


int
OrderLeg_IsSame_ABI( OrderLeg_C* pl,
                        OrderLeg_C* pr,
                        int *is_same,
                        int allow_exceptions )
{ return order_obj_is_same<OrderLegImpl>(pl, pr, is_same, allow_exceptions); }


int
OrderLeg_GetAssetType_ABI( OrderLeg_C* pleg,
                              int *asset_type,
                              int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderLegImpl, OrderAssetType>(
            pleg, &OrderLegImpl::get_asset_type,
            asset_type, "asset_type", allow_exceptions
        );
}

int
OrderLeg_GetSymbol_ABI( OrderLeg_C* pleg,
                           char **buf,
                           size_t *n,
                          int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<OrderLegImpl>(
            pleg, &OrderLegImpl::get_symbol,
            buf, n, allow_exceptions
        );
}

int
OrderLeg_GetInstruction_ABI( OrderLeg_C* pleg,
                                int *instruction,
                                int allow_exceptions )
{
    return ImplAccessor<int>::template
        get<OrderLegImpl, OrderInstruction>(
            pleg, &OrderLegImpl::get_instruction,
            instruction, "instruction", allow_exceptions
        );
}

int
OrderLeg_GetQuantity_ABI( OrderLeg_C* pleg,
                             size_t *quantity,
                             int allow_exceptions )
{
    return ImplAccessor<size_t>::template
        get<OrderLegImpl>(pleg, &OrderLegImpl::get_quantity,
            quantity, "quantity", allow_exceptions
        );
}

int
OrderLeg_AsJsonString_ABI( OrderLeg_C* pleg,
                              char **buf,
                              size_t *n,
                              int allow_exceptions )
{
    return ImplAccessor<char**>::template
        get<OrderLegImpl>(pleg, &OrderLegImpl::as_json_string, buf, n,
            allow_exceptions
        );
}
