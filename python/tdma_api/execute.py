#
# Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses.
#

from ctypes import byref as _REF, c_int, c_size_t, c_double, c_uint, POINTER
import json

from . import clib
from .common import *
from .clib import PCHAR


ORDER_SESSION_NORMAL = 1
ORDER_SESSION_AM = 2
ORDER_SESSION_PM = 3
ORDER_SESSION_SEAMLESS = 4

ORDER_DURATION_DAY = 1
ORDER_DURATION_GOOD_TILL_CANCEL = 2
ORDER_DURATION_FILL_OR_KILL = 3

ORDER_ASSET_TYPE_EQUITY = 1
ORDER_ASSET_TYPE_OPTION = 2
ORDER_ASSET_TYPE_INDEX = 3
ORDER_ASSET_TYPE_MUTUAL_FUND = 4
ORDER_ASSET_TYPE_CASH_EQUIVALENT = 5
ORDER_ASSET_TYPE_FIXED_INCOME = 6
ORDER_ASSET_TYPE_CURRENCY = 7

ORDER_INSTRUCTION_BUY = 1
ORDER_INSTRUCTION_SELL = 2
ORDER_INSTRUCTION_BUY_TO_COVER = 3
ORDER_INSTRUCTION_SELL_SHORT = 4
ORDER_INSTRUCTION_BUY_TO_OPEN = 5
ORDER_INSTRUCTION_BUY_TO_CLOSE = 6
ORDER_INSTRUCTION_SELL_TO_OPEN = 7
ORDER_INSTRUCTION_SELL_TO_CLOSE = 8
ORDER_INSTRUCTION_EXCHANGE = 9

ORDER_TYPE_MARKET = 1
ORDER_TYPE_LIMIT = 2
ORDER_TYPE_STOP = 3
ORDER_TYPE_STOP_LIMIT = 4
ORDER_TYPE_TRAILING_STOP = 5
ORDER_TYPE_MARKET_ON_CLOSE = 6
ORDER_TYPE_EXERCISE = 7
ORDER_TYPE_TRAILING_STOP_LIMIT = 8
ORDER_TYPE_NET_DEBIT = 9
ORDER_TYPE_NET_CREDIT = 10
ORDER_TYPE_NET_ZERO = 11

COMPLEX_ORDER_STRATEGY_TYPE_NONE = 0
COMPLEX_ORDER_STRATEGY_TYPE_COVERED = 1
COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL = 2
COMPLEX_ORDER_STRATEGY_TYPE_BACK_RATIO = 3
COMPLEX_ORDER_STRATEGY_TYPE_CALENDAR = 4
COMPLEX_ORDER_STRATEGY_TYPE_DIAGONAL = 5
COMPLEX_ORDER_STRATEGY_TYPE_STRADDLE = 6
COMPLEX_ORDER_STRATEGY_TYPE_STRANGLE = 7
COMPLEX_ORDER_STRATEGY_TYPE_COLLAR_SYNTHETIC = 8
COMPLEX_ORDER_STRATEGY_TYPE_BUTTERFLY = 9
COMPLEX_ORDER_STRATEGY_TYPE_CONDOR = 10
COMPLEX_ORDER_STRATEGY_TYPE_IRON_CONDOR = 11
COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL_ROLL = 12
COMPLEX_ORDER_STRATEGY_TYPE_COLLAR_WITH_STOCK = 13
COMPLEX_ORDER_STRATEGY_TYPE_DOUBLE_DIAGONAL = 14
COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_BUTTERFLY = 15
COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_CONDOR = 16
COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_IRON_CONDOR = 17
COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_VERTICAL_ROLL = 18
COMPLEX_ORDER_STRATEGY_TYPE_CUSTOM = 19

ORDER_STRATEGY_TYPE_SINGLE = 0
ORDER_STRATEGY_TYPE_OCO = 1
ORDER_STRATEGY_TYPE_TRIGGER = 2


class _OrderLeg_C(clib._CProxy2):
    """C struct representing OrderLeg_C type."""
    pass

class _OrderTicket_C(clib._CProxy2):
    """C struct representing OrderTicket_C type."""
    pass

def order_session_to_str(session):
    """Converts ORDER_SESSION_[] constant to str."""
    return clib.to_str("OrderSession_to_string_ABI", c_int, session)

def order_duration_to_str(duration):
    """Converts ORDER_DURATION_[] constant to str."""
    return clib.to_str("OrderDuration_to_string_ABI", c_int, duration)

def order_asset_type_to_str(asset_type):
    """Converts ORDER_ASSET_TYPE_[] constant to str."""
    return clib.to_str("OrderAssetType_to_string_ABI", c_int, asset_type)

def order_instruction_to_str(instruction):
    """Converts ORDER_INSTRUCTION_[] constant to str."""
    return clib.to_str("OrderInstruction_to_string_ABI", c_int, instruction)

def order_type_to_str(order_type):
    """Converts ORDER_TYPE_[] constant to str."""
    return clib.to_str("OrderType_to_string_ABI", c_int, order_type)

def complex_order_strategy_type_to_str(complex_strategy_type):
    """Converts COMPLEX_ORDER_STRATEGY_TYPE_[] constant to str."""
    return clib.to_str("ComplexOrderStrategyType_to_string_ABI", c_int,
                       complex_strategy_type)

def order_strategy_type_to_str(strategy):
    """Converts ORDER_STRATEGY_TYPE_[] constant to str."""
    return clib.to_str("OrderStrategyType_to_string_ABI", c_int, strategy)

#
# Careful - this is a shared base, unlike our C++ 'OrderObjectProxy'
#
class _OrderObjectBase(clib._ProxyBase):
    """_OrderObjectBase - (Abstract) base order object class.

    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """

    @classmethod
    def _init_from_cproxy(cls, proxy):
        t = cls.__new__(cls)
        t._obj = cls._cproxy_type()()
        t._obj.obj = proxy.obj
        t._obj.type_id = proxy.type_id
        t._alive = True
        return t

    def _is_same(self, other):
        if type(self) != type(other):
            return False
        i = c_int()
        clib.call( self._abi('IsSame'), _REF(self._obj), _REF(other._obj),
                   _REF(i) )
        return bool(i)

    def deep_copy(self):
        """ Returns a new instance with self's fields copied into it. """
        copy = self.__new__(self.__class__)
        copy._obj = self._cproxy_type()()
        clib.call( self._abi('Copy'), _REF(self._obj), _REF(copy._obj))
        copy._alive = True
        return copy

    def as_json(self):
        """ Returns object as json - as dict, list, or None. """
        j = clib.get_str( self._abi('AsJsonString'), self._obj )
        return json.loads(j) if j else None


class OrderLeg( _OrderObjectBase ):
    """OrderLeg - represents a leg within an OrderTicket.

        def __init__(self, asset_type, symbol, instruction, quantity):

         asset_type  :: int :: ORDER_ASSET_TYPE_[] constant
         symbol      :: str :: symbol of security
         instruction :: int :: ORDER_INSTRUCTION_[] constant
         quantity    :: int :: number of shares, contracts etc.

        ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, asset_type, symbol, instruction, quantity):
        super().__init__( c_int(asset_type), PCHAR(symbol),
                          c_int(instruction), c_size_t(quantity) )

    def __eq__(self,other):
        return self._is_same(other)

    @classmethod
    def _cproxy_type(cls):
        return _OrderLeg_C

    def get_asset_type(self):
        """ Returns ORDER_ASSET_TYPE_[] constant of leg. """
        return clib.get_val('OrderLeg_GetAssetType_ABI', c_int, self._obj)

    def get_symbol(self):
        """Returns symbol of leg."""
        return clib.get_str('OrderLeg_GetSymbol_ABI', self._obj)

    def get_instruction(self):
        """ Returns ORDER_INSTRUCTION_[] constant of leg. """
        return clib.get_val('OrderLeg_GetInstruction_ABI', c_int, self._obj)

    def get_quantity(self):
        """ Returns quantity of leg. """
        return clib.get_val('OrderLeg_GetQuantity_ABI', c_size_t, self._obj)


class OrderTicket( _OrderObjectBase ):
    """OrderTicket - represents an order.

        def __init__(self):

        ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self):
        super().__init__()

    def __eq__(self,other):
        return self._is_same(other)

    @classmethod
    def _cproxy_type(cls):
        return _OrderTicket_C

    @classmethod
    def _check_objects(cls, ty, objs):
        if not objs:
            raise ValueError("no " + ty.__name__ + 's')
        for o in objs:
            if not isinstance(o, ty) or not o._obj:
                raise TypeError("not a valid " + ty.__name__)

    def get_session(self):
        """Returns session type as ORDER_SESSION_[] constant."""
        return clib.get_val('OrderTicket_GetSession_ABI', c_int, self._obj)

    def set_session(self, session):
        """Sets session type using ORDER_SESSION_[] constant. Returns self."""
        clib.set_val('OrderTicket_SetSession_ABI', c_int, session, self._obj)
        return self

    def get_duration(self):
        """Returns duration type as ORDER_DURATION_[] constant."""
        return clib.get_val('OrderTicket_GetDuration_ABI', c_int, self._obj)

    def set_duration(self, duration):
        """Sets duration type using ORDER_DURATION_[] constant. Returns self."""
        clib.set_val("OrderTicket_SetDuration_ABI", c_int, duration, self._obj)
        return self

    def get_cancel_time(self):
        """Returns cancel time as str for GTC orders, or None."""
        s = clib.get_str('OrderTicket_GetCancelTime_ABI', self._obj)
        return s if s else None

    def set_cancel_time(self, cancel_time):
        """Sets cancel time (e.g 2019-01-29) for GTC orders. Returns self."""
        clib.set_str('OrderTicket_SetCancelTime_ABI', cancel_time, self._obj)
        return self

    def get_type(self):
        """Returns order type as ORDER_TYPE_[] constant."""
        return clib.get_val('OrderTicket_GetType_ABI', c_int, self._obj)

    def set_type(self, order_type):
        """Sets order type using ORDER_TYPE_[] constant. Returns self."""
        clib.set_val('OrderTicket_SetType_ABI',  c_int, order_type, self._obj)
        return self

    def get_complex_strategy_type(self):
        """ Returns complex strategy type as COMPLEX_ORDER_STRATEGY_TYPE_[]
            constant. """
        return clib.get_val('OrderTicket_GetComplexStrategyType_ABI', c_int,
                            self._obj)

    def set_complex_strategy_type(self, complex_strategy_type):
        """ Sets complex strategy type using COMPELX_ORDER_STRATEGY_TYPE_[]
            constant. Returns self. """
        clib.set_val('OrderTicket_SetComplexStrategyType_ABI', c_int,
                     complex_strategy_type, self._obj)
        return self

    def get_strategy_type(self):
        """Returns strategy type as ORDER_STRATEGY_TYPE_[] constant."""
        return clib.get_val('OrderTicket_GetStrategyType_ABI', c_int,
                            self._obj)

    def set_strategy_type(self, strategy_type):
        """ Sets strategy type using ORDER_STRATEGY_TYPE_[] constant.
            Returns self. """
        clib.set_val('OrderTicket_SetStrategyType_ABI', c_int, strategy_type,
                     self._obj)
        return self

    def get_price(self):
        """Returns (limit) price."""
        return clib.get_val('OrderTicket_GetPrice_ABI', c_double, self._obj)

    def set_price(self, price):
        """Sets (limit) price. Returns self."""
        clib.set_val('OrderTicket_SetPrice_ABI', c_double, price, self._obj)
        return self

    def get_stop_price(self):
        """Returns (stop) price."""
        return clib.get_val('OrderTicket_GetStopPrice_ABI', c_double, self._obj)

    def set_stop_price(self, price):
        """Sets (stop) price. Returns self."""
        clib.set_val('OrderTicket_SetStopPrice_ABI', c_double, price, self._obj)
        return self

    def add_legs(self, *legs):
        """Adds legs (class OrderLeg) to order. Returns self."""
        self._check_objects(OrderLeg, legs)
        l = len(legs)
        array = (_OrderLeg_C * l)(*[leg._obj for leg in legs])
        clib.call('OrderTicket_AddLegs_ABI', _REF(self._obj), array, l)
        return self

    def get_legs(self):
        """Returns all legs (class OrderLeg) of order."""
        p = POINTER(_OrderLeg_C)()
        n = c_size_t()
        clib.call('OrderTicket_GetLegs_ABI', _REF(self._obj), _REF(p), _REF(n))
        legs = []
        for i in range(n.value):
            l = OrderLeg._init_from_cproxy(p[i])
            legs.append(l)
        clib.free_order_leg_buffer(p)
        return legs

    def get_leg(self, n):
        """Returns leg (class OrderLeg) of order at position 'n'."""
        l = _OrderLeg_C()
        clib.call('OrderTicket_GetLeg_ABI', _REF(self._obj), c_size_t(n),
                  _REF(l))
        return OrderLeg._init_from_cproxy(l)

    def remove_leg(self, n):
        """Removes leg (class OrderLeg) of order at position 'n'.
           Returns self."""
        clib.call('OrderTicket_RemoveLeg_ABI', _REF(self._obj), c_size_t(n))
        return self

    def replace_leg(self, n, leg):
        """Replaces leg (class OrderLeg) of order at position 'n'.
           Returns self."""
        self._check_objects(OrderLeg, (leg,))
        clib.call('OrderTicket_ReplaceLeg_ABI', _REF(self._obj), c_size_t(n),
                  _REF(leg._obj))
        return self

    def clear_legs(self):
        """Removes all legs (class OrderLeg) of order. Return self."""
        clib.call('OrderTicket_ClearLegs_ABI', _REF(self._obj))
        return self

    def get_children(self):
        """Returns all child orders (class OrderTicket) of order."""
        p = POINTER(_OrderTicket_C)()
        n = c_size_t()
        clib.call('OrderTicket_GetChildren_ABI', _REF(self._obj), _REF(p),
                  _REF(n))
        legs = []
        for i in range(n.value):
            l = OrderTicket._init_from_cproxy(p[i])
            legs.append(l)
        clib.free_order_ticket_buffer(p)
        return legs

    def add_child(self, child):
        """Add child order (class OrderTicket) to order. Returns self."""
        self._check_objects(OrderTicket, (child,))
        clib.call('OrderTicket_AddChild_ABI', _REF(self._obj),
                  _REF(child._obj))
        return self

    def clear_children(self):
        """Removes all child orders (class OrderTicket) from order.
           Returns self."""
        clib.call('OrderTicket_ClearChildren_ABI', _REF(self._obj))
        return self


class _OrderBuilder:
    @staticmethod
    def _abi_build(fname, *args):
        o = _OrderTicket_C()
        clib.call(fname, *(args + (_REF(o),)) )
        return OrderTicket._init_from_cproxy(o)


class SimpleOrderBuilder( _OrderBuilder ):
    """SimpleOrderBuilder - build basic Equity and Option OrderTickets"""

    class Equity:
        @staticmethod
        def Build(symbol, quantity, is_buy, to_open, limit_price=None):
            """Build a (Market/Limit) Equity OrderTicket

            def Build(symbol, quantity, is_buy, to_open, limit_price=None):

                symbol      :: str   :: equity/ETF symbol
                quantity    :: int   :: # of shares
                is_buy      :: bool  :: buy order (vs sell order)
                to_open     :: bool  :: open position
                                        True = Buy or Sell Short
                                        False = But To Cover or Sell
                limit_price :: float :: limit price (None = Market Order)

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            ot = ORDER_TYPE_LIMIT
            if limit_price is None:
                ot = ORDER_TYPE_MARKET
                limit_price = 0.0
            return _OrderBuilder._abi_build('BuildOrder_Equity_ABI',
                PCHAR(symbol), c_size_t(quantity), c_int(is_buy),
                c_int(to_open), c_int(ot), c_double(limit_price),
                c_double(0.0))

        class Stop:
            @staticmethod
            def Build(symbol, quantity, is_buy, to_open, stop_price,
                      limit_price=None):
                """Build a (Stop/Stop-Limit) Equity OrderTicket

            def Build(symbol, quantity, is_buy, to_open, stop_price,
                      limit_price=None):

                symbol      :: str   :: equity/ETF symbol
                quantity    :: int   :: # of shares
                is_buy      :: bool  :: buy order (vs sell order)
                to_open     :: bool  :: open position
                                        True = Buy or Sell Short
                                        False = But To Cover or Sell
                stop_price  :: float :: stop price
                limit_price :: float :: limit price for stop-limit order
                                        (None = stop-market order)

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                ot = ORDER_TYPE_STOP_LIMIT
                if limit_price is None:
                    ot = ORDER_TYPE_STOP
                    limit_price = 0.0
                return _OrderBuilder._abi_build('BuildOrder_Equity_ABI',
                    PCHAR(symbol), c_size_t(quantity), c_int(is_buy),
                    c_int(to_open), c_int(ot), c_double(limit_price),
                    c_double(stop_price))

    class Option:
        @staticmethod
        def Build1(symbol, quantity, is_buy, to_open, limit_price=None):
            """Build an Option OrderTicket

            def Build1(symbol, quantity, is_buy, to_open, limit_price=None):

                symbol      :: str   :: option symbol
                quantity    :: int   :: # of contracts
                is_buy      :: bool  :: is buy order (vs sell order)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price (None = Market Order)

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Option_ABI',
                PCHAR(symbol), c_size_t(quantity), c_int(is_buy),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, is_call, strike,
                    quantity, is_buy, to_open, limit_price=None):
            """Build an Option OrderTicket

            def Build2(underlying, month, day, year, is_call, strike,
                      quantity, is_buy, to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                is_call     :: bool  :: is call option (vs put option)
                strike      :: float :: strike price of option
                quantity    :: int   :: # of contracts
                is_buy      :: bool  :: is buy order (vs sell order)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price (None = Market Order)

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_OptionEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_int(is_call), c_double(strike), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))



class SpreadOrderBuilder( _OrderBuilder ):
    """SpreadOrderBuilder - build Option Spread OrderTickets"""

    class Vertical:
        """Vertical - build Vertical OrderTickets"""

        @staticmethod
        def Build1(symbol_buy, symbol_sell, quantity, to_open,
                   limit_price=None):
            """Build a Vertical Spread OrderTicket

            def Build1(symbol_buy, symbol_sell, quantity, to_open,
                       limit_price=None):

                symbol_buy  :: str   :: option symbol to buy
                symbol_sell :: str   :: option symbol to sell
                quantity    :: int   :: # of contracts
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order
            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Vertical_ABI',
                PCHAR(symbol_buy), PCHAR(symbol_sell), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, are_calls, strike_buy,
                    strike_sell, quantity, to_open, limit_price=None):
            """Build a Vertical Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls, strike_buy,
                       strike_sell, quantity, to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                are_calls   :: bool  :: is call spread (vs put spread)
                strike_buy  :: float :: strike price to buy
                strike_sell :: float :: strike price to sell
                quantity    :: int   :: # of spreads
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_VerticalEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_int(are_calls), c_double(strike_buy), c_double(strike_sell),
                c_size_t(quantity), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        class Roll:
            """Roll - build Vertical Roll OrderTickets"""

            @staticmethod
            def Build1(symbol_close_buy, symbol_close_sell, symbol_open_buy,
                      symbol_open_sell, quantity, limit_price=None):
                """Build a Vertical Roll Spread OrderTicket

            def Build1(symbol_close_buy, symbol_close_sell,
                       symbol_open_buy, symbol_open_sell, quantity,
                       limit_price=None):

                symbol_close_buy  :: str   :: option symbol to buy-close
                symbol_close_sell :: str   :: option symbol to sell-close
                symbol_open_buy   :: str   :: option symbol to buy-close
                symbol_open_sell  :: str   :: option symbol to sell-close
                quantity          :: int   :: # of spreads to roll
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_VerticalRoll_ABI',
                    PCHAR(symbol_close_buy), PCHAR(symbol_close_sell),
                    PCHAR(symbol_open_buy), PCHAR(symbol_open_sell),
                    c_size_t(quantity), c_size_t(quantity),
                    c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            @staticmethod
            def Build2(underlying, month, day, year, are_calls,
                       strike_close_buy, strike_close_sell, strike_open_buy,
                       strike_open_sell, quantity, limit_price=None):
                """Build a Vertical Roll Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls,
                       strike_close_buy, strike_close_sell, strike_open_buy,
                       strike_open_sell, quantity, limit_price=None):

                underlying        :: str   :: underlying security(e.g 'SPY')
                month             :: int   :: exp month of option
                day               :: int   :: exp day of option
                year              :: int   :: exp year of option
                are_calls         :: bool  :: is call spread (vs put spread)
                strike_close_buy  :: float :: strike price to buy-close
                strike_close_sell :: float :: strike price to sell-close
                strike_open_buy   :: float :: strike price to buy-close
                strike_open_sell  :: float :: strike price to sell-close
                quantity          :: int   :: # of spreads to roll
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_VerticalRollEx_ABI',
                    PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                    c_uint(month), c_uint(day), c_uint(year), c_int(are_calls),
                    c_double(strike_close_buy), c_double(strike_close_sell),
                    c_double(strike_open_buy), c_double(strike_open_sell),
                    c_size_t(quantity), c_size_t(quantity),
                    c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            @staticmethod
            def Build3(underlying, month_close, day_close, year_close,
                       month_open, day_open, year_open, are_calls,
                       strike_close_buy, strike_close_sell, strike_open_buy,
                       strike_open_sell, quantity, limit_price=None):
                """Build a Vertical Roll Spread OrderTicket

            def Build3(underlying, month_close, day_close, year_close,
                       month_open, day_open, year_open, are_calls,
                       strike_close_buy, strike_close_sell, strike_open_buy,
                       strike_open_sell, quantity, limit_price=None):

                underlying        :: str   :: underlying security(e.g 'SPY')
                month_close       :: int   :: exp month of option to close
                day_close         :: int   :: exp day of option to close
                year_close        :: int   :: exp year of option to close
                month_open        :: int   :: exp month of option to open
                day_open          :: int   :: exp day of option to open
                year_open         :: int   :: exp year of option to open
                are_calls         :: bool  :: is call spread (vs put spread)
                strike_close_buy  :: float :: strike price to buy-close
                strike_close_sell :: float :: strike price to sell-close
                strike_open_buy   :: float :: strike price to buy-close
                strike_open_sell  :: float :: strike price to sell-close
                quantity          :: int   :: # of spreads to roll
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_VerticalRollEx_ABI',
                    PCHAR(underlying), c_uint(month_close), c_uint(day_close),
                    c_uint(year_close), c_uint(month_open), c_uint(day_open),
                    c_uint(year_open), c_int(are_calls),
                    c_double(strike_close_buy), c_double(strike_close_sell),
                    c_double(strike_open_buy), c_double(strike_open_sell),
                    c_size_t(quantity), c_size_t(quantity),
                    c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            class Unbalanced:
                """Unbalanced - build Unbalanced Vertical Roll OrderTickets"""

                @staticmethod
                def Build1(symbol_close_buy, symbol_close_sell, symbol_open_buy,
                          symbol_open_sell, quantity_close, quantity_open,
                          limit_price=None):
                    """Build an Unbalanced Vertical Roll Spread OrderTicket

            def Build1(symbol_close_buy, symbol_close_sell, symbol_open_buy,
                       symbol_open_sell, quantity, limit_price=None):

                symbol_close_buy  :: str   :: option symbol to buy-close
                symbol_close_sell :: str   :: option symbol to sell-close
                symbol_open_buy   :: str   :: option symbol to buy-close
                symbol_open_sell  :: str   :: option symbol to sell-close
                quantity_close    :: int   :: # of contracts to close
                quantity_open     :: int   :: # of contracts to open
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                    return _OrderBuilder._abi_build(
                        'BuildOrder_Spread_VerticalRoll_ABI',
                        PCHAR(symbol_close_buy), PCHAR(symbol_close_sell),
                        PCHAR(symbol_open_buy), PCHAR(symbol_open_sell),
                        c_size_t(quantity_close), c_size_t(quantity_open),
                        c_int(limit_price is None),
                        c_double(0.0 if limit_price is None else limit_price))

                @staticmethod
                def Build2(underlying, month, day, year, are_calls,
                           strike_close_buy, strike_close_sell, strike_open_buy,
                           strike_open_sell, quantity_close, quantity_open,
                           limit_price=None):
                    """Build an Unbalanced Vertical Roll Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls,
                       strike_close_buy, strike_close_sell,
                       strike_open_buy, strike_open_sell,
                       quantity_close, quantity_open, limit_price=None):

                underlying        :: str   :: underlying security(e.g 'SPY')
                month             :: int   :: exp month of option
                day               :: int   :: exp day of option
                year              :: int   :: exp year of option
                are_calls         :: bool  :: is call spread (vs put spread)
                strike_close_buy  :: float :: strike price to buy-close
                strike_close_sell :: float :: strike price to sell-close
                strike_open_buy   :: float :: strike price to buy-close
                strike_open_sell  :: float :: strike price to sell-close
                quantity_close    :: int   :: # of contracts to close
                quantity_open     :: int   :: # of contracts to open
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                    return _OrderBuilder._abi_build(
                        'BuildOrder_Spread_VerticalRollEx_ABI',
                        PCHAR(underlying), c_uint(month), c_uint(day),
                        c_uint(year), c_uint(month), c_uint(day), c_uint(year),
                        c_int(are_calls), c_double(strike_close_buy),
                        c_double(strike_close_sell), c_double(strike_open_buy),
                        c_double(strike_open_sell),
                        c_size_t(quantity_close), c_size_t(quantity_open),
                        c_int(limit_price is None),
                        c_double(0.0 if limit_price is None else limit_price))

                @staticmethod
                def Build3(underlying, month_close, day_close, year_close,
                           month_open, day_open, year_open, are_calls,
                           strike_close_buy, strike_close_sell, strike_open_buy,
                           strike_open_sell, quantity_close, quantity_open,
                            limit_price=None):
                    """Build an Unbalanced Vertical Roll Spread OrderTicket

            def Build3(underlying, month_close, day_close, year_close,
                       month_open, day_open, year_open, are_calls,
                       strike_close_buy, strike_close_sell,
                       strike_open_buy, strike_open_sell,
                       quantity_close, quantity_open, limit_price=None):

                underlying        :: str   :: underlying security(e.g 'SPY')
                month_close       :: int   :: exp month of option to close
                day_close         :: int   :: exp day of option to close
                year_close        :: int   :: exp year of option to close
                month_open        :: int   :: exp month of option to open
                day_open          :: int   :: exp day of option to open
                year_open         :: int   :: exp year of option to open
                are_calls         :: bool  :: is call spread (vs put spread)
                strike_close_buy  :: float :: strike price to buy-close
                strike_close_sell :: float :: strike price to sell-close
                strike_open_buy   :: float :: strike price to buy-close
                strike_open_sell  :: float :: strike price to sell-close
                quantity_close    :: int   :: # of contracts to close
                quantity_open     :: int   :: # of contracts to open
                limit_price       :: float :: limit price
                                              > 0.0 for NET_DEBIT
                                              < 0.0 for NET_CREDIT
                                              = 0.0 for NET_ZERO
                                              = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                    return _OrderBuilder._abi_build(
                        'BuildOrder_Spread_VerticalRollEx_ABI',
                        PCHAR(underlying), c_uint(month_close),
                        c_uint(day_close), c_uint(year_close),
                        c_uint(month_open), c_uint(day_open), c_uint(year_open),
                        c_int(are_calls), c_double(strike_close_buy),
                        c_double(strike_close_sell), c_double(strike_open_buy),
                        c_double(strike_open_sell), c_size_t(quantity_close),
                        c_size_t(quantity_open), c_int(limit_price is None),
                        c_double(0.0 if limit_price is None else limit_price))
            # Unbalanced

        # Roll

    # Vertical

    class Butterfly:
        """Butterfly - build Butterfly OrderTickets"""

        @staticmethod
        def Build1(symbol_outer1, symbol_inner, symbol_outer2, quantity, is_buy,
                  to_open, limit_price=None):
            """Build a Butterfly Spread OrderTicket

            def Build1(symbol_outer1, symbol_inner, symbol_outer2,
                       quantity, is_buy, to_open, limit_price=None):

            symbol_outer1 :: str   :: outer option symbol (wing 1)
            symbol_inner  :: str   :: inner option symbol (body)
            symbol_outer2 :: str   :: outer option symbol (wing 2)
            quantity      :: int   :: # of spreads to trade
            is_buy        :: bool  :: is buy order (buy-sell-buy)
            to_open       :: bool  :: open position
            limit_price   :: float :: limit price
                                      > 0.0 for NET_DEBIT
                                      < 0.0 for NET_CREDIT
                                      = 0.0 for NET_ZERO
                                      = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Butterfly_ABI',
                PCHAR(symbol_outer1), PCHAR(symbol_inner),
                PCHAR(symbol_outer2), c_size_t(quantity), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, are_calls, strike_outer1,
                   strike_inner, strike_outer2, quantity, is_buy, to_open,
                   limit_price=None):
            """Build a Butterfly Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls, strike_outer1,
                       strike_inner, strike_outer2, quantity, is_buy, to_open,
                       limit_price=None):

            underlying    :: str   :: underlying security(e.g 'SPY')
            month         :: int   :: exp month of option
            day           :: int   :: exp day of option
            year          :: int   :: exp year of option
            are_calls     :: bool  :: is call spread (vs put spread)
            strike_outer1 :: float :: outer strike price (wing 1)
            strike_inner  :: float :: inner strike price (body)
            strike_outer2 :: float :: outer strike price (wing 2)
            quantity      :: int   :: # of spreads to trade
            is_buy        :: bool  :: is buy order (buy-sell-buy)
            to_open       :: bool  :: open position
            limit_price   :: float :: limit price
                                      > 0.0 for NET_DEBIT
                                      < 0.0 for NET_CREDIT
                                      = 0.0 for NET_ZERO
                                      = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_ButterflyEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_int(are_calls), c_double(strike_outer1), c_double(strike_inner),
                c_double(strike_outer2), c_size_t(quantity), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        class Unbalanced:
            """Unbalanced - build Unbalanced Butterfly OrderTickets"""

            @staticmethod
            def Build1(symbol_outer1, symbol_inner, symbol_outer2, quantity1,
                      quantity2, is_buy, to_open, limit_price=None):
                """Build an Unbalanced Butterfly Spread OrderTicket

            def Build1(symbol_outer1, symbol_inner, symbol_outer2, quantity,
                       quantity2, is_buy, to_open, limit_price=None):

                symbol_outer1 :: str   :: outer option symbol (wing 1)
                symbol_inner  :: str   :: inner option symbol (body)
                symbol_outer2 :: str   :: outer option symbol (wing 2)
                quantity1     :: int   :: # of outer (wing 1) contracts to trade
                quantity2     :: int   :: # of outer (wing 2) contracts to trade
                is_buy        :: bool  :: is buy order (buy-sell-buy)
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build('BuildOrder_Spread_Butterfly_ABI',
                    PCHAR(symbol_outer1), PCHAR(symbol_inner),
                    PCHAR(symbol_outer2), c_size_t(quantity1), c_size_t(quantity2),
                    c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            @staticmethod
            def Build2(underlying, month, day, year, are_calls, strike_outer1,
                       strike_inner, strike_outer2, quantity1, quantity2, is_buy,
                       to_open, limit_price=None):
                """Build a Butterfly Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls, strike_outer1,
                       strike_inner, strike_outer2, quantity1, quantity2,
                       is_buy, to_open, limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month         :: int   :: exp month of option
                day           :: int   :: exp day of option
                year          :: int   :: exp year of option
                are_calls     :: bool  :: is call spread (vs put spread)
                strike_outer1 :: float :: outer strike price (wing 1)
                strike_inner  :: float :: inner strike price (body)
                strike_outer2 :: float :: outer strike price (wing 2)
                quantity1     :: int   :: # of outer (wing 1) contracts to trade
                quantity2     :: int   :: # of outer (wing 2) contracts to trade
                is_buy        :: bool  :: is buy order (buy-sell-buy)
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build('BuildOrder_Spread_ButterflyEx_ABI',
                    PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                    c_int(are_calls), c_double(strike_outer1),
                    c_double(strike_inner), c_double(strike_outer2),
                    c_size_t(quantity1), c_size_t(quantity2), c_int(is_buy),
                    c_int(to_open), c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))
        # Unbalanced

    # Butterfly

    class BackRatio:
        """BackRatio - build BackRatio OrderTickets"""

        @staticmethod
        def Build1(symbol_buy, symbol_sell, quantity_buy, quantity_sell,
                  to_open, limit_price=None):
            """Build a BackRatio Spread OrderTicket

            def Build1(symbol_buy, symbol_sell, quantity_buy, quantity_sell,
                      to_open, limit_price=None):

            symbol_buy    :: str   :: option symbol to buy
            symbol_sell   :: str   :: option symbol to sell
            quantity_buy  :: int   :: # of contracts to buy
            quantity_sell :: int   :: # of contracts to sell
            to_open       :: bool  :: open position
            limit_price   :: float :: limit price
                                      > 0.0 for NET_DEBIT
                                      < 0.0 for NET_CREDIT
                                      = 0.0 for NET_ZERO
                                      = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_BackRatio_ABI',
                PCHAR(symbol_buy), PCHAR(symbol_sell), c_size_t(quantity_buy),
                c_size_t(quantity_sell), c_int(to_open),
                c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, are_calls, strike_buy,
                   strike_sell, quantity_buy, quantity_sell, to_open,
                   limit_price=None):
            """Build a BackRatio Spread OrderTicket

            def Build2(underlying, month, day, year, are_calls, strike_buy,
                       strike_sell, quantity_buy, quantity_sell, to_open,
                       limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month         :: int   :: exp month of option
                day           :: int   :: exp day of option
                year          :: int   :: exp year of option
                are_calls     :: bool  :: is call spread (vs put spread)
                strike_buy    :: float :: strike price to buy
                strike_sell   :: float :: strike price to sell
                quantity_buy  :: int   :: # of contracts to buy
                quantity_sell :: int   :: # of contracts to sell
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_BackRatioEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_int(are_calls), c_double(strike_buy), c_double(strike_sell),
                c_size_t(quantity_buy), c_size_t(quantity_sell),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # BackRatio

    class Calendar:
        """Calendar - build Calendar OrderTickets"""

        @staticmethod
        def Build1(symbol_buy, symbol_sell, quantity, to_open,
                   limit_price=None):
            """Build a Calendar Spread OrderTicket

            def Build1(symbol_buy, symbol_sell, quantity, to_open,
                       limit_price=None):

                symbol_buy  :: str   :: option symbol to buy
                symbol_sell :: str   :: option symbol to sell
                quantity    :: int   :: # of spreads
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Calendar_ABI',
                PCHAR(symbol_buy), PCHAR(symbol_sell), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                   day_sell, year_sell, are_calls, strike, quantity,
                   to_open, limit_price=None):
            """Build a Calendar Spread OrderTicket

            def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                       day_sell, year_sell, are_calls, strike, quantity,
                       to_open, limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month_buy     :: int   :: exp month of option to buy
                day_buy       :: int   :: exp day of option to buy
                year_buy      :: int   :: exp year of option to buy
                month_sell    :: int   :: exp month of option to sell
                day_sell      :: int   :: exp day of option to sell
                year_selll    :: int   :: exp year of option to sell
                are_calls     :: bool  :: is call spread (vs put spread)
                strike        :: float :: strike price to trade
                quantity      :: int   :: # of spreads
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_CalendarEx_ABI',
                PCHAR(underlying), c_uint(month_buy), c_uint(day_buy),
                c_uint(year_buy), c_uint(month_sell), c_uint(day_sell),
                c_uint(year_sell), c_int(are_calls), c_double(strike),
                c_size_t(quantity), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # Calendar

    class Diagonal:
        """Diagonal - build Diagonal OrderTickets"""

        @staticmethod
        def Build1(symbol_buy, symbol_sell, quantity, to_open,
                   limit_price=None):
            """Build a Diagonal Spread OrderTicket

            def Build1(symbol_buy, symbol_sell, quantity, to_open,
                       limit_price=None):

                symbol_buy  :: str   :: option symbol to buy
                symbol_sell :: str   :: option symbol to sell
                quantity    :: int   :: # of spreads
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Diagonal_ABI',
                PCHAR(symbol_buy), PCHAR(symbol_sell), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                   day_sell, year_sell, are_calls, strike_buy, strike_sell,
                   quantity, to_open, limit_price=None):
            """Build a Diagonal Spread OrderTicket

            def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                       day_sell, year_sell, are_calls, strike_buy, strike_sell,
                       quantity, to_open, limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month_buy     :: int   :: exp month of option to buy
                day_buy       :: int   :: exp day of option to buy
                year_buy      :: int   :: exp year of option to buy
                month_sell    :: int   :: exp month of option to sell
                day_sell      :: int   :: exp day of option to sell
                year_selll    :: int   :: exp year of option to sell
                are_calls     :: bool  :: is call spread (vs put spread)
                strike_buy    :: float :: strike price to buy
                strike_sell   :: float :: strike price to sell
                quantity      :: int   :: # of spreads
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_DiagonalEx_ABI',
                PCHAR(underlying), c_uint(month_buy), c_uint(day_buy),
                c_uint(year_buy), c_uint(month_sell), c_uint(day_sell),
                c_uint(year_sell), c_int(are_calls), c_double(strike_buy),
                c_double(strike_sell), c_size_t(quantity), c_int(to_open),
                c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # Diagonal

    class Straddle:
        """Straddle - build Straddle OrderTickets"""

        @staticmethod
        def Build1(symbol_call, symbol_put, quantity, is_buy, to_open,
                  limit_price=None):
            """Build a Straddle Spread OrderTicket

            def Build1(symbol_call, symbol_put, quantity, is_buy, to_open,
                       limit_price=None):

                symbol_call :: str   :: option symbol of call
                symbol_put  :: str   :: option symbol of put
                quantity    :: int   :: # of spreads
                is_buy      :: bool  :: is buy order (buy-buy)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Straddle_ABI',
                PCHAR(symbol_call), PCHAR(symbol_put), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike, quantity, is_buy,
                   to_open, limit_price=None):
            """Build a Straddle Spread OrderTicket

            def Build2(underlying, month, day, year, strike, quantity, is_buy,
                       to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                strike      :: float :: strike price to trade
                quantity    :: int   :: # of spreads to trade
                is_buy      :: bool  :: is buy order (buy-buy)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                         > 0.0 for NET_DEBIT
                                         < 0.0 for NET_CREDIT
                                         = 0.0 for NET_ZERO
                                         = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_StraddleEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike), c_size_t(quantity), c_int(is_buy),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # Straddle

    class Strangle:
        """Strangle - build Strangle OrderTickets"""

        @staticmethod
        def Build1(symbol_call, symbol_put, quantity, is_buy, to_open,
                  limit_price=None):
            """Build a Strangle Spread OrderTicket

            def Build1(symbol_call, symbol_put, quantity, is_buy, to_open,
                       limit_price=None):

                symbol_call :: str   :: option symbol of call
                symbol_put  :: str   :: option symbol of put
                quantity    :: int   :: # of spreads
                is_buy      :: bool  :: is buy order (buy-buy)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Strangle_ABI',
                PCHAR(symbol_call), PCHAR(symbol_put), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike_call, strike_put,
                   quantity, is_buy, to_open, limit_price=None):
            """Build a Strangle Spread OrderTicket

            def Build2(underlying, month, day, year, strike_call, strike_put,
                       quantity, is_buy, to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                strike_call :: float :: strike price of call to trade
                strike_put :: float  :: strike price of put to trade
                quantity    :: int   :: # of spreads to trade
                is_buy      :: bool  :: is buy order (buy-buy)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                         > 0.0 for NET_DEBIT
                                         < 0.0 for NET_CREDIT
                                         = 0.0 for NET_ZERO
                                         = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_StrangleEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike_call), c_double(strike_put), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # Strangle

    class CollarSynthetic:
        """CollarSynthetic - build Synthetic Collar OrderTickets"""
        @staticmethod
        def Build1(symbol_buy, symbol_sell, quantity, to_open,
                   limit_price=None):
            """Build a Synthetic Collar Spread OrderTicket

            def Build1(symbol_buy, symbol_sell, quantity, to_open,
                       limit_price=None):

                symbol_buy  :: str   :: option symbol to buy
                symbol_sell :: str   :: option symbol to sell
                quantity    :: int   :: # of spreads
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                        > 0.0 for NET_DEBIT
                                        < 0.0 for NET_CREDIT
                                        = 0.0 for NET_ZERO
                                        = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_CollarSynthetic_ABI',
                PCHAR(symbol_buy), PCHAR(symbol_sell), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike_call, strike_put,
                   quantity, is_buy, to_open, limit_price=None):
            """Build a Synthetic Collar Spread OrderTicket

            def Build2(underlying, month, day, year, strike_call, strike_put,
                       quantity, is_buy, to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                strike_call :: float :: strike price of call to trade
                strike_put  :: float :: strike price of put to trade
                quantity    :: int   :: # of spreads to trade
                is_buy      :: bool  :: is buy order (buy-buy)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                         > 0.0 for NET_DEBIT
                                         < 0.0 for NET_CREDIT
                                         = 0.0 for NET_ZERO
                                         = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_CollarSyntheticEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike_call), c_double(strike_put), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # CollarSynthetic

    class CollarWithStock:
        """CollarWithStock - build Collar (With Stock) OrderTickets"""

        @staticmethod
        def Build1(symbol_option_buy, symbol_option_sell, symbol_stock,
                  quantity, is_buy, to_open, limit_price=None):
            """Build a Collar With Stock Spread OrderTicket

            def Build1(symbol_option_buy, symbol_option_sell, symbol_stock,
                       quantity, is_buy, to_open, limit_price=None):

                symbol_option_buy  :: str   :: option symbol to buy
                symbol_option_sell :: str   :: option symbol to sell
                symbol_stock       :: str   :: underlying stock to trade
                quantity           :: int   :: # of spreads (contracts)
                is_buy             :: bool  :: is buy order (stock)
                to_open            :: bool  :: open position
                limit_price        :: float :: limit price
                                               > 0.0 for NET_DEBIT
                                               < 0.0 for NET_CREDIT
                                               = 0.0 for NET_ZERO
                                               = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_CollarWithStock_ABI',
                PCHAR(symbol_option_buy), PCHAR(symbol_option_sell),
                PCHAR(symbol_stock), c_size_t(quantity), c_int(is_buy),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike_call, strike_put,
                   quantity, is_buy, to_open, limit_price=None):
            """Build a Collar With Stock Spread OrderTicket

            def Build2(underlying, month, day, year, strike_call, strike_put,
                       quantity, is_buy, to_open, limit_price=None):

                underlying  :: str   :: underlying security(e.g 'SPY')
                month       :: int   :: exp month of option
                day         :: int   :: exp day of option
                year        :: int   :: exp year of option
                strike_call :: float :: strike price of call to trade
                strike_put  :: float :: strike price of put to trade
                quantity    :: int   :: # of spreads (contracts)
                is_buy      :: bool  :: is buy order (stock)
                to_open     :: bool  :: open position
                limit_price :: float :: limit price
                                         > 0.0 for NET_DEBIT
                                         < 0.0 for NET_CREDIT
                                         = 0.0 for NET_ZERO
                                         = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_CollarWithStockEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike_call), c_double(strike_put),
                c_size_t(quantity), c_int(is_buy), c_int(to_open),
                c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # CollarWithStock

    class Condor:
        """Condor - build Condor OrderTickets"""

        @staticmethod
        def Build1(symbol_outer1, symbol_inner1, symbol_inner2, symbol_outer2,
                  quantity, is_buy, to_open, limit_price=None):
            """Build a Condor Spread OrderTicket

            def Build1(symbol_outer1, symbol_inner1, symbol_inner2,
                       symbol_outer2, quantity, is_buy, to_open,
                       limit_price=None):

                symbol_outer1 :: str   :: outer option symbol (wing 1)
                symbol_inner1 :: str   :: inner option symbol (body 1)
                symbol_inner2 :: str   :: inner option symbol (body 2)
                symbol_outer2 :: str   :: outer option symbol (wing 2)
                quantity      :: int   :: # of spreads to trade
                is_buy        :: bool  :: is buy order
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build('BuildOrder_Spread_Condor_ABI',
                PCHAR(symbol_outer1), PCHAR(symbol_inner1),
                PCHAR(symbol_inner2), PCHAR(symbol_outer2),
                c_size_t(quantity), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike_outer1, strike_inner1,
                   strike_inner2, strike_outer2, are_calls, quantity, is_buy,
                   to_open, limit_price=None):
            """Build a Condor Spread OrderTicket

            def Build2(underlying, month, day, year, strike_outer1,
                       strike_inner1, strike_inner2, strike_outer2, are_calls,
                       quantity, is_buy, to_open, limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month         :: int   :: exp month of option
                day           :: int   :: exp day of option
                year          :: int   :: exp year of option
                strike_outer1 :: float :: outer strike price (wing 1)
                strike_inner1 :: float :: inner strike price (body 1)
                strike_inner2 :: float :: inner strike price (body 2)
                strike_outer2 :: float :: outer strike price (wing 2)
                quantity      :: int   :: # of spreads
                is_buy        :: bool  :: is buy order
                to_open       :: bool  :: open position
                limit_price   : float  :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_CondorEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike_outer1), c_double(strike_inner1),
                c_double(strike_inner2), c_double(strike_outer2),
                c_int(are_calls), c_size_t(quantity), c_size_t(quantity),
                c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        class Unbalanced:
            """Unbalanced - build Unbalanced Condor Order Tickets"""

            @staticmethod
            def Build1(symbol_outer1, symbol_inner1, symbol_inner2,
                      symbol_outer2, quantity1, quantity2, is_buy, to_open,
                      limit_price=None):
                """Build an Unbalanced Condor Spread OrderTicket

            def Build1(symbol_outer1, symbol_inner1, symbol_inner2,
                       symbol_outer2, quantity1, quantity2, is_buy, to_open,
                       limit_price=None):

                symbol_outer1 :: str   :: outer option symbol (wing 1)
                symbol_inner1 :: str   :: inner option symbol (body 1)
                symbol_inner2 :: str   :: inner option symbol (body 2)
                symbol_outer2 :: str   :: outer option symbol (wing 2)
                quantity1     :: int   :: # of (wing/body 1) contracts
                quantity2     :: int   :: # of (wing/body 2) contracts
                is_buy        :: bool  :: is buy order
                to_open       :: bool  :: open position
                limit_price   :: float :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build('BuildOrder_Spread_Condor_ABI',
                    PCHAR(symbol_outer1), PCHAR(symbol_inner1),
                    PCHAR(symbol_inner2), PCHAR(symbol_outer2),
                    c_size_t(quantity1), c_size_t(quantity2),
                    c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            @staticmethod
            def Build2(underlying, month, day, year, strike_outer1,
                       strike_inner1, strike_inner2, strike_outer2,
                       are_calls, quantity1, quantity2, is_buy, to_open,
                       limit_price=None):
                """Build an Unbalanced Condor Spread OrderTicket

            def Build2(underlying, month, day, year, strike_outer1,
                       strike_inner1, strike_inner2, strike_outer2,
                       are_calls, quantity1, quantity2, is_buy, to_open,
                       limit_price=None):

                underlying    :: str   :: underlying security(e.g 'SPY')
                month         :: int   :: exp month of option
                day           :: int   :: exp day of option
                year          :: int   :: exp year of option
                strike_outer1 :: float :: outer strike price (wing 1)
                strike_inner1 :: float :: inner strike price (body 1)
                strike_inner2 :: float :: inner strike price (body 2)
                strike_outer2 :: float :: outer strike price (wing 2)
                quantity1     :: int   :: # of (wing/body 1) contracts
                quantity2     :: int   :: # of (wing/body 2) contracts
                is_buy        :: bool  :: is buy order
                to_open       :: bool  :: open position
                limit_price   : float  :: limit price
                                          > 0.0 for NET_DEBIT
                                          < 0.0 for NET_CREDIT
                                          = 0.0 for NET_ZERO
                                          = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_CondorEx_ABI',
                    PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                    c_double(strike_outer1), c_double(strike_inner1),
                    c_double(strike_inner2), c_double(strike_outer2),
                    c_int(are_calls), c_size_t(quantity1), c_size_t(quantity2),
                    c_int(is_buy), c_int(to_open), c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))
        # Unbalanced

    # Condor

    class IronCondor:
        """Iron Condor - build Iron Condor OrderTickets"""

        @staticmethod
        def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                  symbol_put_sell, quantity, to_open, limit_price=None):
            """Build an Iron Condor Spread OrderTicket

            def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                       symbol_put_sell, quantity, to_open, limit_price=None):

                symbol_call_buy  :: str   :: call option symbol to buy
                symbol_call_sell :: str   :: call option symbol to sell
                symbol_put_buy   :: str   :: put option symbol to buy
                symbol_put_sell  :: str   :: put option symbol to sell
                quantity         :: int   :: # of spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_IronCondor_ABI',
                PCHAR(symbol_call_buy), PCHAR(symbol_call_sell),
                PCHAR(symbol_put_buy), PCHAR(symbol_put_sell),
                c_size_t(quantity), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month, day, year, strike_call_buy,
                   strike_call_sell, strike_put_buy, strike_put_sell,
                   quantity, to_open, limit_price=None):
            """Build an Iron Condor Spread OrderTicket

            def Build2(underlying, month, day, year, strike_call_buy,
                       strike_call_sell, strike_put_buy, strike_put_sell,
                       quantity, is_buy, to_open, limit_price=None):

                underlying       :: str   :: underlying security(e.g 'SPY')
                month            :: int   :: exp month of option
                day              :: int   :: exp day of option
                year             :: int   :: exp year of option
                strike_call_buy  :: float :: call strike price to buy
                strike_call_sell :: float :: call strike price to sell
                strike_put_buy   :: float :: put strike price to buy
                strike_put_sell  :: float :: put strike price to sell
                quantity         :: int   :: # of spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_IronCondorEx_ABI',
                PCHAR(underlying), c_uint(month), c_uint(day), c_uint(year),
                c_double(strike_call_buy), c_double(strike_call_sell),
                c_double(strike_put_buy), c_double(strike_put_sell),
                c_size_t(quantity), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        class Unbalanced:
            """Unbalanced - build Unbalanced Iron Condor OrderTickets"""

            @staticmethod
            def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                      symbol_put_sell, quantity_call, quantity_put, to_open,
                      limit_price=None):
                """Build an Unbalanced Iron Condor Spread OrderTicket

            def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                       symbol_put_sell, quantity_call, quantity_put, to_open,
                       limit_price=None):

                symbol_call_buy  :: str   :: call option symbol to buy
                symbol_call_sell :: str   :: call option symbol to sell
                symbol_put_buy   :: str   :: put option symbol to buy
                symbol_put_sell  :: str   :: put option symbol to sell
                quantity_call    :: int   :: # of call spreads to trade
                quantity_put    :: int   :: # of put spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_IronCondor_ABI',
                    PCHAR(symbol_call_buy), PCHAR(symbol_call_sell),
                    PCHAR(symbol_put_buy), PCHAR(symbol_put_sell),
                    c_size_t(quantity_call), c_size_t(quantity_put),
                    c_int(to_open), c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))

            @staticmethod
            def Build2(underlying, month, day, year, strike_call_buy,
                       strike_call_sell, strike_put_buy, strike_put_sell,
                       quantity_call, quantity_put, to_open,
                       limit_price=None):
                """Build an Unbalanced Iron Condor Spread OrderTicket

            def Build2(underlying, month, day, year, strike_call_buy,
                       strike_call_sell, strike_put_buy, strike_put_sell,
                       quantity_call, quantity_put, is_buy, to_open,
                       limit_price=None):

                underlying       :: str   :: underlying security(e.g 'SPY')
                month            :: int   :: exp month of option
                day              :: int   :: exp day of option
                year             :: int   :: exp year of option
                strike_call_buy  :: float :: call strike price to buy
                strike_call_sell :: float :: call strike price to sell
                strike_put_buy   :: float :: put strike price to buy
                strike_put_sell  :: float :: put strike price to sell
                quantity_call    :: int   :: # of call spreads to trade
                quantity_put     :: int   :: # of put spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
                return _OrderBuilder._abi_build(
                    'BuildOrder_Spread_IronCondorEx_ABI',
                    PCHAR(underlying), c_uint(month), c_uint(day),
                    c_uint(year), c_double(strike_call_buy),
                    c_double(strike_call_sell), c_double(strike_put_buy),
                    c_double(strike_put_sell), c_size_t(quantity_call),
                    c_size_t(quantity_put), c_int(to_open),
                    c_int(limit_price is None),
                    c_double(0.0 if limit_price is None else limit_price))
        # Unbalanced

    # IronCondor

    class DoubleDiagonal:
        """DobleDiagonal - build Double Diagonal OrderTickets"""

        @staticmethod
        def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                  symbol_put_sell, quantity, to_open, limit_price=None):
            """Build a Double Diagonal Spread OrderTicket

            def Build1(symbol_call_buy, symbol_call_sell, symbol_put_buy,
                       symbol_put_sell, quantity, to_open, limit_price=None):

                symbol_call_buy  :: str   :: call option symbol to buy
                symbol_call_sell :: str   :: call option symbol to sell
                symbol_put_buy   :: str   :: put option symbol to buy
                symbol_put_sell  :: str   :: put option symbol to sell
                quantity         :: int   :: # of spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_DoubleDiagonal_ABI',
                PCHAR(symbol_call_buy), PCHAR(symbol_call_sell),
                PCHAR(symbol_put_buy), PCHAR(symbol_put_sell),
                c_size_t(quantity), c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))

        @staticmethod
        def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                   day_sell, year_sell, strike_call_buy, strike_call_sell,
                   strike_put_buy, strike_put_sell, quantity, to_open,
                   limit_price=None):
            """Build a Double Diagonal Spread OrderTicket

            def Build2(underlying, month_buy, day_buy, year_buy, month_sell,
                       day_sell, year_sell, strike_call_buy, strike_call_sell,
                       strike_put_buy, strike_put_sell, quantity, to_open,
                       limit_price=None):

                underlying       :: str   :: underlying security(e.g 'SPY')
                month_buy        :: int   :: exp month of option to buy
                day_buy          :: int   :: exp day of option to buy
                year_buy         :: int   :: exp year of option to buy
                month_sell       :: int   :: exp month of option to sell
                day_sell         :: int   :: exp day of option to sell
                year_sell        :: int   :: exp year of option to sell
                strike_call_buy  :: float :: call strike price to buy
                strike_call_sell :: float :: call strike price to sell
                strike_put_buy   :: float :: put strike price to buy
                strike_put_sell  :: float :: put strike price to sell
                quantity         :: int   :: # of spreads to trade
                to_open          :: bool  :: open position
                limit_price      :: float :: limit price
                                             > 0.0 for NET_DEBIT
                                             < 0.0 for NET_CREDIT
                                             = 0.0 for NET_ZERO
                                             = None for Market Order

            returns -> OrderTicket
            throws -> LibraryNotLoaded, CLibException
            """
            return _OrderBuilder._abi_build(
                'BuildOrder_Spread_DoubleDiagonalEx_ABI',
                PCHAR(underlying), c_uint(month_buy), c_uint(day_buy),
                c_uint(year_buy), c_uint(month_sell), c_uint(day_sell),
                c_uint(year_sell), c_double(strike_call_buy),
                c_double(strike_call_sell), c_double(strike_put_buy),
                c_double(strike_put_sell), c_size_t(quantity),
                c_int(to_open), c_int(limit_price is None),
                c_double(0.0 if limit_price is None else limit_price))
    # DoubleDiagonal


class ConditionalOrderBuilder( _OrderBuilder ):
    """ConditionaOrderBuilder - "build Conditional Order OrderTickets"""

    @staticmethod
    def OTO(order_primary, order_conditional):
        """Build a One-Triggers-Other Conditional OrderTicket

        def OTO(order_primary, order_conditional):

            order_primary     :: OrderTicket :: active order
            order_conditional :: OrderTicket :: order to be activated once
                                                primary order is executed

        returns -> OrderTicket
        throws -> LibraryNotLoaded, CLibException
        """
        #check obj are of OrderTicket
        return _OrderBuilder._abi_build("BuildOrder_OneTriggersOther_ABI",
            _REF(order_primary._obj), _REF(order_conditional._obj) )

    @staticmethod
    def OCO(order1, order2):
        """Build a One-Cancels-Other Conditional OrderTicket

        def OCO(order1, order2):

            order1 :: OrderTicket :: active order #1 (until either executes)
            order2 :: OrderTicket :: active order #2 (until either executes)

        returns -> OrderTicket
        throws -> LibraryNotLoaded, CLibException
        """
        #check obj are of OrderTicket
        return _OrderBuilder._abi_build("BuildOrder_OneCancelsOther_ABI",
            _REF(order1._obj), _REF(order2._obj))







