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


class SimpleOrderBuilder:
    @staticmethod
    def _abi_build(fname, *args):
        o = _OrderTicket_C()
        clib.call(fname, *(args + (_REF(o),)) )
        return OrderTicket._init_from_cproxy(o)
        
    @staticmethod
    def build(order_type, asset_type, symbol, instruction, quantity, 
              limit_price=0.0, stop_price=0.0):
        return SimpleOrderBuilder._abi_build('BuildOrder_Simple_ABI', 
                    c_int(order_type), c_int(asset_type), PCHAR(symbol), 
                    c_int(instruction), c_size_t(quantity), 
                    c_double(limit_price), c_double(stop_price))
    
    class Equity:
        @staticmethod
        def build(order_type, symbol, instruction, quantity, limit_price=0.0, 
                  stop_price=0.0):
            return SimpleOrderBuilder._abi_build('BuildOrder_Equity_ABI', 
                        c_int(order_type), PCHAR(symbol), c_int(instruction), 
                        c_size_t(quantity), c_double(limit_price), 
                        c_double(stop_price))
        
        class Market:
            @staticmethod
            def Buy(symbol, quantity):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_MARKET, 
                            symbol, ORDER_INSTRUCTION_BUY, quantity)
            @staticmethod
            def Sell(symbol, quantity):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_MARKET, 
                            symbol, ORDER_INSTRUCTION_SELL, quantity)
                
            @staticmethod
            def Short(symbol, quantity):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_MARKET, 
                            symbol, ORDER_INSTRUCTION_SELL_SHORT, quantity)
                
            @staticmethod
            def Cover(symbol, quantity):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_MARKET, 
                            symbol, ORDER_INSTRUCTION_BUY_TO_COVER, quantity)
                        
        class Limit:
            @staticmethod
            def Buy(symbol, quantity, price):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_LIMIT, 
                            symbol, ORDER_INSTRUCTION_BUY, quantity, price)
            @staticmethod
            def Sell(symbol, quantity, price):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_LIMIT, 
                            symbol, ORDER_INSTRUCTION_SELL, quantity, price)
                
            @staticmethod
            def Short(symbol, quantity, price):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_LIMIT, 
                            symbol, ORDER_INSTRUCTION_SELL_SHORT, quantity, 
                            price)
                
            @staticmethod
            def Cover(symbol, quantity, price):
                return SimpleOrderBuilder.Equity.build(ORDER_TYPE_LIMIT, 
                            symbol, ORDER_INSTRUCTION_BUY_TO_COVER, quantity, 
                            price)
        
        class Stop:
            @staticmethod
            def _build(symbol, instruction, quantity, stop_price, limit_price):
                ot = ORDER_TYPE_STOP_LIMIT if limit_price else ORDER_TYPE_STOP
                return SimpleOrderBuilder.Equity.build(ot, symbol, instruction, 
                            quantity, limit_price, stop_price)
                
            @staticmethod
            def Buy(symbol, quantity, stop_price, limit_price=0.0):
                return SimpleOrderBuilder.Equity.Stop._build(symbol,
                            ORDER_INSTRUCTION_BUY, quantity, stop_price, 
                            limit_price)
                
            @staticmethod
            def Sell(symbol, quantity, stop_price, limit_price=0.0):
                return SimpleOrderBuilder.Equity.Stop._build(symbol,
                            ORDER_INSTRUCTION_SELL, quantity, stop_price, 
                            limit_price)
                
            @staticmethod
            def Short(symbol, quantity, stop_price, limit_price=0.0):
                return SimpleOrderBuilder.Equity.Stop._build(symbol,
                            ORDER_INSTRUCTION_SELL_SHORT, quantity, 
                            stop_price, limit_price)
                
            @staticmethod
            def Cover(symbol, quantity, stop_price, limit_price=0.0):
                return SimpleOrderBuilder.Equity.Stop._build(symbol,
                            ORDER_INSTRUCTION_BUY_TO_COVER, quantity, 
                            stop_price, limit_price)
       
    
    class Option:
        @staticmethod
        def build1(order_type, symbol, quantity, is_buy, to_open, price=0.0):
            return SimpleOrderBuilder._abi_build('BuildOrder_Option_ABI', 
                        c_int(order_type), PCHAR(symbol), c_size_t(quantity), 
                        c_int(is_buy), c_int(to_open), c_double(price))
        
        @staticmethod
        def build2(order_type, underlying, month, day, year, is_call, strike, 
                   quantity, is_buy, to_open, price=0.0):
            return SimpleOrderBuilder._abi_build('BuildOrder_OptionEx_ABI', 
                        c_int(order_type), PCHAR(underlying), c_uint(month), 
                        c_uint(day), c_uint(year), c_int(is_call), 
                        c_double(strike), c_size_t(quantity), c_int(is_buy),
                        c_int(to_open), c_double(price))
        
        class Market:
            @staticmethod
            def BuyToOpen1(symbol, quantity):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_MARKET,
                            symbol, quantity, True, True)
            
            @staticmethod
            def BuyToOpen2(underlying, month, day, year, is_call, strike, 
                           quantity):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_MARKET,
                            underlying, month, day, year, is_call, strike, 
                            quantity, True, True)

            @staticmethod
            def SellToOpen1(symbol, quantity):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_MARKET,
                            symbol, quantity, False, True)
            
            @staticmethod
            def SellToOpen2(underlying, month, day, year, is_call, strike, 
                           quantity):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_MARKET,
                            underlying, month, day, year, is_call, strike, 
                            quantity, False, True)
                
            @staticmethod
            def BuyToClose1(symbol, quantity):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_MARKET,
                            symbol, quantity, True, False)
            
            @staticmethod
            def BuyToClose2(underlying, month, day, year, is_call, strike, 
                           quantity):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_MARKET,
                            underlying, month, day, year, is_call, strike, 
                            quantity, True, False)
                
            @staticmethod
            def SellToClose1(symbol, quantity):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_MARKET,
                            symbol, quantity, False, False)
            
            @staticmethod
            def SellToClose2(underlying, month, day, year, is_call, strike, 
                           quantity):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_MARKET,
                            underlying, month, day, year, is_call, strike, 
                            quantity, False, False)                                                                
        
        class Limit:
            @staticmethod
            def BuyToOpen1(symbol, quantity, price):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_LIMIT,
                            symbol, quantity, True, True, price)
            
            @staticmethod
            def BuyToOpen2(underlying, month, day, year, is_call, strike, 
                           quantity, price):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_LIMIT,
                            underlying, month, day, year, is_call, strike, 
                            quantity, True, True, price)

            @staticmethod
            def SellToOpen1(symbol, quantity, price):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_LIMIT,
                            symbol, quantity, False, True, price)
            
            @staticmethod
            def SellToOpen2(underlying, month, day, year, is_call, strike, 
                           quantity, price):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_LIMIT,
                            underlying, month, day, year, is_call, strike, 
                            quantity, False, True, price)
                
            @staticmethod
            def BuyToClose1(symbol, quantity, price):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_LIMIT,
                            symbol, quantity, True, False, price)
            
            @staticmethod
            def BuyToClose2(underlying, month, day, year, is_call, strike, 
                           quantity, price):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_LIMIT,
                            underlying, month, day, year, is_call, strike, 
                            quantity, True, False, price)
                
            @staticmethod
            def SellToClose1(symbol, quantity, price):
                return SimpleOrderBuilder.Option.build1(ORDER_TYPE_LIMIT,
                            symbol, quantity, False, False, price)
            
            @staticmethod
            def SellToClose2(underlying, month, day, year, is_call, strike, 
                           quantity, price):
                return SimpleOrderBuilder.Option.build2(ORDER_TYPE_LIMIT,
                            underlying, month, day, year, is_call, strike, 
                            quantity, False, False, price)  
    
    
# TODO class SpreadOrderBuilder    
    
# TODO class ConditionalOrderBuilder
    
    
    

