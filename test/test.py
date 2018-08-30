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
import argparse, gc, os

from tdma_api import get, auth, clib, stream

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
        
parser = argparse.ArgumentParser("test tdma_api")
parser.add_argument("account_id", type=str, help="account id" )
parser.add_argument("credentials_path", type=str, 
                    help="path of encrypted credentials file")
parser.add_argument("credentials_password", type=str,
                    help="password to decrypt credentials files")
            
def print_title(s):
    l = len(s) + 4
    print('\n')  
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
    assert "SPY_010118C300" == B("SPY",1,1,2018,True,300)
    assert "SPY_123199P200" == B("SPY",12,31,2099,False,200)
    assert "A_021119P1.5" == B( "A",2,11,2019,False,1.5)
    assert "A_021119P1.5" == B("A",2,11,2019,False,1.500)
    assert "ABCDEF_110121C99.999" == B( "ABCDEF",11,1,2021,True,99.999)
    assert "ABCDEF_110121C99.999" == B( "ABCDEF",11,1,2021,True,99.99900)
    assert "ABCDEF_110121C99" == B( "ABCDEF",11,1,2021,True,99.0)
    assert "ABCDEF_110121C99.001" == B( "ABCDEF",11,1,2021,True,99.001)

    def is_bad(u, m, d, y, c, s):
        try:
            B(u,m,d,y,c,s)
            return False
        except clib.CLibException:
            return True
            
    assert is_bad("",1,1,2018,True,300)
    assert is_bad("SPY_",1,1,2018,True,300)
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
    assert is_bad("SPY",1,31,2018,True,-100.0)
      
def test_throttling(creds): 
    dw = get.get_def_wait_msec()  
    ww = get.get_wait_msec()
    assert dw == ww
    W = 2000
    get.set_wait_msec(W)
    assert get.get_wait_msec() == W
    print('+', get.get_def_wait_msec(), '->', get.get_wait_msec())
    
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
    g = get.QuoteGetter(creds, "SPY")   
    assert g.get_symbol() == "SPY"  
    j = g.get()
    print(str(j))
    g.set_symbol('QQQ')
    assert g.get_symbol() == 'QQQ'
    j = g.get()
    print(str(j))
 

def test_quotes_getters(creds):
    g = get.QuotesGetter(creds, "SPY", "QQQ", "IWM") 
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM"])  
    j = g.get()
    for k, v in j.items():
        print(k)
        print(str(v))
    g.set_symbols('QQQ')  
    assert g.get_symbols() == ['QQQ']
    j = g.get()
    for k, v in j.items():
        print(k)
        print(str(v)) 
    g.add_symbols('SPY')
    g.add_symbols('IWM','GLD')
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM", "GLD"])
    g.remove_symbols('SPY','IWM')
    assert g.get_symbols() == sorted(['QQQ','GLD'])
    j = g.get()
    for k, v in j.items():
        print(k)
        print(str(v))     
    g.remove_symbols('QQQ','GLD')
    assert g.get_symbols() == []
    assert not g.get()
    


def test_market_hours_getters(creds): 
    g = get.MarketHoursGetter(creds, get.MARKET_TYPE_BOND, "2019-07-04")   
    assert g.get_market_type() == get.MARKET_TYPE_BOND
    assert g.get_date() == "2019-07-04"   
    j = g.get()
    print(str(j))   
    g.set_market_type(get.MARKET_TYPE_EQUITY)
    g.set_date("2019-07-05")
    assert g.get_market_type() == get.MARKET_TYPE_EQUITY  
    assert g.get_date() == "2019-07-05" 
    j = g.get()
    print(str(j))  
    
    
def test_movers_getters(creds):   
    g = get.MoversGetter(creds, get.MOVERS_INDEX_COMPX, 
                         get.MOVERS_DIRECTION_TYPE_UP,
                         get.MOVERS_CHANGE_TYPE_PERCENT)
    assert g.get_index() == get.MOVERS_INDEX_COMPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_PERCENT
    j = g.get()
    print(str(j))
    g.set_index(get.MOVERS_INDEX_SPX)
    g.set_direction_type(get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN)
    g.set_change_type(get.MOVERS_CHANGE_TYPE_VALUE)
    assert g.get_index() == get.MOVERS_INDEX_SPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_VALUE  
    j = g.get()
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
    j = g.get()
    print(str(j))   

    pt = get.PERIOD_TYPE_YEAR
    p = get.VALID_PERIODS_BY_PERIOD_TYPE[p][0]
    ft = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][1]
    f = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[ft][0]
    g.set_symbol("QQQ")
    g.set_period(pt, p)
    g.set_frequency(ft, f)
    g.set_extended_hours(False)
    assert g.get_symbol() == "QQQ"
    assert g.get_period() == p
    assert g.get_period_type() == pt
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.is_extended_hours() == False
    j = g.get()
    print(str(j)) 
        
        
def test_historical_range_getters(creds):
    ft = get.FREQUENCY_TYPE_MINUTE
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][-1]
    end = 1528205400000
    start = 1528119000000
    g = get.HistoricalRangeGetter(creds, "SPY", ft, f, start, end, False)
    assert g.get_symbol() == "SPY"
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == start
    assert g.get_end_msec_since_epoch() == end
    assert g.is_extended_hours() == False
    j = g.get()
    print(str(j))   

    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][1]
    g.set_symbol("QQQ")
    g.set_frequency(ft, f)
    g.set_extended_hours(True)
    assert g.get_symbol() == "QQQ"
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == start
    assert g.get_end_msec_since_epoch() == end
    assert g.is_extended_hours() == True
    j = g.get()
    print(str(j))   
    
    
def test_option_chain_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)
    from_date = "2018-07-24"
    to_date = "2018-09-24"
    g = get.OptionChainGetter(creds, "KORS", strikes, 
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
    j = g.get()
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)
    from_date = "2018-08-21"
    to_date = "2018-09-15"
    g.set_symbol("SPY")
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
    j = g.get()
    print(str(j))
    
    
def test_option_chain_strategy_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)
    strategy = get.OptionStrategy.COVERED()
    from_date = "2018-07-24"
    to_date = "2018-09-24"
    g = get.OptionChainStrategyGetter(creds, "KORS", strategy, strikes, 
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
    j = g.get()
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)
    strategy = get.OptionStrategy.VERTICAL(5.0)
    from_date = "2018-08-21"
    to_date = "2018-09-15"
    g.set_symbol("SPY")
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
    j = g.get()
    print(str(j))
    
    
def test_option_chain_analytical_getters(creds):
    strikes = get.OptionStrikes.N_ATM(2)    
    from_date = "2018-07-24"
    to_date = "2018-09-24"
    g = get.OptionChainAnalyticalGetter(creds, "KORS", 20.00, 70.00, 3.00,
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
    j = g.get()
    print(str(j))
        
    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)    
    from_date = "2018-08-21"
    to_date = "2018-09-15"
    g.set_symbol("SPY")
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
    j = g.get()
    print(str(j))


def test_account_info_getters(creds, account_id):
    g = get.AccountInfoGetter(creds, account_id, True, False)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == True
    assert g.returns_orders() == False
    j = g.get()
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    g.set_account_id(account_id)
    g.return_positions(False)
    g.return_orders(True)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == False
    assert g.returns_orders() == True
    j = g.get()
    print(str(j))
    
    
def test_preferences_getter(creds, account_id):
    g = get.PreferencesGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = g.get()
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
         
     
def test_subscription_keys_getter(creds, account_id):
    g = get.StreamerSubscriptionKeysGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = g.get()
    print(str(j))    
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    
    
def test_transaction_history_getters(creds, account_id):
    g = get.TransactionHistoryGetter(creds, account_id,
                                     get.TRANSACTION_TYPE_TRADE,
                                     "SPY" ,"2018-01-01", "2019-01-01")
    assert g.get_account_id() == account_id
    assert g.get_transaction_type() == get.TRANSACTION_TYPE_TRADE
    assert g.get_symbol() == "SPY"
    assert g.get_start_date() == "2018-01-01"
    assert g.get_end_date() == "2019-01-01"
    j = g.get()
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
    j = g.get()
    print(str(j))      
  

def test_individual_transaction_history_getters(creds, account_id):
    g = get.IndividualTransactionHistoryGetter(creds, account_id, "012345678")
    assert g.get_account_id() == account_id
    assert g.get_transaction_id() == "012345678"
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
    j = g.get()
    print(str(j))     
         
    g.return_subscription_keys(True)
    g.return_connection_info(True)
    g.return_preferences(True)
    g.return_surrogate_ids(True)
    assert g.returns_subscription_keys() == True
    assert g.returns_connection_info() == True
    assert g.returns_preferences() == True
    assert g.returns_surrogate_ids() == True
    j = g.get()
    print(str(j)) 
     

def test_instrument_info_getters(creds):
    g = get.InstrumentInfoGetter(creds, get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX,
                                 "GOOGL?")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX
    assert g.get_query_string() == "GOOGL?"
    j = g.get()
    print(str(j)) 
    
    g.set_query(get.INSTRUMENT_SEARCH_TYPE_CUSIP, "78462F103")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_CUSIP
    assert g.get_query_string() == "78462F103"
    j = g.get()
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
                
  
    symbols = ('SPY', 'QQQ')    
    fields = (QS.FIELD_SYMBOL, QS.FIELD_BID_PRICE, QS.FIELD_ASK_PRICE)                
    qs = QS(symbols, fields)
    assert qs.get_command() == "SUBS"
    assert qs.get_service() == stream.SERVICE_TYPE_QUOTE
    assert set(qs.get_symbols()) == set(symbols)
    assert set(qs.get_fields()) == set(fields)
    
    OS = stream.OptionsSubscription
    symbols = ["SPY_081718P286"]
    fields = [OS.FIELD_DELTA, OS.FIELD_GAMMA, OS.FIELD_VEGA, OS.FIELD_VOLATILITY]
    os = OS(symbols, fields)
    assert os.get_service() == stream.SERVICE_TYPE_OPTION
    assert set(os.get_symbols()) == set(symbols)
    assert set(os.get_fields()) == set(fields)    
    
    LOFS = stream.LevelOneFuturesSubscription
    symbols = ('/ES', '/GC')
    fields = (LOFS.FIELD_ASK_SIZE, LOFS.FIELD_BID_SIZE, LOFS.FIELD_FUTURE_EXPIRATION_DATE)
    lofs = LOFS(symbols, fields)
    assert lofs.get_service() == stream.SERVICE_TYPE_LEVELONE_FUTURES
    assert set(lofs.get_symbols()) == set(symbols)
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
                
if __name__ == '__main__':
    print("\n*** TDAmeritradeAPI test.py ***")
    print("+", strftime("%m/%d/%Y %H:%M:%S"))
    print("+", SYSTEM)
    print("+", ARCH)
    print("+ PID:", os.getpid())
    args = parser.parse_args()
    test(init)
    print_title("load credentials") 
    with auth.CredentialsManager(args.credentials_path, \
                                  args.credentials_password, True) as cm:    
        test(test_option_symbol_builder)        
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
        test(test_streaming, cm.credentials)
        
