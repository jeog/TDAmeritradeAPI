
#include <iostream>
#include <chrono>

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

void account_info_getter(std::string id, Credentials& c);
void preferences_getter(std::string id, Credentials& c);
void user_principals_getter(Credentials& c);
void subscription_keys_getter(std::string id, Credentials& c);
void transaction_history_getter(std::string id, Credentials& c);
void individual_transaction_history_getter(std::string id, Credentials& c);

void
test_getters(const string& account_id, Credentials& creds)
{
    using namespace chrono;

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
    this_thread::sleep_for( seconds(3) );


    cout<< endl << "*** TRANSACTION DATA ***" << endl;
    try{
        transaction_history_getter(account_id, creds);
        individual_transaction_history_getter(account_id, creds);
    }catch( ServerError& e){
        cout<<"caught server error: " << e << endl;
    }
}

void
Get(APIGetter& getter)
{
    if( use_live_connection )
        cout<< getter.get() << endl << endl;
    else
        cout<< "CAN NOT TEST GET WITHOUT USING LIVE CONNECTION" << endl;
}

void
transaction_history_getter(string id, Credentials& c)
{
    TransactionHistoryGetter o(c, id, TransactionType::all, "SPy");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;

    if( o.get_symbol() != "SPY")
        throw std::runtime_error("invalid symbol");
    if( o.get_account_id() != id )
        throw std::runtime_error("invalid account id");
    if( o.get_transaction_type() != TransactionType::all )
        throw std::runtime_error("invalid transaction type");
    if( o.get_start_date() != "" )
        throw std::runtime_error("invalid start date");
    if( o.get_end_date() != "" )
        throw std::runtime_error("invalid end date");

    Get(o);

    o.set_transaction_type(TransactionType::trade);
    o.set_symbol("");
    o.set_start_date("2018-01-01");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;

    if( o.get_symbol() != "")
        throw std::runtime_error("invalid symbol");
    if( o.get_account_id() != id )
        throw std::runtime_error("invalid account id");
    if( o.get_transaction_type() != TransactionType::trade )
        throw std::runtime_error("invalid transaction type");
    if( o.get_start_date() != "2018-01-01" )
        throw std::runtime_error("invalid start date");
    if( o.get_end_date() != "" )
        throw std::runtime_error("invalid end date");

    Get(o);
}

void
individual_transaction_history_getter(string id, Credentials& c)
{
    IndividualTransactionHistoryGetter o(c, id, "093432432"); // bad id
    cout<< o.get_account_id() << ' ' << o.get_transaction_id() << endl;
    if( o.get_account_id() != id )
        throw std::runtime_error("invalid account id");
    if( o.get_transaction_id() != "093432432" )
        throw std::runtime_error("invalid transaction id");

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
        throw std::runtime_error("invalid account id");
    if( o.returns_positions() != false )
        throw std::runtime_error("invalid returns positions");
    if( o.returns_orders() != false )
        throw std::runtime_error("invalid returns orders");

    Get(o);

    o.return_orders(true);
    o.return_positions(true);
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;

    if( o.get_account_id() != id )
        throw std::runtime_error("invalid account id");
    if( o.returns_positions() != true )
        throw std::runtime_error("invalid returns positions");
    if( o.returns_orders() != true )
        throw std::runtime_error("invalid returns orders");

    Get(o);

    o.set_account_id("BAD_ACCOUNT_ID");
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;

    if( o.get_account_id() != "BAD_ACCOUNT_ID" )
        throw std::runtime_error("invalid account id");
    if( o.returns_positions() != true )
        throw std::runtime_error("invalid returns positions");
    if( o.returns_orders() != true )
        throw std::runtime_error("invalid returns orders");

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
        throw std::runtime_error("invalid account id");

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
        throw std::runtime_error("invalid returns streamer subscription keys");
    if( o.returns_streamer_connection_info() != false )
        throw std::runtime_error("invalid returns streamer connection info");
    if( o.returns_preferences() != false )
        throw std::runtime_error("invalid returns preferences");
    if( o.returns_surrogate_ids() != false )
        throw std::runtime_error("invalid returns surrogate_ids");

    Get(o);

    o.return_preferences(true);
    o.return_streamer_connection_info(true);
    o.return_streamer_subscription_keys(true);
    o.return_surrogate_ids(true);
    cout<<  o.returns_streamer_subscription_keys()
        << ' ' << o.returns_streamer_connection_info() << ' '
        << o.returns_preferences() << ' ' << o.returns_surrogate_ids() << endl;

    if( o.returns_streamer_subscription_keys() != true )
        throw std::runtime_error("invalid returns streamer subscription keys");
    if( o.returns_streamer_connection_info() != true )
        throw std::runtime_error("invalid returns streamer connection info");
    if( o.returns_preferences() != true )
        throw std::runtime_error("invalid returns preferences");
    if( o.returns_surrogate_ids() != true )
        throw std::runtime_error("invalid returns surrogate_ids");

    Get(o);
}

void
subscription_keys_getter(string id, Credentials& c)
{
    StreamerSubscriptionKeysGetter o(c, id);
    cout<< o.get_account_id() << endl;

    if( o.get_account_id() != id )
        throw std::runtime_error("invalid account id");

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
        throw std::runtime_error("invalid index");
    if( o.get_direction_type() != MoversDirectionType::up )
        throw std::runtime_error("invalid direction type");
    if( o.get_change_type() != MoversChangeType::value )
        throw std::runtime_error("invalid change type");

    Get(o);

    o.set_index(MoversIndex::dji);
    o.set_direction_type(MoversDirectionType::up_and_down);
    o.set_change_type(MoversChangeType::percent);
    cout<< o.get_index() << ' ' << o.get_direction_type() << ' '
        << o.get_change_type() << endl;

    if( o.get_index() != MoversIndex::dji )
        throw std::runtime_error("invalid index");
    if( o.get_direction_type() != MoversDirectionType::up_and_down )
        throw std::runtime_error("invalid direction type");
    if( o.get_change_type() != MoversChangeType::percent )
        throw std::runtime_error("invalid change type");

    Get(o);
}


void
market_hours_getter(Credentials& c)
{
    MarketHoursGetter o(c, MarketType::bond, "2019-07-04");
    cout<< o.get_date() << ' ' << o.get_market_type() << endl;

    if( o.get_date() != "2019-07-04" )
        throw std::runtime_error("invalid date");
    if( o.get_market_type() != MarketType::bond )
        throw std::runtime_error("invalid market type");

    Get(o);

    o.set_date("2019-07-05");
    o.set_market_type(MarketType::equity);
    cout<< o.get_date() << ' ' << o.get_market_type() << endl;

    if( o.get_date() != "2019-07-05" )
        throw std::runtime_error("invalid date");
    if( o.get_market_type() != MarketType::equity )
        throw std::runtime_error("invalid market type");

    Get(o);
}


void
instrument_info_getter(Credentials& c)
{
    InstrumentInfoGetter o(c, InstrumentSearchType::symbol_exact, "SPY");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "SPY" )
        throw std::runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::symbol_exact )
        throw std::runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::symbol_regex, "GOOGL?");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "GOOGL?" )
        throw std::runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::symbol_regex)
        throw std::runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::description_regex, "SPDR S.+P 500.*");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "SPDR S.+P 500.*" )
        throw std::runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::description_regex)
        throw std::runtime_error("invalid search type");

    Get(o);

    o.set_query(InstrumentSearchType::cusip, "78462F103");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;

    if( o.get_query_string() != "78462F103" )
        throw std::runtime_error("invalid query string");
    if( o.get_search_type() != InstrumentSearchType::cusip)
        throw std::runtime_error("invalid search type");

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
                               const std::string& symbol,
                               const OptionStrikes& strikes,
                               OptionContractType contract_type,
                               bool include_quotes,
                               const std::string& from_date,
                               const std::string& to_date,
                               OptionExpMonth exp_month,
                               OptionType option_type)
{
    if( o.get_symbol() != symbol )
        throw std::runtime_error("invalid symbol");
    if( o.get_strikes() != strikes )
        throw std::runtime_error("invalid strikes");
    if( o.get_contract_type() != contract_type )
        throw std::runtime_error("invalid contract type");
    if( o.includes_quotes() != include_quotes )
        throw std::runtime_error("invalid include quotes");
    if( o.get_from_date() != from_date )
        throw std::runtime_error("invalid from date");
    if( o.get_to_date() != to_date )
        throw std::runtime_error("invalid to date");
    if( o.get_exp_month() != exp_month )
        throw std::runtime_error("invalid exp month");
    if( o.get_option_type() != option_type )
        throw std::runtime_error("invalid option type");
}


void option_chain_analytical_getter(Credentials& c)
{
    auto strikes = OptionStrikes::Single(65.00);
    OptionChainAnalyticalGetter ocg(c, "kORS", 30.00, 65.00, 3.0, 100,
                                    strikes, OptionContractType::call, true,
                                    "", "", OptionExpMonth::jan);
    cout<<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::call,
                              true, "", "", OptionExpMonth::jan, OptionType::all);
    if( ocg.get_volatility() != 30.00 )
        throw std::runtime_error("invalid volatility");
    if( ocg.get_underlying_price() != 65.00 )
            throw std::runtime_error("invalid underlying price");
    if( ocg.get_interest_rate() != 3.0 )
            throw std::runtime_error("invalid interest rate");
    if( ocg.get_days_to_exp() != 100 )
            throw std::runtime_error("invalid days to exp");

    Get(ocg);

    strikes = OptionStrikes::Range(OptionRangeType::sbk);
    ocg.set_volatility(40.00);
    ocg.set_underlying_price(70.00);
    ocg.set_interest_rate(1.0);
    ocg.set_days_to_exp(1);
    ocg.set_strikes( strikes );
    ocg.set_contract_type( OptionContractType::put );
    ocg.set_to_date("2020-01-01");
    cout<<ocg << endl;

    check_option_chain_getter(ocg, "KORS", strikes, OptionContractType::put,
                              true, "", "2020-01-01", OptionExpMonth::jan,
                              OptionType::all);
    if( ocg.get_volatility() != 40.00 )
        throw std::runtime_error("invalid volatility");
    if( ocg.get_underlying_price() != 70.00 )
            throw std::runtime_error("invalid underlying price");
    if( ocg.get_interest_rate() != 1.0 )
            throw std::runtime_error("invalid interest rate");
    if( ocg.get_days_to_exp() != 1 )
            throw std::runtime_error("invalid days to exp");

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
        throw std::runtime_error("invalid strategy");

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
        throw std::runtime_error("invalid strategy");

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
        throw std::runtime_error("invalid strategy");

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

ostream&
operator<<(ostream& out, const HistoricalPeriodGetter &hpg)
{
    out << reinterpret_cast<const HistoricalGetterBase&>(hpg);
    out<< ' ' << hpg.get_period_type() << ' ' << hpg.get_period();
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
    HistoricalPeriodGetter hpg(c, "SPY", PeriodType::month, 1,
                               FrequencyType::daily, 1, true);
    cout<< hpg<< endl;

    if( hpg.get_symbol() != "SPY" ){
        throw std::runtime_error("invalid symbol");
    }
    if( hpg.get_period_type() != PeriodType::month ){
        throw std::runtime_error("invalid period type");
    }
    if( hpg.get_period() != 1 ){
        throw std::runtime_error("invalid period");
    }
    if( hpg.get_frequency_type() != FrequencyType::daily ){
        throw std::runtime_error("invalid frequency type");
    }
    if( hpg.get_frequency() != 1 ){
        throw std::runtime_error("invalid frequency");
    }
    if( hpg.is_extended_hours() != true ){
        throw std::runtime_error("invalid is extended hours");
    }

    Get(hpg);

    hpg.set_symbol("QQQ");
    hpg.set_period(PeriodType::day, 3);
    hpg.set_frequency(FrequencyType::minute, 30);
    hpg.set_extended_hours(false);
    cout<< hpg;
    if( hpg.get_symbol() != "QQQ" ){
        throw std::runtime_error("invalid symbol");
    }
    if( hpg.get_period_type() != PeriodType::day ){
        throw std::runtime_error("invalid period type");
    }
    if( hpg.get_period() != 3 ){
        throw std::runtime_error("invalid period");
    }
    if( hpg.get_frequency_type() != FrequencyType::minute ){
        throw std::runtime_error("invalid frequency type");
    }
    if( hpg.get_frequency() != 30 ){
        throw std::runtime_error("invalid frequency");
    }
    if( hpg.is_extended_hours() != false ){
        throw std::runtime_error("invalid is extended hours");
    }

    Get(hpg);

    hpg.set_frequency(FrequencyType::monthly, 1); // bad but cant throw here
    try{
        if( use_live_connection ){
            cout<< hpg.get() << endl << endl;
            throw std::runtime_error("failed to catch exception for bad frequency type");
        }else{
            cout<< "CAN NOT TEST GET WITHOUT USING LIVE CONNECTION" << endl;
        }
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }

    try{
        hpg.set_period(PeriodType::month, 99);
        throw std::runtime_error("failed to catch exception for bad period");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }

    unsigned long long end = 1528205400000;
    unsigned long long start = 1528119000000;
    //unsigned long day_msec = 86400000;
    HistoricalRangeGetter hrg(c, "SPY", FrequencyType::minute, 30,
                               start, end, true);
    cout<< hrg << endl;

    if( hrg.get_symbol() != "SPY" ){
        throw std::runtime_error("invalid symbol");
    }
    if( hrg.get_frequency_type() != FrequencyType::minute ){
        throw std::runtime_error("invalid frequency type");
    }
    if( hrg.get_frequency() != 30 ){
        throw std::runtime_error("invalid frequency");
    }
    if( hrg.get_start_msec_since_epoch() != start ){
        throw std::runtime_error("invalid start msec since epoch");
    }
    if( hrg.get_end_msec_since_epoch() != end ){
        throw std::runtime_error("invalid end msec since epoch");
    }
    if( hrg.is_extended_hours() != true ){
        throw std::runtime_error("invalid is extended hours");
    }

    Get(hrg);

    try{
        hrg.set_frequency(FrequencyType::minute, 31);
        throw std::runtime_error("failed to catch exception for bad frequency");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e << endl;
    }

    hrg.set_frequency(FrequencyType::monthly, 1);
    if( hrg.get_symbol() != "SPY" ){
        throw std::runtime_error("invalid symbol");
    }
    if( hrg.get_frequency_type() != FrequencyType::monthly){
        throw std::runtime_error("invalid frequency type");
    }
    if( hrg.get_frequency() != 1 ){
        throw std::runtime_error("invalid frequency");
    }
    if( hrg.get_start_msec_since_epoch() != start ){
        throw std::runtime_error("invalid start msec since epoch");
    }
    if( hrg.get_end_msec_since_epoch() != end ){
        throw std::runtime_error("invalid end msec since epoch");
    }
    if( hrg.is_extended_hours() != true ){
        throw std::runtime_error("invalid is extended hours");
    }
}

void
quote_getters(Credentials& c)
{
    QuoteGetter qg(c, "spy");
    cout<< qg.get_symbol() << endl;

    if( qg.get_symbol() != "SPY")
        throw std::runtime_error("invalid symbol");

    Get(qg);

    QuoteGetter qg2(move(qg));
    qg2.set_symbol("QQQ");
    cout<< qg2.get_symbol() << endl;

    if( qg2.get_symbol() != "QQQ")
        throw std::runtime_error("invalid symbol");

    Get(qg2);

    QuotesGetter qsg(c, {"spy", "qqq"});
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;
    if( qsg.get_symbols() != set<string>{"SPY","QQQ"} )
        throw std::runtime_error("invalid symbol");

    Get(qsg);

    qsg.set_symbols( {"/es","SPY_081718C276"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276"}  )
        throw std::runtime_error("invalid symbol");

    Get(qsg);

    qsg.add_symbol("IWm");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276", "IWM"})
        throw std::runtime_error("invalid symbols in quotes getter");

    Get(qsg);

    qsg.remove_symbol("/ES");
    qsg.remove_symbol("SPY_081718C276");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string>{"IWM"})
        throw std::runtime_error("invalid symbols in quotes getter");

    Get(qsg);

    qsg.remove_symbol("IWM");
    qsg.remove_symbol("BAD_SYMBOL");

    if( !qsg.get_symbols().empty() )
        throw std::runtime_error("invalid symbols in quotes getter");
    if( use_live_connection ){
        if( qsg.get() != json() )
            throw std::runtime_error("empty quotes getter did not return {}");
    }

    qsg.add_symbols( {"xlf","XLY", "XLE"} );
    if( qsg.get_symbols() != set<string>{"XLF","XLY", "XLE"} )
        throw std::runtime_error("invalid symbols in quotes getter");

    qsg.add_symbol( "XLK" );
    qsg.remove_symbols( {"XLF","XLY", "XLK"} );
    if( qsg.get_symbols() != set<string>{"XLE"} )
        throw std::runtime_error("invalid symbols in quotes getter");

    qsg.remove_symbols( {"A","B", "XLE"});
    if( !qsg.get_symbols().empty() )
        throw std::runtime_error("invalid symbols in quotes getter");

    try{
        qsg.add_symbols( {"","SPY"} );
        throw std::runtime_error("failed to catch exception, add_symbols");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    try{
        qsg.add_symbols( {} );
        throw std::runtime_error("failed to catch exception, add_symbols");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    try{
        qsg.add_symbol( {""} );
        throw std::runtime_error("failed to catch exception, add_symbol");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e << endl;
    }

    qsg.add_symbols( {"XLF","XLY", "XLE"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl;

    if( qsg.get_symbols() != set<string> {"XLF","XLY", "XLE"})
        throw std::runtime_error("invalid symbols in quotes getter");

    Get(qsg);
}


