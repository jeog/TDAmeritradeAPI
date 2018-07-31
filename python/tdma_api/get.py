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
    _fields_ = [
        ("obj", c_void_p), 
        ("type_id", c_int)
        ] 
 
         
   
def get_def_wait_msec():                          
    return clib.get_val('APIGetter_GetDefWaitMSec_ABI', c_ulonglong)
    

def get_wait_msec():
    return clib.get_val('APIGetter_GetWaitMSec_ABI', c_ulonglong)


def set_wait_msec(msec):
    clib.set_val('APIGetter_SetWaitMSec_ABI', c_ulonglong, msec)
        

class _APIGetter:
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
        r = clib.get_str('APIGetter_Get_ABI', self._obj)
        return json.loads(r) if r else None    
    
    def close(self):
        clib.call('APIGetter_Close_ABI', _REF(self._obj))
    
    def is_closed(self):
        return bool(clib.get_val('APIGetter_IsClosed_ABI', c_int, self._obj))
    
    
class QuoteGetter(_APIGetter):
    def __init__(self, creds, symbol):
        super().__init__(creds, clib.PCHAR(symbol))    
        
    def get_symbol(self):
        return clib.get_str(self._abi('GetSymbol'), self._obj) 
    
    def set_symbol(self, symbol):
        clib.set_str(self._abi('SetSymbol'), symbol, self._obj)
        
        
class QuotesGetter(_APIGetter):
    def __init__(self, creds, *symbols): 
        super().__init__(creds, clib.PCHAR_BUFFER(symbols))
 
    def get_symbols(self):      
        return clib.get_strs(self._abi('GetSymbols'), self._obj)          
    
    def set_symbols(self, *symbols):
        clib.set_strs(self._abi('SetSymbols'), symbols, self._obj)


class MarketHoursGetter(_APIGetter):
    def __init__(self, creds, market_type, date):
        super().__init__(creds, c_int(market_type), PCHAR(date))
    
    def get_date(self):
        return clib.get_str(self._abi('GetDate'), self._obj)
    
    def set_date(self, date):
        clib.set_str(self._abi('SetDate'), date, self._obj)
    
    def get_market_type(self):
        return clib.get_val(self._abi('GetMarketType'), c_int, self._obj )
    
    def set_market_type(self, market_type):
        clib.set_val(self._abi('SetMarketType'), c_int, market_type, self._obj)


class MoversGetter(_APIGetter):
    def __init__(self, creds, index, direction_type, change_type):
        super().__init__(creds, c_int(index), c_int(direction_type), 
                         c_int(change_type))
        
    def get_index(self):
        return clib.get_val(self._abi('GetIndex'), c_int, self._obj)
    
    def set_index(self, index):
        clib.set_val(self._abi('SetIndex'), c_int, index, self._obj)
        
    def get_direction_type(self):
        return clib.get_val(self._abi('GetDirectionType'), c_int, self._obj)
    
    def set_direction_type(self, direction_type):
        clib.set_val(self._abi('SetDirectionType'), c_int, direction_type, 
                 self._obj)
        
    def get_change_type(self):
        return clib.get_val(self._abi('GetChangeType'), c_int, self._obj)
    
    def set_change_type(self, change_type):
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
    def __init__(self, creds, *args): 
        super().__init__(creds, *args)
        
    def get_symbol(self):
        return clib.get_str('HistoricalGetterBase_GetSymbol_ABI', 
                            self._obj)
    
    def set_symbol(self, symbol):
        clib.set_str('HistoricalGetterBase_SetSymbol_ABI', symbol, 
                     self._obj)
        
    def get_frequency(self):
        return clib.get_val('HistoricalGetterBase_GetFrequency_ABI', 
                        c_int, self._obj)
        
    def get_frequency_type(self):
        return clib.get_val('HistoricalGetterBase_GetFrequencyType_ABI', 
                        c_int, self._obj)
    
    def is_extended_hours(self):
        return bool(clib.get_val('HistoricalGetterBase_IsExtendedHours_ABI', 
                             c_int, self._obj))
    
    def set_extended_hours(self, extended_hours):
        clib.set_val('HistoricalGetterBase_SetExtendedHours_ABI', c_int, 
                 extended_hours, self._obj)
    
    
class HistoricalPeriodGetter(_HistoricalGetterBase):
    def __init__(self, creds, symbol, period_type, period, frequency_type,
                 frequency, extended_hours):
        super().__init__(creds, PCHAR(symbol), c_int(period_type), 
                         c_int(period), c_int(frequency_type),
                         c_int(frequency), c_int(extended_hours))
        
    def get_period_type(self):
        return clib.get_val(self._abi('GetPeriodType'), c_int, self._obj)
    
    def get_period(self):
        return clib.get_val(self._abi('GetPeriod'), c_int, self._obj)

    def set_period(self, period_type, period):
        clib.call(self._abi('SetPeriod'), _REF(self._obj), c_int(period_type),
                  c_int(period))
        
    def set_frequency(self, frequency_type, frequency):
        clib.call(self._abi('SetFrequency'), _REF(self._obj), 
                  c_int(frequency_type), c_int(frequency))


class HistoricalRangeGetter(_HistoricalGetterBase):
    def __init__(self, creds, symbol, frequency_type, frequency, 
                 start_msec_since_epoch, end_msec_since_epoch, 
                 extended_hours):
        super().__init__(creds, PCHAR(symbol), c_int(frequency_type),
                         c_int(frequency), c_ulonglong(start_msec_since_epoch),
                         c_ulonglong(end_msec_since_epoch), 
                         c_int(extended_hours))
        
    def get_end_msec_since_epoch(self):
        return clib.get_val(self._abi('GetEndMSecSinceEpoch'), c_ulonglong, 
                        self._obj)
        
    def set_end_msec_since_epoch(self, msec):
        clib.set_val(self._abi('SetEndMSecSinceEpoch'), c_ulonglong, msec,
                 self._obj)
        
    def get_start_msec_since_epoch(self):
        return clib.get_val(self._abi('GetStartMSecSinceEpoch'), c_ulonglong, 
                        self._obj)
        
    def set_start_msec_since_epoch(self, msec):
        clib.set_val(self._abi('SetStartMSecSinceEpoch'), c_ulonglong, msec,
                 self._obj)
        
    def set_frequency(self, frequency_type, frequency):
        clib.call(self._abi('SetFrequency'), _REF(self._obj),
                  c_int(frequency_type), c_int(frequency))


class OptionStrikesValue(_Union):
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
    def __init__(self, strikes_type, strikes_value):
        self.strikes_type = strikes_type            
        self.strikes_value = strikes_value
        
    def __eq__(self, o):
        return self.strikes_type == o.strikes_type and \
            self.strikes_value == o.strikes_value
    
    @staticmethod    
    def N_ATM(n):      
        if type(n) is not int:
            raise ValueError("strikes type 'n_atm' requires int")
        return OptionStrikes(OPTION_STRIKES_TYPE_N_ATM, n)    

    @staticmethod
    def SINGLE(price):
        if type(price) is not int and type(price) is not float:
            raise ValueError("strikes type 'single' requires int or float")       
        return OptionStrikes(OPTION_STRIKES_TYPE_SINGLE, price)

    @staticmethod
    def RANGE(option_range):
        if option_range < OPTION_RANGE_TYPE_ITM \
            or option_range > OPTION_RANGE_TYPE_ALL:
            raise ValueError("strikes type range require valid OPTION_RANGE value") 
        return OptionStrikes(OPTION_STRIKES_TYPE_RANGE, option_range)
    
    
class OptionStrategy:
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
        return OptionStrategy(OPTION_STRATEGY_TYPE_COVERED, 0.0)
        
    @staticmethod
    def CALENDAR(): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_CALENDAR, 0.0)
        
    @staticmethod
    def VERTICAL(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_VERTICAL, spread_interval)

    @staticmethod
    def STRANGLE(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_STRANGLE, spread_interval)  

    @staticmethod
    def STRADDLE(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_STRADDLE, spread_interval)
    
    @staticmethod
    def BUTTERFLY(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_BUTTERFLY, spread_interval)
    
    @staticmethod
    def CONDOR(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_CONDOR, spread_interval)
    
    @staticmethod
    def DIAGONAL(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_DIAGONAL, spread_interval)
    
    @staticmethod
    def COLLAR(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_COLLAR, spread_interval)   
    
    @staticmethod
    def ROLL(spread_interval=1.0): 
        return OptionStrategy(OPTION_STRATEGY_TYPE_ROLL, spread_interval)
                                              

class _OptionChainGetterBase(_APIGetter):
    def __init__(self, creds, symbol, *args):    
        if len(args) not in (8,10,12):
            raise ValueError("invalid number of arguments")   
        super().__init__(creds, PCHAR(symbol), *args)

    def get_symbol(self):
        return clib.get_str('OptionChainGetter_GetSymbol_ABI', self._obj)
    
    def set_symbol(self, symbol):
        clib.set_str('OptionChainGetter_SetSymbol_ABI', symbol, self._obj)
        
    def get_strikes(self):
        t = c_int()
        v = OptionStrikesValue()
        clib.call('OptionChainGetter_GetStrikes_ABI', _REF(self._obj), _REF(t), 
                  _REF(v))               
        return OptionStrikes(t.value, v.get(t.value))
    
    def set_strikes(self, strikes):
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        v = OptionStrikesValue.build(strikes.strikes_type, strikes.strikes_value)
        clib.call('OptionChainGetter_SetStrikes_ABI', _REF(self._obj), 
                  c_int(strikes.strikes_type), v)
        
    def get_contract_type(self):
        return clib.get_val('OptionChainGetter_GetContractType_ABI', c_int, 
                        self._obj)
    
    def set_contract_type(self, contract_type):
        clib.set_val('OptionChainGetter_SetContractType_ABI', c_int, contract_type, 
                 self._obj)
        
    def includes_quotes(self):
        return bool(clib.get_val('OptionChainGetter_IncludesQuotes_ABI', c_int, 
                             self._obj))
    
    def include_quotes(self, include_quotes):
        clib.set_val('OptionChainGetter_IncludeQuotes_ABI', c_int, include_quotes, 
                 self._obj)
        
    def get_from_date(self):
        return clib.get_str('OptionChainGetter_GetFromDate_ABI', self._obj)
    
    def set_from_date(self, from_date):
        clib.set_str('OptionChainGetter_SetFromDate_ABI', from_date, self._obj)
 
    def get_to_date(self):
        return clib.get_str('OptionChainGetter_GetToDate_ABI', self._obj)
    
    def set_to_date(self, from_date):
        clib.set_str('OptionChainGetter_SetToDate_ABI', from_date, self._obj)       
        
    def get_exp_month(self):
        return clib.get_val('OptionChainGetter_GetExpMonth_ABI', c_int, self._obj)
    
    def set_exp_month(self, exp_month):
        clib.set_val('OptionChainGetter_SetExpMonth_ABI', c_int, exp_month, 
                 self._obj)
        
    def get_option_type(self):
        return clib.get_val('OptionChainGetter_GetOptionType_ABI', c_int, 
                        self._obj)
    
    def set_option_type(self, option_type):
        clib.set_val('OptionChainGetter_SetOptionType_ABI', c_int, option_type, 
                 self._obj)    
          
          
class OptionChainGetter(_OptionChainGetterBase):
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
        t = c_int()
        s = c_double()      
        clib.call(self._abi('GetStrategy'), _REF(self._obj), _REF(t), _REF(s))               
        return OptionStrategy(t.value, s.value)
    
    def set_strategy(self, strategy):
        if not isinstance(strategy, OptionStrategy):
            raise ValueError("strategy not instance of OptionStrategy")       
        clib.call(self._abi('SetStrategy'), _REF(self._obj), 
                  c_int(strategy.strategy_type), 
                  c_double(strategy.spread_interval))


class OptionChainAnalyticalGetter(_OptionChainGetterBase):
    def __init__(self, creds, symbol, volatility, underlying_price, 
                 interest_rate, days_to_exp, strikes, contract_type, 
                  include_quotes, from_date, to_date, exp_month, option_type):
        if not isinstance(strikes, OptionStrikes):
            raise ValueError("strikes not instance of OptionStrikes")
        v = OptionStrikesValue.build(strikes.strikes_type, strikes.strikes_value)
        super().__init__(creds, symbol, c_double(volatility),
                         c_double(underlying_price), c_double(interest_rate), 
                         c_uint(days_to_exp), c_int(strikes.strikes_type), v, 
                         c_int(contract_type), c_int(include_quotes), 
                         PCHAR(from_date), PCHAR(to_date), c_int(exp_month), 
                         c_int(option_type))
        
    def get_volatility(self):
        return clib.get_val(self._abi('GetVolatility'), c_double, self._obj)

    def set_volatility(self, volatility):
        clib.set_val(self._abi('SetVolatility'), c_double, volatility, self._obj)
    
    def get_underlying_price(self):
        return clib.get_val(self._abi('GetUnderlyingPrice'), c_double, self._obj)

    def set_underlying_price(self, price):
        clib.set_val(self._abi('SetUnderlyingPrice'), c_double, price, self._obj)
        
    def get_interest_rate(self):
        return clib.get_val(self._abi('GetInterestRate'), c_double, self._obj)
    
    def set_interest_rate(self, interest_rate):
        clib.set_val(self._abi('SetInterestRate'), c_double, interest_rate, 
                 self._obj)
    
    def get_days_to_exp(self):
        return clib.get_val(self._abi('GetDaysToExp'), c_uint, self._obj)
    
    def set_days_to_exp(self, days):
        clib.set_val(self._abi('SetDaysToExp'), c_uint, days, self._obj)
    
    
class _AccountGetterBase(_APIGetter):
    def __init__(self, creds, account_id, *args):
        super().__init__(creds, PCHAR(account_id), *args)
        
    def get_account_id(self):
        return clib.get_str('AccountGetterBase_GetAccountId_ABI', self._obj)
    
    def set_account_id(self, account_id):
        clib.set_str('AccountGetterBase_SetAccountId_ABI', account_id, 
                     self._obj)


class AccountInfoGetter(_AccountGetterBase):
    def __init__(self, creds, account_id, return_positions, return_orders):
        super().__init__(creds, account_id, c_int(int(return_positions)), 
                         c_int(int(return_orders)))
        
    def returns_positions(self):
        return bool(clib.get_val(self._abi('ReturnsPositions'), c_int, self._obj))

    def return_positions(self, return_positions):
        clib.set_val(self._abi('ReturnPositions'), c_int, int(return_positions), 
                 self._obj)
 
    def returns_orders(self):
        return bool(clib.get_val(self._abi('ReturnsOrders'), c_int, self._obj))

    def return_orders(self, return_orders):
        clib.set_val(self._abi('ReturnOrders'), c_int, int(return_orders), 
                 self._obj)       
        

class PreferencesGetter(_AccountGetterBase):
    def __init__(self, creds, account_id):
        super().__init__(creds, account_id) 
                         

class StreamerSubscriptionKeysGetter(_AccountGetterBase):
    def __init__(self, creds, account_id):
        super().__init__(creds, account_id) 


class TransactionHistoryGetter(_AccountGetterBase):
    def __init__(self, creds, account_id, transaction_type, symbol, 
                 start_date, end_date):
        super().__init__(creds, account_id, c_int(transaction_type), 
                         PCHAR(symbol), PCHAR(start_date), PCHAR(end_date))
        
    def get_transaction_type(self):
        return clib.get_val(self._abi('GetTransactionType'), c_int, self._obj)
    
    def set_transaction_type(self, transaction_type):
        clib.set_val(self._abi('SetTransactionType'), c_int, transaction_type,
                     self._obj)
        
    def get_symbol(self):
        return clib.get_str(self._abi('GetSymbol'), self._obj)
    
    def set_symbol(self, symbol):
        clib.set_str(self._abi('SetSymbol'), symbol, self._obj)   
        
    def get_start_date(self):
        return clib.get_str(self._abi('GetStartDate'), self._obj)
    
    def set_start_date(self, start_date):
        clib.set_str(self._abi('SetStartDate'), start_date, self._obj)               
    
    def get_end_date(self):
        return clib.get_str(self._abi('GetEndDate'), self._obj)
    
    def set_end_date(self, end_date):
        clib.set_str(self._abi('SetEndDate'), end_date, self._obj)      
        
        
class IndividualTransactionHistoryGetter(_AccountGetterBase):
    def __init__(self, creds, account_id, transaction_id):
        super().__init__(creds, account_id, PCHAR(transaction_id))
       
    def get_transaction_id(self):
        return clib.get_str(self._abi('GetTransactionId'), self._obj)
    
    def set_transaction_id(self, transaction_id):
        clib.set_str(self._abi('SetTransactionId'), transaction_id, self._obj)    


class UserPrincipalsGetter(_APIGetter):
    def __init__(self, creds, subscription_keys, connection_info, preferences,
                 surrogate_ids):
        super().__init__(creds, c_int(int(subscription_keys)),
                         c_int(int(connection_info)), c_int(int(preferences)),
                         c_int(int(surrogate_ids)) )

    def returns_subscription_keys(self):
        return bool(clib.get_val(self._abi('ReturnsSubscriptionKeys'), 
                                 c_int, self._obj))

    def return_subscription_keys(self, return_subscription_keys):
        clib.set_val(self._abi('ReturnSubscriptionKeys'), c_int, 
                     int(return_subscription_keys), self._obj)
  
    def returns_connection_info(self):
        return bool(clib.get_val(self._abi('ReturnsConnectionInfo'), 
                                 c_int, self._obj))

    def return_connection_info(self, return_connection_info):
        clib.set_val(self._abi('ReturnConnectionInfo'), c_int, 
                     int(return_connection_info), self._obj)
        
    def returns_preferences(self):
        return bool(clib.get_val(self._abi('ReturnsPreferences'), 
                                 c_int, self._obj))

    def return_preferences(self, return_preferences):
        clib.set_val(self._abi('ReturnPreferences'), c_int, 
                     int(return_preferences), self._obj)
              
    def returns_surrogate_ids(self):
        return bool(clib.get_val(self._abi('ReturnsSurrogateIds'), 
                                 c_int, self._obj))

    def return_surrogate_ids(self, return_surrogate_ids):
        clib.set_val(self._abi('ReturnSurrogateIds'), c_int, 
                     int(return_surrogate_ids), self._obj)
                      
                      
class InstrumentInfoGetter(_APIGetter):
    def __init__(self, creds, search_type, query_string):
        super().__init__(creds, c_int(search_type), PCHAR(query_string))
       
    def get_search_type(self):
        return clib.get_val(self._abi('GetSearchType'), c_int, self._obj)
           
    def get_query_string(self):
        return clib.get_str(self._abi('GetQueryString'), self._obj)

    def set_query(self, search_type, query_string):
        clib.call(self._abi('SetQuery'), _REF(self._obj), 
                  c_int(search_type), PCHAR(query_string))
        



