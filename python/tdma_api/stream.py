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

"""tdma_api/stream.py the streaming interface

The stream module provides the StreamerSession class and Subscription 
objects for getting streaming market data from the TDAmeritrade API.

StreamingSession mirrors the C++ version of the object from the 
TDAmeritradeAPI shared library, using the exported C/ABI methods through
ctypes.py to replicate the functionality. 

Before using you'll need to have obtained a valid Credentials object. (See
tdma_api.auth.__doc__ or README.md for details.)

Each session is passed a Credentials object, callback function,
and some optional timeout args.

The session is started by passing a collection of Subscription objects for
the particular streaming services desired. Subscriptions can also be added 
later. THERE CAN BE ONLY ONE ACTIVE SESSION  FOR EACH PRIMARY ACCOUNT ID.

Subscription types fall into two categories:

Field/Symbol - these are created with symbol strings and data fields defined
in the appropriately named module constants. (e.g QuotesSubscription)

Venue/Duration - these are created with Venue and/or Duration types defined
in the appropriately named module constants. (e.g NasdaqActivesSubscription)

See README_STREAMING.md for a more thorough explanation.
"""

from ctypes import byref as _REF, Structure as _Structure, c_int, c_void_p, \
                    c_ulonglong, CFUNCTYPE, c_char_p, c_ulong, pointer, POINTER
from inspect import signature
                    
import json

from . import clib
from .clib import PCHAR, PCHAR_BUFFER

DEF_CONNECT_TIMEOUT = 3000
DEF_LISTENING_TIMEOUT = 30000
DEF_SUBSCRIBE_TIMEOUT = 1500

CALLBACK_FUNC_TYPE = CFUNCTYPE(None, c_int, c_int, c_ulonglong, c_char_p)
CALLBACK_NARGS = 4

SERVICE_TYPE_NONE = 0
SERVICE_TYPE_QUOTE = 1
SERVICE_TYPE_OPTION = 2
SERVICE_TYPE_LEVELONE_FUTURES = 3
SERVICE_TYPE_LEVELONE_FOREX = 4
SERVICE_TYPE_LEVELONE_FUTURES_OPTIONS = 5
SERVICE_TYPE_NEWS_HEADLINE = 6
SERVICE_TYPE_CHART_EQUITY = 7
#SERVICE_TYPE_CHART_FOREX = 8
SERVICE_TYPE_CHART_FUTURES = 9
SERVICE_TYPE_CHART_OPTIONS = 10
SERVICE_TYPE_TIMESALE_EQUITY = 11
#SERVICE_TYPE_TIMESALE_FOREX = 12
SERVICE_TYPE_TIMESALE_FUTURES = 13
SERVICE_TYPE_TIMESALE_OPTIONS = 14
SERVICE_TYPE_ACTIVES_NASDAQ = 15
SERVICE_TYPE_ACTIVES_NYSE = 16
SERVICE_TYPE_ACTIVES_OTCBB = 17
SERVICE_TYPE_ACTIVES_OPTIONS = 18
SERVICE_TYPE_ADMIN = 19

QOS_EXPRESS = 0 
QOS_REAL_TIME = 1
QOS_FAST = 2
QOS_MODERATE = 3
QOS_SLOW = 4
QOS_DELAYED = 5

CALLBACK_TYPE_LISTENING_START = 0
CALLBACK_TYPE_LISTENING_STOP = 1
CALLBACK_TYPE_DATA = 2
CALLBACK_TYPE_REQUES_RESPONSE = 3
CALLBACK_TYPE_NOTIFY = 4
CALLBACK_TYPE_TIMEOUT = 5
CALLBACK_TYPE_ERROR = 6


def service_type_to_str(service):
    """Converts SERVICE_TYPE_[] constant to str."""
    return clib.to_str("StreamerServiceType_to_string_ABI", c_int, service)
    
def callback_type_to_str(cb_type):
    """Converts CALLBACK_TYPE_[] constant to str."""
    return clib.to_str("StreamingCallbackType_to_string_ABI", c_int, cb_type)
    

class _StreamingSession_C(_Structure): 
    """C struct representing StreamingSession_C type. (IMPLEMENTATION DETAIL)"""
    _fields_ = [
        ("obj", c_void_p), 
        ("type_id", c_int), 
        ("ctx", c_void_p)
        ] 
                              

class StreamingSession:
    """StreamingSession - object used for accessing the Streaming interface.
    
    The authenticated user creates a StreamingSession using their Credentials
    object and a callback function that will be called
    whenever session state changes or data is returned from the server.
    
    Callback: 
        def callback(int, int, int, json)
            arg1 :: int    :: CALLBACK_TYPE_[] constant
            arg2 :: int    :: SERVICE_TYPE_[] constant
            arg3 :: int    :: timestamp
            arg4 :: object :: message/data as json(list, dict, or None)  
            
        The callback is called from a different thread while the GIL(global
        interpreter lock) is released so BE CAREFUL what you do inside
        the function:
            - DO NOT call back into the session i.e use its methods 
            - DO NOT block the callback thread for extended periods
            - DO NOT assume exceptions raised will be handled
    
    In order to start the connection call .start() with a collection of
    Subscription objects. THERE CAN BE ONLY ONE ACTIVE SESSION FOR EACH
    PRIMARY ACCOUNT.
    
    The SERVICE_TYPE_[] constant passed to the callback(arg2) corresponds with 
    the Subscription classes, e.g SERVICE_TYPE_QUOTE --> QuotesSubscription. 
    Use the SERVICE_TO_SUBSCRIPTION dict to return the corresponding 
    class object or None if there isn't one (e.g SERVICE_TYPE_ADMIN --> None)    
    
    When done call .stop() to logout and close the connection.
    
        def __init__( self, creds, callback, 
                      connect_timeout=DEF_CONNECT_TIMEOUT,
                      listening_timeout=DEF_LISTENING_TIMEOUT,
                      subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT ):
        
            creds :: Credentials :: instance class received from auth.py            
            
            callback           :: func :: callback function for changes in
                                          session state or returned data                                               
            connect_timeout    :: int  :: time to wait for connection
            listening_timeout  :: int  :: time to wait for any message
            subscribe_timeout  :: int  :: time to wait for subscription
        
        ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__( self, creds, callback, 
                  connect_timeout=DEF_CONNECT_TIMEOUT,
                  listening_timeout=DEF_LISTENING_TIMEOUT,
                  subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT ):                
        self._creds = creds   
        self._cb_raw = callback
        self._cb_wrapper = self._build_callback_wrapper(callback)
        self._obj = _StreamingSession_C()
        clib.call(self._abi('Create'), _REF(creds), self._cb_wrapper, 
                  c_ulong(connect_timeout), c_ulong(listening_timeout), 
                  c_ulong(subscribe_timeout), _REF(self._obj))    
        self._alive = True                         
                                                     
    def __del__(self):         
        if hasattr(self,'_alive') and self._alive:
            self._alive = False 
            try:
                try:                                  
                    clib.call(self._abi('Destroy'), _REF(self._obj))
                except clib.CLibException as e:
                    print("CLibException in", self.__del__, ":", str(e))                                  
            except:
                pass
                   
    @property
    def credentials(self):
        return self._creds
    
    @classmethod
    def _build_callback_wrapper(cls, cb):
        if len(signature(cb).parameters) != CALLBACK_NARGS:
            raise TypeError("callback requires %i args" % CALLBACK_NARGS)
        f = lambda a,b,c,d : cb(a,b,c, json.loads(d.decode()) if d else None)
        return CALLBACK_FUNC_TYPE(f)
    
    @classmethod
    def _abi(cls, f):
        return "{}_{}_ABI".format(cls.__name__,f)
    
    @classmethod
    def _check_subs(cls, subs):
        if not subs:
            raise ValueError("no subscriptions")
        for s in subs:
            if not isinstance(s, _StreamingSubscription) or not s._obj:            
                raise TypeError("not a valid _StreamingSubscription")
            
    def _subscription_abi_call(self, fname, subscriptions):
        self._check_subs(subscriptions)
        l = len(subscriptions)       
        subs = (POINTER(_StreamingSubscription_C) * l)\
               (*[pointer(s._obj) for s in subscriptions])
        results = (c_int * l)(*([0] *l))               
        clib.call(self._abi(fname), _REF(self._obj), subs, l, results)
        return [bool(r) for r in results]

    def start(self, *subscriptions):
        """Start the session and login with one or more subscription objects.
                      
            def start(self, *subscriptions):
            
                *subscriptions :: object :: instances of class derived from 
                                            _StreamingSubscription
                                           
            returns -> collection of bool indicating success / failure of each
            throws  -> LibraryNotLoaded, CLibException 
        """
        return self._subscription_abi_call("Start", subscriptions)
                   
    def stop(self):
        """Stop the session, logout, and clear subscriptions."""
        clib.call(self._abi("Stop"), _REF(self._obj))

    def is_active(self):                    
        """Returns if the session is active."""
        return bool(clib.get_val(self._abi("IsActive"), c_int, self._obj))
    
    def add_subscriptions(self, *subscriptions):
        """Add subscriptions to an ACTIVE session.
        
            def add_subscriptions(self, *subscriptions):
            
                *subscriptions :: object :: instances of class derived from 
                                            _StreamingSubscription
                                           
            returns -> collection of bool indicating success / failure of each
            throws   -> LibraryNotLoaded, CLibException 
        """
        return self._subscription_abi_call("AddSubscriptions", subscriptions)
    
    def set_qos(self, qos):
        """Sets/changes the quality-of-service.
        
            def set_qos(self, qos):
            
                qos :: int :: QOS_[] constant indicating quality-of-service
                                           
            returns -> bool indicating success / failure
            throws   -> LibraryNotLoaded, CLibException 
        """
        r = c_int()
        clib.call(self._abi("SetQOS"), _REF(self._obj), c_int(qos), _REF(r) )
        return bool(r)
        
    def get_qos(self):
        """Returns the quality-of-service."""
        return clib.get_val(self._abi("GetQOS"), c_int, self._obj)
        
        
        
class _StreamingSubscription_C(_Structure): 
    """C struct representing StreamingSubscription_C type. (IMPLEMENTATION DETAIL)"""
    _fields_ = [
        ("obj", c_void_p), 
        ("type_id", c_int)
        ] 


class _StreamingSubscription:
    """_StreamingSubscription - Base Subscription class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, *args):        
        self._obj = _StreamingSubscription_C()            
        args = args + (_REF(self._obj),)            
        clib.call(self._abi('Create'), *args)    
        self._alive = True 
                                                  
    def __del__(self):         
        if hasattr(self,'_alive') and self._alive:
            self._alive = False 
            try:
                try:                                  
                    clib.call(self._abi('Destroy'), _REF(self._obj))
                except clib.CLibException as e:
                    print("CLibException in", self.__del__, ":", str(e))                                   
            except:
                pass
            
    @classmethod
    def _abi(cls, f):
        return "{}_{}_ABI".format(cls.__name__,f)
                
    def get_service(self):
        """Returns the service type as SERVICE_TYPE_[] constant."""
        return clib.get_val("StreamingSubscription_GetService_ABI", c_int,
                             self._obj )
    
    def get_command(self):
        """Returns the command string."""
        return clib.get_str("StreamingSubscription_GetCommand_ABI", self._obj)
        
    
class _SubscriptionBySymbolBase(_StreamingSubscription):
    """_SubscriptionBySymbolBase - Base Subscription class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, symbols, fields):
        sbuf = PCHAR_BUFFER(symbols)                                  
        fbuf = (c_int * len(fields))(*[c_int(f) for f in fields])    
        super().__init__(sbuf, len(symbols), fbuf, len(fields))               
        
    def get_symbols(self):
        """Returns symbols in subscription."""
        return clib.get_strs("SubscriptionBySymbolBase_GetSymbols_ABI", 
                              self._obj)

    def get_fields(self):
        """Returns fields in subscription as self.FIELD_[] constant."""        
        return clib.get_vals(self._abi("GetFields"), c_int, self._obj, 
                             clib.free_fields_buffer)   
    

class QuotesSubscription(_SubscriptionBySymbolBase):
    """QuotesSubscription - streaming quotes. 
    
    StreamingSession calls back when quote fields change.
    
    Supported Markets/Products:
        - Listed (NYSE, AMEX, Pacific quotes and trades)
        - NASDAQ (quotes and trades)
        - OTCBB (quotes and trades)
        - Pinks (quotes only)
        - Mutual Funds (no quotes)
        - Indicies (trades only)
        - Indicators
        
    def __init__(self, symbols, fields):
    
        symbols :: [str, str...] :: symbols to get data for
        fields  :: [int, int...] :: self.FIELD_[] constant values indicating
                                    what type of data to return
        
        throws -> LibraryNotLoaded, CLibException
    """
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_BID_PRICE = 1
    FIELD_ASK_PRICE = 2
    FIELD_LAST_PRICE = 3
    FIELD_BID_SIZE = 4
    FIELD_ASK_SIZE = 5
    FIELD_ASK_ID = 6
    FIELD_BID_ID = 7
    FIELD_TOTAL_VOLUME = 8
    FIELD_LAST_SIZE = 9
    FIELD_TRADE_TIME = 10
    FIELD_QUOTE_TIME = 11
    FIELD_HIGH_PRICE = 12
    FIELD_LOW_PRICE = 13
    FIELD_BID_TICK = 14
    FIELD_CLOSE_PRICE = 15
    FIELD_EXCHANGE_ID = 16
    FIELD_MARGINABLE = 17
    FIELD_SHORTABLE = 18
    FIELD_ISLAND_BID = 19
    FIELD_ISLAND_ASK = 20
    FIELD_ISLAND_VOLUME = 21
    FIELD_QUOTE_DAY = 22
    FIELD_TRADE_DAY = 23
    FIELD_VOLATILITY = 24
    FIELD_DESCRIPTION = 25
    FIELD_LAST_ID = 26
    FIELD_DIGITS = 27
    FIELD_OPEN_PRICE = 28
    FIELD_NET_CHANGE = 29
    FIELD_HIGH_52_WEEK = 30
    FIELD_LOW_52_WEEK = 31
    FIELD_PE_RATIO = 32
    FIELD_DIVIDEND_AMOUNT = 33
    FIELD_DIVIDEND_YEILD = 34
    FIELD_ISLAND_BID_SIZE = 35
    FIELD_ISLAND_ASK_SIZE = 36
    FIELD_NAV = 37
    FIELD_FUND_PRICE = 38
    FIELD_EXCHANGED_NAME = 39
    FIELD_DIVIDEND_DATE = 40
    FIELD_REGULAR_MARKET_QUOTE = 41
    FIELD_REGULAR_MARKET_TRADE = 42
    FIELD_REGULAR_MARKET_LAST_PRICE = 43
    FIELD_REGULAR_MARKET_LAST_SIZE = 44
    FIELD_REGULAR_MARKET_TRADE_TIME = 45
    FIELD_REGULAR_MARKET_TRADE_DAY = 46
    FIELD_REGULAR_MARKET_NET_CHANGE = 47
    FIELD_SECURITY_STATUS = 48
    FIELD_MARK = 49
    FIELD_QUOTE_TIME_AS_LONG = 50
    FIELD_TRADE_TIME_AS_LONG = 51
    FIELD_REGULAR_MARKET_TRADE_TIME_AS_LONG = 52
 
 
_FIELDS_SUBSCRIPTION__doc__ = """\
{name}Subscription - {instr} quotes.

    StreamingSession calls back when quote fields change.
    
    def __init__(self, symbols, fields):
    
        symbols :: [str, str...] :: symbols to get data for
        fields  :: [int, int...] :: self.FIELD_[] constant values indicating
                                    what type of data to return
        
        throws -> LibraryNotLoaded, CLibException
"""
    
class OptionsSubscription(_SubscriptionBySymbolBase):
    __doc__ = _FIELDS_SUBSCRIPTION__doc__.format(name="Options", 
                                                 instr="Option")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_DESCRIPTION = 1
    FIELD_BID_PRICE = 2
    FIELD_ASK_PRICE = 3
    FIELD_LAST_PRICE = 4    
    FIELD_HIGH_PRICE = 5
    FIELD_LOW_PRICE = 6
    FIELD_CLOSE_PRICE = 7
    FIELD_TOTAL_VOLUME = 8
    FIELD_OPEN_INTEREST = 9
    FIELD_VOLATILITY = 10
    FIELD_QUOTE_TIME = 11
    FIELD_TRADE_TIME = 12
    FIELD_MONEY_INTRINSIC_VALUE = 13
    FIELD_QUOTE_DAY = 14
    FIELD_TRADE_DAY = 15
    FIELD_EXPIRATION_YEAR = 16
    FIELD_MULTIPLIER = 17
    FIELD_DIGITS = 18
    FIELD_OPEN_PRICE = 19
    FIELD_BID_SIZE = 20
    FIELD_ASK_SIZE = 21
    FIELD_LAST_SIZE = 22
    FIELD_NET_CHANGE = 23
    FIELD_STRIKE_PRICE = 24
    FIELD_CONTRACT_TYPE = 25
    FIELD_UNDERLYING = 26
    FIELD_EXPIRATION_MONTH = 27
    FIELD_DELIVERABLES = 28
    FIELD_TIME_VALUE = 29
    FIELD_EXPIRATION_DAY = 30
    FIELD_DAYS_TO_EXPIRATION = 31
    FIELD_DELTA = 32
    FIELD_GAMMA = 33
    FIELD_THETA =34
    FIELD_VEGA = 35
    FIELD_RHO = 36
    FIELD_SECURITY_STATUS = 37
    FIELD_THEORETICAL_OPTION_VALUE = 38
    FIELD_UNDERLYING_PRICE = 39
    FIELD_UV_EXPIRATION_TYPE = 40
    FIELD_MARK = 41
    
    
class LevelOneFuturesSubscription(_SubscriptionBySymbolBase):
    __doc__ = _FIELDS_SUBSCRIPTION__doc__.format(name="LevelOneFutures", 
                                                 instr="Futures")
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_BID_PRICE = 1
    FIELD_ASK_PRICE = 2
    FIELD_LAST_PRICE = 3
    FIELD_BID_SIZE = 4
    FIELD_ASK_SIZE = 5
    FIELD_ASK_ID = 6
    FIELD_BID_ID = 7
    FIELD_TOTAL_VOLUME = 8
    FIELD_LAST_SIZE = 9
    FIELD_QUOTE_TIME = 10
    FIELD_TRADE_TIME = 11
    FIELD_HIGH_PRICE = 12
    FIELD_LOW_PRICE = 13
    FIELD_CLOSE_PRICE = 14
    FIELD_EXCHANGE_ID = 15
    FIELD_DESCRIPTION = 16
    FIELD_LAST_ID = 17
    FIELD_OPEN_PRICE = 18 
    FIELD_NET_CHANGE = 19
    FIELD_FUTURE_PERCENT_CHANGE = 20
    FIELD_EXCHANGE_NAME = 21
    FIELD_SECURITY_STATUS = 22
    FIELD_OPEN_INTEREST = 23
    FIELD_MARK = 24
    FIELD_TICK = 25
    FIELD_TICK_AMOUNT = 26
    FIELD_PRODUCT = 27
    FIELD_FUTURE_PRICE_FORMAT = 28
    FIELD_FUTURE_TRADING_HOURS = 29
    FIELD_FUTURE_IS_TRADABLE = 30
    FIELD_FUTURE_MULTIPLIER = 31
    FIELD_FUTURE_IS_ACTIVE = 32
    FIELD_FUTURE_SETTLEMENT_PRICE = 33
    FIELD_FUTURE_ACTIVE_SYMBOL = 34
    FIELD_FUTURE_EXPIRATION_DATE = 35
    
    
class LevelOneForexSubscription(_SubscriptionBySymbolBase):
    __doc__ = _FIELDS_SUBSCRIPTION__doc__.format(name="LevelOneForex", 
                                                 instr="Forex")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0 
    FIELD_BID_PRICE = 1 
    FIELD_ASK_PRICE = 2
    FIELD_LAST_PRICE = 3
    FIELD_BID_SIZE = 4
    FIELD_ASK_SIZE = 5
    FIELD_TOTAL_VOLUME = 6 
    FIELD_LAST_SIZE = 7
    FIELD_QUOTE_TIME = 8
    FIELD_TRADE_TIME = 9
    FIELD_HIGH_PRICE = 10
    FIELD_LOW_PRICE = 11
    FIELD_CLOSE_PRICE = 12
    FIELD_EXCHANGE_ID = 13
    FIELD_DESCRIPTION = 14
    FIELD_OPEN_PRICE = 15
    FIELD_NET_CHANGE = 16
    FIELD_PERCENT_CHANGE =17 
    FIELD_EXCHANGE_NAME = 18
    FIELD_DIGITS = 19
    FIELD_SECURITY_STATUS = 20
    FIELD_TICK = 21
    FIELD_TICK_AMOUNT = 22
    FIELD_PRODUCT = 23
    FIELD_TRADING_HOURS = 24
    FIELD_IS_TRADABLE = 25
    FIELD_MARKET_MAKER = 26
    FIELD_HIGH_52_WEEK = 27
    FIELD_LOW_52_WEEK = 28
    FIELD_MARK = 29
    
class LevelOneFuturesOptionsSubscription(_SubscriptionBySymbolBase):
    __doc__ = _FIELDS_SUBSCRIPTION__doc__.format(name="LevelOneFuturesOptions", 
                                                 instr="Futures-Options")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_BID_PRICE = 1
    FIELD_ASK_PRICE = 2
    FIELD_LAST_PRICE = 3
    FIELD_BID_SIZE = 4
    FIELD_ASK_SIZE = 5
    FIELD_ASK_ID = 6
    FIELD_BID_ID = 7
    FIELD_TOTAL_VOLUME = 8
    FIELD_LAST_SIZE = 9
    FIELD_QUOTE_TIME = 10
    FIELD_TRADE_TIME = 11
    FIELD_HIGH_PRICE = 12
    FIELD_LOW_PRICE = 13
    FIELD_CLOSE_PRICE = 14
    FIELD_EXCHANGE_ID = 15
    FIELD_DESCRIPTION = 16
    FIELD_LAST_ID = 17
    FIELD_OPEN_PRICE = 18 
    FIELD_NET_CHANGE = 19
    FIELD_FUTURE_PERCENT_CHANGE = 20
    FIELD_EXCHANGE_NAME = 21
    FIELD_SECURITY_STATUS = 22
    FIELD_OPEN_INTEREST = 23
    FIELD_MARK = 24
    FIELD_TICK = 25
    FIELD_TICK_AMOUNT = 26
    FIELD_PRODUCT = 27
    FIELD_FUTURE_PRICE_FORMAT = 28
    FIELD_FUTURE_TRADING_HOURS = 29
    FIELD_FUTURE_IS_TRADABLE = 30
    FIELD_FUTURE_MULTIPLIER = 31
    FIELD_FUTURE_IS_ACTIVE = 32
    FIELD_FUTURE_SETTLEMENT_PRICE = 33
    FIELD_FUTURE_ACTIVE_SYMBOL = 34
    FIELD_FUTURE_EXPIRATION_DATE = 35
    
    
class NewsHeadlineSubscription(_SubscriptionBySymbolBase):
    """NewsHeadlineSubscription - news headlines as a sequence.

    def __init__(self, symbols, fields):
    
        symbols :: [str, str...] :: symbols to get data for
        fields  :: [int, int...] :: self.FIELD_[] constant values 
                                    indicating what type of data to return
        
        throws -> LibraryNotLoaded, CLibException
    """
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_ERROR_CODE = 1 
    FIELD_STORY_DATETIME = 2
    FIELD_HEADLINE_ID = 3
    FIELD_STATUS = 4
    FIELD_HEADLINE = 5 
    FIELD_STORY_ID = 6
    FIELD_COUNT_FOR_KEYWORD = 7
    FIELD_KEYWORD_ARRAY = 8
    FIELD_IS_HOST = 9
    FIELD_STORY_SOURCE = 10
    
    
_CHART_SUBSCRIPTION__doc__ = """\
Chart{instr}Subscription - 1-min OHLCV {instr} values as a sequence.

    The bar falls on the 0 second and includes data between 0 and 59 seconds. 
    (e.g 9:30 bar is 9:30:00 -> 9:30:59)
    
    def __init__(self, symbols, fields):
    
        symbols :: [str, str...] :: symbols to get data for
        fields  :: [int, int...] :: self.FIELD_[] constant values indicating
                                    what type of data to return
        
        throws -> LibraryNotLoaded, CLibException
"""

# note - doesn't inherit from _ChartSubscriptionBase    
class ChartEquitySubscription(_SubscriptionBySymbolBase):
    __doc__ = _CHART_SUBSCRIPTION__doc__.format(instr="Equity")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    FIELD_SYMBOL = 0
    FIELD_OPEN_PRICE = 1 
    FIELD_HIGH_PRICE = 2
    FIELD_LOW_PRICE = 3
    FIELD_CLOSE_PRICE = 4
    FIELD_VOLUME = 5
    FIELD_SEQUENCE = 6 
    FIELD_CHART_TIME = 7 
    FIELD_CHART_DAY = 8
    
    
class _ChartSubscriptionBase(_SubscriptionBySymbolBase):   
    """_ChartSubscriptionBase - Base Subscription class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)      
    
    def get_fields(self):
        """Returns fields in subscription."""
        return clib.get_vals("ChartSubscriptionBase_GetFields_ABI", c_int, 
                             self._obj, clib.free_fields_buffer)       
    
    FIELD_SYMBOL = 0
    FIELD_CHART_TIME = 1
    FIELD_OPEN_PRICE = 2 
    FIELD_HIGH_PRICE = 3
    FIELD_LOW_PRICE = 4
    FIELD_CLOSE_PRICE = 5
    FIELD_VOLUME = 6
    
    
class ChartFuturesSubscription(_ChartSubscriptionBase):
    __doc__ = _CHART_SUBSCRIPTION__doc__.format(instr="Futures")
     
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
            
    
class ChartOptionsSubscription(_ChartSubscriptionBase):
    __doc__ = _CHART_SUBSCRIPTION__doc__.format(instr="Options")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  


_TIMESALE_SUBSCRIPTION__doc__ = """\
Timesale{instr}Subscription - time & sales {instr} trades as a sequence.

    StreamingSession calls back with new trade(s).
    
    def __init__(self, symbols, fields):
    
        symbols :: [str, str...] :: symbols to get data for
        fields  :: [int, int...] :: self.FIELD_[] constant values indicating
                                    what type of data to return
        
        throws -> LibraryNotLoaded, CLibException    
"""

class _TimesaleSubscriptionBase(_SubscriptionBySymbolBase):
    """_TimesaleSubscriptionBase - Base Subscription class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
        
    def get_fields(self):
        """Returns fields in subscription."""
        return clib.get_vals("TimesaleSubscriptionBase_GetFields_ABI", c_int, 
                             self._obj, clib.free_fields_buffer)      
       
    FIELD_SYMBOL = 0
    FIELD_TRADE_TIME = 1
    FIELD_LAST_PRICE =2
    FIELD_LAST_SIZE = 3
    FIELD_LAST_SEQUENCE = 4
    
    
class TimesaleEquitySubscription(_TimesaleSubscriptionBase):
    __doc__ = _TIMESALE_SUBSCRIPTION__doc__.format(instr="Equity")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)                                 
    
    
class TimesaleFuturesSubscription(_TimesaleSubscriptionBase):
    __doc__ = _TIMESALE_SUBSCRIPTION__doc__.format(instr="Futures")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
    
    
class TimesaleOptionsSubscription(_TimesaleSubscriptionBase):
    __doc__ = _TIMESALE_SUBSCRIPTION__doc__.format(instr="Options")
    
    def __init__(self, symbols, fields):  
        super().__init__(symbols, fields)  
            
            
class _ActivesSubscriptionBase(_StreamingSubscription):
    """_ActivesSubscriptionBase - Base Subscription class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, *args):                        
        cargs = [c_int(a) for a in args]           
        super().__init__(*cargs)                  
        
    def get_duration(self):
        """Returns duration type of subscription."""
        return clib.get_val("ActivesSubscriptionBase_GetDuration_ABI", c_int,
                             self._obj)

    DURATION_TYPE_ALL_DAY = 0
    DURATION_TYPE_MIN_60 = 1
    DURATION_TYPE_MIN_30 = 2
    DURATION_TYPE_MIN_10 = 3
    DURATION_TYPE_MIN_5 = 4
    DURATION_TYPE_MIN_1 = 5


_ACTIVES_DURATION_SUBSCRIPTION__doc__ = """\
{market}ActivesSubscription - most active {market} securities.   

    def __init__(self, duration):
    
        duration :: int :: self.DURATION_TYPE_[] constant indicating the time
                           period over which to find most active
                 
        throws -> LibraryNotLoaded, CLibException    
"""

class NasdaqActivesSubscription(_ActivesSubscriptionBase):
    __doc__ = _ACTIVES_DURATION_SUBSCRIPTION__doc__.format(market="NASDAQ")
    
    def __init__(self, duration):
        super().__init__(duration)
        
        
class NYSEActivesSubscription(_ActivesSubscriptionBase):
    __doc__ = _ACTIVES_DURATION_SUBSCRIPTION__doc__.format(market="NYSE")
    
    def __init__(self, duration):
        super().__init__(duration)
        
        
class OTCBBActivesSubscription(_ActivesSubscriptionBase):
    __doc__ = _ACTIVES_DURATION_SUBSCRIPTION__doc__.format(market="OTCBB")
    
    def __init__(self, duration):
        super().__init__(duration)       


class OptionActivesSubscription(_ActivesSubscriptionBase):
    """OptionActivesSubscription - most active options.   

    def __init__(self, venue, duration):
    
        venue    :: int :: self.VENUE_TYPE_[] constant indicating type and 
                           and sorting of active options
        duration :: int :: self.DURATION_TYPE_[] constant indicating the time
                           period over which to find most active
                 
        throws -> LibraryNotLoaded, CLibException    
    """
    def __init__(self, venue, duration):
        super().__init__(venue, duration) 
        
    def get_venue(self):
        """Returns venue type of subscription."""
        return clib.get_val("OptionActivesSubscription_GetVenue_ABI", c_int,
                             self._obj)   
   
    VENUE_TYPE_OPTS = 0 
    VENUE_TYPE_CALLS = 1 
    VENUE_TYPE_PUTS = 2 
    VENUE_TYPE_OPTS_DESC = 3 # descending
    VENUE_TYPE_CALLS_DESC = 4 # descending
    VENUE_TYPE_PUTS_DESC = 5 # descending
    
    
SERVICE_TO_SUBSCRIPTION = {
    SERVICE_TYPE_NONE : None,    
    SERVICE_TYPE_QUOTE : QuotesSubscription,
    SERVICE_TYPE_OPTION : OptionsSubscription,
    SERVICE_TYPE_LEVELONE_FUTURES : LevelOneFuturesSubscription,
    SERVICE_TYPE_LEVELONE_FOREX : LevelOneForexSubscription,
    SERVICE_TYPE_LEVELONE_FUTURES_OPTIONS : LevelOneFuturesOptionsSubscription,
    SERVICE_TYPE_NEWS_HEADLINE : NewsHeadlineSubscription,
    SERVICE_TYPE_CHART_EQUITY : ChartEquitySubscription,
    SERVICE_TYPE_CHART_FUTURES : ChartFuturesSubscription,
    SERVICE_TYPE_CHART_OPTIONS : ChartOptionsSubscription,
    SERVICE_TYPE_TIMESALE_EQUITY : TimesaleEquitySubscription,
    SERVICE_TYPE_TIMESALE_FUTURES : TimesaleFuturesSubscription,
    SERVICE_TYPE_TIMESALE_OPTIONS : TimesaleOptionsSubscription,
    SERVICE_TYPE_ACTIVES_NASDAQ : NasdaqActivesSubscription,
    SERVICE_TYPE_ACTIVES_NYSE : NYSEActivesSubscription,
    SERVICE_TYPE_ACTIVES_OTCBB : OTCBBActivesSubscription,
    SERVICE_TYPE_ACTIVES_OPTIONS : OptionActivesSubscription,
    SERVICE_TYPE_ADMIN : None
    }  
    
