#include <tdma_api_get.h>
#include <tdma_api_streaming.h>
#include <iostream>
#include <thread>
#include <algorithm>

using namespace tdma;
using namespace std;

long long msec_since_epoch();

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

void streaming(string id, Credentials& c);

void option_symbol_builder();


int main(int argc, char* argv[])
{
    using namespace chrono;

    if (argc < 4 ) {
        cerr << "invalid # of args" << endl;
        cerr << "  args: [account id] [path to credentials filed] [password]" << endl;
        return 1;
    }
    
    cout<< argv[0] << endl << argv[1] << endl << argv[2] << endl << argv[3] << endl;
    std::string account_id(argv[1]);
    std::string creds_path(argv[2]);
    std::string password(argv[3]);

    if (account_id.empty() || creds_path.empty() || password.empty()) {        
        cerr << "invalid args" << endl;
        cerr << "  args: [account id] [path to credentials filed] [password]" << endl;
        return 1;        
    }

    auto callback = [&]( int cb_type, int ss_type,
                         unsigned long long timestamp, const char* msg )
        {
            cout<< to_string(static_cast<StreamingCallbackType>(cb_type)) << endl
                << "\t service: "
                << to_string(static_cast<StreamerServiceType>(ss_type)) << endl
                << "\t timestamp: " << timestamp << endl
                << "\t content: " << json::parse(msg) << endl << endl;
        };


    //using ft = QuotesSubscription::FieldType;
    //using tsft = TimesaleSubscriptionBase::FieldType;

    try {

        CredentialsManager cmanager(creds_path, password);

        Credentials ccc;
        ccc = cmanager.credentials;
        Credentials ccc2( ccc );

        ccc2 = move(ccc);
        Credentials ccc4( move(ccc2 ));

        /*
        QuotesSubscription spy_quote_sub({"SPY"}, {ft::symbol, ft::last_price});
        TimesaleFuturesSubscription es_quote_sub( {"/ES"}, {tsft::symbol,
                                                           tsft::trade_time,
                                                           tsft::last_price } );

        StreamingSession *ss =
            StreamingSession::Create(cmanager.credentials, account_id, callback);

        ss->start(spy_quote_sub);
        string in;
        cin >> in;
        ss->add_subscription(es_quote_sub);
        cin >> in;
        ss->stop();

        StreamingSession::Destroy(ss);
        */

        cout<< "*** SET WAIT ***" << endl;
        cout<< APIGetter::get_wait_msec().count() << " --> ";
        APIGetter::set_wait_msec( milliseconds(1500) );
        cout<< APIGetter::get_wait_msec().count() << endl;

        cout<< "*** QUOTE DATA ***" << endl;
        quote_getters(cmanager.credentials);
        cout<< "WaitRemaining: " << APIGetter::wait_remaining().count() << endl;

        historical_getters(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        /*
        QuotesSubscription spy_quote_sub({"SPY"}, {ft::symbol, ft::last_price});
        NYSEActivesSubscription actives_sub(DurationType::min_60);

        auto ss = StreamingSession::Create(cmanager.credentials, account_id, callback,
                         milliseconds(3000), milliseconds(15000));

        ss->start( {spy_quote_sub, actives_sub} );
         */

        cout<< "*** OPTION DATA ***" << endl;
        option_chain_getter(cmanager.credentials);
        option_chain_strategy_getter(cmanager.credentials);
        option_chain_analytical_getter(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        /*
        QuotesSubscription qqq_quote_sub({"SPY","QQQ","IWM"},
                                         {ft::symbol, ft::last_size});
        ss->add_subscription(qqq_quote_sub);
        */

        cout<< "*** MARKET DATA ***" << endl;
        instrument_info_getter(cmanager.credentials);
        market_hours_getter(cmanager.credentials);
        movers_getter(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        /*
        try{
            ss->start(spy_quote_sub);
        }catch(StreamingException& e){
            cout<< "succesffuly caught StreamingException" << endl;
        }
        ss->stop();
        */

        cout<< "*** ACCOUNT DATA ***" << endl;
        account_info_getter(account_id, cmanager.credentials);
        preferences_getter(account_id, cmanager.credentials);
        user_principals_getter(cmanager.credentials);
        subscription_keys_getter(account_id, cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        /*
        TimesaleFuturesSubscription sp_quote_sub( {"/ES"}, {tsft::symbol,
                                                           tsft::trade_time,
                                                           tsft::last_price } );
        ss->start( sp_quote_sub );
        */

        cout<< "*** TRANSACTION DATA ***" << endl;
        try{
            transaction_history_getter(account_id, cmanager.credentials);
            individual_transaction_history_getter(account_id, cmanager.credentials);
        }catch( ServerError& e){
            cout<<"caught server error: " << e.what() << " ("
                << e.status_code() << ")" << endl;
        }

        option_symbol_builder();

        streaming(account_id, cmanager.credentials);

        string in;
        cin >> in;
        return 0;

    }catch( tdma::APIException& e ){
        std::cerr<< "APIException: " << e.what() << std::endl;
    }catch( json::exception& e){
        std::cerr<< "json::exception: " << e.what() << std::endl;
    }catch( std::exception& e ){
        std::cerr<< "std::exception: " << e.what() << std::endl;
    }catch( ... ){
        std::cerr<< "unknown exception!" << std::endl;
    }

    return -1;
}

void option_symbol_builder()
{
    auto is_good = [](std::string sym, std::string underlying, unsigned int month,
                      unsigned int day, unsigned int year, bool is_call,
                      double strike){
        std::string o = tdma::BuildOptionSymbol(underlying, month, day, year,
                                                is_call, strike);
        if( o != sym )
            throw std::runtime_error(
                "option strings don't match(" + o + "," + sym + ")"
                );
    };

    auto is_bad= [](std::string underlying, unsigned int month, unsigned int day,
                    unsigned int year, bool is_call, double strike){
        string o;
        try{
            o = tdma::BuildOptionSymbol(underlying, month, day, year,
                                                    is_call, strike);
        }catch(std::exception& e ){
            cout<< "successfully caught exception: " << e.what() << endl;
            return;
        }
        throw std::runtime_error("failed to catch exception for: " + o);
    };

    is_good("SPY_010118C300", "SPY",1,1,2018,true,300);
    is_good("SPY_123199P200", "SPY",12,31,2099,false,200);
    is_good("A_021119P1.5", "A",2,11,2019,false,1.5);
    is_good("A_021119P1.5", "A",2,11,2019,false,1.500);
    is_good("ABCDEF_110121C99.999", "ABCDEF",11,1,2021,true,99.999);
    is_good("ABCDEF_110121C99.999", "ABCDEF",11,1,2021,true,99.99900);
    is_good("ABCDEF_110121C99", "ABCDEF",11,1,2021,true,99.0);
    is_good("ABCDEF_110121C99.001", "ABCDEF",11,1,2021,true,99.001);

    is_bad("",1,1,2018,true,300);
    is_bad("SPY_",1,1,2018,true,300);
    is_bad("SPY_",1,1,2018,true,300);
    is_bad("_SPY",1,1,2018,true,300);
    is_bad("SP_Y",1,1,2018,true,300);
    is_bad("SPY",0,1,2018,true,300);
    is_bad("SPY",13,1,2018,true,300);
    is_bad("SPY",1,0,2018,true,300);
    is_bad("SPY",1,32,2018,true,300);
    is_bad("SPY",1,31,999,true,300);
    is_bad("SPY",1,31,10001,true,300);
    is_bad("SPY",1,31,18,true,300);
    is_bad("SPY",1,31,2018,true,0.0);
    is_bad("SPY",1,31,2018,true,-100.0);
}



template<typename S>
void display_sub(S& sub,
    typename enable_if<is_base_of<SubscriptionBySymbolBase, S>::value, S>::type* _ = nullptr)
{
    cout<< "COMMAND: " << sub.get_command() << endl;
    cout<< "SERVICE: " << to_string(sub.get_service()) << endl;
    cout<< "SYMBOLS: ";
    auto sym = sub.get_symbols();
    for(string s : sym)
        cout<< s << " ";
    cout<<endl;
    cout<< "FIELDS: ";
    auto fields = sub.get_fields();
    for(auto f : fields)
        cout<< f << " ";
    cout<<endl;
}

template<typename S>
void display_sub(S& sub,
    typename enable_if<is_base_of<ActivesSubscriptionBase, S>::value &&
    !is_same<S,OptionActivesSubscription>::value>::type* _ = nullptr)
{
    cout<< "COMMAND: " << sub.get_command() << endl;
    cout<< "SERVICE: " << to_string(sub.get_service()) << endl;
    cout<< "DURATION: " << to_string(sub.get_duration()) << endl;
}


void display_sub(OptionActivesSubscription& sub)
{
    display_sub(reinterpret_cast<ActivesSubscriptionBase&>(sub));
    cout<< "VENUE: " << to_string(sub.get_venue()) << endl;
}

template<typename S, typename F>
void
test_sub_fields_symbols(S& sub, std::string name, std::set<std::string> symbols,
                           std::set<F> fields, StreamerServiceType sst,
                           std::string command="SUBS")
{
    if( symbols != sub.get_symbols() )
        throw std::runtime_error(name + ": bad symbols");
    if( fields != sub.get_fields() )
        throw std::runtime_error(name + ": bad fields");
    if( sst != sub.get_service() )
        throw std::runtime_error(name + ": bad StreamerServiceType");
    if( command != sub.get_command() )
        throw std::runtime_error(name + ": bad command");
}


template<typename S>
void
test_sub_duration(S& sub, std::string name, DurationType dt,
                    StreamerServiceType sst, std::string command="SUBS")
{
    if( dt != sub.get_duration() )
        throw std::runtime_error(name + ": bad duration");
    if( sst != sub.get_service() )
        throw std::runtime_error(name + ": bad StreamerServiceType");
    if( command != sub.get_command() )
        throw std::runtime_error(name + ": bad command");
}

template<typename S>
void
test_sub_duration_venue(S& sub, std::string name, DurationType dt,
                           VenueType vt, StreamerServiceType sst,
                           std::string command="SUBS")
{
    if( vt != sub.get_venue() )
        throw std::runtime_error(name + ": bad venue");
    test_sub_duration(sub, name, dt, sst, command);
}

void
streaming(string id, Credentials& c)
{
    using namespace chrono;
    json j;

    auto callback = []( int cb_type, int ss_type,
                         unsigned long long timestamp, const char* msg )
        {
            cout<< to_string(static_cast<StreamingCallbackType>(cb_type)) << endl
                << "\t service: "
                << to_string(static_cast<StreamerServiceType>(ss_type)) << endl
                << "\t timestamp: " << timestamp << endl
                << "\t content: " << json::parse(string(msg)) << endl << endl;
        };


    using ft = QuotesSubscription::FieldType;
    using oft = OptionsSubscription::FieldType;
    using fxft = LevelOneForexSubscription::FieldType;
    using foft = LevelOneFuturesOptionsSubscription::FieldType;
    using fft = LevelOneFuturesSubscription::FieldType;
    using nft = NewsHeadlineSubscription::FieldType;
    using ceft = ChartEquitySubscription::FieldType;
    using cft = ChartSubscriptionBase::FieldType;
    using tsft = TimesaleSubscriptionBase::FieldType;


    set<string> symbols1 = {"SPY"};
    set<ft> fields1 = {ft::symbol, ft::last_price};
    QuotesSubscription q1(symbols1, fields1);
    display_sub(q1);
    test_sub_fields_symbols(q1, "QuotesSubscription", symbols1, fields1,
                            StreamerServiceType::QUOTE);

    set<string> symbols2 = {"SPY_081718C286", "SPY_081718P286"};
    set<oft> fields2 = {oft::symbol, oft::last_price, oft::delta};
    OptionsSubscription q2( symbols2, fields2 );
    display_sub(q2);
    test_sub_fields_symbols(q2, "OptionsSubscription", symbols2, fields2,
                            StreamerServiceType::OPTION);

    set<string> symbols3 = {"/ES", "/GC"};
    set<fft> fields3 = {fft::symbol, fft::last_price};
    LevelOneFuturesSubscription q3( symbols3, fields3);
    display_sub(q3);
    test_sub_fields_symbols(q3, "LevelOneFuturesSubscription", symbols3, fields3,
                            StreamerServiceType::LEVELONE_FUTURES);

    set<string> symbols4 = {"EUR/USD"};
    set<fxft> fields4 = {fxft::symbol, fxft::quote_time};
    LevelOneForexSubscription q4( symbols4, fields4);
    display_sub(q4);
    test_sub_fields_symbols(q4, "LevelOneForexSubscription", symbols4, fields4,
                            StreamerServiceType::LEVELONE_FOREX);

    set<string> symbols5 = {"/ESZ92800"};
    set<foft> fields5 = {foft::symbol, foft::open_interest};
    LevelOneFuturesOptionsSubscription q5(symbols5, fields5);
    display_sub(q5);
    test_sub_fields_symbols(q5, "LevelOneFuturesOptionsSubscription", symbols5,
                            fields5, StreamerServiceType::LEVELONE_FUTURES_OPTIONS);

    set<string> symbols6 = {"SPY", "GOOG"};
    set<nft> fields6 = {nft::symbol, nft::headline};
    NewsHeadlineSubscription q6(symbols6, fields6);
    display_sub(q6);
    test_sub_fields_symbols(q6, "NewsHeadlineSubscription", symbols6, fields6,
                           StreamerServiceType::NEWS_HEADLINE);

    set<string> symbols7 = {"SPY"};
    set<ceft> fields7 = {ceft::symbol, ceft::open_price};
    ChartEquitySubscription q7(symbols7, fields7);
    display_sub(q7);
    test_sub_fields_symbols(q7, "ChartEquitySubscription", symbols7, fields7,
                            StreamerServiceType::CHART_EQUITY);

    set<string> symbols9 = {"/ZN"};
    set<cft> fields9 = {cft::symbol, cft::high_price};
    ChartFuturesSubscription q9(symbols9, fields9);
    display_sub(q9);
    test_sub_fields_symbols(q9, "ChartFuturesSubscription", symbols9, fields9,
                            StreamerServiceType::CHART_FUTURES);

    set<string> symbols10 = {"SPY_081718C276"};
    set<cft> fields10 = {cft::symbol, cft::volume};
    ChartOptionsSubscription q10(symbols10, fields10);
    display_sub(q10);
    test_sub_fields_symbols(q10, "ChartOptionsSubscription", symbols10, fields10,
                            StreamerServiceType::CHART_OPTIONS);

    set<string> symbols11 = {"EEM"};
    set<tsft> fields11 = {tsft::symbol, tsft::trade_time, tsft::last_price };
    TimesaleEquitySubscription q11( symbols11, fields11 );
    display_sub(q11);
    test_sub_fields_symbols(q11, "TimesaleEquitySubscription", symbols11,
                            fields11, StreamerServiceType::TIMESALE_EQUITY);

    set<string> symbols13 = {"/ES", "/GC"};
    set<tsft> fields13 = {tsft::symbol, tsft::trade_time, tsft::last_price };
    TimesaleFuturesSubscription q13( symbols13, fields13);
    display_sub(q13);
    test_sub_fields_symbols(q13, "TimesaleFuturesSubscription", symbols13,
                            fields13, StreamerServiceType::TIMESALE_FUTURES);

    set<string> symbols14 = {"SPY_081718C276"};
    set<tsft> fields14 = {tsft::symbol, tsft::trade_time, tsft::last_price };
    TimesaleOptionsSubscription q14( symbols14, fields14 );
    display_sub(q14);
    test_sub_fields_symbols(q14, "TimesaleOptionsSubscription", symbols14,
                            fields14, StreamerServiceType::TIMESALE_OPTIONS);

    NasdaqActivesSubscription q15(DurationType::all_day);
    display_sub(q15);
    test_sub_duration(q15, "NasdaqActivesSubscription", DurationType::all_day,
                      StreamerServiceType::ACTIVES_NASDAQ);

    NYSEActivesSubscription q16(DurationType::min_60);
    display_sub(q16);
    test_sub_duration(q16, "NYSEActivesSubscription", DurationType::min_60,
                      StreamerServiceType::ACTIVES_NYSE);

    OTCBBActivesSubscription q17(DurationType::min_10);
    display_sub(q17);
    test_sub_duration(q17, "OTCBBActivesSubscription", DurationType::min_10,
                      StreamerServiceType::ACTIVES_OTCBB);

    OptionActivesSubscription q18(VenueType::opts, DurationType::min_1);
    display_sub(q18);
    test_sub_duration_venue(q18, "OptionActivesSubscription",
                            DurationType::min_1, VenueType::opts,
                            StreamerServiceType::ACTIVES_OPTIONS);


    {
        auto ss = StreamingSession::Create(c, callback,
                                           milliseconds(3000),
                                           milliseconds(15000),
                                           StreamingSession::DEF_SUBSCRIBE_TIMEOUT);
        bool res;
        deque<bool> results;

        results = ss->start( {q1,q2} );
        for(auto r : results)
            cout<< boolalpha << r << ' ';
        cout<<endl;

        res = ss->add_subscription( q3 );
        cout<< boolalpha << res << endl;

        std::this_thread::sleep_for( seconds(5) );

        results = ss->add_subscriptions( {q4,q5,q6} );
        for(auto r : results)
            cout<< boolalpha << r << ' ';
        cout<<endl;

        std::this_thread::sleep_for( seconds(5) );
        ss->stop();

        res = ss->start( q7 );
        cout<< boolalpha << res << endl;

        results = ss->add_subscriptions( {q9,q10});
        for(auto r : results)
            cout<< boolalpha << r << ' ';
        cout<<endl;

        std::this_thread::sleep_for( seconds(5) );

        res = ss->add_subscription( q11 );
        cout<< boolalpha << res << endl;

        try{
            ss->start( {q11, q13} );
            cerr << "failed to catch 'already running' exception" <<endl;
            return;
        }catch(StreamingException &e){
            cout<< "successfully caught: " << e.what() << endl;
        }

        auto ss2 = StreamingSession::Create(c, callback,
                                           milliseconds(3000),
                                           milliseconds(15000));
        try{
            ss2->start(q13);
            cerr << "failed to catch 'already active' exception" << endl;
            return;
        }catch(StreamingException& e){
            cout<< "successfully caught: " << e.what() << endl;
        }

        std::this_thread::sleep_for( seconds(3) );
        ss.reset();
        std::this_thread::sleep_for( seconds(3) );

        results = ss2->start( {q11, q13, q14} );
        for(auto r : results)
            cout<< boolalpha << r << ' ';
        cout<<endl;

        std::shared_ptr<StreamingSession> ss3(ss2);

        results = ss3->add_subscriptions( {q15, q16, q17, q18} );
        for(auto r : results)
            cout<< boolalpha << r << ' ';
        cout<<endl;

        std::this_thread::sleep_for( seconds(5) );
        ss2->stop();

        ss = ss2;
        auto ss4 = std::move(ss2);
    }

}



void
transaction_history_getter(string id, Credentials& c)
{
    TransactionHistoryGetter o(c, id, TransactionType::trade, "SPY");
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
    OptionChainAnalyticalGetter ocg(c, "KORS", 30.00, 65.00, 3.0, 100,
                                    OptionStrikes::Single(65.00),
                                    OptionContractType::call, true,
                                    "", "", OptionExpMonth::aug);
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
    OptionChainStrategyGetter ocg(c, "KORS", OptionStrategy::Vertical(),
                                  OptionStrikes::N_ATM(2), OptionContractType::call,
                                  false, "","",OptionExpMonth::aug);
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
    OptionChainGetter ocg(c, "KORS", OptionStrikes::N_ATM(1), OptionContractType::call,
                          false, "2018-06-24", "2018-08-24");   
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
    QuoteGetter qg(c, "SPY");
    cout<< qg.get_symbol() << endl << qg.get() << endl << endl;

    QuoteGetter qg2(move(qg));
    qg2.set_symbol("QQQ");

    cout<< qg2.get_symbol() << endl << qg2.get() << endl << endl;

    QuotesGetter qsg(c, {"SPY", "QQQ"});
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.set_symbols( {"/ES","SPY_081718C276"} );
    for( auto s: qsg.get_symbols() ){
        cout<< s << ' ';
    }
    cout<< endl << qsg.get() << endl << endl;

    qsg.add_symbol("IWM");
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

    qsg.add_symbols( {"XLF","XLY", "XLE"} );
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

long long
msec_since_epoch()
{
    using namespace chrono;
    auto tse = (steady_clock::now().time_since_epoch());
    return duration_cast<milliseconds>(tse).count();
}


