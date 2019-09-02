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
from time import strftime, perf_counter, sleep, gmtime, mktime
import argparse, gc, os, json

from tdma_api import get, auth, clib, stream, execute, common

SYSTEM = system()
ARCH = architecture()[0]

CONFIG_DIR = "Release/"
#CONFIG_DIR = "Debug/"
REL_LIB_PATH = os.path.join("../../", CONFIG_DIR, "libTDAmeritradeAPI.so")
if SYSTEM == 'Windows':
    if '64' in ARCH:
        REL_LIB_PATH = os.path.join("../../vsbuild/x64/", CONFIG_DIR,
                                    "TDAmeritradeAPI.dll")
    else:
        REL_LIB_PATH = os.path.join("../../vsbuild/Win32/", CONFIG_DIR,
                                    "TDAmeritradeAPI.dll")

TEST_DIR = os.path.dirname(os.path.realpath(__file__))
LIBRARY_PATH = os.path.join(TEST_DIR, REL_LIB_PATH)
PACKAGE_BUILD_INFO_PATH = os.path.join(TEST_DIR, '../../python/tdma_api_build.info')

parser = argparse.ArgumentParser("test tdma_api")
parser.add_argument("account_id", type=str, help="account id" )
parser.add_argument("credentials_path", type=str,
                    help="path of encrypted credentials file")
parser.add_argument("credentials_password", type=str,
                    help="password to decrypt credentials files")
parser.add_argument("--no-live-connect", action='store_true')

use_live_connection = True

jprint = lambda j: print( json.dumps(j, indent=4) )

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
        else:
            print("+ Successfully loaded: ", LIBRARY_PATH)
            print("+ Last Build:", clib.lib_build_datetime())

def test_share_connections():
    assert get.is_sharing_connections() 
    get.share_connections(False)
    assert not get.is_sharing_connections()
    get.share_connections(True)
    assert get.is_sharing_connections()

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
        except clib.CLibException as e:
            print("successfully caught exception building option:", str(e))
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

    def is_bad_sym(s):
        try:
            common.check_option_symbol(s)
            return False
        except clib.CLibException as e:
            print("successfully caught exception checking option:", str(e))
            return True

    assert is_bad_sym("010118C300")
    assert is_bad_sym("SpY__010118C300")
    assert is_bad_sym("_SPY010118C300")
    assert is_bad_sym("SP_Y010118C300")
    assert is_bad_sym("SPY_00118C300")
    assert is_bad_sym("SPY_000118C300")
    assert is_bad_sym("SPY_010018C300")
    assert is_bad_sym("SPY_01018C300")
    assert is_bad_sym("SPY_01010C300")
    assert is_bad_sym("SPY_01010C300")
    assert is_bad_sym("SPY_010118300")
    assert is_bad_sym("SPY_010118P")
    assert is_bad_sym("SPY_010118P-300")
    assert is_bad_sym("SPY_010118P300.")
    assert is_bad_sym("SPY_010118P300x")


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
    jprint(j)
    g.set_symbol('qqq')
    assert g.get_symbol() == 'QQQ'
    j = Get(g)
    jprint(j)


def test_quotes_getters(creds):
    g = get.QuotesGetter(creds, "SPY", "qqq", "IwM")
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM"])
    j = Get(g)
    jprint(j)
    g.set_symbols('QqQ')
    assert g.get_symbols() == ['QQQ']
    j = Get(g)
    jprint(j)
    g.add_symbols('SPy')
    g.add_symbols('iwm','GLD')
    assert sorted(g.get_symbols()) == sorted(["SPY", "QQQ", "IWM", "GLD"])
    g.remove_symbols('sPY','IWM')
    assert g.get_symbols() == sorted(['QQQ','GLD'])
    j = Get(g)
    jprint(j)
    g.remove_symbols('QQQ','gld')
    assert g.get_symbols() == []
    assert not Get(g)


def test_market_hours_getters(creds):
    dstr = str(gmtime().tm_year + 1)
    g = get.MarketHoursGetter(creds, get.MARKET_TYPE_BOND, dstr + "-07-04")
    assert g.get_market_type() == get.MARKET_TYPE_BOND
    assert g.get_date() == (dstr + "-07-04")
    j = Get(g)
    jprint(j)
    g.set_market_type(get.MARKET_TYPE_EQUITY)
    g.set_date(dstr + "-07-05")
    assert g.get_market_type() == get.MARKET_TYPE_EQUITY
    assert g.get_date() == (dstr + "-07-05")
    j = Get(g)
    jprint(j)


def test_movers_getters(creds):
    g = get.MoversGetter(creds, get.MOVERS_INDEX_COMPX,
                         get.MOVERS_DIRECTION_TYPE_UP,
                         get.MOVERS_CHANGE_TYPE_PERCENT)
    assert g.get_index() == get.MOVERS_INDEX_COMPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_PERCENT
    j = Get(g)
    jprint(j)
    g.set_index(get.MOVERS_INDEX_SPX)
    g.set_direction_type(get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN)
    g.set_change_type(get.MOVERS_CHANGE_TYPE_VALUE)
    assert g.get_index() == get.MOVERS_INDEX_SPX
    assert g.get_direction_type() == get.MOVERS_DIRECTION_TYPE_UP_AND_DOWN
    assert g.get_change_type() == get.MOVERS_CHANGE_TYPE_VALUE
    j = Get(g)
    jprint(j)


def test_historical_period_getters(creds):
    pt = get.PERIOD_TYPE_DAY
    p = get.VALID_PERIODS_BY_PERIOD_TYPE[pt][0]
    ft = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][0]
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][1]
    # 1-day / 5-min
    g = get.HistoricalPeriodGetter(creds, "SPY", pt, p, ft, f, True)
    assert g.get_symbol() == "SPY"
    assert g.get_period() == p
    assert g.get_period_type() == pt
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.is_extended_hours() == True
    assert g.get_msec_since_epoch() is None
    j = Get(g) #### DEBUG
    jprint(j)

    today = int(mktime(gmtime())*1000)
    tomorrow = today + (60*60*24*1000)
    days_ago_70 = today - (60*60*24*1000)*70

    pt = get.PERIOD_TYPE_YEAR
    p = get.VALID_PERIODS_BY_PERIOD_TYPE[p][0] # 1
    ft = get.VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][2] # monthly
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][0] # 1
    #1-year / 1-month
    g.set_symbol("qqq")
    g.set_period(pt, p)
    g.set_frequency(ft, f)
    g.set_extended_hours(False)
    #include today by adding 'end' date
    g.set_msec_since_epoch(tomorrow)
    assert g.get_symbol() == "QQQ"
    assert g.get_period() == p
    assert g.get_period_type() == pt
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.is_extended_hours() == False
    assert g.get_msec_since_epoch() == tomorrow
    j = Get(g)
    jprint(j)

    g.set_msec_since_epoch(None)
    assert g.get_msec_since_epoch() is None
    g.set_msec_since_epoch(days_ago_70 * -1)
    assert g.get_msec_since_epoch() == (days_ago_70 * -1)
    j = Get(g)
    jprint(j)

    gg = get.HistoricalPeriodGetter(creds, "SPY", pt, p, ft, f, True, days_ago_70)
    assert gg.get_symbol() == "SPY"
    assert gg.get_period() == p
    assert gg.get_period_type() == pt
    assert gg.get_frequency() == f
    assert gg.get_frequency_type() == ft
    assert gg.is_extended_hours() == True
    assert gg.get_msec_since_epoch() == days_ago_70

def test_historical_range_getters(creds):
    ft = get.FREQUENCY_TYPE_MINUTE
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][-1]

    today = int(mktime(gmtime())*1000)
    end = today
    start = today - (60*60*24*1000)*10
    days_ago_70 = today - (60*60*24*1000)*70

    g = get.HistoricalRangeGetter(creds, "SpY", ft, f, start, end, False)
    assert g.get_symbol() == "SPY"
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == start
    assert g.get_end_msec_since_epoch() == end
    assert g.is_extended_hours() == False
    j = Get(g)
    jprint(j)

    ft = get.FREQUENCY_TYPE_DAILY
    f = get.VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][0]
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
    jprint(j)

    end = today +  (60*60*24*1000)*10
    g.set_end_msec_since_epoch(end)
    g.set_start_msec_since_epoch(days_ago_70)
    ft = get.FREQUENCY_TYPE_MONTHLY
    f = 1
    g.set_frequency(ft, f)
    assert g.get_frequency() == f
    assert g.get_frequency_type() == ft
    assert g.get_start_msec_since_epoch() == days_ago_70
    assert g.get_end_msec_since_epoch() == end
    j = Get(g)
    jprint(j)
    if use_live_connection:
        assert j["candles"]
        #assert len(j["candles"]) == 2


# TODO - dynamically build/get chain dates
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
    jprint(j)

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
    jprint(j)


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
    jprint(j)

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
    jprint(j)


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
    jprint(j)

    strikes = get.OptionStrikes.RANGE(get.OPTION_RANGE_TYPE_ITM)
    from_date = "2019-02-01"
    to_date = "2019-03-01"
    g.set_symbol("SpY")
    g.set_volatility(100.10)
    g.set_underlying_price(49.9999)
    g.set_interest_rate(15.001)
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
    assert g.get_underlying_price() == 49.9999
    assert g.get_interest_rate() == 15.001
    assert g.get_days_to_exp() == 1000
    assert g.get_strikes() == strikes
    assert g.get_contract_type() == get.OPTION_CONTRACT_TYPE_ALL
    assert g.includes_quotes() == False
    assert g.get_from_date() == from_date
    assert g.get_to_date() == to_date
    assert g.get_exp_month() == get.OPTION_EXP_MONTH_ALL
    assert g.get_option_type() == get.OPTION_TYPE_S
    j = Get(g)
    jprint(j)


def test_account_info_getters(creds, account_id):
    g = get.AccountInfoGetter(creds, account_id, True, False)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == True
    assert g.returns_orders() == False
    j = Get(g)
    jprint(j)
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    g.set_account_id(account_id)
    g.return_positions(False)
    g.return_orders(True)
    assert g.get_account_id() == account_id
    assert g.returns_positions() == False
    assert g.returns_orders() == True
    j = Get(g)
    jprint(j)


def test_preferences_getter(creds, account_id):
    g = get.PreferencesGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = Get(g)
    jprint(j)
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"


def test_subscription_keys_getter(creds, account_id):
    g = get.StreamerSubscriptionKeysGetter(creds, account_id)
    assert g.get_account_id() == account_id
    j = Get(g)
    jprint(j)
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"


def test_transaction_history_getters(creds, account_id):
    y = gmtime().tm_year
    start = str(y - 1) + "-01-01"
    end = str(y) + "-01-01"
    g = get.TransactionHistoryGetter(creds, account_id,
                                     get.TRANSACTION_TYPE_TRADE,
                                     "spy" ,start, end)
    assert g.get_account_id() == account_id
    assert g.get_transaction_type() == get.TRANSACTION_TYPE_TRADE
    assert g.get_symbol() == "SPY"
    assert g.get_start_date() == start
    assert g.get_end_date() == end
    j = Get(g)
    jprint(j)

    start = str(y - 1) + "-02-02"
    end = str(y) + "-02-02"
    g.set_account_id("BAD_ACCOUNT_ID")
    assert g.get_account_id() == "BAD_ACCOUNT_ID"
    g.set_account_id(account_id)
    g.set_transaction_type(get.TRANSACTION_TYPE_ALL)
    g.set_symbol("")
    g.set_start_date(start)
    g.set_end_date(end)
    assert g.get_account_id() == account_id
    assert g.get_transaction_type() == get.TRANSACTION_TYPE_ALL
    assert g.get_symbol() == ""
    assert g.get_start_date() == start
    assert g.get_end_date() == end
    j = Get(g)
    jprint(j)


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
    jprint(j)

    g.return_subscription_keys(True)
    g.return_connection_info(True)
    g.return_preferences(True)
    g.return_surrogate_ids(True)
    assert g.returns_subscription_keys() == True
    assert g.returns_connection_info() == True
    assert g.returns_preferences() == True
    assert g.returns_surrogate_ids() == True
    j = Get(g)
    jprint(j)


def test_instrument_info_getters(creds):
    g = get.InstrumentInfoGetter(creds, get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX,
                                 "GOOGL*")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_SYMBOL_REGEX
    assert g.get_query_string() == "GOOGL*"
    j = Get(g)
    jprint(j)

    g.set_query(get.INSTRUMENT_SEARCH_TYPE_CUSIP, "78462F103")
    assert g.get_search_type() == get.INSTRUMENT_SEARCH_TYPE_CUSIP
    assert g.get_query_string() == "78462F103"
    j = Get(g)
    jprint(j)


def test_order_getters(creds, account_id):

    def build_iso8601_date(offset_days):
        gmt_to_est = -4
        sse_now = mktime(gmtime())
        sse_then = sse_now + gmt_to_est - (24 * 60 * 60 * offset_days)
        t = gmtime(sse_then)
        return "-".join( [str(t.tm_year), str(t.tm_mon).zfill(2),
                          str(t.tm_mday).zfill(2)] )

    dt_to = build_iso8601_date(0)
    dt_from = build_iso8601_date(59)

    ogall = get.OrdersGetter(creds, account_id, 10, dt_from, dt_to)
    assert ogall.get_order_status_type() == get.ORDER_STATUS_TYPE_ALL

    og = get.OrdersGetter(creds, account_id, 10, dt_from, dt_to,
                          get.ORDER_STATUS_TYPE_CANCELED)
    assert og.get_account_id() == account_id
    assert og.get_nmax_results() == 10
    assert og.get_from_entered_time() == dt_from
    assert og.get_to_entered_time() == dt_to
    assert og.get_order_status_type() == get.ORDER_STATUS_TYPE_CANCELED

    orders = []
    tmp = Get(og)
    print("Orders(Canceled):", str(tmp))
    if tmp:
        orders.extend(tmp)

    og.set_order_status_type(get.ORDER_STATUS_TYPE_FILLED)
    assert og.get_order_status_type() == get.ORDER_STATUS_TYPE_FILLED

    tmp = Get(og)
    print("Orders(Filled):", str(tmp))
    if tmp:
        orders.extend(tmp)

    og.set_order_status_type(get.ORDER_STATUS_TYPE_ALL)
    assert og.get_order_status_type() == get.ORDER_STATUS_TYPE_ALL

    tmp = Get(og)
    print("Orders(ALL):", str(tmp))

    dt_to = build_iso8601_date(1)
    dt_from = build_iso8601_date(10)
    og.set_nmax_results(1)
    og.set_from_entered_time(dt_from)
    og.set_to_entered_time(dt_to)
    og.set_order_status_type(get.ORDER_STATUS_TYPE_REJECTED)
    assert og.get_account_id() == account_id
    assert og.get_nmax_results() == 1
    assert og.get_from_entered_time() == dt_from
    assert og.get_to_entered_time() == dt_to
    assert og.get_order_status_type() == get.ORDER_STATUS_TYPE_REJECTED

    tmp = Get(og)
    print("Orders(Rejected):", str(tmp))
    if tmp:
        orders.extend(tmp)

    try:
        og.set_account_id("")
        raise Exception("failed to catch exception(1)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))

    try:
        og.set_nmax_results(0)
        raise Exception("failed to catch exception(2)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))

    try:
        og.set_from_entered_time("")
        raise Exception("failed to catch exception(3)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))

    try:
        og.set_from_entered_time("200-01-01")
        raise Exception("failed to catch exception(4)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))

    try:
        og.set_to_entered_time("")
        raise Exception("failed to catch exception(5)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))

    try:
        og.set_to_entered_time(dt_to[:-2] + '1')
        raise Exception("failed to catch exception(6)")
    except clib.CLibException as e:
        print("+ successfully caught exception: ", str(e))


    if not orders:
        print( "NO CANCELED/FILLED/REJECTED ORDERS TO USE"
                " - SKIP OrderGetter (non-exception) tests" )

        if use_live_connection:
            try:
                o = get.OrderGetter(creds, account_id, "000000000")
                o.get()
                raise Exception("failed to catch exception (7)")
            except clib.CLibException as e:
                print("+ successfully caught exception: ", str(e))
        else:
            print("GET requires 'use_live_connection=True'")

    else:
        id1 = str(orders[0]['orderId'])
        o = get.OrderGetter(creds, account_id, id1)
        assert o.get_account_id() == account_id
        assert o.get_order_id() == id1

        oinfo1 = Get(o)
        print("Order(%s):" % id1, str(oinfo1) )

        if len(orders) > 1:
            id2 = str(orders[1]['orderId'])
            o.set_order_id(id2)
            assert o.get_account_id() == account_id
            assert o.get_order_id() == id2

            oinfo2 = Get(o)
            print("Order(%s):" % id2, str(oinfo2) )

        try:
            o.set_account_id("")
            raise Exception("failed to catch exception(8)")
        except clib.CLibException as e:
            print("+ successfully caught exception: ", str(e))

        try:
            o.set_order_id("")
            raise Exception("failed to catch exception(9)")
        except clib.CLibException as e:
            print("+ successfully caught exception: ", str(e))

        if use_live_connection:
            o.set_order_id("000000000")
            try:
                o.get()
                raise Exception("failed to catch exception (10)")
            except clib.CLibException as e:
                print("+ successfully caught exception: ", str(e))
        else:
            print("GET requires 'use_live_connection=True'")


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
    assert qs.get_command() == stream.COMMAND_TYPE_SUBS
    assert qs.get_service() == stream.SERVICE_TYPE_QUOTE
    assert set(qs.get_symbols()) == {'SPY','QQQ'}
    assert set(qs.get_fields()) == set(fields)
    
    # ADD     
    qs2 = QS(symbols, fields)
    qs2.set_command(stream.COMMAND_TYPE_ADD)
    symbols = ("IWM",)   
    qs2.set_symbols(symbols)
    assert qs2.get_command() == stream.COMMAND_TYPE_ADD
    assert qs2.get_service() == stream.SERVICE_TYPE_QUOTE
    assert set(qs2.get_symbols()) == {'IWM'}
    assert set(qs2.get_fields()) == set(fields)
    
    # VIEW
    qs3 = QS(symbols, fields)
    fields = (QS.FIELD_LAST_PRICE,)
    qs3.set_fields(fields)
    qs3.set_command(stream.COMMAND_TYPE_VIEW)
    assert qs3.get_command() == stream.COMMAND_TYPE_VIEW
    assert qs3.get_service() == stream.SERVICE_TYPE_QUOTE
    assert set(qs3.get_symbols()) == {'IWM'}
    assert set(qs3.get_fields()) == set(fields) 
    
    # UNSUBS
    qs4 = QS( ("SPY",), [], stream.COMMAND_TYPE_UNSUBS)    
    assert qs4.get_command() == stream.COMMAND_TYPE_UNSUBS
    assert qs4.get_service() == stream.SERVICE_TYPE_QUOTE   

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
    
    #VIEW
    cos2 = COS(symbols, fields, stream.COMMAND_TYPE_UNSUBS)
    assert cos2.get_command() == stream.COMMAND_TYPE_UNSUBS
    cos2.set_command(stream.COMMAND_TYPE_VIEW)
    cos2.set_fields( (COS.FIELD_SYMBOL, COS.FIELD_VOLUME) )
    assert cos2.get_service() == stream.SERVICE_TYPE_CHART_OPTIONS
    assert set(cos2.get_symbols()) == set(symbols)
    assert set(cos2.get_fields()) == {COS.FIELD_SYMBOL, COS.FIELD_VOLUME}

    TES = stream.TimesaleEquitySubscription
    symbols = ("EEM",)
    fields = (TES.FIELD_SYMBOL, TES.FIELD_LAST_PRICE)
    tes = TES(symbols,fields)
    assert tes.get_service() == stream.SERVICE_TYPE_TIMESALE_EQUITY
    assert set(tes.get_symbols()) == set(symbols)
    assert set(tes.get_fields()) == set(fields)
    
    #UNSUBS
    tes2 = TES(symbols, fields, stream.COMMAND_TYPE_UNSUBS)
    tes2.set_fields( [] )
    assert tes2.get_command() == stream.COMMAND_TYPE_UNSUBS
    assert set(tes2.get_symbols()) == set(symbols)
    assert set(tes2.get_fields()) == set()
    tes2.set_fields( fields )
    assert set(tes2.get_fields()) == set(fields)
    

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
    assert nas.get_command() == stream.COMMAND_TYPE_SUBS
    assert nas.get_duration() == NAS.DURATION_TYPE_ALL_DAY

    NYAS = stream.NYSEActivesSubscription
    nyas = NYAS(NAS.DURATION_TYPE_MIN_1)
    assert nyas.get_service() == stream.SERVICE_TYPE_ACTIVES_NYSE
    assert nyas.get_command() == stream.COMMAND_TYPE_SUBS
    assert nyas.get_duration() == NYAS.DURATION_TYPE_MIN_1

    OCAS = stream.OTCBBActivesSubscription
    ocas = OCAS(OCAS.DURATION_TYPE_MIN_60)
    assert ocas.get_service() == stream.SERVICE_TYPE_ACTIVES_OTCBB
    assert ocas.get_command() == stream.COMMAND_TYPE_SUBS
    assert ocas.get_duration() == OCAS.DURATION_TYPE_MIN_60
    
    #ADD
    ocas2 = OCAS(OCAS.DURATION_TYPE_ALL_DAY, stream.COMMAND_TYPE_ADD)
    ocas2.set_command(stream.COMMAND_TYPE_ADD)
    ocas2.set_duration(OCAS.DURATION_TYPE_MIN_1)
    assert ocas2.get_service() == stream.SERVICE_TYPE_ACTIVES_OTCBB
    assert ocas2.get_command() == stream.COMMAND_TYPE_ADD
    assert ocas2.get_duration() == OCAS.DURATION_TYPE_MIN_1
    
    OPAS = stream.OptionActivesSubscription
    opas = OPAS(OPAS.VENUE_TYPE_PUTS_DESC, OPAS.DURATION_TYPE_MIN_30)
    assert opas.get_service() == stream.SERVICE_TYPE_ACTIVES_OPTIONS
    assert opas.get_command() == stream.COMMAND_TYPE_SUBS
    assert opas.get_duration() == OPAS.DURATION_TYPE_MIN_30
    
    #UNSUBS
    opas2 = OPAS(OPAS.VENUE_TYPE_PUTS_DESC, OPAS.DURATION_TYPE_MIN_30,
                 stream.COMMAND_TYPE_UNSUBS)
    opas2.set_venue(OPAS.VENUE_TYPE_CALLS)
    opas2.set_duration(OPAS.DURATION_TYPE_MIN_60)
    assert opas2.get_service() == stream.SERVICE_TYPE_ACTIVES_OPTIONS
    assert opas2.get_command() == stream.COMMAND_TYPE_UNSUBS
    assert opas2.get_duration() == OPAS.DURATION_TYPE_MIN_60
    assert opas2.get_venue() == OPAS.VENUE_TYPE_CALLS   
    
    #RAW
    rparams = { "keys":"BAD", "fields":"BAD" }
    nbookbad = stream.RawSubscription("BAD1", "BAD2", rparams)
    nbookgood = nbookbad.deep_copy()
    assert nbookbad == nbookgood
    assert nbookbad is not nbookgood
    assert nbookbad.get_service_str() == "BAD1"
    assert nbookgood.get_service_str() == "BAD1"
    assert nbookbad.get_command_str() == "BAD2"
    assert nbookgood.get_command_str() == "BAD2"    
    assert nbookbad.get_parameters()["keys"] == rparams["keys"]
    assert nbookbad.get_parameters()["fields"] == rparams["fields"]
    assert nbookgood.get_parameters()["keys"] == rparams["keys"]
    assert nbookgood.get_parameters()["fields"] == rparams["fields"]
    nbookgood.set_service_str("NASDAQ_BOOK")
    nbookgood.set_command_str("SUBS")
    rparams = { "keys":"GOOG,APPL", "fields":"0,1,2" }
    nbookgood.set_parameters( rparams )
    assert nbookbad != nbookgood   
    del nbookbad
    gc.collect()
    assert nbookgood.get_service_str() == "NASDAQ_BOOK"
    assert nbookgood.get_command_str() == "SUBS"   
    assert nbookgood.get_parameters()["keys"] == rparams["keys"]
    assert nbookgood.get_parameters()["fields"] == rparams["fields"]   

    if not use_live_connection:
        print("STREAMING SESSION test requires 'use_live_connection=True'")
        return

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
    
    assert session.add_subscriptions(qs2)
    _pause(10)
    
    assert session.add_subscriptions(qs3)
    _pause(10)
    
    assert session.add_subscriptions(qs4)
    _pause(10)

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

    assert session2.add_subscriptions(cos2)
    _pause(10)
    
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
    _pause(5)

    assert all(session2.add_subscriptions(ocas2, opas2, nbookgood))
    _pause(5)
    
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

    print("+ Leg Deep Copy")
    leg2c = leg2.deep_copy()

    assert leg2c == leg2
    assert leg2c is not leg2
    assert leg2c._obj is not leg2._obj
    assert leg2c._obj.obj != leg2._obj.obj
    assert leg2c._obj.type_id == leg2._obj.type_id

    leg2d = leg2c.deep_copy()

    assert leg2c == leg2d
    assert leg2c is not leg2d
    assert leg2c._obj is not leg2d._obj
    assert leg2c._obj.obj != leg2d._obj.obj
    assert leg2c._obj.type_id == leg2d._obj.type_id

    assert leg2.as_json() == leg2c.as_json() == leg2d.as_json()
    assert leg2._alive and leg2d._alive and leg2c._alive

    leg2 = leg2d
    leg2c = None
    leg2d = None

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

    print("+ Order Deep Copy")
    order2c = order2.deep_copy()

    assert order2 == order2c
    assert order2 is not order2c
    assert order2._obj is not order2c._obj
    assert order2._obj.obj != order2c._obj.obj
    assert order2._obj.type_id == order2c._obj.type_id
    assert order2.as_json() == order2c.as_json()
    assert order2._alive and order2c._alive

    order2 = order2c
    order2c = None

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

    test_exc(1, B.Equity.Build, "", 1, 1,1)
    test_exc(2, B.Equity.Build, "SPY", 0, 0,0 )
    test_exc(3, B.Equity.Build, "SPY", 1, 0,1, 0.0)
    test_exc(4, B.Equity.Build, "SPY", 1, 1,0, -1)
    test_exc(5, B.Equity.Stop.Build, "SPY", 1, 1,1, 0.0, 1.00)
    test_exc(6, B.Equity.Stop.Build, "SPY", 1, 0,0, -1.0, 0.00)
    test_exc(7, B.Equity.Stop.Build, "SPY", 1, 0,1, 1.0, -1.00)

    def _build(order_type, asset_type, symbol, instruction, quantity,
              limit_price=0.0, stop_price=0.0):
        return B._abi_build('BuildOrder_Simple_ABI',
             execute.c_int(asset_type), execute.PCHAR(symbol),
             execute.c_size_t(quantity), execute.c_int(instruction),
             execute.c_int(order_type),  execute.c_double(limit_price),
             execute.c_double(stop_price))

    # MARKET
    mb = B.Equity.Build('SPY', 100,1 ,1)
    print( mb.as_json() )
    check_order(mb, MARKET, 'SPY', BUY, 100)
    mb2 = _build(MARKET, EQUITY, 'SPY', BUY, 100)
    assert mb2 == mb
    assert mb2.as_json() == mb.as_json()

    ms = B.Equity.Build('SPY', 200,0,0)
    print( ms.as_json() )
    check_order(ms, MARKET,'SPY', SELL, 200)
    ms2 = _build(MARKET, EQUITY, 'SPY', SELL, 200)
    assert ms2 == ms
    assert ms2.as_json() == ms.as_json()

    mss = B.Equity.Build('QQQ', 99,0,1)
    print( mss.as_json() )
    check_order(mss, MARKET,'QQQ', SHORT, 99)
    mss2 = _build(MARKET, EQUITY, 'QQQ', SHORT, 99)
    assert mss2 == mss
    assert mss2.as_json() == mss.as_json()

    mbc = B.Equity.Build('QQQ', 1,1,0)
    print( mbc.as_json() )
    check_order(mbc, MARKET,'QQQ', COVER, 1)
    mbc2 = _build(MARKET, EQUITY, 'QQQ', COVER, 1)
    assert mbc2 == mbc
    assert mbc2.as_json() == mbc.as_json()

    # LIMIT
    lb = B.Equity.Build('SPY', 100, 1, 1, 300.01)
    print( lb.as_json() )
    check_order(lb, LIMIT, 'SPY', BUY, 100, 300.01)
    lb2 = _build(LIMIT, EQUITY, 'SPY', BUY, 100, 300.01)
    assert lb2 == lb
    assert lb2.as_json() == lb.as_json()

    ls = B.Equity.Build('SPY', 200, 0, 0, 299.9999)
    print( ls.as_json() )
    check_order(ls, LIMIT,'SPY', SELL, 200, 299.9999)
    ls2 = _build(LIMIT, EQUITY, 'SPY', SELL, 200, 299.9999)
    assert ls2 == ls
    assert ls2.as_json() == ls.as_json()

    lss = B.Equity.Build('QQQ', 99, 0, 1, 300)
    print( lss.as_json() )
    check_order(lss, LIMIT,'QQQ', SHORT, 99, 300)
    lss2 = _build(LIMIT, EQUITY, 'QQQ', SHORT, 99, 300)
    assert lss2 == lss
    assert lss2.as_json() == lss.as_json()

    lbc = B.Equity.Build('QQQ', 1, 1, 0, .01)
    print( lbc.as_json() )
    check_order(lbc, LIMIT,'QQQ', COVER, 1, .01)
    lbc2 = _build(LIMIT, EQUITY, 'QQQ', COVER, 1, .01)
    assert lbc2 == lbc
    assert lbc2.as_json() == lbc.as_json()

    # STOP
    sb = B.Equity.Stop.Build('SPY', 100,  1, 1, 300.01)
    print( sb.as_json() )
    check_order(sb, STOP, 'SPY', BUY, 100, 0, 300.01)
    sb2 = _build(STOP, EQUITY, 'SPY', BUY, 100, 0, 300.01)
    assert sb2 == sb
    assert sb2.as_json() == sb.as_json()

    ss = B.Equity.Stop.Build('SPY', 200, 0, 0, 299.9999)
    print( ss.as_json() )
    check_order(ss, STOP,'SPY', SELL, 200, 0, 299.9999)
    ss2 = _build(STOP, EQUITY, 'SPY', SELL, 200, 0, 299.9999)
    assert ss2 == ss
    assert ss2.as_json() == ss.as_json()

    sss = B.Equity.Stop.Build('QQQ', 99, 0, 1, 300)
    print( sss.as_json() )
    check_order(sss, STOP,'QQQ', SHORT, 99, 0, 300)
    sss2 = _build(STOP, EQUITY, 'QQQ', SHORT, 99, 0, 300)
    assert sss2 == sss
    assert sss2.as_json() == sss.as_json()

    sbc = B.Equity.Stop.Build('QQQ', 1, 1, 0, .01)
    print( sbc.as_json() )
    check_order(sbc, STOP,'QQQ', COVER, 1, 0, .01)
    sbc2 = _build(STOP, EQUITY, 'QQQ', COVER, 1, 0, .01)
    assert sbc2 == sbc
    assert sbc2.as_json() == sbc.as_json()

    # STOP_LIMIT
    slb = B.Equity.Stop.Build('SPY', 100, 1, 1, 300.01, 300.09)
    print( slb.as_json() )
    check_order(slb, STOP_LIMIT, 'SPY', BUY, 100, 300.09, 300.01)
    slb2 = _build(STOP_LIMIT, EQUITY, 'SPY', BUY, 100, 300.09, 300.01)
    assert slb2 == slb
    assert slb2.as_json() == slb.as_json()

    sls = B.Equity.Stop.Build('SPY', 200, 0,0, 299.9999, 299.8888)
    print( sls.as_json() )
    check_order(sls, STOP_LIMIT,'SPY', SELL, 200, 299.8888, 299.9999)
    sls2 = _build(STOP_LIMIT, EQUITY, 'SPY', SELL, 200, 299.8888, 299.9999)
    assert sls2 == sls
    assert sls2.as_json() == sls.as_json()

    slss = B.Equity.Stop.Build('QQQ', 99, 0, 1, 300, 299)
    print( slss.as_json() )
    check_order(slss, STOP_LIMIT,'QQQ', SHORT, 99, 299, 300)
    slss2 = _build(STOP_LIMIT, EQUITY, 'QQQ', SHORT, 99, 299, 300)
    assert slss2 == slss
    assert slss2.as_json() == slss.as_json()

    slbc = B.Equity.Stop.Build('QQQ', 1, 1, 0, .01, .0001)
    print( slbc.as_json() )
    check_order(slbc, STOP_LIMIT,'QQQ', COVER, 1, .0001, .01)
    slbc2 = _build(STOP_LIMIT, EQUITY, 'QQQ', COVER, 1, .0001, .01)
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

    test_exc(1, B.Option.Build1, "", 1, 1,1)
    test_exc(2, B.Option.Build1, "SPY_011720C300", 0, 0,1)
    test_exc(3, B.Option.Build2, "", 1, 17, 2020, True, 300, 1, 1, 1)
    test_exc(4, B.Option.Build2, "SPY", 0, 17, 2020, True, 300, 1, 0,1)
    test_exc(5, B.Option.Build2, "SPY", 1, 0, 2020, True, 300, 1, 1, 0)
    test_exc(6, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 1, 0, 0)
    test_exc(7, B.Option.Build2, "SPY", 1, 17, 0, True, 0.0, 1, 1, 1)
    test_exc(8, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 0, 0, 1)
    test_exc(9, B.Option.Build1, "", 1, 1, 1, 100.00)
    test_exc(10, B.Option.Build1, "SPY_011720C300", 0, 0, 1, 100)
    test_exc(11, B.Option.Build1, "SPY_011720C300", 1, 1, 0, 0.0)
    test_exc(12, B.Option.Build1, "SPY_011720C300", 1, 0,0, -1)
    test_exc(13, B.Option.Build2, "", 1, 17, 2020, True, 300, 1, 1, 1, 100.0)
    test_exc(14, B.Option.Build2, "SPY", 0, 17, 2020, True, 300, 1, 0, 1, 100.0)
    test_exc(15, B.Option.Build2, "SPY", 1, 0, 2020, True, 300, 1, 1, 0, 100.0)
    test_exc(16, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 1, 0,0, 100.0)
    test_exc(17, B.Option.Build2, "SPY", 1, 17, 0, True, 0.0, 1, 1,1, 100.0)
    test_exc(18, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 0, 0,1, 100.0)
    test_exc(19, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 1, 1, 0, .0)
    test_exc(20, B.Option.Build2, "SPY", 1, 17, 0, True, 300, 1, 0, 0, -100.0)

    def _build(order_type, asset_type, symbol, instruction, quantity,
              limit_price=0.0, stop_price=0.0):
        return B._abi_build('BuildOrder_Simple_ABI',
             execute.c_int(asset_type), execute.PCHAR(symbol),
             execute.c_size_t(quantity), execute.c_int(instruction),
             execute.c_int(order_type),  execute.c_double(limit_price),
             execute.c_double(stop_price))

    # MARKET
    mbto = B.Option.Build1('SPY_011720C300', 1, True, True)
    print( mbto.as_json() )
    check_order(mbto, MARKET, 'SPY_011720C300', BTO, 1)
    mbto2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1, True, True )
    assert mbto2 == mbto
    mbto3 = _build(MARKET, OPTION, 'SPY_011720C300', BTO, 1)
    assert mbto3 == mbto2
    assert mbto3.as_json() == mbto2.as_json() == mbto.as_json()

    msto = B.Option.Build1('SPY_011720P200', 99, False, True)
    print( msto.as_json() )
    check_order(msto, MARKET, 'SPY_011720P200', STO, 99)
    msto2 = B.Option.Build2("SPY", 1, 17, 2020, False, 200, 99, False, True)
    assert msto2 == msto
    msto3 = _build(MARKET, OPTION, 'SPY_011720P200', STO, 99)
    assert msto3 == msto2
    assert msto3.as_json() == msto2.as_json() == msto.as_json()

    mbtc = B.Option.Build1('SPY_011720C300', 1, True, False)
    print( mbtc.as_json() )
    check_order(mbtc, MARKET, 'SPY_011720C300', BTC, 1)
    mbtc2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1, True, False)
    assert mbtc2 == mbtc
    mbtc3 = _build(MARKET, OPTION, 'SPY_011720C300', BTC, 1)
    assert mbtc3 == mbtc2
    assert mbtc3.as_json() == mbtc2.as_json() == mbtc.as_json()

    mstc = B.Option.Build1('SPY_011720C300', 1, False, False)
    print( mstc.as_json() )
    check_order(mstc, MARKET, 'SPY_011720C300', STC, 1)
    mstc2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1, False, False)
    assert mstc2 == mstc
    mstc3 = _build(MARKET, OPTION, 'SPY_011720C300', STC, 1)
    assert mstc3 == mstc2
    assert mstc3.as_json() == mstc2.as_json() == mstc.as_json()

    # LIMIT
    lbto = B.Option.Build1('SPY_011720C300', 1,True, True, 9.99)
    print( lbto.as_json() )
    check_order(lbto, LIMIT, 'SPY_011720C300', BTO, 1, 9.99)
    lbto2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1,True, True, 9.99)
    assert lbto2 == lbto
    lbto3 = _build(LIMIT, OPTION, 'SPY_011720C300', BTO, 1, 9.99)
    assert lbto3 == lbto2
    assert lbto3.as_json() == lbto2.as_json() == lbto.as_json()

    lsto = B.Option.Build1('SPY_011720P200', 99, False, True, .009)
    print( lsto.as_json() )
    check_order(lsto, LIMIT, 'SPY_011720P200', STO, 99, .009)
    lsto2 = B.Option.Build2("SPY", 1, 17, 2020, False, 200, 99,False, True, .009)
    assert lsto2 == lsto
    lsto3 = _build(LIMIT, OPTION, 'SPY_011720P200', STO, 99, .009)
    assert lsto3 == lsto2
    assert lsto3.as_json() == lsto2.as_json() == lsto.as_json()

    lbtc = B.Option.Build1('SPY_011720C300', 1, True, False, 10)
    print( lbtc.as_json() )
    check_order(lbtc, LIMIT, 'SPY_011720C300', BTC, 1, 10.00)
    lbtc2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1, True, False, 10)
    assert lbtc2 == lbtc
    lbtc3 = _build(LIMIT, OPTION, 'SPY_011720C300', BTC, 1, 10)
    assert lbtc3 == lbtc2
    assert lbtc3.as_json() == lbtc2.as_json() == lbtc.as_json()

    lstc = B.Option.Build1('SPY_011720C300', 1, False, False, 10.0001)
    print( lstc.as_json() )
    check_order(lstc, LIMIT, 'SPY_011720C300', STC, 1, 10.0001)
    lstc2 = B.Option.Build2("SPY", 1, 17, 2020, True, 300, 1, False, False, 10.0001)
    assert lstc2 == lstc
    lstc3 = _build(LIMIT, OPTION, 'SPY_011720C300', STC, 1, 10.0001)
    assert lstc3 == lstc2
    assert lstc3.as_json() == lstc2.as_json() == lstc.as_json()



BUY_TO_OPEN = execute.ORDER_INSTRUCTION_BUY_TO_OPEN
BUY_TO_CLOSE = execute.ORDER_INSTRUCTION_BUY_TO_CLOSE
SELL_TO_OPEN = execute.ORDER_INSTRUCTION_SELL_TO_OPEN
SELL_TO_CLOSE = execute.ORDER_INSTRUCTION_SELL_TO_CLOSE
OPTION = execute.ORDER_ASSET_TYPE_OPTION
EQUITY = execute.ORDER_ASSET_TYPE_EQUITY
MARKET = execute.ORDER_TYPE_MARKET
NET_CREDIT = execute.ORDER_TYPE_NET_CREDIT
NET_DEBIT = execute.ORDER_TYPE_NET_DEBIT
NET_ZERO = execute.ORDER_TYPE_NET_ZERO
DURATION_DAY = execute.ORDER_DURATION_DAY
STRATEGY_SINGLE = execute.ORDER_STRATEGY_TYPE_SINGLE
SESSION_NORMAL = execute.ORDER_SESSION_NORMAL
LEG = execute.OrderLeg

def check_spread_order_ticket(order, otype, cmplx, legs, price=0.0, opt=""):
    E = execute
    cmplx_str = E.complex_order_strategy_type_to_str(cmplx)
    otype_str = E.order_type_to_str(otype)

    opt = '-' + opt if opt else opt
    title = cmplx_str + opt + '-' + otype_str + '-('
    for l in legs:
        title += E.order_instruction_to_str(l.get_instruction()) + '/'
    title = title[:-1] + ')'
    print("+ CHECK SPREAD TICKET:", title )

    #object
    assert order.get_duration() == DURATION_DAY
    assert order.get_complex_strategy_type() == cmplx
    assert order.get_strategy_type() == STRATEGY_SINGLE
    assert order.get_type() == otype
    assert order.get_session() == SESSION_NORMAL
    assert order.get_price() == price
    assert not order.get_children()

    olegs = order.get_legs()
    assert olegs
    print("+   CHECK LEG (OBJECT)...", end='  ')
    for i in range(len(olegs)):
        print(str(i), end='  ')
        assert legs[i] == olegs[i]
        assert legs[i] is not olegs[i]
    print()

    #json
    j = order.as_json()
    assert j
    assert j["duration"] == E.order_duration_to_str(DURATION_DAY)
    assert j["complexOrderStrategyType"] == cmplx_str
    assert j["orderStrategyType"] == \
        E.order_strategy_type_to_str(STRATEGY_SINGLE)
    assert j["orderType"] == otype_str
    assert j["session"] == E.order_session_to_str(SESSION_NORMAL)
    if price:
        assert float(j["price"]) == price
    assert "childOrderStrategies" not in j

    print("+   CHECK LEG (JSON)...", end='  ')
    for i in range(len(legs)):
        print(str(i), end='  ')
        jj = j["orderLegCollection"][i]
        assert jj["instruction"] == \
            E.order_instruction_to_str(legs[i].get_instruction())
        assert jj["quantity"] == legs[i].get_quantity()
        assert jj["instrument"]["assetType"] == \
            E.order_asset_type_to_str(legs[i].get_asset_type())
        assert jj["instrument"]["symbol"] == legs[i].get_symbol()
    print()


def test_execute_spread_option_builders():
    print('\n  *** Vertical ***')
    test_execute_spread_vertical_builders()
    print('\n  *** Vertical Roll***')
    test_execute_spread_vertical_roll_builders()
    print('\n  *** Vertical Roll (Unbalanced)***')
    test_execute_spread_vertical_roll_unbalanced_builders()
    print('\n  *** Butterfly ***')
    test_execute_spread_butterfly_builders()
    print('\n  *** Butterfly (Unbalanced) ***')
    test_execute_spread_butterfly_unbalanced_builders()
    print('\n  *** BackRatio ***')
    test_execute_spread_backratio_builders()
    print('\n  *** Calendar ***')
    test_execute_spread_calendar_builders()
    print('\n  *** Diagonal ***')
    test_execute_spread_diagonal_builders()
    print('\n  *** Straddle ***')
    test_execute_spread_straddle_builders()
    print('\n  *** Strangle ***')
    test_execute_spread_strangle_builders()
    print('\n  *** Collar Synthetic ***')
    test_execute_spread_collar_synthetic_builders()
    print('\n  *** Collar With Stock ***')
    test_execute_spread_collar_with_stock_builders()
    print('\n  *** Condor ***')
    test_execute_spread_condor_builders();
    print('\n  *** Condor (Unbalanced) ***')
    test_execute_spread_condor_unbalanced_builders();
    print('\n  *** Iron Condor ***')
    test_execute_spread_iron_condor_builders();
    print('\n  *** Iron Condor (Unbalanced) ***')
    test_execute_spread_iron_condor_unbalanced_builders();
    print('\n  *** Double Diagonal ***')
    test_execute_spread_double_diagonal_builders()


def test_execute_spread_vertical_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL
    B = execute.SpreadOrderBuilder.Vertical

    # MARKET
    order = B.Build1("SPY_011720C300", "SPY_011720C350", 1, True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, True, 300, 350, 1, True)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 99, False)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 99, False)
    assert order == order2

    # LIMIT
    order = B.Build1("SPY_011720C300", "SPY_011720C350", 1, True, 4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020, True, 300, 350, 1, True, 4.99)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 99, False, -.99)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 99, False, -.99)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 99, False, 0)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 99, False, 0)
    assert order == order2


def _test_execute_spread_vertical_roll_builders(CMPLX, B1, Q, unbalanced):

    # MARKET - NEW EXP
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C350",
                           "SPY_032020C375", "SPY_032020C325", *q)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[0][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_CLOSE, Q[0][0] ),
             LEG(OPTION,"SPY_032020C375", BUY_TO_OPEN, Q[0][1] ),
             LEG(OPTION,"SPY_032020C325", SELL_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0, "Build3 (New Exp)")
    order2 = B1.Build3("SPY", 1, 17, 2020, 3, 20, 2020, True, 300,
                             350, 375, 325, *q)
    assert order == order2

    # MARKET - SAME EXP
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250",
                           "SPY_011720P250", "SPY_011720P225", *q)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[1][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[1][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0,
                              "Build2 (Same Exp)")
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 250, 225, *q)
    assert order == order2

    # NET_CREDIT - NEW EXP
    q = Q[2] if unbalanced else [Q[2][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C350",
                           "SPY_032020C375", "SPY_032020C325", *q, limit_price=-.01)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_032020C375", BUY_TO_OPEN, Q[2][1] ),
             LEG(OPTION,"SPY_032020C325", SELL_TO_OPEN, Q[2][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .01,
                              "Build3 (New Exp)")
    order2 = B1.Build3("SPY", 1, 17, 2020, 3, 20, 2020, True, 300,
                             350, 375, 325, *q, limit_price=-.01)
    assert order == order2

    # NET_DEBIT - SAME EXP
    q = Q[3] if unbalanced else [Q[3][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250",
                           "SPY_011720P250", "SPY_011720P225", *q, limit_price=.01)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[3][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[3][1] ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, .01,
                              "Build2 (Same Exp)")
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 250, 225, *q,
                      limit_price=.01)
    assert order == order2

    # NET_ZERO - SAME EXP
    q = Q[4] if unbalanced else [Q[4][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250",
                           "SPY_011720P250", "SPY_011720P225", *q, limit_price=.0)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, Q[4][0] ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, Q[4][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[4][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[4][1] ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0,
                              "Build2 (Same Exp)")
    order2 = B1.Build2("SPY", 1, 17, 2020, False,
                              200, 250, 250, 225, *q, limit_price=0)
    assert order == order2


def test_execute_spread_vertical_roll_builders():
    _test_execute_spread_vertical_roll_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_VERTICAL_ROLL,
        execute.SpreadOrderBuilder.Vertical.Roll,
        [(1,1),(99,99),(1,1),(99,99),(99999,99999)],
        False)

def test_execute_spread_vertical_roll_unbalanced_builders():
    _test_execute_spread_vertical_roll_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_VERTICAL_ROLL,
        execute.SpreadOrderBuilder.Vertical.Roll.Unbalanced,
        [(1,2),(99,1),(1,2),(99,1),(99998,99999)],
        True)


def _test_execute_spread_butterfly_builders(CMPLX, B1,Q, unbalanced):

    # MARKET - BUY_TO_OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350", *q,
                      is_buy=True, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] + Q[0][1] ),
             LEG(OPTION,"SPY_011720C350", BUY_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, True, 300, 325, 350, *q,
                       is_buy=True, to_open=True)
    assert order == order2

    # MARKET - SELL_TO_OPEN
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250", "SPY_011720P300", *q,
                      is_buy=False, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720P200", SELL_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[1][0] + Q[1][1] ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, Q[1][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 300, *q,
                       is_buy=False, to_open=True)
    assert order == order2

    # MARKET - BUY_TO_CLOSE
    q = Q[2] if unbalanced else [Q[2][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350", *q,
                      is_buy=True, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_CLOSE, Q[2][0] + Q[2][1] ),
             LEG(OPTION,"SPY_011720C350", BUY_TO_CLOSE, Q[2][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, True, 300, 325, 350, *q,
                       is_buy=True, to_open=False)
    assert order == order2

    # MARKET - SELL_TO_CLOSE
    q = Q[3] if unbalanced else [Q[3][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250", "SPY_011720P300", *q,
                      is_buy=False, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720P200", SELL_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, Q[3][0] + Q[3][1] ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, Q[3][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 300, *q,
                       is_buy=False, to_open=False)
    assert order == order2

    # NET_DEBIT - BUY_TO_OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350", *q,
                      is_buy=True, to_open=True, limit_price=4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] + Q[0][1] ),
             LEG(OPTION,"SPY_011720C350", BUY_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B1.Build2("SPY", 1, 17, 2020, True, 300, 325, 350, *q,
                       is_buy=True, to_open=True, limit_price=4.99)
    assert order == order2

    # NET_CREDIT - SELL_TO_OPEN
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250", "SPY_011720P300", *q,
                      is_buy=False, to_open=True, limit_price=-.999)
    legs = [ LEG(OPTION,"SPY_011720P200", SELL_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[1][0] + Q[1][1] ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, Q[1][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .999)
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 300, *q,
                        is_buy=False, to_open=True, limit_price=-.999)
    assert order == order2

    # NET_ZERO - BUY_TO_CLOSE
    q = Q[2] if unbalanced else [Q[2][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350",
                      is_buy=True, to_open=False, *q, limit_price=0.0)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_CLOSE, Q[2][0] + Q[2][1] ),
             LEG(OPTION,"SPY_011720C350", BUY_TO_CLOSE, Q[2][1] ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, True, 300, 325, 350, *q,
                        is_buy=True, to_open=False,  limit_price=0.0)
    assert order == order2

    # NET_CREDIT - SELL_TO_CLOSE
    q = Q[3] if unbalanced else [Q[3][0]]
    order = B1.Build1("SPY_011720P200", "SPY_011720P250", "SPY_011720P300",
                      is_buy=False, to_open=False, *q, limit_price=-.01)
    legs = [ LEG(OPTION,"SPY_011720P200", SELL_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, Q[3][0] + Q[3][1] ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, Q[3][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .01)
    order2 = B1.Build2("SPY", 1, 17, 2020, False, 200, 250, 300,
                       is_buy=False, to_open=False, *q, limit_price=-.01)
    assert order == order2


def test_execute_spread_butterfly_builders():
    _test_execute_spread_butterfly_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_BUTTERFLY,
        execute.SpreadOrderBuilder.Butterfly,
        [(1,1),(99,99),(1,1),(99999,99999)],
        False)


def test_execute_spread_butterfly_unbalanced_builders():
    _test_execute_spread_butterfly_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_BUTTERFLY,
        execute.SpreadOrderBuilder.Butterfly.Unbalanced,
        [(1,2),(49,99),(10,1),(99998,99999)],
        True )

def test_execute_spread_backratio_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_BACK_RATIO
    B = execute.SpreadOrderBuilder.BackRatio

    # MARKET
    order = B.Build1("SPY_011720C300", "SPY_011720C350", 1,2, True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, 2 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, True, 300, 350, 1, 2, True)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 99,98, False)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 98 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 99, 98, False)
    assert order == order2

    # LIMIT
    order = B.Build1("SPY_011720C300", "SPY_011720C350", 1, 2, True, 4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, 2 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020, True, 300, 350, 1,2, True, 4.99)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 99,98, False, -.99)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 98 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 99,98, False, -.99)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_011720P250", 1000,1, False, 0)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 1000 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 1 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, False, 200, 250, 1000,1, False, 0)
    assert order == order2


def test_execute_spread_calendar_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_CALENDAR
    B = execute.SpreadOrderBuilder.Calendar

    # MARKET
    order = B.Build1("SPY_011720C300", "SPY_032020C300", 1, True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C300", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, True, 300, 1,  True)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_032020P200", 99, False)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020P200", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, False, 200, 99,  False)
    assert order == order2

    # LIMIT
    order = B.Build1("SPY_011720C300", "SPY_032020C300", 1,  True, 4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C300", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, True, 300, 1,
                      True, 4.99)
    assert order == order2

    order = B.Build1("SPY_032020P200", "SPY_011720P200", 99, False, -.99)
    legs = [ LEG(OPTION,"SPY_032020P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P200", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B.Build2("SPY", 3, 20, 2020, 1, 17, 2020, False, 200, 99,
                      False, -.99)
    assert order == order2

    order = B.Build1("SPY_032020P200", "SPY_011720P200", 1000, False, 0)
    legs = [ LEG(OPTION,"SPY_032020P200", BUY_TO_CLOSE, 1000 ),
             LEG(OPTION,"SPY_011720P200", SELL_TO_CLOSE, 1000 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 3, 20, 2020, 1, 17, 2020, False, 200, 1000,
                      False, 0)
    assert order == order2


def test_execute_spread_diagonal_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_DIAGONAL
    B = execute.SpreadOrderBuilder.Diagonal

    # MARKET
    order = B.Build1("SPY_011720C300", "SPY_032020C350", 1, True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C350", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, True, 300, 350, 1,
                      True)
    assert order == order2

    order = B.Build1("SPY_011720P200", "SPY_032020P250", 99, False)
    legs = [ LEG(OPTION,"SPY_011720P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020,False, 200, 250,
                      99,  False)
    assert order == order2

    # LIMIT
    order = B.Build1("SPY_011720C300", "SPY_032020C350", 1,  True, 4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C350", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020,True, 300, 350, 1,
                      True, 4.99)
    assert order == order2

    order = B.Build1("SPY_032020P200", "SPY_011720P250", 99, False, -.99)
    legs = [ LEG(OPTION,"SPY_032020P200", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B.Build2("SPY",3, 20, 2020,1, 17, 2020, False, 200, 250, 99,
                      False, -.99)
    assert order == order2

    order = B.Build1("SPY_032020P200", "SPY_011720P250", 1000, False, 0)
    legs = [ LEG(OPTION,"SPY_032020P200", BUY_TO_CLOSE, 1000 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 1000 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 3, 20, 2020,1, 17, 2020, False, 200, 250, 1000,
                      False, 0)
    assert order == order2


def test_execute_spread_straddle_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_STRADDLE
    B = execute.SpreadOrderBuilder.Straddle

    # MARKET - BUY_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 1,
                      is_buy=True, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P300", BUY_TO_OPEN, 1) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 1,
                       is_buy=True, to_open=True)
    assert order == order2

    # MARKET - SELL_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 99,
                      is_buy=False, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, 99) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 99,
                       is_buy=False, to_open=True)
    assert order == order2

    # MARKET - BUY_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 1,
                      is_buy=True, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P300", BUY_TO_CLOSE, 1) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 1,
                       is_buy=True, to_open=False)
    assert order == order2

    # MARKET - SELL_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 99,
                      is_buy=False, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, 99) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 99,
                       is_buy=False, to_open=False)
    assert order == order2

    # NET_DEBIT - BUY_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 1,
                      is_buy=True, to_open=True, limit_price=4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P300", BUY_TO_OPEN, 1) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020,  300, 1,
                       is_buy=True, to_open=True, limit_price=4.99)
    assert order == order2

    # NET_CREDIT - SELL_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 99,
                      is_buy=False, to_open=True, limit_price=-.999)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, 99) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .999)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 99,
                       is_buy=False, to_open=True, limit_price=-.999)
    assert order == order2

    # NET_ZERO - BUY_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 1,
                      is_buy=True, to_open=False, limit_price=0)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P300", BUY_TO_CLOSE, 1) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 1,
                       is_buy=True, to_open=False, limit_price=0.0)
    assert order == order2

    # NET_CREDIT - SELL_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P300", 99,
                      is_buy=False, to_open=False, limit_price=-.01)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, 99) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .01)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 99,
                       is_buy=False, to_open=False, limit_price=-.01)
    assert order == order2


def test_execute_spread_strangle_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_STRANGLE
    B = execute.SpreadOrderBuilder.Strangle

    # MARKET - BUY_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 1,
                      is_buy=True, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 1) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 1,
                       is_buy=True, to_open=True)
    assert order == order2

    # MARKET - SELL_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 99,
                      is_buy=False, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_OPEN, 99) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020,  300, 250,99,
                       is_buy=False, to_open=True)
    assert order == order2

    # MARKET - BUY_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 1,
                      is_buy=True, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 1) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300,250, 1,
                       is_buy=True, to_open=False)
    assert order == order2

    # MARKET - SELL_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 99,
                      is_buy=False, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300,250, 99,
                       is_buy=False, to_open=False)
    assert order == order2

    # NET_DEBIT - BUY_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 1,
                      is_buy=True, to_open=True, limit_price=4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 1) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250,1,
                       is_buy=True, to_open=True, limit_price=4.99)
    assert order == order2

    # NET_CREDIT - SELL_TO_OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 99,
                      is_buy=False, to_open=True, limit_price=-.999)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_OPEN, 99) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .999)
    order2 = B.Build2("SPY", 1, 17, 2020,  300, 250,99,
                       is_buy=False, to_open=True, limit_price=-.999)
    assert order == order2

    # NET_ZERO - BUY_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 1,
                      is_buy=True, to_open=False, limit_price=0)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 1) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250,1,
                       is_buy=True, to_open=False, limit_price=0.0)
    assert order == order2

    # NET_CREDIT - SELL_TO_CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", 99,
                      is_buy=False, to_open=False, limit_price=-.01)
    legs = [ LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 99) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .01)
    order2 = B.Build2("SPY", 1, 17, 2020,  300, 250,99,
                       is_buy=False, to_open=False, limit_price=-.01)
    assert order == order2


def test_execute_spread_collar_synthetic_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_COLLAR_SYNTHETIC
    B = execute.SpreadOrderBuilder.CollarSynthetic

    # MARKET
    order = B.Build1("SPY_011720C300", "SPY_011720P300", 1, True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 300, 1, True, True)
    assert order == order2

    order = B.Build1("SPY_011720P250", "SPY_011720C300", 99, False)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99, False, False)
    assert order == order2

    # LIMIT
    order = B.Build1("SPY_011720C300", "SPY_011720P300", 1, True, -1.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, 1.99)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 300, 1, True, True, -1.99)
    assert order == order2

    order = B.Build1("SPY_011720P250", "SPY_011720C300", 99, False, .01)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, .01)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99, False, False, .01)
    assert order == order2

    order = B.Build1("SPY_011720P250", "SPY_011720C300", 99, True, .0)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, .0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99, False, True, .0)
    assert order == order2


def test_execute_spread_collar_with_stock_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_COLLAR_WITH_STOCK
    B = execute.SpreadOrderBuilder.CollarWithStock

    # MARKET - SHORT STOCK TO OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", "SPY", 1,
                      is_buy=False, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_OPEN, 1),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_SELL_SHORT, 100) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 1,
                       is_buy=False, to_open=True)
    assert order == order2

    # MARKET - BUY STOCK TO OPEN
    order = B.Build1("SPY_011720P250",  "SPY_011720C300", "SPY", 99,
                      is_buy=True, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_BUY, 9900) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99,
                       is_buy=True, to_open=True)
    assert order == order2

    # MARKET - SELL STOCK TO CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", "SPY", 1,
                      is_buy=False, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 1),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_SELL, 100) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 1,
                       is_buy=False, to_open=False)
    assert order == order2

    # MARKET - COVER STOCK TO CLOSE
    order = B.Build1("SPY_011720P250",  "SPY_011720C300", "SPY", 99,
                      is_buy=True, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_BUY_TO_COVER, 9900) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99,
                       is_buy=True, to_open=False)
    assert order == order2

    # NET_CREDIT - SHORT STOCK TO OPEN
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", "SPY", 1,
                      is_buy=False, to_open=True, limit_price=-250.01 )
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_OPEN, 1),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_SELL_SHORT, 100) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, 250.01)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 1,
                       is_buy=False, to_open=True, limit_price=-250.01)
    assert order == order2

    # NET_DEBIT - BUY STOCK TO OPEN
    order = B.Build1("SPY_011720P250",  "SPY_011720C300", "SPY", 99,
                      is_buy=True, to_open=True, limit_price=299.999)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_OPEN, 99),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_BUY, 9900) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 299.999)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99,
                       is_buy=True, to_open=True, limit_price=299.999)
    assert order == order2

    # NET_CREDIT - SELL STOCK TO CLOSE
    order = B.Build1("SPY_011720C300",  "SPY_011720P250", "SPY", 1,
                      is_buy=False, to_open=False, limit_price=-300)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, 1 ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, 1),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_SELL, 100) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, 300)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 1,
                       is_buy=False, to_open=False, limit_price=-300)
    assert order == order2

    # NET_DEBIT - COVER STOCK TO CLOSE
    order = B.Build1("SPY_011720P250",  "SPY_011720C300", "SPY", 99,
                      is_buy=True, to_open=False, limit_price=300)
    legs = [ LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, 99),
             LEG(EQUITY, "SPY", execute.ORDER_INSTRUCTION_BUY_TO_COVER, 9900) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 300)
    order2 = B.Build2("SPY", 1, 17, 2020, 300, 250, 99,
                       is_buy=True, to_open=False, limit_price=300)
    assert order == order2


def _test_execute_spread_condor_builders(CMPLX, B1,Q, unbalanced):

    # MARKET - BUY_TO_OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350",
                      "SPY_011720C375", *q, is_buy=True, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, Q[0][1] ),
             LEG(OPTION,"SPY_011720C375", BUY_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 350, 375, True,  *q,
                       is_buy=True, to_open=True)
    assert order == order2

    # MARKET - SELL_TO_OPEN
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720P300", "SPY_011720P275", "SPY_011720P250",
                      "SPY_011720P225", *q, is_buy=False, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P275", BUY_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[1][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[1][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020,  300, 275, 250, 225, False, *q,
                       is_buy=False, to_open=True)
    assert order == order2

    # MARKET - BUY_TO_CLOSE
    q = Q[2] if unbalanced else [Q[2][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350",
                      "SPY_011720C375", *q, is_buy=True, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_CLOSE, Q[2][1] ),
             LEG(OPTION,"SPY_011720C375", BUY_TO_CLOSE, Q[2][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 350, 375, True,  *q,
                       is_buy=True, to_open=False)
    assert order == order2

    # MARKET - SELL_TO_CLOSE
    q = Q[3] if unbalanced else [Q[3][0]]
    order = B1.Build1("SPY_011720P300", "SPY_011720P275", "SPY_011720P250",
                      "SPY_011720P225", *q, is_buy=False, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P275", BUY_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, Q[3][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_CLOSE, Q[3][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020,  300, 275, 250, 225, False, *q,
                       is_buy=False, to_open=False)
    assert order == order2

    # NET_DEBIT - BUY_TO_OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350",
                      "SPY_011720C375", *q, is_buy=True, to_open=True,
                      limit_price=4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_OPEN, Q[0][1] ),
             LEG(OPTION,"SPY_011720C375", BUY_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 350, 375, True,  *q,
                       is_buy=True, to_open=True, limit_price=4.99)
    assert order == order2

    # NET_CREDIT - SELL_TO_OPEN
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720P300", "SPY_011720P275", "SPY_011720P250",
                      "SPY_011720P225", *q, is_buy=False, to_open=True,
                      limit_price=-.999)
    legs = [ LEG(OPTION,"SPY_011720P300", SELL_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P275", BUY_TO_OPEN, Q[1][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[1][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[1][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .999)
    order2 = B1.Build2("SPY", 1, 17, 2020,  300, 275, 250, 225, False, *q,
                       is_buy=False, to_open=True, limit_price=-.999)
    assert order == order2

    # NET_ZERO - BUY_TO_CLOSE
    q = Q[2] if unbalanced else [Q[2][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720C350",
                      "SPY_011720C375", *q, is_buy=True, to_open=False,
                      limit_price=0)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_CLOSE, Q[2][0] ),
             LEG(OPTION,"SPY_011720C350", SELL_TO_CLOSE, Q[2][1] ),
             LEG(OPTION,"SPY_011720C375", BUY_TO_CLOSE, Q[2][1] ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 350, 375, True,  *q,
                       is_buy=True, to_open=False, limit_price=0.0)
    assert order == order2

    # NET_CREDIT - SELL_TO_CLOSE
    q = Q[3] if unbalanced else [Q[3][0]]
    order = B1.Build1("SPY_011720P300", "SPY_011720P275", "SPY_011720P250",
                      "SPY_011720P225", *q, is_buy=False, to_open=False,
                      limit_price=-.01)
    legs = [ LEG(OPTION,"SPY_011720P300", SELL_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P275", BUY_TO_CLOSE, Q[3][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_CLOSE, Q[3][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_CLOSE, Q[3][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .01)
    order2 = B1.Build2("SPY", 1, 17, 2020,  300, 275, 250, 225, False, *q,
                       is_buy=False, to_open=False, limit_price=-.01)
    assert order == order2

def test_execute_spread_condor_builders():
    _test_execute_spread_condor_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_CONDOR,
        execute.SpreadOrderBuilder.Condor,
        [(1,1),(99,99),(1,1),(99999,99999)],
        False)


def test_execute_spread_condor_unbalanced_builders():
    _test_execute_spread_condor_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_CONDOR,
        execute.SpreadOrderBuilder.Condor.Unbalanced,
        [(1,2),(49,99),(10,1),(99998,99999)],
        True )


def _test_execute_spread_iron_condor_builders(CMPLX, B1,Q, unbalanced):

    # MARKET - TO OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720P250",
                      "SPY_011720P225", *q, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[0][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 250, 225, *q,
                       to_open=True)
    assert order == order2

    # MARKET - TO CLOSE
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720C325", "SPY_011720C300", "SPY_011720P225",
                      "SPY_011720P250", *q, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C325", BUY_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720P225", BUY_TO_CLOSE, Q[1][1] ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, Q[1][1] ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B1.Build2("SPY", 1, 17, 2020, 325, 300, 225, 250, *q,
                       to_open=False)
    assert order == order2

    # NET_DEBIT - TO OPEN
    q = Q[0] if unbalanced else [Q[0][0]]
    order = B1.Build1("SPY_011720C300", "SPY_011720C325", "SPY_011720P250",
                      "SPY_011720P225", *q, to_open=True, limit_price=4.99)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720C325", SELL_TO_OPEN, Q[0][0] ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, Q[0][1] ),
             LEG(OPTION,"SPY_011720P225", SELL_TO_OPEN, Q[0][1] ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 4.99)
    order2 = B1.Build2("SPY", 1, 17, 2020, 300, 325, 250, 225, *q,
                       to_open=True, limit_price=4.99)
    assert order == order2

    # NET_CREDIT - TO CLOSE
    q = Q[1] if unbalanced else [Q[1][0]]
    order = B1.Build1("SPY_011720C325", "SPY_011720C300", "SPY_011720P225",
                      "SPY_011720P250", *q, to_open=False, limit_price=-.99)
    legs = [ LEG(OPTION,"SPY_011720C325", BUY_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720C300", SELL_TO_CLOSE, Q[1][0] ),
             LEG(OPTION,"SPY_011720P225", BUY_TO_CLOSE, Q[1][1] ),
             LEG(OPTION,"SPY_011720P250", SELL_TO_CLOSE, Q[1][1] ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B1.Build2("SPY", 1, 17, 2020, 325, 300, 225, 250, *q,
                       to_open=False, limit_price=-.99)
    assert order == order2


def test_execute_spread_iron_condor_builders():
    _test_execute_spread_iron_condor_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_IRON_CONDOR,
        execute.SpreadOrderBuilder.IronCondor,
        [(1,1),(99,99),(1,1),(99999,99999)],
        False)

def test_execute_spread_iron_condor_unbalanced_builders():
    _test_execute_spread_iron_condor_builders(
        execute.COMPLEX_ORDER_STRATEGY_TYPE_UNBALANCED_IRON_CONDOR,
        execute.SpreadOrderBuilder.IronCondor.Unbalanced,
        [(1,2),(49,99),(10,1),(99998,99999)],
        True )


def test_execute_spread_double_diagonal_builders():
    CMPLX = execute.COMPLEX_ORDER_STRATEGY_TYPE_DOUBLE_DIAGONAL
    B = execute.SpreadOrderBuilder.DoubleDiagonal

    # MARKET - TO OPEN
    order = B.Build1("SPY_011720C300", "SPY_032020C325", "SPY_011720P250",
                      "SPY_032020P225", 1, to_open=True)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C325", SELL_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020P225", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, 300, 325, 250, 225, 1,
                       to_open=True)
    assert order == order2

    # MARKET - TO CLOSE
    order = B.Build1("SPY_011720C325", "SPY_032020C300", "SPY_011720P225",
                      "SPY_032020P250", 99, to_open=False)
    legs = [ LEG(OPTION,"SPY_011720C325", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P225", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, MARKET, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, 325, 300, 225, 250, 99,
                       to_open=False)
    assert order == order2

    # NET_DEBIT - TO OPEN
    order = B.Build1("SPY_011720C300", "SPY_032020C325", "SPY_011720P250",
                      "SPY_032020P225", 1, to_open=True, limit_price=5.01)
    legs = [ LEG(OPTION,"SPY_011720C300", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020C325", SELL_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_011720P250", BUY_TO_OPEN, 1 ),
             LEG(OPTION,"SPY_032020P225", SELL_TO_OPEN, 1 ) ]
    check_spread_order_ticket(order, NET_DEBIT, CMPLX, legs, 5.01)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, 300, 325, 250, 225, 1,
                       to_open=True, limit_price=5.01)
    assert order == order2

    # NET_CREDIT - TO CLOSE
    order = B.Build1("SPY_011720C325", "SPY_032020C300", "SPY_011720P225",
                      "SPY_032020P250", 99, to_open=False, limit_price=-.99)
    legs = [ LEG(OPTION,"SPY_011720C325", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P225", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_CREDIT, CMPLX, legs, .99)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, 325, 300, 225, 250, 99,
                       to_open=False, limit_price=-.99)
    assert order == order2

    # NET_ZERO - TO CLOSE
    order = B.Build1("SPY_011720C325", "SPY_032020C300", "SPY_011720P225",
                      "SPY_032020P250", 99, to_open=False, limit_price=0)
    legs = [ LEG(OPTION,"SPY_011720C325", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020C300", SELL_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_011720P225", BUY_TO_CLOSE, 99 ),
             LEG(OPTION,"SPY_032020P250", SELL_TO_CLOSE, 99 ) ]
    check_spread_order_ticket(order, NET_ZERO, CMPLX, legs, 0)
    order2 = B.Build2("SPY", 1, 17, 2020, 3, 20, 2020, 325, 300, 225, 250, 99,
                       to_open=False, limit_price=0)
    assert order == order2


def test_execute_oco_oto_builders():
    #EXIT
    #stop
    co1 = execute.SimpleOrderBuilder.Equity.Stop.Build("SPY", 100, False, False,
                                                  stop_price=250.00)

    #target
    to1 = execute.SimpleOrderBuilder.Equity.Build("SPY", 100, False, False,
                                                  limit_price= 300.00)

    oco1 = execute.ConditionalOrderBuilder.OCO(co1, to1)

    #ENTRY
    #buy
    b1 = execute.SimpleOrderBuilder.Equity.Build("SPY", 100, True, True)

    #COMPLETE#
    oto1 = execute.ConditionalOrderBuilder.OTO(b1, oco1)
    print("FULL ORDER:")
    print( oto1.as_json() )

    # primary (buy/market)
    assert oto1.get_strategy_type() == execute.ORDER_STRATEGY_TYPE_TRIGGER
    assert oto1.get_type() == execute.ORDER_TYPE_MARKET
    leg = oto1.get_leg(0)
    assert leg.get_symbol() == "SPY"
    assert leg.get_quantity() == 100
    assert leg.get_asset_type() == execute.ORDER_ASSET_TYPE_EQUITY
    assert leg.get_instruction() == execute.ORDER_INSTRUCTION_BUY

    kids = oto1.get_children()
    assert len(kids) == 1

    # secondary (OCO)
    assert oco1 == kids[0]
    assert oco1 is not kids[0]
    oco1 = None
    assert kids[0]._alive
    assert kids[0].get_strategy_type() == execute.ORDER_STRATEGY_TYPE_OCO
    kids2 = kids[0].get_children()
    assert len(kids2) == 2

    # secondary 1 (sell-stop)
    assert co1 == kids2[0]
    assert co1 is not kids2[0]
    assert kids2[0].get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
    assert kids2[0].get_type() == execute.ORDER_TYPE_STOP
    leg = kids2[0].get_leg(0)
    assert leg.get_symbol() == "SPY"
    assert leg.get_quantity() == 100
    assert leg.get_asset_type() == execute.ORDER_ASSET_TYPE_EQUITY
    assert leg.get_instruction() == execute.ORDER_INSTRUCTION_SELL

    # secondary 2 (sell-limit)
    assert to1 == kids2[1]
    assert to1 is not kids2[1]
    assert kids2[1].get_strategy_type() == execute.ORDER_STRATEGY_TYPE_SINGLE
    assert kids2[1].get_type() == execute.ORDER_TYPE_LIMIT
    leg = kids2[1].get_leg(0)
    assert leg.get_symbol() == "SPY"
    assert leg.get_quantity() == 100
    assert leg.get_asset_type() == execute.ORDER_ASSET_TYPE_EQUITY
    assert leg.get_instruction() == execute.ORDER_INSTRUCTION_SELL

    oto1.clear_children()
    assert not oto1.get_children()
    print("STRIPPED ORDER:")
    print(oto1.as_json())


# LIVE ORDERS !
#def test_execute_transactions(creds, account_id):
#    order = execute.SimpleOrderBuilder.Equity.Build("XLF", 1, True, True, 1.99)
#    print( order.get_price() )
#    l = order.get_leg(0)
#    print( l.get_quantity() )
#    print( l.get_symbol() )
#    print( l.get_instruction() )
#    print( order.get_duration() )
#    print( order.get_session() )
#    oid = execute.send_order(creds, account_id, order)
#    print("order ID: ", oid)
#    success = execute.cancel_order(creds, account_id, oid)
#    print("cancel: ", str(success))



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
        #test(test_execute_transactions,cm.credentials, args.account_id)
        
        test(test_option_symbol_builder)
        test(test_execute_order_objects)
        test(test_execute_order_builders)
        test(test_share_connections)
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
        test(test_order_getters, cm.credentials, args.account_id)
        test(test_streaming, cm.credentials)
                

