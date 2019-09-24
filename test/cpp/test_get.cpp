
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "test.h"

#include "tdma_api_get.h"

using namespace tdma;
using namespace std;

void quote_getters(Credentials& c);
void historical_getters(Credentials& c);

void option_chain_getter(Credentials& c);
void option_chain_strategy_getter(Credentials& c);
void option_chain_analytical_getter(Credentials& c);

void instrument_info_getter(Credentials& c);

void market_hours_getter(Credentials& c);

void movers_getter(Credentials& c);

void account_info_getter(string id, Credentials& c);
void preferences_getter(string id, Credentials& c);
void user_principals_getter(Credentials& c);
void subscription_keys_getter(string id, Credentials& c);
void transaction_history_getter(string id, Credentials& c);
void individual_transaction_history_getter(string id, Credentials& c);
void order_getters(string id, Credentials& c);

void
test_getters(const string& account_id, Credentials& creds)
{
    using namespace chrono;

    if( !APIGetter::is_sharing_connections() )
        throw new std::runtime_error("not sharing connections (default)");

    cout<< endl << "*** STOP SHARING CONNECTIONS ***" << endl;
    APIGetter::share_connections(false);
    if( APIGetter::is_sharing_connections() )
        throw new std::runtime_error("sharing connections");

    cout<< endl << "*** RE(START) SHARING CONNECTIONS ***" << endl;
    APIGetter::share_connections(true);
    if( !APIGetter::is_sharing_connections() )
        throw new std::runtime_error("not sharing connections");

    cout<< endl <<"*** SET WAIT ***" << endl;
    cout<< APIGetter::get_wait_msec().count() << " --> ";
    APIGetter::set_wait_msec( milliseconds(1500) );
    cout<< APIGetter::get_wait_msec().count() << endl;

    cout<< endl << "*** QUOTE DATA ***" << endl;
    quote_getters(creds);
    cout<< "WaitRemaining: " << APIGetter::wait_remaining().count() << endl;

    historical_getters(creds);
    this_thread::sleep_for( seconds(3) );

    cout<< endl << "*** OPTION DATA ***" << endl;
    option_chain_getter(creds);
    option_chain_strategy_getter(creds);
    option_chain_analytical_getter(creds);
    this_thread::sleep_for( seconds(3) );


    cout<< endl << "*** MARKET DATA ***" << endl;
    instrument_info_getter(creds);
    market_hours_getter(creds);
    movers_getter(creds);
    this_thread::sleep_for( seconds(3) );

    cout<< endl << "*** ACCOUNT DATA ***" << endl;
    account_info_getter(account_id, creds);
    preferences_getter(account_id, creds);
    user_principals_getter(creds);
    subscription_keys_getter(account_id, creds);
    order_getters(account_id, creds);
    this_thread::sleep_for( seconds(3) );


    cout<< endl << "*** TRANSACTION DATA ***" << endl;
    try{
        transaction_history_getter(account_id, creds);
        individual_transaction_history_getter(account_id, creds);
    }catch( ServerError& e){
        cout<<"caught server error: " << e << endl;
    }
    this_thread::sleep_for( seconds(3) );

    cout<< endl << """*** ORDERS DATA ***" << endl;
    order_getters(account_id, creds);

}


void
Get(APIGetter& getter, bool test_exc = false, string test_exc_msg="")
{
    if( use_live_connection ){
        cout<< getter.get().dump(4) << endl << endl;
        if(test_exc)
            throw runtime_error(test_exc_msg);
    }else
        cout<< "CAN NOT TEST GET WITHOUT USING LIVE CONNECTION" << endl;
}

json
GetJson(APIGetter& getter, bool test_exc = false, string test_exc_msg="")
{
    json j;
    if( use_live_connection ){
        j = getter.get();
        if(test_exc)
            throw runtime_error(test_exc_msg);
        cout<< j.dump(4) << endl << endl;
    }
    else{
        cout<< "CAN NOT TEST GET WITHOUT USING LIVE CONNECTION" << endl;
    }
    return j;
}

string
build_order_date(tm* t)
{
    stringstream ss;
    ss << (t->tm_year + 1900) << setfill('0') << '-' << setw(2)
       << (t->tm_mon + 1) << '-' << setw(2) << t->tm_mday;
    return ss.str();
}

void
order_getters(string id, Credentials& c)
{
    std::vector<string> orders;

    auto tp_now = chrono::system_clock::now() - chrono::hours(5);
    auto tp_1ago = tp_now - chrono::hours(24);
    auto tp_20ago = tp_now - chrono::hours(20 * 24);
    auto tp_59ago = tp_now - chrono::hours(59 * 24);

    time_t tt_now = chrono::system_clock::to_time_t(tp_now);
    time_t tt_1ago = chrono::system_clock::to_time_t(tp_1ago);
    time_t tt_20ago = chrono::system_clock::to_time_t(tp_20ago);
    time_t tt_59ago = chrono::system_clock::to_time_t(tp_59ago);

    tm tm_now = *gmtime(&tt_now);
    tm tm_1ago = *gmtime(&tt_1ago);
    tm tm_20ago = *gmtime(&tt_20ago);
    tm tm_59ago = *gmtime(&tt_59ago);

    string fromdate1 = build_order_date(&tm_59ago);
    string todate1 = build_order_date(&tm_now);
    string fromdate2 = build_order_date(&tm_20ago);
    string todate2 = build_order_date(&tm_1ago);

    OrdersGetter os(c, id, 10, fromdate1, todate1, OrderStatusType::CANCELED);

    cout<< os.get_account_id() << ' ' << os.get_nmax_results() << ' '
        << os.get_from_entered_time() << ' ' << os.get_to_entered_time() << ' '
        << os.get_order_status_type() << endl;

    if( os.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( os.get_nmax_results() != 10 )
        throw runtime_error("invalid nmax results");
    if( os.get_from_entered_time() != fromdate1)
        throw runtime_error("invalid from_entered_time");
    if( os.get_to_entered_time() != todate1)
        throw runtime_error("invalid to_entered_time");
    if( os.get_order_status_type() != OrderStatusType::CANCELED )
        throw runtime_error("invalid order_status_type");

    json orders_j = GetJson(os);

    for(auto& o : orders_j){
        orders.push_back( to_string((unsigned long long)o["orderId"]) );
    }

    os.set_order_status_type(OrderStatusType::WORKING);
    if( os.get_order_status_type() != OrderStatusType::WORKING )
        throw runtime_error("invalid order_status_type");

    orders_j = GetJson(os);

    for(auto& o : orders_j){
        orders.push_back( to_string((unsigned long long)o["orderId"]) );
    }

    os.set_order_status_type(OrderStatusType::ALL);
    if( os.get_order_status_type() != OrderStatusType::ALL )
        throw runtime_error("invalid order_status_type");

    orders_j = GetJson(os);

    for(auto& o : orders_j){
        orders.push_back( to_string((unsigned long long)o["orderId"]) );
    }

    os.set_nmax_results(1);
    os.set_from_entered_time(fromdate2);
    os.set_to_entered_time(todate2);
    os.set_order_status_type(OrderStatusType::REJECTED);

    cout<< os.get_account_id() << ' ' << os.get_nmax_results() << ' '
        << os.get_from_entered_time() << ' ' << os.get_to_entered_time() << ' '
        << os.get_order_status_type() << endl;

    if( os.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( os.get_nmax_results() != 1 )
        throw runtime_error("invalid nmax results");
    if( os.get_from_entered_time() != fromdate2 )
        throw runtime_error("invalid from_entered_time");
    if( os.get_to_entered_time() != todate2 )
        throw runtime_error("invalid to_entered_time");
    if( os.get_order_status_type() != OrderStatusType::REJECTED )
        throw runtime_error("invalid order_status_type");

    try{
        os.set_account_id("");
        throw runtime_error("failed to catch .set_account_id() exc");
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }
    try{
        os.set_from_entered_time("");
        throw runtime_error("failed to catch .set_from_entered_time() exc");
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }
    try{
        os.set_to_entered_time("");
        throw runtime_error("failed to catch .set_to_entered_time() exc");
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }
    try{
        os.set_nmax_results(0);
        throw runtime_error("failed to catch .set_from_nmax_results() exc");
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }

    OrdersGetter osall(c, id, 10, fromdate1, todate1);
    if( osall.get_order_status_type() != OrderStatusType::ALL )
        throw runtime_error("invalid order_status_type");

    if( orders.empty() ){
        cout<< "NO CANCELED/QUEUED/WORKING ORDERS OVER LAST 59 DAYS"
            << " - SKIP OrderGetter (non-exception) tests" << endl;

        OrderGetter o(c, id, "000000000");
        try{
            Get(o, true, "failed to catch .get() w/ bad order id exc");
        }catch(InvalidRequest& e){
            cout<< "successfully caught: " << e << endl << endl;
        }
    }else{
        string order = orders[0];
        OrderGetter o(c, id, order);
        cout<< o.get_account_id() << ' ' << o.get_order_id() << endl;

        if( o.get_order_id() != order)
            throw runtime_error("invalid order_id");
        if( o.get_account_id() != id )
            throw runtime_error("invalid account id");

        Get(o);

        order = orders.size() > 1 ? orders[1] : orders[0];
        o.set_order_id(order);
        cout<< o.get_account_id() << ' ' << o.get_order_id() << endl;

        if( o.get_order_id() != order)
            throw runtime_error("invalid order_id");
        if( o.get_account_id() != id )
            throw runtime_error("invalid account id");

        Get(o);

        o.set_order_id("000000000");
        try{
            Get(o, true, "failed to catch .get() with bad order id exc");
        }catch(InvalidRequest& e){
            cout<< "successfully caught: " << e << endl << endl;
        }
        try{
            o.set_account_id("");
            throw runtime_error("failed to catch .set_account_id() exc");
        }catch(APIException& e){
            cout<< "successfully caught: " << e << endl << endl;
        }
        try{
            o.set_order_id("");
            throw runtime_error("failed to catch .set_account_id() exc");
        }catch(APIException& e){
            cout<< "successfully caught: " << e << endl << endl;
        }
    }
}


void
transaction_history_getter(string id, Credentials& c)
{
   TransactionHistoryGetter o(c, id, TransactionType::all, "SPy");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;

    if( o.get_symbol() != "SPY")
        throw runtime_error("invalid symbol");
    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( o.get_transaction_type() != TransactionType::all )
        throw runtime_error("invalid transaction type");
    if( o.get_start_date() != "" )
        throw runtime_error("invalid start date");
    if( o.get_end_date() != "" )
        throw runtime_error("invalid end date");

    Get(o);

    o.set_transaction_type(TransactionType::trade);
    o.set_symbol("");
    o.set_start_date("2018-01-01");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;

    if( o.get_symbol() != "")
        throw runtime_error("invalid symbol");
    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( o.get_transaction_type() != TransactionType::trade )
        throw runtime_error("invalid transaction type");
    if( o.get_start_date() != "2018-01-01" )
        throw runtime_error("invalid start date");
    if( o.get_end_date() != "" )
        throw runtime_error("invalid end date");

    Get(o);
}

void
individual_transaction_history_getter(string id, Credentials& c)
{
    IndividualTransactionHistoryGetter o(c, id, "093432432"); // bad id
    cout<< o.get_account_id() << ' ' << o.get_transaction_id() << endl;
    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( o.get_transaction_id() != "093432432" )
        throw runtime_error("invalid transaction id");

    try{
        Get(o);
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }
}

void
account_info_getter(string id, Credentials& c)
{
    AccountInfoGetter o(c, id, false, false);
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;

    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( o.returns_positions() != false )
        throw runtime_error("invalid returns positions");
    if( o.returns_orders() != false )
        throw runtime_error("invalid returns orders");

    Get(o);

    o.return_orders(true);
    o.return_positions(true);
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;

    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");
    if( o.returns_positions() != true )
        throw runtime_error("invalid returns positions");
    if( o.returns_orders() != true )
        throw runtime_error("invalid returns orders");

    Get(o);

    o.set_account_id("BAD_ACCOUNT_ID");
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;

    if( o.get_account_id() != "BAD_ACCOUNT_ID" )
        throw runtime_error("invalid account id");
    if( o.returns_positions() != true )
        throw runtime_error("invalid returns positions");
    if( o.returns_orders() != true )
        throw runtime_error("invalid returns orders");

    try{
        Get(o);
    }catch(APIException& e){
        cout<< "successfully caught: " << e << endl << endl;
    }
}


void
preferences_getter(string id, Credentials& c)
{
    PreferencesGetter o(c, id);
    cout<< o.get_account_id() << endl;

    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");

    Get(o);
}


void
user_principals_getter(Credentials& c)
{
    UserPrincipalsGetter o(c, false, false, false, false);
    cout<< o.returns_streamer_subscription_keys()
        << ' ' << o.returns_streamer_connection_info() << ' '
        << o.returns_preferences() << ' ' << o.returns_surrogate_ids() << endl;

    if( o.returns_streamer_subscription_keys() != false )
        throw runtime_error("invalid returns streamer subscription keys");
    if( o.returns_streamer_connection_info() != false )
        throw runtime_error("invalid returns streamer connection info");
    if( o.returns_preferences() != false )
        throw runtime_error("invalid returns preferences");
    if( o.returns_surrogate_ids() != false )
        throw runtime_error("invalid returns surrogate_ids");

    Get(o);

    o.return_preferences(true);
    o.return_streamer_connection_info(true);
    o.return_streamer_subscription_keys(true);
    o.return_surrogate_ids(true);
    cout<<  o.returns_streamer_subscription_keys()
        << ' ' << o.returns_streamer_connection_info() << ' '
        << o.returns_preferences() << ' ' << o.returns_surrogate_ids() << endl;

    if( o.returns_streamer_subscription_keys() != true )
        throw runtime_error("invalid returns streamer subscription keys");
    if( o.returns_streamer_connection_info() != true )
        throw runtime_error("invalid returns streamer connection info");
    if( o.returns_preferences() != true )
        throw runtime_error("invalid returns preferences");
    if( o.returns_surrogate_ids() != true )
        throw runtime_error("invalid returns surrogate_ids");

    Get(o);
}

void
subscription_keys_getter(string id, Credentials& c)
{
    StreamerSubscriptionKeysGetter o(c, id);
    cout<< o.get_account_id() << endl;

    if( o.get_account_id() != id )
        throw runtime_error("invalid account id");

    Get(o);
}


void
movers_getter(Credentials& c)
{
    MoversGetter o(c, MoversIndex::compx, MoversDirectionType::up,
                   MoversChangeType::value);
    cout<< o.get_index() << ' ' << o.get_direction_type() << ' '
        << o.get_change_type() << endl;

    if( o.get_index() != MoversIndex::compx )
        throw runtime_error("invalid index");
    if( o.get_direction_type() != MoversDirectionType::up )
        throw runtime_error("invalid direction type");
    if( o.get_change_type() != MoversChangeType::value )
        throw runtime_error("invalid change type");

    Get(o);

    o.set_index(MoversIndex::dji);
    o.set_direction_type(MoversDirectionType::up_and_down);
    o.set_change_type(MoversChangeType::percent);
    cout<< o.get_index() << ' ' << o.get_direction_type() << ' '
        << o.get_change_type() << endl;

    if( o.get_index() != MoversIndex::dji )
        throw runtime_error("invalid index");
    if( o.get_direction_type() != MoversDirectionType::up_and_down )
        throw runtime_error("invalid direction type");
    if( o.get_change_type() != MoversChangeType::percent )
        throw runtime_error("invalid change type");

    Get(o);
}


void
market_hours_getter(Credentials& c)
{
    time_t t = time(0);
    struct tm *time_struct = localtime(&t);  
    string yr_str = std::to_string(time_struct->tm_year + 1900 + 1);

    MarketHoursGetter o(c, MarketType::bond, yr_str + "-07-04");
    cout<< o.get_date() << ' ' << o.get_market_type() << endl;

    if( o.get_date() != yr_str + "-07-04" )
        throw runtime_error("invalid date");
    if( o.get_market_type() != MarketType::bond )
        throw runtime_error("invalid market type");

    Get(o);

    o.set_date(yr_str + "-07-05");
    o.set_market_type(MarketType::equity);
    cout<< o.get_date() << ' ' << o.get_market_type() << endl;

    if( o.get_date() != yr_str + "-07-05" )
        throw runtime_error("invalid date");
    if( o.get_market_type() != MarketType::equity )
        throw runtime_error("invalid market type");

    Get(o);
}


void
instrument_info_getter(Credentials& c)
{
    InstrumentInfoGetter o(c, InstrumentSearchType::symbol_exact, "SPY");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "SPY" )
        throw runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::symbol_exact )
        throw runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::symbol_regex, "GOOGL*");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "GOOGL*" )
        throw runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::symbol_regex)
        throw runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::description_regex, "SPDR S.+P 500.*");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "SPDR S.+P 500.*" )
        throw runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::description_regex)
        throw runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::cusip, "78462F103");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "78462F103" )
        throw runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::cusip)
        throw runtime_error("invalid search type");

    Get(o);
}


ostream&
operator<<(ostream& out, const OptionChainGetter& ocg)
{
    out<< ocg.get_symbol() << ' ' << ocg.get_strikes() << ' '
       << ocg.get_contract_type() << ' ' << ocg.includes_quotes() << ' '
       << ocg.get_from_date() << ' ' << ocg.get_to_date() << ' '
       << ocg.get_exp_month() << ' ' << ocg.get_option_type();
    return out;
}

ostream&
operator<<(ostream& out, const OptionChainStrategyGetter &ocsg)
{
    out << reinterpret_cast<const OptionChainGetter&>(ocsg);
    out<< ' ' << ocsg.get_strategy();
    return out;
}

ostream&
operator<<(ostream& out, const OptionChainAnalyticalGetter &ocag)
{
    out << reinterpret_cast<const OptionChainGetter&>(ocag);
    out<< ' ' << ocag.get_volatility() << ' ' << ocag.get_underlying_price()
       << ' ' << ocag.get_interest_rate() << ' ' << ocag.get_days_to_exp();
    return out;
}

void
check_option_chain_getter( const OptionChainGetter& o,
                               const string& symbol,
                               const OptionStrikes& strikes,
                               OptionContractType contract_type,
                               bool include_quotes,
                               const string& from_date,
                               const string& to_date,
                               OptionExpMonth exp_month,
                               OptionType option_type)
{
    if( o.get_symbol() != symbol )
        throw runtime_error("invalid symbol");
    if( o.get_strikes() != strikes )
        throw runtime_error("invalid strikes");
    if( o.get_contract_type() != contract_type )
        throw runtime_error("invalid contract type");
    if( o.includes_quotes() != include_quotes )
        throw runtime_error("invalid include quotes");
    if( o.get_from_date() != from_date )
        throw runtime_error("invalid from date");
    if( o.get_to_date() != to_date )
        throw runtime_error("invalid to date");
    if( o.get_exp_month() != exp_month )
        throw runtime_error("invalid exp month");
    if( o.get_option_type() != option_type )
        throw runtime_error("invalid option type");
}


void option_chain_analytical_getter(Credentials& c)
{
    auto strikes = OptionStrikes::Single(65.00);
    OptionChainAnalyticalGetter ocg(c, "kORS", 30.50, 65.00, 3.0, 100,
                                    strikes, OptionContractType::call, true,
                                    "", "", OptionExpMonth::jan);
    cout<<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              true, "", "", OptionExpMonth::jan, OptionType::all);
    if( ocg.get_volatility() != 30.50 )
        throw runtime_error("invalid volatility");
    if( ocg.get_underlying_price() != 65.00 )
            throw runtime_error("invalid underlying price");
    if( ocg.get_interest_rate() != 3.0 )
            throw runtime_error("invalid interest rate");
    if( ocg.get_days_to_exp() != 100 )
            throw runtime_error("invalid days to exp");

    Get(ocg);

    strikes = OptionStrikes::Range(OptionRangeType::sbk);
    ocg.set_volatility(40.5555);
    ocg.set_underlying_price(70.25);
    ocg.set_interest_rate(1.001);
    ocg.set_days_to_exp(1);
    ocg.set_strikes( strikes );
    ocg.set_contract_type( OptionContractType::put );
    ocg.set_to_date("2020-01-01");
    cout<<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::put,
                              true, "", "2020-01-01", OptionExpMonth::jan,
                              OptionType::all);
    if( ocg.get_volatility() != 40.5555 )
        throw runtime_error("invalid volatility");
    if( ocg.get_underlying_price() != 70.25 )
            throw runtime_error("invalid underlying price");
    if( ocg.get_interest_rate() != 1.001 )
            throw runtime_error("invalid interest rate");
    if( ocg.get_days_to_exp() != 1 )
            throw runtime_error("invalid days to exp");

    Get(ocg);
}


void option_chain_strategy_getter(Credentials& c)
{
    auto strikes = OptionStrikes::N_ATM(2);
    auto strategy = OptionStrategy::Vertical();
    OptionChainStrategyGetter ocg(c, "KOrS", strategy, strikes,
                                  OptionContractType::call, false, "","",
                                  OptionExpMonth::jan);
    cout <<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              false, "", "", OptionExpMonth::jan,
                              OptionType::all);
    if( ocg.get_strategy() != strategy )
        throw runtime_error("invalid strategy");

    Get(ocg);

    OptionChainStrategyGetter ocg2(move(ocg));
    ocg = move(ocg2);

    strikes = OptionStrikes::Single(70.00);
    strategy = OptionStrategy::Calendar();
    ocg.set_strategy(strategy);
    ocg.set_strikes(strikes);
    ocg.set_contract_type(OptionContractType::put);
    cout<< ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::put,
                              false, "", "", OptionExpMonth::jan,
                              OptionType::all);
    if( ocg.get_strategy() != strategy )
        throw runtime_error("invalid strategy");

    Get(ocg);

    strikes = OptionStrikes::Range(OptionRangeType::otm);
    strategy = OptionStrategy::Condor(4);
    ocg.set_strikes(strikes);
    ocg.set_exp_month(OptionExpMonth::all);
    ocg.set_from_date("2019-01-18");
    ocg.set_to_date("2019-02-15");
    ocg.set_strategy(strategy);
    ocg.include_quotes(true);
    ocg.set_option_type(OptionType::s);
    cout<< ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::put,
                              true, "2019-01-18", "2019-02-15",
                              OptionExpMonth::all, OptionType::s);
    if( ocg.get_strategy() != strategy )
        throw runtime_error("invalid strategy");

    Get(ocg);
}

void option_chain_getter(Credentials& c)
{
    auto strikes = OptionStrikes::N_ATM(1);
    OptionChainGetter ocg(c, "kors", strikes, OptionContractType::call,
                          false, "2019-01-18", "2019-02-15");
    cout<<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              false, "2019-01-18", "2019-02-15",
                              OptionExpMonth::all, OptionType::all);

    Get(ocg);

    strikes = OptionStrikes::Single(70.00);
    ocg.set_strikes(strikes);
    ocg.set_exp_month(OptionExpMonth::jul);
    ocg.include_quotes(true);
    cout<< ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              true, "2019-01-18", "2019-02-15",
                              OptionExpMonth::jul, OptionType::all);

    Get(ocg);

    strikes = OptionStrikes::Range(OptionRangeType::otm);
    ocg.set_strikes(strikes);
    ocg.set_option_type(OptionType::ns);
    ocg.include_quotes(false);
    cout<< ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              false, "2019-01-18", "2019-02-15",
                              OptionExpMonth::jul, OptionType::ns);

    Get(ocg);
}


ostream&
operator<<(ostream& out, const HistoricalGetterBase &hg)
{
    out<< ' ' << hg.get_symbol() << ' ' << hg.get_frequency_type()
       << ' ' << hg.get_frequency() << ' ' << boolalpha
       << hg.is_extended_hours();
    return out;
}

ostream& // TODO UPDATE
operator<<(ostream& out, const HistoricalPeriodGetter &hpg)
{
    out << reinterpret_cast<const HistoricalGetterBase&>(hpg);
    out<< ' ' << hpg.get_period_type() << ' ' << hpg.get_period()
        << ' ' << hpg.get_msec_since_epoch();
    return out;
}

ostream&
operator<<(ostream& out, const HistoricalRangeGetter &hrg)
{
    out << reinterpret_cast<const HistoricalGetterBase&>(hrg);
    out<< ' ' << hrg.get_start_msec_since_epoch()
        << ' ' << hrg.get_end_msec_since_epoch();
    return out;
}

void
historical_getters(Credentials& c)
{
    using namespace std::chrono;
    auto tp_now = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );
    auto tp_tomorrow = tp_now + hours(24);
    auto tp_3ago = tp_now - hours(24*3);
    auto tp_70ago = tp_now - hours(24*70);

    HistoricalPeriodGetter hpg(c, "SPY", PeriodType::month, 1,
                               FrequencyType::daily, 1, true,
                               tp_tomorrow.count());
    cout<< hpg<< endl;

    if( hpg.get_symbol() != "SPY" ){
        throw runtime_error("invalid symbol");
    }
    if( hpg.get_period_type() != PeriodType::month ){
        throw runtime_error("invalid period type");
    }
    if( hpg.get_period() != 1 ){
        throw runtime_error("invalid period");
    }
    if( hpg.get_frequency_type() != FrequencyType::daily ){
        throw runtime_error("invalid frequency type");
    }
    if( hpg.get_frequency() != 1 ){
        throw runtime_error("invalid frequency");
    }
    if( hpg.is_extended_hours() != true ){
        throw runtime_error("invalid is extended hours");
    }
    if( hpg.get_msec_since_epoch() != tp_tomorrow.count() ){
        throw runtime_error("invalid msec_since_epoch");
    }

    Get(hpg);

    hpg.set_symbol("QQQ");
    hpg.set_period(PeriodType::day, 3);
    hpg.set_frequency(FrequencyType::minute, 30);
    hpg.set_extended_hours(false);
    hpg.set_msec_since_epoch(0);
    cout<< hpg << endl;
    if( hpg.get_symbol() != "QQQ" ){
        throw runtime_error("invalid symbol");
    }
    if( hpg.get_period_type() != PeriodType::day ){
        throw runtime_error("invalid period type");
    }
    if( hpg.get_period() != 3 ){
        throw runtime_error("invalid period");
    }
    if( hpg.get_frequency_type() != FrequencyType::minute ){
        throw runtime_error("invalid frequency type");
    }
    if( hpg.get_frequency() != 30 ){
        throw runtime_error("invalid frequency");
    }
    if( hpg.is_extended_hours() != false ){
        throw runtime_error("invalid is extended hours");
    }
    if( hpg.get_msec_since_epoch() != 0 ){
        throw runtime_error("invalid msec_since_epoch");
    }

    hpg.set_frequency(FrequencyType::monthly, 1); // bad but cant throw here
    try{
        Get(hpg, true, "failed to catch exception for bad frequency type");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }
    hpg.set_frequency(FrequencyType::minute, 30);

    try{
        hpg.set_period(PeriodType::month, 99);
        throw runtime_error("failed to catch exception for bad period");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }

    /* 10-25-18 */
    hpg.set_msec_since_epoch(tp_3ago.count());
    if( hpg.get_msec_since_epoch() != tp_3ago.count() ){
        throw runtime_error("invalid msec_since_epoch");
    }

    hpg.set_msec_since_epoch(tp_3ago.count() * -1);
    if( hpg.get_msec_since_epoch() != (tp_3ago.count()*-1) ){
        throw runtime_error("invalid msec_since_epoch");
    }

    cout<< hpg << endl;
    Get(hpg);

    HistoricalPeriodGetter hpg2(c, "SPY", PeriodType::month, 1,
                                   FrequencyType::daily, 1, true);
    if( hpg2.get_msec_since_epoch() != 0 ){
        throw runtime_error("invalid msec_since_epoch");
    }

    unsigned long long end = tp_tomorrow.count();
    unsigned long long start = tp_3ago.count();
    //unsigned long day_msec = 86400000;
    HistoricalRangeGetter hrg(c, "SPY", FrequencyType::minute, 30,
                               start, end, true);
    cout<< hrg << endl;

    if( hrg.get_symbol() != "SPY" ){
        throw runtime_error("invalid symbol");
    }
    if( hrg.get_frequency_type() != FrequencyType::minute ){
        throw runtime_error("invalid frequency type");
    }
    if( hrg.get_frequency() != 30 ){
        throw runtime_error("invalid frequency");
    }
    if( hrg.get_start_msec_since_epoch() != start ){
        throw runtime_error("invalid start msec since epoch");
    }
    if( hrg.get_end_msec_since_epoch() != end ){
        throw runtime_error("invalid end msec since epoch");
    }
    if( hrg.is_extended_hours() != true ){
        throw runtime_error("invalid is extended hours");
    }

    Get(hrg);

    try{
        hrg.set_frequency(FrequencyType::minute, 31);
        throw runtime_error("failed to catch exception for bad frequency");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }

    start = tp_70ago.count();
    hrg.set_start_msec_since_epoch(start);
    hrg.set_frequency(FrequencyType::monthly, 1);
    cout<< hrg << endl;
    if( hrg.get_symbol() != "SPY" ){
        throw runtime_error("invalid symbol");
    }
    if( hrg.get_frequency_type() != FrequencyType::monthly){
        throw runtime_error("invalid frequency type");
    }
    if( hrg.get_frequency() != 1 ){
        throw runtime_error("invalid frequency");
    }
    if( hrg.get_start_msec_since_epoch() != start ){
        throw runtime_error("invalid start msec since epoch");
    }
    if( hrg.get_end_msec_since_epoch() != end ){
        throw runtime_error("invalid end msec since epoch");
    }
    if( hrg.is_extended_hours() != true ){
        throw runtime_error("invalid is extended hours");
    }

    auto j = GetJson(hrg);
    if( use_live_connection ){
        auto ji = j.find("candles");
        if( ji == j.end() )
            throw runtime_error("no historical candles in json");
        //if( ji->size() != 2 )
        //    throw runtime_error("not 2 monthly candles in json");
    }
}


void
quote_getters(Credentials& c)
{
    QuoteGetter qg(c, "spy");
    cout<< qg.get_symbol() << endl;

    if( qg.get_symbol() != "SPY")
        throw runtime_error("invalid symbol");

    if( qg.get_timeout() != std::chrono::milliseconds(0) )
        throw runtime_error("invalid timeout");

    qg.set_timeout(std::chrono::milliseconds(3001));
    if( qg.get_timeout() != std::chrono::milliseconds(3001) )
        throw runtime_error("invalid timeout(2)");

    Get(qg);

    QuoteGetter qg2(move(qg));
    qg2.set_symbol("QQQ");
    cout<< qg2.get_symbol() << endl;

    if( qg2.get_symbol() != "QQQ")
        throw runtime_error("invalid symbol");

    Get(qg2);

    QuotesGetter qsg(c, {"spy", "qqq"});
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;
    if( qsg.get_symbols() != set<string>{"SPY","QQQ"} )
        throw runtime_error("invalid symbol");

    Get(qsg);

    qsg.set_symbols( {"/es","SPY_081718C276"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276"}  )
        throw runtime_error("invalid symbol");

    Get(qsg);

    qsg.add_symbol("IWm");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276", "IWM"})
        throw runtime_error("invalid symbols in quotes getter");

    Get(qsg);

    qsg.remove_symbol("/ES");
    qsg.remove_symbol("SPY_081718C276");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"IWM"})
        throw runtime_error("invalid symbols in quotes getter");

    Get(qsg);

    qsg.remove_symbol("IWM");
    qsg.remove_symbol("BAD_SYMBOL");

    if( !qsg.get_symbols().empty() )
        throw runtime_error("invalid symbols in quotes getter");
    if( use_live_connection ){
        if( qsg.get() != json() )
            throw runtime_error("empty quotes getter did not return {}");
    }

    qsg.add_symbols( {"xlf","XLY", "XLE"} );
    if( qsg.get_symbols() != set<string>{"XLF","XLY", "XLE"} )
        throw runtime_error("invalid symbols in quotes getter");

    qsg.add_symbol( "XLK" );
    qsg.remove_symbols( {"XLF","XLY", "XLK"} );
    if( qsg.get_symbols() != set<string>{"XLE"} )
        throw runtime_error("invalid symbols in quotes getter");

    qsg.remove_symbols( {"A","B", "XLE"});
    if( !qsg.get_symbols().empty() )
        throw runtime_error("invalid symbols in quotes getter");

    try{
        qsg.add_symbols( {"","SPY"} );
        throw runtime_error("failed to catch exception, add_symbols");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    try{
        qsg.add_symbols( {} );
        throw runtime_error("failed to catch exception, add_symbols");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    try{
        qsg.add_symbol( {""} );
        throw runtime_error("failed to catch exception, add_symbol");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    qsg.add_symbols( {"XLF","XLY", "XLE"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string> {"XLF","XLY", "XLE"})
        throw runtime_error("invalid symbols in quotes getter");

    Get(qsg);
}







