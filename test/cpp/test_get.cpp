
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

    cout<< "*** SET WAIT ***" << endl;
    cout<< APIGetter::get_wait_msec().count() << " --> ";
    APIGetter::set_wait_msec( milliseconds(1500) );
    cout<< APIGetter::get_wait_msec().count() << endl;

    cout<< "*** QUOTE DATA ***" << endl;
    quote_getters(creds);
    cout<< "WaitRemaining: " << APIGetter::wait_remaining().count() << endl;

    historical_getters(creds);
    this_thread::sleep_for( seconds(3) );

    cout<< "*** OPTION DATA ***" << endl;
    option_chain_getter(creds);
    option_chain_strategy_getter(creds);
    option_chain_analytical_getter(creds);
    this_thread::sleep_for( seconds(3) );


    cout<< "*** MARKET DATA ***" << endl;
    instrument_info_getter(creds);
    market_hours_getter(creds);
    movers_getter(creds);
    this_thread::sleep_for( seconds(3) );

    cout<< "*** ACCOUNT DATA ***" << endl;
    account_info_getter(account_id, creds);
    preferences_getter(account_id, creds);
    user_principals_getter(creds);
    subscription_keys_getter(account_id, creds);
    this_thread::sleep_for( seconds(3) );


    cout<< "*** TRANSACTION DATA ***" << endl;
    try{
        transaction_history_getter(account_id, creds);
        individual_transaction_history_getter(account_id, creds);
    }catch( ServerError& e){
     cout<<"caught server error: " << e.what() << " ("
         << e.status_code() << ")" << endl;
    }

}

void
transaction_history_getter(string id, Credentials& c)
{
    TransactionHistoryGetter o(c, id, TransactionType::trade, "SPy");
    if( o.get_symbol() != "SPY")
        throw std::runtime_error("invalid symbol");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;
    cout<< o.get() << endl << endl;

    o.set_transaction_type(TransactionType::trade);
    o.set_symbol("");
    o.set_start_date("2018-01-01");
    cout<< o.get_account_id() << ' ' << o.get_transaction_type() << ' '
        << o.get_symbol() << ' ' << o.get_start_date() << ' '
        << o.get_end_date() << endl;
    cout<< o.get() << endl << endl;
}

void
individual_transaction_history_getter(string id, Credentials& c)
{
    IndividualTransactionHistoryGetter o(c, id, "093432432"); // bad id
    cout<< o.get_account_id() << ' ' << o.get_transaction_id() << endl;
    try{
        cout<< o.get() << endl << endl;
    }catch(APIException& e){
        cout<< "successfully caught: " << e.what() << endl << endl;
    }
}

void
account_info_getter(string id, Credentials& c)
{
    AccountInfoGetter o(c, id, false, false);
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;
    cout<< o.get() << endl << endl;

    o.return_orders(true);
    o.return_positions(true);
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;
    cout<< o.get() << endl << endl;

    o.set_account_id("BAD_ACCOUNT_ID");
    cout<< o.get_account_id() << ' ' << o.returns_positions() << ' '
        << o.returns_orders() << endl;
    try{
        cout<< o.get() << endl << endl;
    }catch(APIException& e){
        cout<< "successfully caught: " << e.what() << endl << endl;
    }
}


void
preferences_getter(string id, Credentials& c)
{
    PreferencesGetter o(c, id);
    cout<< o.get_account_id() << endl;
    cout<< o.get() << endl << endl;
}


void
user_principals_getter(Credentials& c)
{
    UserPrincipalsGetter o(c, false, false, false, false);
    cout<< o.returns_streamer_subscription_keys()
        << ' ' << o.returns_streamer_connection_info() << ' '
        << o.returns_preferences() << ' ' << o.returns_surrogate_ids() << endl;
    cout<< o.get() << endl << endl;

    o.return_preferences(true);
    o.return_streamer_connection_info(true);
    o.return_streamer_subscription_keys(true);
    o.return_surrogate_ids(true);
    cout<<  o.returns_streamer_subscription_keys()
        << ' ' << o.returns_streamer_connection_info() << ' '
        << o.returns_preferences() << ' ' << o.returns_surrogate_ids() << endl;
    cout<< o.get() << endl << endl;
}

void
subscription_keys_getter(string id, Credentials& c)
{
    StreamerSubscriptionKeysGetter o(c, id);
    cout<< o.get_account_id() << endl;
    cout<< o.get() << endl << endl;
}


void
movers_getter(Credentials& c)
{
    MoversGetter o(c, MoversIndex::compx, MoversDirectionType::up,
                   MoversChangeType::value);
    cout<< o.get_index() << ' ' << o.get_direction_type() << ' '
        << o.get_change_type() << endl << o.get() << endl << endl;

    o.set_index(MoversIndex::dji);
    o.set_direction_type(MoversDirectionType::up_and_down);
    o.set_change_type(MoversChangeType::percent);
    cout<< o.get_index() << ' ' << o.get_direction_type() << ' '
        << o.get_change_type() << endl << o.get() << endl << endl;
}


void
market_hours_getter(Credentials& c)
{
    MarketHoursGetter o(c, MarketType::bond, "2019-07-04");
    cout<< o.get_date() << ' ' << o.get_market_type() << endl
        << o.get() << endl << endl;

    o.set_date("2019-07-05");
    o.set_market_type(MarketType::equity);
    cout<< o.get_date() << ' ' << o.get_market_type() << endl
        << o.get() << endl << endl;
}


void
instrument_info_getter(Credentials& c)
{
    InstrumentInfoGetter o(c, InstrumentSearchType::symbol_exact, "SPY");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;
    cout<< o.get() << endl << endl;

    o.set_query(InstrumentSearchType::symbol_regex, "GOOGL?");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;
    cout<< o.get() << endl << endl;

    o.set_query(InstrumentSearchType::description_regex, ""
                "SPDR S.+P 500.*");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;
    cout<< o.get() << endl << endl;

    o.set_query(InstrumentSearchType::cusip, "78462F103");
    cout<< o.get_query_string() << ' ' << o.get_search_type() << endl;
    cout<< o.get() << endl << endl;
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


void option_chain_analytical_getter(Credentials& c)
{
    OptionChainAnalyticalGetter ocg(c, "kORS", 30.00, 65.00, 3.0, 100,
                                    OptionStrikes::Single(65.00),
                                    OptionContractType::call, true,
                                    "", "", OptionExpMonth::aug);
    if( ocg.get_symbol() != "KORS")
        throw std::runtime_error("invalid symbol");
    cout<<ocg << endl << ocg.get() << endl << endl;

    ocg.set_volatility(40.00);
    ocg.set_underlying_price(70.00);
    ocg.set_interest_rate(1.0);
    ocg.set_days_to_exp(1);
    ocg.set_strikes( OptionStrikes::Range(OptionRangeType::sbk) );
    ocg.set_contract_type( OptionContractType::put );
    ocg.set_to_date("2020-01-01");
    cout<<ocg << endl << ocg.get() << endl << endl;
}


void option_chain_strategy_getter(Credentials& c)
{
    OptionChainStrategyGetter ocg(c, "KOrS", OptionStrategy::Vertical(),
                                  OptionStrikes::N_ATM(2), OptionContractType::call,
                                  false, "","",OptionExpMonth::aug);
    if( ocg.get_symbol() != "KORS")
        throw std::runtime_error("invalid symbol");
    cout<<ocg << endl << ocg.get() << endl << endl;

    OptionChainStrategyGetter ocg2(move(ocg));
    ocg = move(ocg2);

    ocg.set_strategy(OptionStrategy::Calendar());
    ocg.set_strikes(OptionStrikes::Single(70.00));
    ocg.set_contract_type(OptionContractType::put);
    cout<< ocg << ocg.get() << endl << endl;

    ocg.set_strikes(OptionStrikes::Range(OptionRangeType::otm));
    ocg.set_exp_month(OptionExpMonth::all);
    ocg.set_from_date("2018-06-24");
    ocg.set_to_date("2018-08-24");
    ocg.set_strategy(OptionStrategy::Condor(4));
    cout<< ocg << endl << ocg.get() << endl << endl;
}

void option_chain_getter(Credentials& c)
{
    OptionChainGetter ocg(c, "kors", OptionStrikes::N_ATM(1), OptionContractType::call,
                          false, "2018-06-24", "2018-08-24");
    if( ocg.get_symbol() != "KORS")
        throw std::runtime_error("invalid symbol");
    cout<<ocg << endl << ocg.get() << endl << endl;

    ocg.set_strikes(OptionStrikes::Single(70.00));
    ocg.set_exp_month(OptionExpMonth::jul);
    ocg.include_quotes(true);
    cout<< ocg << endl << ocg.get() << endl << endl;

    ocg.set_strikes(OptionStrikes::Range(OptionRangeType::otm));
    ocg.set_option_type(OptionType::ns);
    ocg.include_quotes(false);
    cout<< ocg << endl << ocg.get() << endl << endl;
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
    cout<< hpg<< endl << hpg.get() << endl << endl;

    hpg.set_symbol("QQQ");
    hpg.set_period(PeriodType::day, 3);
    hpg.set_frequency(FrequencyType::minute, 30);
    if( hpg.get_frequency_type() != FrequencyType::minute ||
        hpg.get_frequency() != 30){
        throw std::runtime_error("failed to set frequency/type");
    }
    hpg.set_extended_hours(false);
    cout<< hpg << hpg.get() << endl << endl;

    hpg.set_frequency(FrequencyType::monthly, 1); // bad but cant throw here
    try{
        hpg.get(); // but can throw here
        throw std::runtime_error("failed to catch exception for bad frequency type");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e.what() << endl;
    }

    try{
        hpg.set_period(PeriodType::month, 99);
        throw std::runtime_error("failed to catch exception for bad period");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e.what() << endl;
    }

    unsigned long long end = 1528205400000;
    unsigned long long start = 1528119000000;
    //unsigned long day_msec = 86400000;
    HistoricalRangeGetter hrg(c, "SPY", FrequencyType::minute, 30,
                               start, end, true);
    cout<< hrg << hrg.get() << endl << endl;

    try{
        hrg.set_frequency(FrequencyType::minute, 31);
        throw std::runtime_error("failed to catch exception for bad frequency");
    }catch(ValueException& e){
        cout<< "succesfully caught: " << e.what() << endl;
    }

    hrg.set_frequency(FrequencyType::monthly, 1);
    if( hrg.get_frequency_type() != FrequencyType::monthly ||
        hrg.get_frequency() != 1){
        throw std::runtime_error("failed to set frequency/type");
    }
}

void
quote_getters(Credentials& c)
{
    QuoteGetter qg(c, "spy");
    if( qg.get_symbol() != "SPY")
        throw std::runtime_error("invalid symbol");
    cout<< qg.get_symbol() << endl << qg.get() << endl << endl;

    QuoteGetter qg2(move(qg));
    qg2.set_symbol("QQQ");

    cout<< qg2.get_symbol() << endl << qg2.get() << endl << endl;

    QuotesGetter qsg(c, {"spy", "qqq"});
    if( qsg.get_symbols() != set<string>{"SPY","QQQ"} )
        throw std::runtime_error("invalid symbol");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.set_symbols( {"/es","SPY_081718C276"} );
    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276"}  )
        throw std::runtime_error("invalid symbol");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.add_symbol("IWm");
    if( qsg.get_symbols() != set<string>{"/ES","SPY_081718C276", "IWM"})
        throw std::runtime_error("invalid symbols in quotes getter");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.remove_symbol("/ES");
    qsg.remove_symbol("SPY_081718C276");
    if( qsg.get_symbols() != set<string>{"IWM"})
        throw std::runtime_error("invalid symbols in quotes getter");
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.remove_symbol("IWM");
    qsg.remove_symbol("BAD_SYMBOL");
    if( !qsg.get_symbols().empty() )
        throw std::runtime_error("invalid symbols in quotes getter");
    if( qsg.get() != json() )
        throw std::runtime_error("empty quotes getter did not return {}");

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
        cout<< "successfully caught exception: " << e.what() << endl;
    }
    try{
        qsg.add_symbols( {} );
        throw std::runtime_error("failed to catch exception, add_symbols");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e.what() << endl;
    }
    try{
        qsg.add_symbol( {""} );
        throw std::runtime_error("failed to catch exception, add_symbol");
    }catch( ValueException& e){
        cout<< "successfully caught exception: " << e.what() << endl;
    }

    qsg.add_symbols( {"XLF","XLY", "XLE"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;
}


