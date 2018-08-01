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
from time import strftime, perf_counter
import argparse

from tdma_api import get, auth, clib

SYSTEM = system()
ARCH = architecture()[0]
LIBRARY_PATH = "../Release/libTDAmeritradeAPI.so"
if SYSTEM == 'Windows':
    if '64' in ARCH:
        LIBRARY_PATH = "../vsbuild/x64/Release/TDAmeritradeAPI.dll"
    else: 
        LIBRARY_PATH = "../vsbuild/Win32/Release/TDAmeritradeAPI.dll"
        
parser = argparse.ArgumentParser("test tdma_api")
parser.add_argument("account_id", type=str, help="account id" )
parser.add_argument("credentials_path", type=str, 
                    help="path of encrypted credentials file")
parser.add_argument("credentials_password", type=str,
                    help="password to decrypt credentials files")
args = parser.parse_args()

            
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
        if not clib.init(LIBRARY_PATH):        
            raise clib.LibraryNotLoaded()   
      
      
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
    ntests = 3
    start = perf_counter()
    for _ in range(ntests):
        g.get()  
        print(" ...") 
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
      
        
if __name__ == '__main__':
    print("\n*** TDAmeritradeAPI test.py ***")
    print("+", strftime("%m/%d/%Y %H:%M:%S"))
    print("+", SYSTEM)
    print("+", ARCH)
    test(init)
    print_title("load credentials") 
    with auth.CredentialsManager(args.credentials_path, \
                                  args.credentials_password, True) as cm:    
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
        
