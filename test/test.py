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

from platform import system, architecture
from traceback import print_exc
from time import strftime, perf_counter, sleep
import argparse, gc, os, json

from tdma_api import get, auth, clib, stream, execute

SYSTEM = system()
ARCH = architecture()[0]

CONFIG_DIR = "Release/"
#CONFIG_DIR = "Debug/"
REL_LIB_PATH = os.path.join("../", CONFIG_DIR, "libTDAmeritradeAPI.so")
if SYSTEM == 'Windows':
    if '64' in ARCH:
        REL_LIB_PATH = os.path.join("../vsbuild/x64/", CONFIG_DIR,
                                    "TDAmeritradeAPI.dll")
    else: 
        REL_LIB_PATH = os.path.join("../vsbuild/Win32/", CONFIG_DIR,
                                    "TDAmeritradeAPI.dll")

TEST_DIR = os.path.dirname(os.path.realpath(__file__))
LIBRARY_PATH = os.path.join(TEST_DIR, REL_LIB_PATH)
PACKAGE_BUILD_INFO_PATH = os.path.join(TEST_DIR, '../python/tdma_api_build.info')
        
parser = argparse.ArgumentParser("test tdma_api")
parser.add_argument("account_id", type=str, help="account id" )
parser.add_argument("credentials_path", type=str, 
                    help="path of encrypted credentials file")
parser.add_argument("credentials_password", type=str,
                    help="password to decrypt credentials files")
parser.add_argument("--no-live-connect", action='store_true')

use_live_connection = True

def Get(getter):
    if use_live_connection:
        return getter.get()
    else:
        print("GET requires 'use_live_connection=True'")
        return {}
            
def print_title(s):
    l = len(s) + 4
    print()  
    print('+' * l)
    print('+', s, '+')
    print('+' * l, '\n')
    
               
def test(func, *args):    
    try:        
        print_title(func.__name__)        
        func(*args)       
        print("+ Success")                                     
    except:
        print("- Failed ")
        print_exc()               
        raise SystemExit()        
                 
                               
def init():    
    if not clib._lib:
        print("+ try to manually load: ", LIBRARY_PATH)
        if not clib.init(LIBRARY_PATH):        
            raise clib.LibraryNotLoaded()   
      
def test_option_symbol_builder():
    B = get.build_option_symbol          
    assert "SPY_010118C300" == B("SpY",1,1,2018,True,300)
    assert "SPY_123199P200" == B("SPY",12,31,2099,False,200)
    assert "A_021119P1.5" == B( "a",2,11,2019,False,1.5)
    assert "A_021119P1.5" == B("A",2,11,2019,False,1.500)
    assert "ABCDEF_110121C99.999" == B( "ABCDEF",11,1,2021,True,99.999)
    assert "ABCDEF_110121C99.999" == B( "ABCDEF",11,1,2021,True,99.99900)
    assert "ABCDEF_110121C99" == B( "ABCDEF",11,1,2021,True,99.0)
    assert "ABCDEF_110121C99.001" == B( "abcdef",11,1,2021,True,99.001)

    def is_bad(u, m, d, y, c, s):
        try:
            B(u,m,d,y,c,s)
            return False
        except clib.CLibException:
            return True
            
    assert is_bad("",1,1,2018,True,300)
    assert is_bad("SpY_",1,1,2018,True,300)
    assert is_bad("SPY_",1,1,2018,True,300)
    assert is_bad("_SPY",1,1,2018,True,300)
    assert is_bad("SP_Y",1,1,2018,True,300)
    assert is_bad("SPY",0,1,2018,True,300)
    assert is_bad("SPY",13,1,2018,True,300)
    assert is_bad("SPY",1,0,2018,True,300)
    assert is_bad("SPY",1,32,2018,True,300)
    assert is_bad("SPY",1,31,999,True,300)
    assert is_bad("SPY",1,31,10001,True,300)
    assert is_bad("SPY",1,31,18,True,300)
    assert is_bad("SPY",1,31,2018,True,0.0)
    assert is_bad("spy",1,31,2018,True,-100.0)
      
def test_throttling(creds): 
    dw = get.get_def_wait_msec()  
    ww = get.get_wait_msec()
    assert dw == ww
    W = 2000
    get.set_wait_msec(W)
    assert get.get_wait_msec() == W
    print('+', get.get_def_wait_msec(), '->', get.get_wait_msec())
    
    if not use_live_connection:
        print("THROTTLE test requires 'use_live_connection=True'")
        return
    
    print("+ throttling test - BEGIN")
    g = get.QuoteGetter(creds, "SPY")
    ntests = 5
    start = perf_counter()
    for _ in range(ntests):
        a = get.wait_remaining()
        print(str(a) + "...", end='')
        sleep(W/1000/10)
        b = get.wait_remaining()
        print(str(b) + "...", end = '')
        assert a - b >= (W/1000/10)               
        g.get()         
        print(str(get.wait_remaining())) 
    end = perf_counter()
    t = end - start
    tmsec = int(t*1000)
    print("+ throttling test - END - %i calls, %i msec" % (ntests,tmsec))
    assert tmsec >= (W*ntests)
                    
            
def test_quote_getters(creds):
    g = get.QuoteGetter(creds, "SpY")   
    assert g.get_symbol() == "SPY"  
    j = Get(g)
    print(str(j))
    g.set_symbol('qqq')
    assert g.get_symbol() == 'QQQ'
    j = Get(g)
    print(str(j))
 

def test_quotes_getters(creds):    
    g = get.QuotesGetter(creds, "SPY", "qqq", "IwM") 
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM"])  
    j = Get(g)
    for k, v in j.items():
        print(k)
        print(str(v))
    g.set_symbols('QqQ')  
    assert g.get_symbols() == ['QQQ']
    j = Get(g)
    for k, v in j.items():
        print(k)
        print(str(v)) 
    g.add_symbols('SPy')
    g.add_symbols('iwm','GLD')
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM", "GLD"])
    g.remove_symbols('sPY','IWM')
    assert g.get_symbols() == sorted(['QQQ','GLD'])
    j = Get(g)
    for k, v in j.items():
        print(k)
        print(str(v))     
    g.remove_symbols('QQQ','gld')
    assert g.get_symbols() == []
    assert not Get(g)
    


def test_market_hours_getters(creds): 
    g = get.MarketHoursGetter(creds, get.MARKET_TYPE_BOND, "2019-07-04")   
    assert g.get_market_type() == get.MARKET_TYPE_BOND
    assert g.get_date() == "2019-07-04"   
    j = Get(g)
    print(str(j))   
    g.set_market_type(get.MARKET_TYPE_EQUITY)
    g.set_date("2019-07-05")
    assert g.get_market_type() == get.MARKET_TYPE_EQUITY  
    assert g.get_date() == "2019-07-05" 
    j = Get(g)
    print(str(j))  
    
    
def test_movers_getters(creds):   
    g = get.MoversGetter(creds, get.MOVERS_INDEX_COMPX, 
                         get.MOVERS_DIRECTION_TYPE_UP,
                         get.MOVERS_CHANGE_TYPE_PERCENT)
    assert g.get_index() == get.MOVERS_INDEX_COMPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_PERCENT
    j = Get(g)
    print(str(j))
    g.set_index(get.MOVERS_INDEX_SPX)
    g.set_direction_type(get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN)
    g.set_change_type(get.MOVERS_CHANGE_TYPE_VALUE)
    assert g.get_index() == get.MOVERS_INDEX_SPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_VALUE  
    j = Get(g)
    print(str(j))  
    
 
def test_historical_period_getters(creds):
    pt = get.PERIOD_TYPE_DAY
    p = get.VALID_PERIODS_BY_PERIOD_TYPE[pt][0]
    ft = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][0]
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][1]
    g = get.HistoricalPeriodGetter(creds, "SPY", pt, p, ft, f, True)
    assert g.get_symbol() == "SPY"
    assert g.get_period() == p    
    assert g.get_period_type() == pt
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.is_extended_hours() == True
    j = Get(g)
    print(str(j))   

    pt = get.PERIOD_TYPE_YEAR
    p = get.VALID_PERIODS_BY_PERIOD_TYPE[p][0]
    ft = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][1]
    f = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[ft][0]
    g.set_symbol("qqq")
    g.set_period(pt, p)
    g.set_frequency(ft, f)
    g.set_extended_hours(False)
    assert g.get_symbol() == "QQQ"
    assert g.get_period() == p
    assert g.get_period_type() == pt
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.is_extended_hours() == False
    j = Get(g)
    print(str(j)) 
        
        
def test_historical_range_getters(creds):
    ft = get.FREQUENCY_TYPE_MINUTE
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][-1]
    end = 1528205400000
    start = 1528119000000
    g = get.HistoricalRangeGetter(creds, "SpY", ft, f, start, end, False)
    assert g.get_symbol() == "SPY"
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == start
    assert g.get_end_msec_since_epoch() == end
    assert g.is_extended_hours() == False
    j = Get(g)
    print(str(j))   

    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][1]
    g.set_symbol("qqq")
    g.set_frequency(ft, f)
    g.set_extended_hours(True)
    assert g.get_symbol() == "QQQ"
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == start
    assert g.get_end_msec_since_epoch() == end
    assert g.is_extended_hours() == True
    j = Get(g)
    print(str(j))   
    
    
def test_option_chain_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)
    from_date = "2019-01-01"
    to_date = "2019-02-01"
    g = get.OptionChainGetter(creds, "kors", strikes, 
                              get.OPTION_CONTRACT_TYPE_CALL, True,
                              from_date, to_date, get.OPTION_EXP_MONTH_AUG,
                              get.OPTION_TYPE_ALL)
    assert g.get_symbol() == 'KORS'    
    assert g.get_strikes() == strikes    
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_CALL
    assert g.includes_quotes() == True
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_AUG
    assert g.get_option_type() == get.OPTION_TYPE_ALL
    j = Get(g)
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)
    from_date = "2019-01-01"
    to_date = "2019-02-01"
    g.set_symbol("SPy")
    g.set_strikes(strikes)
    g.set_contract_type(get.OPTION_CONTRACT_TYPE_ALL)
    g.include_quotes(False)
    g.set_from_date(from_date)
    g.set_to_date(to_date)
    g.set_exp_month(get.OPTION_EXP_MONTH_ALL)
    g.set_option_type(get.OPTION_TYPE_S)
    
    assert g.get_symbol() == 'SPY'    
    assert g.get_strikes() == strikes
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_ALL
    assert g.includes_quotes() == False
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_ALL
    assert g.get_option_type() == get.OPTION_TYPE_S
    j = Get(g)
    print(str(j))
    
    
def test_option_chain_strategy_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)
    strategy = get.OptionStrategy.COVERED()
    from_date = "2019-01-01"
    to_date = "2019-02-01"
    g = get.OptionChainStrategyGetter(creds, "kORS", strategy, strikes, 
                                      get.OPTION_CONTRACT_TYPE_CALL, True,
                                      from_date, to_date, 
                                      get.OPTION_EXP_MONTH_AUG, 
                                      get.OPTION_TYPE_ALL)
    assert g.get_symbol() == 'KORS'  
    assert g.get_strategy() == strategy  
    assert g.get_strikes() == strikes    
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_CALL
    assert g.includes_quotes() == True
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_AUG
    assert g.get_option_type() == get.OPTION_TYPE_ALL
    j = Get(g)
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)
    strategy = get.OptionStrategy.VERTICAL(5.0)
    from_date = "2019-02-01"
    to_date = "2019-03-01"
    g.set_symbol("sPY")
    g.set_strategy(strategy)
    g.set_strikes(strikes)    
    g.set_contract_type(get.OPTION_CONTRACT_TYPE_ALL)
    g.include_quotes(False)
    g.set_from_date(from_date)
    g.set_to_date(to_date)
    g.set_exp_month(get.OPTION_EXP_MONTH_ALL)
    g.set_option_type(get.OPTION_TYPE_S)
    
    assert g.get_symbol() == 'SPY'    
    assert g.get_strategy() == strategy
    assert g.get_strikes() == strikes
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_ALL
    assert g.includes_quotes() == False
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_ALL
    assert g.get_option_type() == get.OPTION_TYPE_S
    j = Get(g)
    print(str(j))
    
    
def test_option_chain_analytical_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)    
    from_date = "2019-01-01"
    to_date = "2019-02-01"
    g = get.OptionChainAnalyticalGetter(creds, "KORs", 20.00, 70.00, 3.00,
                                        100, strikes, 
                                        get.OPTION_CONTRACT_TYPE_CALL, True,
                                        from_date, to_date, 
                                        get.OPTION_EXP_MONTH_AUG, 
                                        get.OPTION_TYPE_ALL)
    assert g.get_symbol() == 'KORS'  
    assert g.get_volatility() == 20.00
    assert g.get_underlying_price() == 70.00
    assert g.get_interest_rate() == 3.00
    assert g.get_days_to_exp() == 100
    assert g.get_strikes() == strikes    
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_CALL
    assert g.includes_quotes() == True
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_AUG
    assert g.get_option_type() == get.OPTION_TYPE_ALL
    j = Get(g)
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)    
    from_date = "2019-02-01"
    to_date = "2019-03-01"
    g.set_symbol("SpY")
    g.set_volatility(100.10)
    g.set_underlying_price(49.99)
    g.set_interest_rate(15.01)
    g.set_days_to_exp(1000)
    g.set_strikes(strikes)    
    g.set_contract_type(get.OPTION_CONTRACT_TYPE_ALL)
    g.include_quotes(False)
    g.set_from_date(from_date)
    g.set_to_date(to_date)
    g.set_exp_month(get.OPTION_EXP_MONTH_ALL)
    g.set_option_type(get.OPTION_TYPE_S)
    
    assert g.get_symbol() == 'SPY'    
    assert g.get_volatility() == 100.10
    assert g.get_underlying_price() == 49.99
    assert g.get_interest_rate() == 15.01
    assert g.get_days_to_exp() == 1000
    assert g.get_strikes() == strikes
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_ALL
    assert g.includes_quotes() == False
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_ALL
    assert g.get_option_type() == get.OPTION_TYPE_S
    j = Get(g)
    print(str(j))


def test_account_info_getters(creds, account_id):
    g = get.AccountInfoGetter(creds, account_id, True, False)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == True
    assert g.returns_orders() == False
    j = Get(g)
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    g.set_account_id(account_id)
    g.return_positions(False)
    g.return_orders(True)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == False
    assert g.returns_orders() == True
    j = Get(g)
    print(str(j))
    
    
def test_preferences_getter(creds, account_id):
    g = get.PreferencesGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = Get(g)
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
         
     
def test_subscription_keys_getter(creds, account_id):
    g = get.StreamerSubscriptionKeysGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = Get(g)
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    
    
def test_transaction_history_getters(creds, account_id):
    g = get.TransactionHistoryGetter(creds, account_id,
                                     get.TRANSACTION_TYPE_TRADE,
                                     "spy" ,"2018-01-01", "2019-01-01")
    assert g.get_account_id() == account_id
    assert g.get_transaction_type() == get.TRANSACTION_TYPE_TRADE
    assert g.get_symbol() == "SPY"
    assert g.get_start_date() == "2018-01-01"
    assert g.get_end_date() == "2019-01-01"
    j = Get(g)
    print(str(j))
    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    g.set_account_id(account_id)
    g.set_transaction_type(get.TRANSACTION_TYPE_ALL)
    g.set_symbol("")
    g.set_start_date("2018-02-02")
    g.set_end_date("2019-02-02")
    assert g.get_account_id() == account_id
    assert g.get_transaction_type() == get.TRANSACTION_TYPE_ALL
    assert g.get_symbol() == ""
    assert g.get_start_date() == "2018-02-02"
    assert g.get_end_date() == "2019-02-02"  
    j = Get(g)
    print(str(j))      
  

def test_individual_transaction_history_getters(creds, account_id):
    g = get.IndividualTransactionHistoryGetter(creds, account_id, "012345678")
    assert g.get_account_id() == account_id
    assert g.get_transaction_id() == "012345678"
    if use_live_connection:
        try:
            g.get()
        except clib.CLibException as e:
            assert e.error_code == 103                    
    
    g.set_account_id("BAD_ACCOUNT_ID")
    g.set_transaction_id("BAD_TRANSACTION_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    assert g.get_transaction_id() == "BAD_TRANSACTION_ID"  
     
     
def test_user_principals_getters(creds):
    g = get.UserPrincipalsGetter(creds, False, False, False, False)
    assert g.returns_subscription_keys() == False
    assert g.returns_connection_info() == False
    assert g.returns_preferences() == False
    assert g.returns_surrogate_ids() == False
    j = Get(g)
    print(str(j))     
         
    g.return_subscription_keys(True)
    g.return_connection_info(True)
    g.return_preferences(True)
    g.return_surrogate_ids(True)
    assert g.returns_subscription_keys() == True
    assert g.returns_connection_info() == True
    assert g.returns_preferences() == True
    assert g.returns_surrogate_ids() == True
    j = Get(g)
    print(str(j)) 
     

def test_instrument_info_getters(creds):
    g = get.InstrumentInfoGetter(creds, get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX,
                                 "GOOGL?")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX
    assert g.get_query_string() == "GOOGL?"
    j = Get(g)
    print(str(j)) 
    
    g.set_query(get.INSTRUMENT_SEARCH_TYPE_CUSIP, "78462F103")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_CUSIP
    assert g.get_query_string() == "78462F103"
    j = Get(g)
    print(str(j))    


def test_streaming(creds):
    
    QS = stream.QuotesSubscription
    try:
        QS( [], (QS.FIELD_SYMBOL, QS.FIELD_BID_PRICE, QS.FIELD_ASK_PRICE))
        raise Exception("failed to catch exception(1)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))    
    try:
        QS( ("SPY",), [])
        raise Exception("failed to catch exception(2)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e)) 
    try:
        QS( ("SPY",), [100000])
        raise Exception("failed to catch exception(3)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e)) 
                
  
    symbols = ('spy', 'QQQ')    
    fields = (QS.FIELD_SYMBOL, QS.FIELD_BID_PRICE, QS.FIELD_ASK_PRICE)                
    qs = QS(symbols, fields)
    assert qs.get_command() == "SUBS"
    assert qs.get_service() == stream.SERVICE_TYPE_QUOTE
    assert set(qs.get_symbols()) == {'SPY','QQQ'}
    assert set(qs.get_fields()) == set(fields)
    
    OS = stream.OptionsSubscription
    symbols = ["SpY_081718P286"]
    fields = [OS.FIELD_DELTA, OS.FIELD_GAMMA, OS.FIELD_VEGA, OS.FIELD_VOLATILITY]
    os = OS(symbols, fields)
    assert os.get_service() == stream.SERVICE_TYPE_OPTION
    assert set(os.get_symbols()) == set([s.upper() for s in symbols])
    assert set(os.get_fields()) == set(fields)    
    
    LOFS = stream.LevelOneFuturesSubscription
    symbols = ('/es', '/GC')
    fields = (LOFS.FIELD_ASK_SIZE, LOFS.FIELD_BID_SIZE, LOFS.FIELD_FUTURE_EXPIRATION_DATE)
    lofs = LOFS(symbols, fields)
    assert lofs.get_service() == stream.SERVICE_TYPE_LEVELONE_FUTURES
    assert set(lofs.get_symbols()) == set([s.upper() for s in symbols])
    assert set(lofs.get_fields()) == set(fields)  
    
    LOFXS = stream.LevelOneForexSubscription
    symbols = ('EUR/USD',)
    fields = (LOFXS.FIELD_DIGITS, LOFXS.FIELD_TRADING_HOURS)
    lofxs = LOFXS(symbols, fields)
    assert lofxs.get_service() == stream.SERVICE_TYPE_LEVELONE_FOREX
    assert set(lofxs.get_symbols()) == set(symbols)
    assert set(lofxs.get_fields()) == set(fields)          
    
    LOFOS = stream.LevelOneFuturesOptionsSubscription
    symbols = ('/ESZ92800')
    fields = (LOFOS.FIELD_HIGH_PRICE, LOFOS.FIELD_LOW_PRICE)
    lofos = LOFOS(symbols, fields)
    assert lofos.get_service() == stream.SERVICE_TYPE_LEVELONE_FUTURES_OPTIONS
    assert set(lofos.get_symbols()) == set(symbols)
    assert set(lofos.get_fields()) == set(fields)  
    
    NHS = stream.NewsHeadlineSubscription
    symbols = ("SPY", "GOOG", "TSLA")
    fields = (NHS.FIELD_HEADLINE, NHS.FIELD_STORY_DATETIME, NHS.FIELD_STORY_SOURCE)
    nhs = NHS(symbols, fields)
    assert nhs.get_service() == stream.SERVICE_TYPE_NEWS_HEADLINE
    assert set(nhs.get_symbols()) == set(symbols)
    assert set(nhs.get_fields()) == set(fields)  
    
    CES = stream.ChartEquitySubscription
    symbols = ("SPY",)
    fields = (CES.FIELD_CHART_TIME, CES.FIELD_CLOSE_PRICE, CES.FIELD_VOLUME)
    ces = CES(symbols, fields)
    assert ces.get_service() == stream.SERVICE_TYPE_CHART_EQUITY
    assert set(ces.get_symbols()) == set(symbols)
    assert set(ces.get_fields()) == set(fields)  
    
    CFS = stream.ChartFuturesSubscription
    symbols = ["/ZN"]
    fields = (CFS.FIELD_HIGH_PRICE,)
    cfs = CFS(symbols,fields)
    assert cfs.get_service() == stream.SERVICE_TYPE_CHART_FUTURES
    assert set(cfs.get_symbols()) == set(symbols)
    assert set(cfs.get_fields()) == set(fields)  
    
    COS = stream.ChartOptionsSubscription
    symbols = ("SPY_081718C276",)
    fields = (COS.FIELD_LOW_PRICE,)
    cos = COS(symbols, fields)
    assert cos.get_service() == stream.SERVICE_TYPE_CHART_OPTIONS
    assert set(cos.get_symbols()) == set(symbols)
    assert set(cos.get_fields()) == set(fields)  
    
    TES = stream.TimesaleEquitySubscription
    symbols = ("EEM",)
    fields = (TES.FIELD_SYMBOL, TES.FIELD_LAST_PRICE)
    tes = TES(symbols,fields)
    assert tes.get_service() == stream.SERVICE_TYPE_TIMESALE_EQUITY
    assert set(tes.get_symbols()) == set(symbols)
    assert set(tes.get_fields()) == set(fields)  
    
    TFS = stream.TimesaleFuturesSubscription
    symbols = ("/ES", "/GC")
    tfs = TFS(symbols, fields)
    assert tfs.get_service() == stream.SERVICE_TYPE_TIMESALE_FUTURES
    assert set(tfs.get_symbols()) == set(symbols)
    assert set(tfs.get_fields()) == set(fields)
    
    TOS = stream.TimesaleOptionsSubscription
    symbols = ("SPY_081718C276",)  
    tos = TOS(symbols, fields)
    assert tos.get_service() == stream.SERVICE_TYPE_TIMESALE_OPTIONS
    assert set(tos.get_symbols()) == set(symbols)
    assert set(tos.get_fields()) == set(fields) 
    
    NAS = stream.NasdaqActivesSubscription        
    nas = NAS(NAS.DURATION_TYPE_ALL_DAY)
    assert nas.get_service() == stream.SERVICE_TYPE_ACTIVES_NASDAQ    
    assert nas.get_command() == "SUBS"
    assert nas.get_duration() == NAS.DURATION_TYPE_ALL_DAY
    
    NYAS = stream.NYSEActivesSubscription    
    nyas = NYAS(NAS.DURATION_TYPE_MIN_1)
    assert nyas.get_service() == stream.SERVICE_TYPE_ACTIVES_NYSE   
    assert nyas.get_command() == "SUBS"
    assert nyas.get_duration() == NYAS.DURATION_TYPE_MIN_1
    
    OCAS = stream.OTCBBActivesSubscription
    ocas = OCAS(OCAS.DURATION_TYPE_MIN_60)
    assert ocas.get_service() == stream.SERVICE_TYPE_ACTIVES_OTCBB   
    assert ocas.get_command() == "SUBS"
    assert ocas.get_duration() == OCAS.DURATION_TYPE_MIN_60
    
    OPAS = stream.OptionActivesSubscription
    opas = OPAS(OPAS.VENUE_TYPE_PUTS_DESC, OPAS.DURATION_TYPE_MIN_30)
    assert opas.get_service() == stream.SERVICE_TYPE_ACTIVES_OPTIONS
    assert opas.get_command() == "SUBS"
    assert opas.get_duration() == OPAS.DURATION_TYPE_MIN_30   
        
        
    def callback( cb, ss, ts, msg):
        print("--CALLBACK" + "-" * 70)
        print("  Type      ::", stream.callback_type_to_str(cb).ljust(16))
        print("  Service   ::", stream.service_type_to_str(ss).ljust(25))
        print("  TimeStamp ::", str(ts))
        print("  Msg/Data  ::", str(msg))
        print("-" * 80)

    _pause = lambda s : print("pause for %f sec..." % s) or sleep(s)

    session = stream.StreamingSession(creds, callback)
    assert not session.is_active()
    assert session.get_qos() == stream.QOS_FAST
    
    try:
        session.start()
        raise Exception("failed to catch exception(4)")
    except ValueError as e:
        print("+ successfully caught exception: ", str(e))
    try:
        class Dummy(stream._StreamingSubscription):
            def __init__(self):
                self._obj = None
        session.start(Dummy(), qs)
        raise Exception("failed to catch exception(5)")
    except TypeError as e:
        print("+ successfully caught exception: ", str(e))    
    try:       
        session.start(os,1,2,3)
        raise Exception("failed to catch exception(6)")
    except TypeError as e:
        print("+ successfully caught exception: ", str(e)) 
 
    assert all(session.start(qs))    
    _pause(1)
    
    try:
        session.start(os)
        raise Exception("failed to catch exception(7)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))    
    
    assert all(session.add_subscriptions(os))
    _pause(1)
    
    assert all(session.add_subscriptions(lofs))
    _pause(1) 
    
    assert all(session.add_subscriptions(lofxs, lofos))
    assert session.set_qos(stream.QOS_REAL_TIME)
    assert session.get_qos() == stream.QOS_REAL_TIME
    
    ## EXC THROWN BEFORE HERE (at least never got through C++ constructor)
    session2 = stream.StreamingSession(creds, callback)
    
    try:
        session2.start(nhs)
        raise Exception("failed to catch exception(8)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))
    
    _pause(10)
    print("... pause done")
    session.stop()
    
    assert not session.is_active()
    assert not session2.is_active()
    _pause(3)
    
    assert all(session2.start(nhs, ces, cfs, cos))
    assert session2.is_active()
    
    try:
        session.add_subscriptions(tes, tfs)
        raise Exception("failed to catch exception(9)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))
    try:
        session.start(tes, tfs)
        raise Exception("failed to catch exception(10)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))
        
    _pause(3)
    assert all(session2.add_subscriptions(tes, tfs, tos, nas, nyas, ocas, opas))
    _pause(10)
    
    session2.stop()
    assert not session2.is_active()
    
    session = None
    session2 = None
    _pause(.5)
    gc.collect()
    _pause(1)
                
                
def test_execute_order_objects():
    def test_exc(n, func, *args):
        try:    
            func( *args )
            raise Exception("failed to catch exception(%i)" % n)   
        except (clib.CLibException, TypeError) as e:
            print("+ successfully caught exception: ", str(e))

    OLEG = execute.OrderLeg
    OTICK = execute.OrderTicket
    
    test_exc(1, OLEG, execute.ORDER_ASSET_TYPE_EQUITY, "", 
             execute.ORDER_INSTRUCTION_BUY, 100)
    test_exc(2, OLEG, execute.ORDER_ASSET_TYPE_EQUITY, "SPY", 
             execute.ORDER_INSTRUCTION_BUY, 0)
    test_exc(3, OLEG, 0, "",  execute.ORDER_INSTRUCTION_BUY, 100)
    test_exc(4, OLEG, 8, "",  execute.ORDER_INSTRUCTION_BUY, 100)
    test_exc(5, OLEG, execute.ORDER_ASSET_TYPE_EQUITY, "", 0, 100)
    test_exc(6, OLEG, execute.ORDER_ASSET_TYPE_EQUITY, "", 10, 100)
    
    leg1 = OLEG( execute.ORDER_ASSET_TYPE_EQUITY, "SPY", 
                 execute.ORDER_INSTRUCTION_BUY, 100)
    order1 = OTICK()
    
    test_exc(7, order1.set_duration, 0)
    test_exc(8, order1.set_duration, 4)
    test_exc(9, order1.set_session, 0)
    test_exc(10, order1.set_session, 5)
    test_exc(11, order1.set_type, -1)
    test_exc(12, order1.set_type, 12)
    test_exc(13, order1.add_legs, None)
    
    order1.set_duration(execute.ORDER_DURATION_DAY) \
          .set_session(execute.ORDER_SESSION_NORMAL) \
          .set_type(execute.ORDER_TYPE_STOP_LIMIT) \
          .set_price(300.001) \
          .set_stop_price(299.999) \
          .add_legs(leg1)
          
    def check_order1(o):          
        assert o.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
        assert o.get_duration() == execute.ORDER_DURATION_DAY
        assert o.get_session() == execute.ORDER_SESSION_NORMAL
        assert o.get_type() == execute.ORDER_TYPE_STOP_LIMIT
        assert o.get_price() == 300.001
        assert o.get_stop_price() == 299.999
        
        leg1b = o.get_leg(0)
        
        assert leg1b.get_asset_type() == execute.ORDER_ASSET_TYPE_EQUITY
        assert leg1b.get_symbol() == "SPY"
        assert leg1b.get_instruction() == execute.ORDER_INSTRUCTION_BUY
        assert leg1b.get_quantity() == 100
        
    print("Order 1 JSON:")
    print( str(order1.as_json()) )
    check_order1(order1)
    
    leg1_old = order1.get_leg(0)
    order1.remove_leg(0)
    assert not order1.get_legs()    
    
    leg1 = OLEG( execute.ORDER_ASSET_TYPE_EQUITY, "SPY", 
                 execute.ORDER_INSTRUCTION_SELL, 200)

    order1.add_legs(leg1)  
    assert order1.get_leg(0).get_instruction() == execute.ORDER_INSTRUCTION_SELL
    assert order1.get_leg(0).get_quantity() == 200   
        
    leg1 = leg1_old
    test_exc(14, order1.replace_leg, 1, leg1)    
    order1.replace_leg(0, leg1)
    check_order1(order1) 
    
    test_exc(15, order1.get_leg, 1)
            
    leg2 = OLEG( execute.ORDER_ASSET_TYPE_OPTION, "SPY_011720C300", 
                             execute.ORDER_INSTRUCTION_BUY_TO_OPEN, 2)
    leg3 = OLEG( execute.ORDER_ASSET_TYPE_OPTION, "SPY_011720C350", 
                             execute.ORDER_INSTRUCTION_SELL_TO_OPEN, 2)
    
    order2 = OTICK()
    
    test_exc(16, order2.set_complex_strategy_type, -1)
    test_exc(17, order2.set_complex_strategy_type, 20)
    
    order2.set_duration(execute.ORDER_DURATION_GOOD_TILL_CANCEL) \
          .set_cancel_time("2019-01-01") \
          .set_session(execute.ORDER_SESSION_NORMAL) \
          .set_type(execute.ORDER_TYPE_NET_DEBIT) \
          .set_price(9.99) \
          .set_complex_strategy_type(execute.COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL) \
          .add_legs(leg2, leg3)   
          
    def check_order2(o):
        assert o.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
        assert o.get_complex_strategy_type() \
            == execute.COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL 
        assert o.get_duration() == execute.ORDER_DURATION_GOOD_TILL_CANCEL
        assert o.get_cancel_time() == "2019-01-01"
        assert o.get_session() == execute.ORDER_SESSION_NORMAL
        assert o.get_type() == execute.ORDER_TYPE_NET_DEBIT      
        assert o.get_price() == 9.99
        
        leg2b, leg3b = o.get_legs()
        
        assert leg2b.get_asset_type() == execute.ORDER_ASSET_TYPE_OPTION
        assert leg2b.get_symbol() == "SPY_011720C300"
        assert leg2b.get_instruction() == execute.ORDER_INSTRUCTION_BUY_TO_OPEN
        assert leg2b.get_quantity() == 2   
    
        assert leg3b.get_asset_type() == execute.ORDER_ASSET_TYPE_OPTION
        assert leg3b.get_symbol() == "SPY_011720C350"
        assert leg3b.get_instruction() == execute.ORDER_INSTRUCTION_SELL_TO_OPEN
        assert leg3b.get_quantity() == 2   
    
    print("Order 2 JSON:")
    print( str(order2.as_json()) )
    check_order2(order2)
    
    order3 = OTICK()
    order3.set_strategy_type(execute.ORDER_STRATEGY_TYPE_OCO) \
          .add_child(order1) \
          .add_child(order2)
          
    order1.clear_legs()
    assert not order1.get_children() 
    order2 = None
          
    assert order3.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_OCO
    
    order1b, order2b = order3.get_children()
    
    check_order1(order1b)
    check_order2(order2b)
    
    print("Order 3 JSON:")
    print( str(order3.as_json()) )
    
    order3.clear_children()
    assert not order3.get_children()
          
          
#TODO check exceptions  
def test_execute_order_builders():
    print('*** Simple-Equity ***')
    test_execute_simple_equity_builders()
    print('\n*** Simple-Option ***')
    test_execute_simple_option_builders()
    print('\n*** Spread-Option ***')
    test_execute_spread_option_builders()
    print('\n*** OCO / OTO ***')
    test_execute_oco_oto_builders()
            
  
def test_execute_simple_equity_builders():
    B = execute.SimpleOrderBuilder    
    EQUITY = execute.ORDER_ASSET_TYPE_EQUITY
    BUY = execute.ORDER_INSTRUCTION_BUY
    SELL = execute.ORDER_INSTRUCTION_SELL
    SHORT = execute.ORDER_INSTRUCTION_SELL_SHORT
    COVER = execute.ORDER_INSTRUCTION_BUY_TO_COVER
    MARKET = execute.ORDER_TYPE_MARKET
    LIMIT = execute.ORDER_TYPE_LIMIT
    STOP = execute.ORDER_TYPE_STOP
    STOP_LIMIT = execute.ORDER_TYPE_STOP_LIMIT
        
    def check_order(o, oty, sym, instr, q, lp=0, sp=0 ):
        assert o.get_type() == oty
        assert o.get_price() == lp 
        assert o.get_stop_price() == sp 
        assert o.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
        assert o.get_complex_strategy_type() == \
            execute.COMPLEX_ORDER_STRATEGY_TYPE_NONE            
        l = o.get_legs()
        assert len(l) == 1
        l = l[0]            
        assert l.get_asset_type() == execute.ORDER_ASSET_TYPE_EQUITY 
        assert l.get_symbol() == sym 
        assert l.get_instruction() == instr 
        assert l.get_quantity() == q 
        
    def test_exc(n, func, *args):
        try:    
            func( *args )
            raise Exception("failed to catch exception(%i)" % n)   
        except clib.CLibException as e:
            print("+ successfully caught exception: ", str(e))
            
    test_exc(1, B.Equity.Market.Buy, "", 1)
    test_exc(2, B.Equity.Market.Sell, "SPY", 0)
    test_exc(3, B.Equity.Limit.Short, "SPY", 1, 0.0)
    test_exc(4, B.Equity.Limit.Cover, "SPY", 1, -1)
    test_exc(5, B.Equity.Stop.Buy, "SPY", 1, 0.0, 1.00)
    test_exc(6, B.Equity.Stop.Sell, "SPY", 1, -1.0, 0.00)
    test_exc(7, B.Equity.Stop.Short, "SPY", 1, 1.0, -1.00)    
    
    # MARKET
    mb = B.Equity.Market.Buy('SPY', 100)
    print( mb.as_json() )
    check_order(mb, MARKET, 'SPY', BUY, 100)    
    mb2 = B.build(MARKET, EQUITY, 'SPY', BUY, 100)    
    assert mb2 == mb
    assert mb2.as_json() == mb.as_json()
    
    ms = B.Equity.Market.Sell('SPY', 200)
    print( ms.as_json() )
    check_order(ms, MARKET,'SPY', SELL, 200)
    ms2 = B.build(MARKET, EQUITY, 'SPY', SELL, 200)
    assert ms2 == ms
    assert ms2.as_json() == ms.as_json()
    
    mss = B.Equity.Market.Short('QQQ', 99)
    print( mss.as_json() )
    check_order(mss, MARKET,'QQQ', SHORT, 99)
    mss2 = B.build(MARKET, EQUITY, 'QQQ', SHORT, 99)
    assert mss2 == mss
    assert mss2.as_json() == mss.as_json()
    
    mbc = B.Equity.Market.Cover('QQQ', 1)
    print( mbc.as_json() )
    check_order(mbc, MARKET,'QQQ', COVER, 1)
    mbc2 = B.build(MARKET, EQUITY, 'QQQ', COVER, 1)
    assert mbc2 == mbc    
    assert mbc2.as_json() == mbc.as_json()
    
    # LIMIT
    lb = B.Equity.Limit.Buy('SPY', 100, 300.01)
    print( lb.as_json() )
    check_order(lb, LIMIT, 'SPY', BUY, 100, 300.01)    
    lb2 = B.build(LIMIT, EQUITY, 'SPY', BUY, 100, 300.01)
    assert lb2 == lb
    assert lb2.as_json() == lb.as_json()
    
    ls = B.Equity.Limit.Sell('SPY', 200, 299.9999)
    print( ls.as_json() )
    check_order(ls, LIMIT,'SPY', SELL, 200, 299.9999)
    ls2 = B.build(LIMIT, EQUITY, 'SPY', SELL, 200, 299.9999)
    assert ls2 == ls
    assert ls2.as_json() == ls.as_json()
    
    lss = B.Equity.Limit.Short('QQQ', 99, 300)
    print( lss.as_json() )
    check_order(lss, LIMIT,'QQQ', SHORT, 99, 300)
    lss2 = B.build(LIMIT, EQUITY, 'QQQ', SHORT, 99, 300)
    assert lss2 == lss
    assert lss2.as_json() == lss.as_json()
    
    lbc = B.Equity.Limit.Cover('QQQ', 1, .01)
    print( lbc.as_json() )
    check_order(lbc, LIMIT,'QQQ', COVER, 1, .01)
    lbc2 = B.build(LIMIT, EQUITY, 'QQQ', COVER, 1, .01)
    assert lbc2 == lbc    
    assert lbc2.as_json() == lbc.as_json()
    
    # STOP
    sb = B.Equity.Stop.Buy('SPY', 100, 300.01)
    print( sb.as_json() )
    check_order(sb, STOP, 'SPY', BUY, 100, 0, 300.01)    
    sb2 = B.build(STOP, EQUITY, 'SPY', BUY, 100, 0, 300.01)
    assert sb2 == sb
    assert sb2.as_json() == sb.as_json()
    
    ss = B.Equity.Stop.Sell('SPY', 200, 299.9999)
    print( ss.as_json() )
    check_order(ss, STOP,'SPY', SELL, 200, 0, 299.9999)
    ss2 = B.build(STOP, EQUITY, 'SPY', SELL, 200, 0, 299.9999)
    assert ss2 == ss
    assert ss2.as_json() == ss.as_json()
    
    sss = B.Equity.Stop.Short('QQQ', 99, 300)
    print( sss.as_json() )
    check_order(sss, STOP,'QQQ', SHORT, 99, 0, 300)
    sss2 = B.build(STOP, EQUITY, 'QQQ', SHORT, 99, 0, 300)
    assert sss2 == sss
    assert sss2.as_json() == sss.as_json()
    
    sbc = B.Equity.Stop.Cover('QQQ', 1, .01)
    print( sbc.as_json() )
    check_order(sbc, STOP,'QQQ', COVER, 1, 0, .01)
    sbc2 = B.build(STOP, EQUITY, 'QQQ', COVER, 1, 0, .01)
    assert sbc2 == sbc    
    assert sbc2.as_json() == sbc.as_json()
    
    # STOP_LIMIT
    slb = B.Equity.Stop.Buy('SPY', 100, 300.01, 300.09)
    print( slb.as_json() )
    check_order(slb, STOP_LIMIT, 'SPY', BUY, 100, 300.09, 300.01)    
    slb2 = B.build(STOP_LIMIT, EQUITY, 'SPY', BUY, 100, 300.09, 300.01)
    assert slb2 == slb
    assert slb2.as_json() == slb.as_json()
    
    sls = B.Equity.Stop.Sell('SPY', 200, 299.9999, 299.8888)
    print( sls.as_json() )
    check_order(sls, STOP_LIMIT,'SPY', SELL, 200, 299.8888, 299.9999)
    sls2 = B.build(STOP_LIMIT, EQUITY, 'SPY', SELL, 200, 299.8888, 299.9999)
    assert sls2 == sls
    assert sls2.as_json() == sls.as_json()
    
    slss = B.Equity.Stop.Short('QQQ', 99, 300, 299)
    print( slss.as_json() )
    check_order(slss, STOP_LIMIT,'QQQ', SHORT, 99, 299, 300)
    slss2 = B.build(STOP_LIMIT, EQUITY, 'QQQ', SHORT, 99, 299, 300)
    assert slss2 == slss
    assert slss2.as_json() == slss.as_json()
    
    slbc = B.Equity.Stop.Cover('QQQ', 1, .01, .0001)
    print( slbc.as_json() )
    check_order(slbc, STOP_LIMIT,'QQQ', COVER, 1, .0001, .01)
    slbc2 = B.build(STOP_LIMIT, EQUITY, 'QQQ', COVER, 1, .0001, .01)
    assert slbc2 == slbc    
    assert slbc2.as_json() == slbc.as_json()
    
    
def test_execute_simple_option_builders():
    B = execute.SimpleOrderBuilder    
    OPTION = execute.ORDER_ASSET_TYPE_OPTION
    BTO = execute.ORDER_INSTRUCTION_BUY_TO_OPEN
    BTC = execute.ORDER_INSTRUCTION_BUY_TO_CLOSE
    STO = execute.ORDER_INSTRUCTION_SELL_TO_OPEN
    STC = execute.ORDER_INSTRUCTION_SELL_TO_CLOSE
    MARKET = execute.ORDER_TYPE_MARKET
    LIMIT = execute.ORDER_TYPE_LIMIT
        
    def check_order(o, oty, sym, instr, q, lp=0, sp=0 ):
        assert o.get_type() == oty
        assert o.get_price() == lp 
        assert o.get_stop_price() == sp 
        assert o.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
        assert o.get_complex_strategy_type() == \
            execute.COMPLEX_ORDER_STRATEGY_TYPE_NONE            
        l = o.get_legs()
        assert len(l) == 1
        l = l[0]            
        assert l.get_asset_type() == OPTION 
        assert l.get_symbol() == sym 
        assert l.get_instruction() == instr 
        assert l.get_quantity() == q 
    
    def test_exc(n, func, *args):
        try:    
            func( *args )
            raise Exception("failed to catch exception(%i)" % n)   
        except clib.CLibException as e:
            print("+ successfully caught exception: ", str(e))
            
    test_exc(1, B.Option.Market.BuyToOpen1, "", 1)
    test_exc(2, B.Option.Market.SellToOpen1, "SPY_011720C300", 0)
    test_exc(3, B.Option.Market.BuyToOpen2, "", 1, 17, 2020, True, 300, 1)
    test_exc(4, B.Option.Market.SellToOpen2, "SPY", 0, 17, 2020, True, 300, 1)
    test_exc(5, B.Option.Market.BuyToClose2, "SPY", 1, 0, 2020, True, 300, 1)
    test_exc(6, B.Option.Market.SellToClose2, "SPY", 1, 17, 0, True, 300, 1)
    test_exc(7, B.Option.Market.BuyToOpen2, "SPY", 1, 17, 0, True, 0.0, 1)
    test_exc(8, B.Option.Market.SellToOpen2, "SPY", 1, 17, 0, True, 300, 0)
    test_exc(9, B.Option.Limit.BuyToOpen1, "", 1, 100.00)
    test_exc(10, B.Option.Limit.SellToOpen1, "SPY_011720C300", 0, 100)
    test_exc(11, B.Option.Limit.BuyToClose1, "SPY_011720C300", 1, 0.0)
    test_exc(12, B.Option.Limit.SellToClose1, "SPY_011720C300", 1, -1)
    test_exc(13, B.Option.Limit.BuyToOpen2, "", 1, 17, 2020, True, 300, 1, 100.0)
    test_exc(14, B.Option.Limit.SellToOpen2, "SPY", 0, 17, 2020, True, 300, 1, 100.0)
    test_exc(15, B.Option.Limit.BuyToClose2, "SPY", 1, 0, 2020, True, 300, 1, 100.0)
    test_exc(16, B.Option.Limit.SellToClose2, "SPY", 1, 17, 0, True, 300, 1, 100.0)
    test_exc(17, B.Option.Limit.BuyToOpen2, "SPY", 1, 17, 0, True, 0.0, 1, 100.0)
    test_exc(18, B.Option.Limit.SellToOpen2, "SPY", 1, 17, 0, True, 300, 0, 100.0)
    test_exc(19, B.Option.Limit.BuyToClose2, "SPY", 1, 17, 0, True, 300, 1, .0)
    test_exc(20, B.Option.Limit.SellToClose2, "SPY", 1, 17, 0, True, 300, 1, -100.0)
    
    # MARKET
    mbto = B.Option.Market.BuyToOpen1('SPY_011720C300', 1)
    print( mbto.as_json() )
    check_order(mbto, MARKET, 'SPY_011720C300', BTO, 1)   
    mbto2 = B.Option.Market.BuyToOpen2("SPY", 1, 17, 2020, True, 300, 1)      
    assert mbto2 == mbto
    mbto3 = B.build(MARKET, OPTION, 'SPY_011720C300', BTO, 1)
    assert mbto3 == mbto2
    assert mbto3.as_json() == mbto2.as_json() == mbto.as_json()
    
    msto = B.Option.Market.SellToOpen1('SPY_011720P200', 99)
    print( msto.as_json() )
    check_order(msto, MARKET, 'SPY_011720P200', STO, 99)   
    msto2 = B.Option.Market.SellToOpen2("SPY", 1, 17, 2020, False, 200, 99)      
    assert msto2 == msto
    msto3 = B.build(MARKET, OPTION, 'SPY_011720P200', STO, 99)
    assert msto3 == msto2
    assert msto3.as_json() == msto2.as_json() == msto.as_json()
    
    mbtc = B.Option.Market.BuyToClose1('SPY_011720C300', 1)
    print( mbtc.as_json() )
    check_order(mbtc, MARKET, 'SPY_011720C300', BTC, 1)   
    mbtc2 = B.Option.Market.BuyToClose2("SPY", 1, 17, 2020, True, 300, 1)      
    assert mbtc2 == mbtc
    mbtc3 = B.build(MARKET, OPTION, 'SPY_011720C300', BTC, 1)
    assert mbtc3 == mbtc2
    assert mbtc3.as_json() == mbtc2.as_json() == mbtc.as_json()
  
    mstc = B.Option.Market.SellToClose1('SPY_011720C300', 1)
    print( mstc.as_json() )
    check_order(mstc, MARKET, 'SPY_011720C300', STC, 1)   
    mstc2 = B.Option.Market.SellToClose2("SPY", 1, 17, 2020, True, 300, 1)      
    assert mstc2 == mstc
    mstc3 = B.build(MARKET, OPTION, 'SPY_011720C300', STC, 1)
    assert mstc3 == mstc2
    assert mstc3.as_json() == mstc2.as_json() == mstc.as_json()
      
    # LIMIT
    lbto = B.Option.Limit.BuyToOpen1('SPY_011720C300', 1, 9.99)
    print( lbto.as_json() )
    check_order(lbto, LIMIT, 'SPY_011720C300', BTO, 1, 9.99)   
    lbto2 = B.Option.Limit.BuyToOpen2("SPY", 1, 17, 2020, True, 300, 1, 9.99)      
    assert lbto2 == lbto
    lbto3 = B.build(LIMIT, OPTION, 'SPY_011720C300', BTO, 1, 9.99)
    assert lbto3 == lbto2
    assert lbto3.as_json() == lbto2.as_json() == lbto.as_json()
    
    lsto = B.Option.Limit.SellToOpen1('SPY_011720P200', 99, .009)
    print( lsto.as_json() )
    check_order(lsto, LIMIT, 'SPY_011720P200', STO, 99, .009)   
    lsto2 = B.Option.Limit.SellToOpen2("SPY", 1, 17, 2020, False, 200, 99, .009)      
    assert lsto2 == lsto
    lsto3 = B.build(LIMIT, OPTION, 'SPY_011720P200', STO, 99, .009)
    assert lsto3 == lsto2
    assert lsto3.as_json() == lsto2.as_json() == lsto.as_json()
    
    lbtc = B.Option.Limit.BuyToClose1('SPY_011720C300', 1, 10)
    print( lbtc.as_json() )
    check_order(lbtc, LIMIT, 'SPY_011720C300', BTC, 1, 10.00)   
    lbtc2 = B.Option.Limit.BuyToClose2("SPY", 1, 17, 2020, True, 300, 1, 10)      
    assert lbtc2 == lbtc
    lbtc3 = B.build(LIMIT, OPTION, 'SPY_011720C300', BTC, 1, 10)
    assert lbtc3 == lbtc2
    assert lbtc3.as_json() == lbtc2.as_json() == lbtc.as_json()
  
    lstc = B.Option.Limit.SellToClose1('SPY_011720C300', 1, 10.0001)
    print( lstc.as_json() )
    check_order(lstc, LIMIT, 'SPY_011720C300', STC, 1, 10.0001)   
    lstc2 = B.Option.Limit.SellToClose2("SPY", 1, 17, 2020, True, 300, 1, 10.0001)      
    assert lstc2 == lstc
    lstc3 = B.build(LIMIT, OPTION, 'SPY_011720C300', STC, 1, 10.0001)
    assert lstc3 == lstc2
    assert lstc3.as_json() == lstc2.as_json() == lstc.as_json()
    

def test_execute_spread_option_builders():
    pass
    
def test_execute_oco_oto_builders():
    pass
          
                    
if __name__ == '__main__':    
    print("\n*** TDAmeritradeAPI test.py ***")
    print("+", strftime("%m/%d/%Y %H:%M:%S"))
    print("+", SYSTEM)
    print("+", ARCH)
    print("+ PID:", os.getpid())
    try:
        print("+ PACKAGE BUILD INFO:")
        with open(PACKAGE_BUILD_INFO_PATH, 'r') as f:
            d = json.load(f)
            for k,v in sorted(d.items()):
                print("+    ", k, str(v))            
    except BaseException as e:
        print('- Failed to loade package build info:', str(e))
    args = parser.parse_args()
    use_live_connection = not args.no_live_connect 
    print("+ live-connection:", str(use_live_connection))
    test(init)
    print_title("load credentials") 
    with auth.CredentialsManager(args.credentials_path, \
                                  args.credentials_password, True) as cm:    
        test(test_option_symbol_builder)        
        test(test_execute_order_objects)
        test(test_execute_order_builders)        
        test(test_quote_getters, cm.credentials)        
        test(test_throttling, cm.credentials)        
        test(test_quotes_getters, cm.credentials)             
        test(test_market_hours_getters, cm.credentials)
        test(test_movers_getters, cm.credentials)
        test(test_historical_period_getters, cm.credentials)
        test(test_historical_range_getters, cm.credentials)
        test(test_option_chain_getters, cm.credentials)
        test(test_option_chain_strategy_getters, cm.credentials)
        test(test_option_chain_analytical_getters, cm.credentials)
        test(test_account_info_getters, cm.credentials, args.account_id)
        test(test_preferences_getter, cm.credentials, args.account_id)
        test(test_subscription_keys_getter, cm.credentials, args.account_id)                       
        test(test_transaction_history_getters, cm.credentials, args.account_id)
        test(test_individual_transaction_history_getters, cm.credentials, 
             args.account_id)
        test(test_user_principals_getters, cm.credentials)
        test(test_instrument_info_getters, cm.credentials)
        if use_live_connection:
            test(test_streaming, cm.credentials)
        else:
            print("STREAMING test requires 'use_live_connection=True'")
        
