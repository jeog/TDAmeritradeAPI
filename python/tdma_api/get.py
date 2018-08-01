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

"""tdma_api/get.py the http/get interface

The get module provides 'getter' objects and utilities for getting quote, 
historical, market, and account information from the TDAmeritrade API.

Each getter object mirrors the C++ version of the object from the 
TDAmeritradeAPI shared library, using the exported C/ABI methods through
ctypes.py to replicate the functionality. (More details can be found in
README_GET.md.)

Before using you'll need to have obtained a valid Credentials object. (See
tdma_api.auth.__doc__ or README.md for details.)

Each object sets up an underlying HTTPS/GET connection(via libcurl) using 
the credentials object and the relevant arguments for that particular 
request. The connection sets the Keep-Alive header and will execute a 
request each time .get() is called, until the .close() method is called or 
the object is destroyed. 

.get() will pass the returned json to json.loads(), returning a built-in
object or throwing a CLibException on failure. Each .get() call is 
throttled globally to limit excessive requests. Methods are provided
to get/set this wait time.

Each object provides accessor methods for getting and setting the relevant
arguments passed to the constructor.

As certain getters require a complex set of possibly conflicting arguments, 
understanding and handling CLibExceptions is vital. These can be thrown
from ANY operation(construction, access, get et al.) and contain an error
code, name and message.

A number of constants and utility classes are provided for building specific
argument types required by certain getters (i.e OptionStrikes and 
OptionStrategy static methods for the Option Getters).
"""

from ctypes import byref as _REF, Structure as _Structure, c_int, c_void_p, \
                    c_ulonglong, c_double, Union as _Union, c_uint                 
import json

from . import clib
from .clib import PCHAR

MARKET_TYPE_EQUITY = 0
MARKET_TYPE_OPTION = 1
MARKET_TYPE_FUTURE = 2
MARKET_TYPE_BOND = 3
MARKET_TYPE_FOREX = 4

MOVERS_INDEX_COMPX = 0
MOVERS_INDEX_DJI = 1
MOVERS_INDEX_SPX = 2

MOVERS_DIRECTION_TYPE_UP = 0
MOVERS_DIRECTION_TYPE_DOWN = 1
MOVERS_DIRECTION_TYPE_UP_AND_DOWN = 2

MOVERS_CHANGE_TYPE_VALUE = 0
MOVERS_CHANGE_TYPE_PERCENT = 1

PERIOD_TYPE_DAY = 0
PERIOD_TYPE_MONTH = 1
PERIOD_TYPE_YEAR = 2
PERIOD_TYPE_YTD = 3

FREQUENCY_TYPE_MINUTE = 0
FREQUENCY_TYPE_DAILY = 1
FREQUENCY_TYPE_WEEKLY = 2
FREQUENCY_TYPE_MONTHLY = 3

OPTION_STRATEGY_TYPE_COVERED = 0
OPTION_STRATEGY_TYPE_VERTICAL = 1
OPTION_STRATEGY_TYPE_CALENDAR = 2
OPTION_STRATEGY_TYPE_STRANGLE = 3
OPTION_STRATEGY_TYPE_STRADDLE = 4
OPTION_STRATEGY_TYPE_BUTTERFLY = 5
OPTION_STRATEGY_TYPE_CONDOR = 6
OPTION_STRATEGY_TYPE_DIAGONAL = 7
OPTION_STRATEGY_TYPE_COLLAR = 8
OPTION_STRATEGY_TYPE_ROLL = 9

OPTION_RANGE_TYPE_ITM = 1
OPTION_RANGE_TYPE_NTM = 2
OPTION_RANGE_TYPE_OTM = 3
OPTION_RANGE_TYPE_SAK = 4
OPTION_RANGE_TYPE_SBK = 5
OPTION_RANGE_TYPE_SNK = 6
OPTION_RANGE_TYPE_ALL = 7

OPTION_EXP_MONTH_JAN = 0
OPTION_EXP_MONTH_FEB = 1
OPTION_EXP_MONTH_MAR = 2
OPTION_EXP_MONTH_APR = 3
OPTION_EXP_MONTH_MAY = 4
OPTION_EXP_MONTH_JUN = 5
OPTION_EXP_MONTH_JUL = 6
OPTION_EXP_MONTH_AUG = 7
OPTION_EXP_MONTH_SEP = 8
OPTION_EXP_MONTH_OCT = 9
OPTION_EXP_MONTH_NOV = 10
OPTION_EXP_MONTH_DEC = 11
OPTION_EXP_MONTH_ALL = 12

OPTION_TYPE_S = 0
OPTION_TYPE_NS = 1
OPTION_TYPE_ALL = 2

OPTION_STRIKES_TYPE_N_ATM = 0
OPTION_STRIKES_TYPE_SINGLE = 1
OPTION_STRIKES_TYPE_RANGE = 2

OPTION_CONTRACT_TYPE_CALL = 0;
OPTION_CONTRACT_TYPE_PUT = 1;
OPTION_CONTRACT_TYPE_ALL = 2;

TRANSACTION_TYPE_ALL = 0
TRANSACTION_TYPE_TRADE = 1
TRANSACTION_TYPE_BUY_ONLY = 2
TRANSACTION_TYPE_SELL_ONLY = 3
TRANSACTION_TYPE_CASH_IN_OR_CASH_OUT = 4
TRANSACTION_TYPE_CHECKING = 5
TRANSACTION_TYPE_DIVIDEND = 6
TRANSACTION_TYPE_INTEREST = 7
TRANSACTION_TYPE_OTHER = 8
TRANSACTION_TYPE_ADVISOR_FEES = 9

INSTRUMENT_SEARCH_TYPE_SYMBOL_EXACT = 0
INSTRUMENT_SEARCH_TYPE_SYMBOL_SEARCH = 1
INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX = 2
INSTRUMENT_SEARCH_TYPE_DESCRIPTION_SEARCH = 3
INSTRUMENT_SEARCH_TYPE_DESCRIPTION_REGEX = 4
INSTRUMENT_SEARCH_TYPE_CUSIP = 5


class Getter_C(_Structure): 
    """C struct representing Getter_C type. (IMPLEMENTATION DETAIL)"""
    _fields_ = [
        ("obj", c_void_p), 
        ("type_id", c_int)
        ] 
          
   
def get_def_wait_msec():      
    """get default minimum wait milliseconds between .get() calls"""                    
    return clib.get_val('APIGetter_GetDefWaitMSec_ABI', c_ulonglong) 

def get_wait_msec():
    """get current minimum wait milliseconds between .get() calls"""
    return clib.get_val('APIGetter_GetWaitMSec_ABI', c_ulonglong)

def set_wait_msec(msec):
    """set current minimum wait milliseconds between .get() calls"""
    clib.set_val('APIGetter_SetWaitMSec_ABI', c_ulonglong, msec)
        

class _APIGetter:
    """_APIGetter - Base getter class. DO NOT INSTANTIATE!
    
    ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, creds, *args):        
        self._obj = Getter_C()
        self._creds = creds       
        args = args + (_REF(self._obj),)            
        clib.call(self._abi('Create'), _REF(creds), *args)    
        self._alive = True 
                                                  
    def __del__(self):         
        if hasattr(self,'_alive') and self._alive:
            self._alive = False 
            try:
                try:                                  
                    clib.call(self._abi('Destroy'), _REF(self._obj))
                except clib.CLibException as e:
                    print("CLibException in", self.__del__, ":", str(e))
                except clib.LibraryNotLoaded:
                    pass                      
            except NameError:
                pass
                   
    @property
    def credentials(self):
        return self._creds
    
    # NOTE because of how the ABI calls for derived objects are structured
    # _abi() can only be called: 
    #      1) by non-base classes, or
    #      2) for Create/Destroy abi methods
    @classmethod
    def _abi(cls, f):
        return "{}_{}_ABI".format(cls.__name__,f)

    def get(self):
        """Makes HTTPS/GET request and returns parsed data.
        
        Request is built with getter instance's current parameters;
        response data is parsed via json.loads and returned in
        the form of a built-in type or None.    
        """
        r = clib.get_str('APIGetter_Get_ABI', self._obj)
        return json.loads(r) if r else None    
    
    def close(self):
        """Closes underlying connection."""
        clib.call('APIGetter_Close_ABI', _REF(self._obj))
    
    def is_closed(self):
        """Returns True/False if underlying connection is open/closed."""
        return bool(clib.get_val('APIGetter_IsClosed_ABI', c_int, self._obj))
    
    
class QuoteGetter(_APIGetter):
    """QuoteGetter - Retrieve quotes for a single security.
    
        def __init__(self, creds, symbol):
        
             creds  :: Credentials :: instance class received from auth.py
             symbol :: str         :: (case sensitive) symbol            
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, creds, symbol):
        super().__init__(creds, clib.PCHAR(symbol))    
        
    def get_symbol(self):
        """Returns symbol being used."""
        return clib.get_str(self._abi('GetSymbol'), self._obj) 
    
    def set_symbol(self, symbol):
        """Sets/changes symbol to use."""
        clib.set_str(self._abi('SetSymbol'), symbol, self._obj)
        
        
class QuotesGetter(_APIGetter):
    """QuotesGetter - Retrieve quotes for multiple securities
    
        def __init__(self, creds, *symbols):
        
             creds   :: Credentials :: instance received from auth.py
             symbols :: str, str... :: multiple (case sensitive) symbols
            
         ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, creds, *symbols): 
        super().__init__(creds, clib.PCHAR_BUFFER(symbols))
 
    def get_symbols(self):   
        """Returns list of symbols being used."""   
        return clib.get_strs(self._abi('GetSymbols'), self._obj)          
    
    def set_symbols(self, *symbols):
        """Sets/changes symbols to use."""
        clib.set_strs(self._abi('SetSymbols'), symbols, self._obj)


class MarketHoursGetter(_APIGetter):
    """MarketHoursGetter - Retrieve market operation times for a certain date.
    
        def __init__(self, creds, market_type, date):
        
             creds :: Credentials :: instance received from auth.py
             
             market_type :: int :: MARKET_TYPE_[] constant indicating type of
                                   market to get times for e.g BOND
             date        :: str :: iso8601 date to get times for* 
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException
         
         *date formats: "yyyy-MM-dd", "yyyy-MM-dd'T'HH::mm::ssz"
    """    
    def __init__(self, creds, market_type, date):
        super().__init__(creds, c_int(market_type), PCHAR(date))
    
    def get_date(self):
        """Returns date string being used."""
        return clib.get_str(self._abi('GetDate'), self._obj)
    
    def set_date(self, date):
        """Sets/changes iso8601 date string to use."""
        clib.set_str(self._abi('SetDate'), date, self._obj)
    
    def get_market_type(self):
        """Returns MARKET_TYPE_[] constant being used."""
        return clib.get_val(self._abi('GetMarketType'), c_int, self._obj )
    
    def set_market_type(self, market_type):
        """Sets/changes MARKET_TYPE_[] constant to use."""
        clib.set_val(self._abi('SetMarketType'), c_int, market_type, self._obj)


class MoversGetter(_APIGetter):
    """MoversGetter - Retrieve top 10 up/down movers. 
    
        def __init__(self, creds, index, direction_type, change_type):
        
             creds  :: Credentials :: instance received from auth.py      
                    
             index          :: int :: MOVERS_INDEX_[] constant of market 
                                      index e.g $COMPX
             direction_type :: int :: MOVERS_DIRECTION_TYPE_[] constant
                                      indicating direction of move
             change_type    :: int :: MOVERS_CHANGE_TYPE_[] constant
                                      indicating value vs. percent
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """       
    def __init__(self, creds, index, direction_type, change_type):
        super().__init__(creds, c_int(index), c_int(direction_type), 
                         c_int(change_type))
        
    def get_index(self):
        """Returns MOVERS_INDEX_[] being used."""
        return clib.get_val(self._abi('GetIndex'), c_int, self._obj)
    
    def set_index(self, index):
        """Sets/changes MOVERS_INDEX_[] constant to use."""
        clib.set_val(self._abi('SetIndex'), c_int, index, self._obj)
        
    def get_direction_type(self):
        """Returns MOVERS_DIRECTION_TYPE_[] constant being used."""
        return clib.get_val(self._abi('GetDirectionType'), c_int, self._obj)
    
    def set_direction_type(self, direction_type):
        """Sets/changes MOVERS_DIRECTION_TYPE_[] constant to use."""
        clib.set_val(self._abi('SetDirectionType'), c_int, direction_type, 
                 self._obj)
        
    def get_change_type(self):
        """Returns MOVERS_CHANGE_TYPE_[] constant being used."""
        return clib.get_val(self._abi('GetChangeType'), c_int, self._obj)
    
    def set_change_type(self, change_type):
        """Sets/changes MOVERS_CHANGE_TYPE_[] constant to use."""
        clib.set_val(self._abi('SetChangeType'), c_int, change_type, self._obj)


VALID_PERIODS_BY_PERIOD_TYPE = {
    PERIOD_TYPE_DAY : (1,2,3,4,5,10),
    PERIOD_TYPE_MONTH : (1,2,3,6),
    PERIOD_TYPE_YEAR : (1,2,3,5,10,15,20),
    PERIOD_TYPE_YTD : (1,)
    }

VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE = {
    PERIOD_TYPE_DAY : (FREQUENCY_TYPE_MINUTE,),
    PERIOD_TYPE_MONTH : (FREQUENCY_TYPE_DAILY, FREQUENCY_TYPE_WEEKLY),
    PERIOD_TYPE_YEAR : (FREQUENCY_TYPE_DAILY, FREQUENCY_TYPE_WEEKLY,
                        FREQUENCY_TYPE_MONTHLY),
    PERIOD_TYPE_YTD : (FREQUENCY_TYPE_WEEKLY,)
    }

VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    FREQUENCY_TYPE_MINUTE : (1,5,10,30),
    FREQUENCY_TYPE_DAILY : (1,),
    FREQUENCY_TYPE_WEEKLY : (1,),
    FREQUENCY_TYPE_MONTHLY : (1,)    
    }

class _HistoricalGetterBase(_APIGetter):
    """_HistoricalGetterBase - Base getter class. DO NOT INSTANTIATE!
    
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """
    def __init__(self, creds, *args): 
        super().__init__(creds, *args)
        
    def get_symbol(self):
        """Returns symbol being used."""
        return clib.get_str('HistoricalGetterBase_GetSymbol_ABI', self._obj)
    
    def set_symbol(self, symbol):
        """Sets/changes symbol to use."""
        clib.set_str('HistoricalGetterBase_SetSymbol_ABI', symbol, self._obj)
        
    def get_frequency(self):        
        """Returns frequency value being used."""
        return clib.get_val('HistoricalGetterBase_GetFrequency_ABI', 
                        c_int, self._obj)
        
    def get_frequency_type(self):
        """Returns FREQUENCY_TYPE_[] constant of frequency type being used."""
        return clib.get_val('HistoricalGetterBase_GetFrequencyType_ABI', 
                        c_int, self._obj)
        
    def set_frequency(self, frequency_type, frequency):
        """Sets/changes FREQUENCY_TYPE_[] constant AND frequency size to use."""
        clib.call(self._abi('SetFrequency'), _REF(self._obj), 
                  c_int(frequency_type), c_int(frequency))
    
    def is_extended_hours(self):
        """Returns True if retrieving extended hours data."""
        return bool(clib.get_val('HistoricalGetterBase_IsExtendedHours_ABI', 
                             c_int, self._obj))
    
    def set_extended_hours(self, extended_hours):
        """Sets/changes the retrieval of extended hours data.(True/False)"""
        clib.set_val('HistoricalGetterBase_SetExtendedHours_ABI', c_int, 
                 extended_hours, self._obj)
    
    
class HistoricalPeriodGetter(_HistoricalGetterBase):
    """HistoricalPeriodGetter - Retrieve historical data over a certain period.

    def __init__(self, creds, symbol, period_type, period, frequency_type,
                 frequency, extended_hours):
    
         creds  :: Credentials :: instance received from auth.py      
                
         symbol         :: str  :: (case sensitive) symbol 
         period_type    :: int  :: PERIOD_TYPE_[] constant for type of 
                                   period (DAY, MONTH, YEAR, YTD)
         period         :: int  :: number of periods
         frequency_type :: int  :: FREQUENCY_TYPE_[] constant for type of
                                   frequency (MINUTE, DAILY, WEEKLY, MONTHLY)
         frequency      :: int  :: size of frequency e.g 3 with 
                                   FREQUENCY_TYPE_MINUTE -> 3 minutes
         extended_hours :: bool :: retrieve extended hour data
     
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
     
     IMPORTANT - Only certain combinations of periods, frequencies, and types
     of periods/frequencies can be used. These valid relationships can be found
     in the following dicts:
         VALID_PERIODS_BY_PERIOD_TYPE
         VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE
         VALID_FREQUENCIES_BY_FREQUENCY_TYPE
         
     If invalid combinations are used constructor will THROW CLibException. 
     Invalid combinations passed to set_[] methods will not throw UNTIL the 
     .get() method is called.
    """    
    def __init__(self, creds, symbol, period_type, period, frequency_type,
                 frequency, extended_hours):
        super().__init__(creds, PCHAR(symbol), c_int(period_type), 
                         c_int(period), c_int(frequency_type),
                         c_int(frequency), c_int(extended_hours))
        
    def get_period_type(self):
        """Returns PERIOD_TYPE_[] constant being used."""
        return clib.get_val(self._abi('GetPeriodType'), c_int, self._obj)
    
    def get_period(self):
        """Returns number of periods being used."""
        return clib.get_val(self._abi('GetPeriod'), c_int, self._obj)

    def set_period(self, period_type, period):
        """Sets/changes PERIOD_TYPE_[] constant AND number of periods to use."""
        clib.call(self._abi('SetPeriod'), _REF(self._obj), c_int(period_type),
                  c_int(period))
        

class HistoricalRangeGetter(_HistoricalGetterBase):
    """HistoricalRangeGetter - Retrieve historical data within a date range.

    def __init__(self, creds, symbol, frequency_type, frequency, 
                 start_msec_since_epoch, end_msec_since_epoch,
                extended_hours):
    
         creds  :: Credentials :: instance received from auth.py      
                
         symbol                 :: str  :: (case sensitive) symbol   
         frequency_type         :: int  :: FREQUENCY_TYPE_[] constant for 
                                           type of frequency e.g MINUTE
         frequency              :: int  :: size of frequency e.g 3 with 
                                           FREQUENCY_TYPE_MINUTE -> 3 minutes                                   
         start_msec_since_epoch :: int  :: milliseconds for start of range*
         end_msec_since_epoch   :: int  :: milliseconds for end of range*         
         extended_hours         :: bool :: retrieve extended hour data
     
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
     
     IMPORTANT - Only certain combinations of frequencies and frequency types
     can be used. These valid relationships can be found in the following dict:
         VALID_FREQUENCIES_BY_FREQUENCY_TYPE
         
     If invalid combinations are used constructor will THROW CLibException. 
     Invalid combinations passed to set_[] methods will not throw UNTIL the 
     .get() method is called.
     
     * milliseconds from epoch = milliseconds since midnight Jan-01-1970
    """      
    def __init__(self, creds, symbol, frequency_type, frequency, 
                 start_msec_since_epoch, end_msec_since_epoch, 
                 extended_hours):
        super().__init__(creds, PCHAR(symbol), c_int(frequency_type),
                         c_int(frequency), c_ulonglong(start_msec_since_epoch),
                         c_ulonglong(end_msec_since_epoch), 
                         c_int(extended_hours))
        
    def get_end_msec_since_epoch(self):
        """Returns milliseconds since epoch for end of range being used."""
        return clib.get_val(self._abi('GetEndMSecSinceEpoch'), c_ulonglong, 
                        self._obj)
        
    def set_end_msec_since_epoch(self, msec):
        """Sets/changes milliseconds since epoch for end of range to use."""
        clib.set_val(self._abi('SetEndMSecSinceEpoch'), c_ulonglong, msec,
                 self._obj)
        
    def get_start_msec_since_epoch(self):
        """Returns milliseconds since epoch for start of range being used."""
        return clib.get_val(self._abi('GetStartMSecSinceEpoch'), c_ulonglong, 
                        self._obj)
        
    def set_start_msec_since_epoch(self, msec):
        """Sets/changes milliseconds since epoch for start of range to use."""
        clib.set_val(self._abi('SetStartMSecSinceEpoch'), c_ulonglong, msec,
                 self._obj)


class OptionStrikesValue(_Union):
    """C Union representing StrikesValue argument type.(IMPLEMENTATION DETAIL)"""
    _fields_ = [
        ('n_atm', c_int),
        ('single', c_double),
        ('range', c_int)
        ]
    
    @classmethod
    def build(cls, strikes_type, strikes_value):
        v = OptionStrikesValue()
        if strikes_type == OPTION_STRIKES_TYPE_N_ATM:
            v.n_atm = strikes_value
        elif strikes_type == OPTION_STRIKES_TYPE_SINGLE:
            v.single = strikes_value
        elif strikes_type == OPTION_STRIKES_TYPE_RANGE:
            v.range = strikes_value
        else:
            raise RuntimeError("invalid strikes_type(" + str(strikes_type) +")") 
        return v
    
    def get(self, strikes_type):        
        if strikes_type == OPTION_STRIKES_TYPE_N_ATM:
            return self.n_atm
        elif strikes_type == OPTION_STRIKES_TYPE_SINGLE:
            return self.single
        elif strikes_type == OPTION_STRIKES_TYPE_RANGE:
            return self.range
        else:
            raise RuntimeError("invalid strikes_type(" + str(strikes_type) +")") 
       

class OptionStrikes:
    """OptionStrikes - object passed to Option Getters to indicate strike types.
    
        This is a utility class for building an argument required by the Option 
        Getters. These getters can return data for different combinations of 
        strike types and values. (e.g 'n' strikes vs. a strike at a certain 
        price.)
        
        CREATE INSTANCES WITH THE STATIC METHODS         
    """
    def __init__(self, strikes_type, strikes_value):
        self.strikes_type = strikes_type            
        self.strikes_value = strikes_value
        
    def __eq__(self, o):
        return self.strikes_type == o.strikes_type and \
            self.strikes_value == o.strikes_value
    
    @staticmethod    
    def N_ATM(n):    
        """Build an OptionStrikes object for 'n' strikes around the ATM strike.
        
            @staticmethod
            def N_ATM(n):
                n :: int :: number of strikes around the ATM strike
        """
        if type(n) is not int:
            raise ValueError("strikes type 'n_atm' requires int")
        return OptionStrikes(OPTION_STRIKES_TYPE_N_ATM, n)    

    @staticmethod
    def SINGLE(price):
        """Build an OptionStrikes object for a single strike.
        
            @staticmethod
            def SINGLE(price):
                price :: float :: price level of strike
        """
        if type(price) is not int and type(price) is not float:
            raise ValueError("strikes type 'single' requires int or float")       
        return OptionStrikes(OPTION_STRIKES_TYPE_SINGLE, price)

    @staticmethod
    def RANGE(option_range):
        """Build an OptionStrikes object for a pre-defined range of strikes.
        
            @staticmethod
            def RANGE(option_range):
                option_range :: int :: OPTION_RANGE_[] constant indicating
                                       type of pre-defined range e.g 'ITM'
        """        
        if option_range < OPTION_RANGE_TYPE_ITM \
            or option_range > OPTION_RANGE_TYPE_ALL:
            raise ValueError("strikes type range require valid OPTION_RANGE value") 
        return OptionStrikes(OPTION_STRIKES_TYPE_RANGE, option_range)
    
    
class OptionStrategy:
    """OptionStrategy - object passed to Option Getter to indicate strategy.
    
        This is a utility class for building an argument required by 
        OptionChainStrategyGetter. This getter can return data for different 
        combinations of strategy types and values. (e.g a 'CALENDAR' spread
        doesn't require any additional values, whereas a 'VERTICAL' spread
        needs the value of the spread width/interval)
        
        CREATE INSTANCES WITH THE STATIC METHODS        
    """    
    def __init__(self, strategy_type, spread_interval):
        if strategy_type < OPTION_STRATEGY_TYPE_COVERED \
            or strategy_type > OPTION_STRATEGY_TYPE_ROLL:
            raise ValueError("strategy type requires valid OPTION_STRATEGY value")
        self.strategy_type = strategy_type
        self.spread_interval = spread_interval
        
    def __eq__(self, o):
        return self.strategy_type == o.strategy_type and \
            self.spread_interval == o.spread_interval
            
    @staticmethod
    def COVERED(): 
        """Build a 'Covered' strategy object."""
        return OptionStrategy(OPTION_STRATEGY_TYPE_COVERED, 0.0)
        
    @staticmethod
    def CALENDAR(): 
        """Build a 'Calendar' spread strategy object."""
        return OptionStrategy(OPTION_STRATEGY_TYPE_CALENDAR, 0.0)
        
    @staticmethod
    def VERTICAL(spread_interval=1.0): 
        """Build a 'Vertical' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_VERTICAL, spread_interval)

    @staticmethod
    def STRANGLE(spread_interval=1.0): 
        """Build a 'Strangle' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_STRANGLE, spread_interval)  

    @staticmethod
    def STRADDLE(spread_interval=1.0): 
        """Build a 'Straddle' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_STRADDLE, spread_interval)
    
    @staticmethod
    def BUTTERFLY(spread_interval=1.0): 
        """Build a 'Butterfly' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_BUTTERFLY, spread_interval)
    
    @staticmethod
    def CONDOR(spread_interval=1.0): 
        """Build a 'Condor' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_CONDOR, spread_interval)
    
    @staticmethod
    def DIAGONAL(spread_interval=1.0): 
        """Build a 'Diagonal' spread (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_DIAGONAL, spread_interval)
    
    @staticmethod
    def COLLAR(spread_interval=1.0): 
        """Build a 'Collar' spread (with desired interval) strategy object."""
        return OptionStrategy(OPTION_STRATEGY_TYPE_COLLAR, spread_interval)   
    
    @staticmethod
    def ROLL(spread_interval=1.0): 
        """Build a 'Roll' (with desired interval) strategy object."""        
        return OptionStrategy(OPTION_STRATEGY_TYPE_ROLL, spread_interval)
                                              

class _OptionChainGetterBase(_APIGetter):
    """_OptionChainGetterBase - Base getter class. DO NOT INSTANTIATE!
    
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """    
    def __init__(self, creds, symbol, *args):    
        if len(args) not in (8,10,12):
            raise ValueError("invalid number of arguments")   
        super().__init__(creds, PCHAR(symbol), *args)

    def get_symbol(self):
        """Returns symbol being used."""
        return clib.get_str('OptionChainGetter_GetSymbol_ABI', self._obj)
    
    def set_symbol(self, symbol):
        """Sets/changes symbol to use."""
        clib.set_str('OptionChainGetter_SetSymbol_ABI', symbol, self._obj)
        
    def get_strikes(self):
        """Returns OptionStrikes instance being used."""
        t = c_int()
        v = OptionStrikesValue()
        clib.call('OptionChainGetter_GetStrikes_ABI', _REF(self._obj), _REF(t), 
                  _REF(v))               
        return OptionStrikes(t.value, v.get(t.value))
    
    def set_strikes(self, strikes):
        """Sets/changes OptionStrikes instance to use."""
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        v = OptionStrikesValue.build(strikes.strikes_type, 
                                     strikes.strikes_value)
        clib.call('OptionChainGetter_SetStrikes_ABI', _REF(self._obj), 
                  c_int(strikes.strikes_type), v)
        
    def get_contract_type(self):
        """Returns OPTION_CONTRACT_TYPE_[] constant being used."""
        return clib.get_val('OptionChainGetter_GetContractType_ABI', c_int, 
                        self._obj)
    
    def set_contract_type(self, contract_type):
        """Sets/changes OPTION_CONTRACT_TYPE_[] constant to use."""
        clib.set_val('OptionChainGetter_SetContractType_ABI', c_int, 
                     contract_type, self._obj)
        
    def includes_quotes(self):
        """Returns True if option chains include quotes of underlying."""
        return bool(clib.get_val('OptionChainGetter_IncludesQuotes_ABI', 
                                 c_int, self._obj))
    
    def include_quotes(self, include_quotes):
        """Sets/changes if option chains should include quotes of underlying."""
        clib.set_val('OptionChainGetter_IncludeQuotes_ABI', c_int, 
                     include_quotes, self._obj)
        
    def get_from_date(self):
        """Returns iso8601 date string of start of range being used."""
        return clib.get_str('OptionChainGetter_GetFromDate_ABI', self._obj)
    
    def set_from_date(self, from_date):
        """Sets/changes iso8601 date string of start of range to use."""
        clib.set_str('OptionChainGetter_SetFromDate_ABI', from_date, self._obj)
 
    def get_to_date(self):
        """Returns iso8601 date string of end of range being used."""        
        return clib.get_str('OptionChainGetter_GetToDate_ABI', self._obj)
    
    def set_to_date(self, to_date):
        """Sets/changes iso8601 date string of end of range to use."""        
        clib.set_str('OptionChainGetter_SetToDate_ABI', to_date, self._obj)       
        
    def get_exp_month(self):
        """Returns OPTION_EXP_MONTH_[] constant being used."""
        return clib.get_val('OptionChainGetter_GetExpMonth_ABI', c_int, 
                            self._obj)
    
    def set_exp_month(self, exp_month):
        """Sets/changes OPTION_EXP_MONTH_[] constant to use."""
        clib.set_val('OptionChainGetter_SetExpMonth_ABI', c_int, exp_month, 
                 self._obj)
        
    def get_option_type(self):
        """Returns OPTION_TYPE_[] constant being used."""
        return clib.get_val('OptionChainGetter_GetOptionType_ABI', c_int, 
                        self._obj)
    
    def set_option_type(self, option_type):
        """Sets/changes OPTION_TYPE_[] constant being used."""
        clib.set_val('OptionChainGetter_SetOptionType_ABI', c_int, option_type, 
                 self._obj)    
          
          
class OptionChainGetter(_OptionChainGetterBase):
    """OptionChainGetter - Retrieve standard option chain.

    def __init__(self, creds, symbol, strikes, contract_type, include_quotes,
                 from_date, to_date, exp_month, option_type):
    
         creds          :: Credentials   :: instance received from auth.py                      
         symbol         :: str           :: (case sensitive) symbol of 
                                            underlying security
         strikes        :: OptionStrikes :: (see OptionStrikes.__doc__)
         contract_type  :: int           :: OPTION_CONTRACT_TYPE_[] constant
                                            indicating CALL, PUT, ALL
         include_quotes :: bool          :: include underlying quote info
         from_date      :: str           :: iso8601 date for start of range*
         to_date        :: str           :: iso8601 date for end of range*
         exp_month      :: int           :: OPTION_EXP_MONTH_[] constant
                                            for desired expiration months
         option_type    :: int           :: OPTION_TYPE_[] constant indicating
                                            S(standard), NS(non-standard), ALL       
     
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    
    *date formats: "yyyy-MM-dd", "yyyy-MM-dd'T'HH::mm::ssz"
    """     
    def __init__(self, creds, symbol, strikes, contract_type, include_quotes, 
                 from_date, to_date, exp_month, option_type):
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        v = OptionStrikesValue.build(strikes.strikes_type, strikes.strikes_value)
        super().__init__(creds, symbol, c_int(strikes.strikes_type), v, 
                         c_int(contract_type), c_int(include_quotes), 
                         PCHAR(from_date), PCHAR(to_date), c_int(exp_month), 
                         c_int(option_type))
              
                
class OptionChainStrategyGetter(_OptionChainGetterBase):
    """OptionChainStrategyGetter - Retrieve strategy/spread option chains.

    def __init__(self, creds, symbol, strategy, strikes, contract_type, 
                 include_quotes, from_date, to_date, exp_month, option_type):
    
         creds          :: Credentials    :: instance received from auth.py                      
         symbol         :: str            :: (case sensitive) symbol of 
                                             underlying security
         strategy       :: OptionStrategy :: (see OptionStrategy.__doc__)
         strikes        :: OptionStrikes  :: (see OptionStrikes.__doc__)
         contract_type  :: int            :: OPTION_CONTRACT_TYPE_[] constant
                                            indicating CALL, PUT, ALL
         include_quotes :: bool           :: include underlying quote info
         from_date      :: str            :: iso8601 date for start of range*
         to_date        :: str            :: iso8601 date for end of range*
         exp_month      :: int            :: OPTION_EXP_MONTH_[] constant
                                             for desired expiration months
         option_type    :: int            :: OPTION_TYPE_[] constant indicating
                                             S(standard), NS(non-standard), ALL       
     
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    
    *date formats: "yyyy-MM-dd", "yyyy-MM-dd'T'HH::mm::ssz"
    """       
    def __init__(self, creds, symbol, strategy, strikes, contract_type, 
                  include_quotes, from_date, to_date, exp_month, option_type):
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        if not isinstance(strategy, OptionStrategy):
            raise ValueError("strategy not instance of OptionStrategy")
        v = OptionStrikesValue.build(strikes.strikes_type, strikes.strikes_value)
        super().__init__(creds, symbol, c_int(strategy.strategy_type),
                         c_double(strategy.spread_interval), 
                         c_int(strikes.strikes_type), v, c_int(contract_type), 
                         c_int(include_quotes), PCHAR(from_date), 
                         PCHAR(to_date), c_int(exp_month), c_int(option_type))
        
    def get_strategy(self):
        """Returns OptionStrategy instance being used."""
        t = c_int()
        s = c_double()      
        clib.call(self._abi('GetStrategy'), _REF(self._obj), _REF(t), _REF(s))               
        return OptionStrategy(t.value, s.value)
    
    def set_strategy(self, strategy):
        """Sets/changes OptionStrategy instance to use."""
        if not isinstance(strategy, OptionStrategy):
            raise ValueError("strategy not instance of OptionStrategy")       
        clib.call(self._abi('SetStrategy'), _REF(self._obj), 
                  c_int(strategy.strategy_type), 
                  c_double(strategy.spread_interval))


class OptionChainAnalyticalGetter(_OptionChainGetterBase):
    """OptionChainAnalyticalGetter - Retrieve option chains w/ analytics.
    
    This getter takes speculative option parameters(volatility, days to 
    expiration etc.) and returns additional calculated values.

    def __init__(self, creds, symbol, volatility, underlying_price,
                 interest_rate, days_to_exp, strikes, contract_type, 
                 include_quotes, from_date, to_date, exp_month, option_type):
    
         creds          :: Credentials   :: instance received from auth.py                      
         symbol         :: str           :: (case sensitive) symbol of 
                                            underlying security
         volatility     :: float         :: implied volatility 
         underlying_price :: float       :: price of underlying security
         interest_rate  :: float         :: market interest rate
         days_to_exp    :: int           :: days until expiration         
         strikes        :: OptionStrikes :: (see OptionStrikes.__doc__)
         contract_type  :: int           :: OPTION_CONTRACT_TYPE_[] constant
                                            indicating CALL, PUT, ALL
         include_quotes :: bool          :: include underlying quote info
         from_date      :: str           :: iso8601 date for start of range*
         to_date        :: str           :: iso8601 date for end of range*
         exp_month      :: int           :: OPTION_EXP_MONTH_[] constant
                                            for desired expiration months
         option_type    :: int           :: OPTION_TYPE_[] constant indicating
                                            S(standard), NS(non-standard), ALL       
     
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    
    *date formats: "yyyy-MM-dd", "yyyy-MM-dd'T'HH::mm::ssz"
    """       
    def __init__(self, creds, symbol, volatility, underlying_price, 
                 interest_rate, days_to_exp, strikes, contract_type, 
                  include_quotes, from_date, to_date, exp_month, option_type):
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        v = OptionStrikesValue.build(strikes.strikes_type, 
                                     strikes.strikes_value)
        super().__init__(creds, symbol, c_double(volatility),
                         c_double(underlying_price), c_double(interest_rate), 
                         c_uint(days_to_exp), c_int(strikes.strikes_type), v, 
                         c_int(contract_type), c_int(include_quotes), 
                         PCHAR(from_date), PCHAR(to_date), c_int(exp_month), 
                         c_int(option_type))
        
    def get_volatility(self):
        """Returns implied volatility(for calculations) being used."""
        return clib.get_val(self._abi('GetVolatility'), c_double, self._obj)

    def set_volatility(self, volatility):
        """Sets/changes implied volatility(for calculations) to use."""
        clib.set_val(self._abi('SetVolatility'), c_double, volatility, 
                     self._obj)
    
    def get_underlying_price(self):
        """Returns underlying price(for calculations) being used."""
        return clib.get_val(self._abi('GetUnderlyingPrice'), c_double, 
                            self._obj)

    def set_underlying_price(self, price):
        """Sets/changes underlying price(for calculations) to use."""
        clib.set_val(self._abi('SetUnderlyingPrice'), c_double, price, 
                     self._obj)
        
    def get_interest_rate(self):
        """Returns market interest rate(for calculations) being used."""
        return clib.get_val(self._abi('GetInterestRate'), c_double, 
                            self._obj)
    
    def set_interest_rate(self, interest_rate):
        """Sets/changes market interest rate(for calculations) to use."""
        clib.set_val(self._abi('SetInterestRate'), c_double, interest_rate, 
                 self._obj)
    
    def get_days_to_exp(self):
        """Returns days until expiration(for calculations) being used."""
        return clib.get_val(self._abi('GetDaysToExp'), c_uint, self._obj)
    
    def set_days_to_exp(self, days):
        """Sets/changes days until expiration(for calculations) to use."""
        clib.set_val(self._abi('SetDaysToExp'), c_uint, days, self._obj)
    
    
class _AccountGetterBase(_APIGetter):
    """_AccountGetterBase - Base getter class. DO NOT INSTANTIATE!
    
     ALL METHODS THROW -> LibraryNotLoaded, CLibException
    """       
    def __init__(self, creds, account_id, *args):
        super().__init__(creds, PCHAR(account_id), *args)
        
    def get_account_id(self):
        """Returns user account ID being used."""
        return clib.get_str('AccountGetterBase_GetAccountId_ABI', self._obj)
    
    def set_account_id(self, account_id):
        """Sets/changes user account ID to use."""
        clib.set_str('AccountGetterBase_SetAccountId_ABI', account_id, 
                     self._obj)


class AccountInfoGetter(_AccountGetterBase):
    """AccountInfoGetter - Retrieve user account information.
    
        def __init__(self, creds, account_id, return_positions, return_orders):
        
             creds :: Credentials :: instance received from auth.py
             
             account_id       :: str  :: user account ID
             return_positions :: bool :: return position information
             return_orders    :: bool :: return order information
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException         

    """       
    def __init__(self, creds, account_id, return_positions, return_orders):
        super().__init__(creds, account_id, c_int(int(return_positions)), 
                         c_int(int(return_orders)))
        
    def returns_positions(self):
        """Returns if position information is retrieved."""
        return bool(clib.get_val(self._abi('ReturnsPositions'), c_int, self._obj))

    def return_positions(self, return_positions):
        """Sets/changes if position information should be retrieved."""
        clib.set_val(self._abi('ReturnPositions'), c_int, int(return_positions), 
                 self._obj)
 
    def returns_orders(self):
        """Returns if order information is retrieved."""
        return bool(clib.get_val(self._abi('ReturnsOrders'), c_int, self._obj))

    def return_orders(self, return_orders):
        """Sets/changes if order information should be retreived."""
        clib.set_val(self._abi('ReturnOrders'), c_int, int(return_orders), 
                 self._obj)       
        

class PreferencesGetter(_AccountGetterBase):
    """PreferencesGetter - Retrieve user preferences.
    
        def __init__(self, creds, account_id):
        
             creds      :: Credentials :: instance received from auth.py             
             account_id :: str         :: user account ID       
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException         

    """      
    def __init__(self, creds, account_id):
        super().__init__(creds, account_id) 
                         

class StreamerSubscriptionKeysGetter(_AccountGetterBase):
    """StreamerSubscriptionKeysGetter - Retrieve streamer keys.
    
        def __init__(self, creds, account_id):
        
             creds      :: Credentials :: instance received from auth.py             
             account_id :: str         :: user account ID       
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException         

    """      
    def __init__(self, creds, account_id):
        super().__init__(creds, account_id) 


class TransactionHistoryGetter(_AccountGetterBase):
    """TransactionHistoryGetter - Retrieve info from past transactions.
    
        def __init__(self, creds, account_id, transaction_type, symbol,
                     start_date, end_date):
        
             creds :: Credentials :: instance received from auth.py  
                        
             account_id       :: str :: user account ID       
             transaction_type :: int :: TRANSACTION_TYPE_[] constant indicating
                                        type of transaction e.g TRADE
             symbol           :: str :: symbol to search for
             start_date       :: str :: iso8601 date of start of range*
             end_date         :: str :: iso8601 date of end of range*
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException   
         
         *date format: "yyyy-MM-dd"     
    """      
    def __init__(self, creds, account_id, transaction_type, symbol, 
                 start_date, end_date):
        super().__init__(creds, account_id, c_int(transaction_type), 
                         PCHAR(symbol), PCHAR(start_date), PCHAR(end_date))
        
    def get_transaction_type(self):
        """Returns TRANSACTION_TYPE_[] constant being used."""
        return clib.get_val(self._abi('GetTransactionType'), c_int, self._obj)
    
    def set_transaction_type(self, transaction_type):
        """Sets/changes TRANSACTION_TYPE_[] constant to use."""
        clib.set_val(self._abi('SetTransactionType'), c_int, transaction_type,
                     self._obj)
        
    def get_symbol(self):
        """Returns search symbol being used."""
        return clib.get_str(self._abi('GetSymbol'), self._obj)
    
    def set_symbol(self, symbol):
        """Sets/changes search symbol to use."""
        clib.set_str(self._abi('SetSymbol'), symbol, self._obj)   
        
    def get_start_date(self):
        """Returns iso8601 date string of start of range being used."""
        return clib.get_str(self._abi('GetStartDate'), self._obj)
    
    def set_start_date(self, start_date):
        """Sets/changes iso8601 date string of start of range to use."""
        clib.set_str(self._abi('SetStartDate'), start_date, self._obj)               
    
    def get_end_date(self):
        """Returns iso8601 date string of end of range being used."""        
        return clib.get_str(self._abi('GetEndDate'), self._obj)
    
    def set_end_date(self, end_date):
        """Sets/changes iso8601 date string of end of range to use."""        
        clib.set_str(self._abi('SetEndDate'), end_date, self._obj)      
        
        
class IndividualTransactionHistoryGetter(_AccountGetterBase):
    """IndividualTransactionHistoryGetter - Retrieve past transaction info.
    
        def __init__(self, creds, account_id, transaction_id):
        
             creds :: Credentials :: instance received from auth.py  
                        
             account_id     :: str :: user account ID       
             transaction_id :: str :: transaction ID
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException           
    """     
    def __init__(self, creds, account_id, transaction_id):
        super().__init__(creds, account_id, PCHAR(transaction_id))
       
    def get_transaction_id(self):
        """Returns transaction ID being used."""
        return clib.get_str(self._abi('GetTransactionId'), self._obj)
    
    def set_transaction_id(self, transaction_id):
        """Sets/changes transaction ID to use."""
        clib.set_str(self._abi('SetTransactionId'), transaction_id, self._obj)    


class UserPrincipalsGetter(_APIGetter):
    """UserPrincipalsGetter - Retrieve user principal details.
    
        def __init__(self, creds, subscription_keys, connection_info, 
                     preferences, surrogate_ids):
        
             creds :: Credentials :: instance received from auth.py
             
             subscription_keys :: bool :: return streamer subscription keys
             connection_info   :: bool :: return streamer connection info
             preferences       :: bool :: return user preferences
             surrogate_ids     :: bool :: return user surrogate IDs
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException         

    """     
    def __init__(self, creds, subscription_keys, connection_info, 
                 preferences, surrogate_ids):
        super().__init__(creds, c_int(int(subscription_keys)),
                         c_int(int(connection_info)), 
                         c_int(int(preferences)),
                         c_int(int(surrogate_ids)) )

    def returns_subscription_keys(self):
        """Returns if streamer subscription keys are being retrieved."""
        return bool(clib.get_val(self._abi('ReturnsSubscriptionKeys'), 
                                 c_int, self._obj))

    def return_subscription_keys(self, return_subscription_keys):
        """Sets/changes if streamer subscription keys should be retrieved."""
        clib.set_val(self._abi('ReturnSubscriptionKeys'), c_int, 
                     int(return_subscription_keys), self._obj)
  
    def returns_connection_info(self):
        """Returns if streamer connection info is being retrieved."""
        return bool(clib.get_val(self._abi('ReturnsConnectionInfo'), 
                                 c_int, self._obj))

    def return_connection_info(self, return_connection_info):
        """Sets/changes if streamer connection info should be retrieved."""
        clib.set_val(self._abi('ReturnConnectionInfo'), c_int, 
                     int(return_connection_info), self._obj)
        
    def returns_preferences(self):
        """Returns if user preferences are being retrieved."""
        return bool(clib.get_val(self._abi('ReturnsPreferences'), 
                                 c_int, self._obj))

    def return_preferences(self, return_preferences):
        """Sets/changes if user preferences should be retrieved."""
        clib.set_val(self._abi('ReturnPreferences'), c_int, 
                     int(return_preferences), self._obj)
              
    def returns_surrogate_ids(self):
        """Returns if user surrogate IDs are being retrieved."""
        return bool(clib.get_val(self._abi('ReturnsSurrogateIds'), 
                                 c_int, self._obj))

    def return_surrogate_ids(self, return_surrogate_ids):
        """Sets/changes if user surrogate IDs should be retrieved."""
        clib.set_val(self._abi('ReturnSurrogateIds'), c_int, 
                     int(return_surrogate_ids), self._obj)
                      
                      
class InstrumentInfoGetter(_APIGetter):
    """InstrumentInfoGetter - Search for and retrieve instrument info.
    
    This getter allows search for valid instruments based on
    name, description, or CUSIP - with regular expression support. If
    found it returns relevant information about that instrument.
    
        def __init__(self, creds, search_type, query_string):
        
             creds :: Credentials :: instance received from auth.py
             
             search_type  :: int :: INSTRUMENT_SEARCH_TYPE_[] constant 
                                    for type of search e.g SYMBOL_REGEX
             query_string :: str :: string to use in the search/lookup
         
         ALL METHODS THROW -> LibraryNotLoaded, CLibException         

    """      
    def __init__(self, creds, search_type, query_string):
        super().__init__(creds, c_int(search_type), PCHAR(query_string))
       
    def get_search_type(self):
        """Returns INSTRUMENT_SEARCH_TYPE_[] being used."""
        return clib.get_val(self._abi('GetSearchType'), c_int, self._obj)
           
    def get_query_string(self):
        """Returns query string being used."""
        return clib.get_str(self._abi('GetQueryString'), self._obj)

    def set_query(self, search_type, query_string):
        """Sets/changes INSTRUMENT_SEARCH_TYPE_[] AND query string to use."""
        clib.call(self._abi('SetQuery'), _REF(self._obj), 
                  c_int(search_type), PCHAR(query_string))
        



