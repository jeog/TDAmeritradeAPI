#include <tdma_api_get.h>
#include <tdma_api_streaming.h>
#include <iostream>
#include <thread>

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


int main(int argc, char* argv[])
{
    using namespace chrono;

    //cout << Test::name1 << endl;

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

    auto callback = [&]( StreamingCallbackType cb_type,
                         StreamerService service,
                         unsigned long long timestamp, json j )
        {
            cout<< cb_type << endl
                << "\t service: " << StreamerService(service) << endl
                << "\t timestamp: " << timestamp << endl
                << "\t content: " << j << endl << endl;
        };


    using ft = QuotesSubscription::FieldType;
    using tsft = TimesalesSubscriptionBase::FieldType;

    try {

        CredentialsManager cmanager(creds_path, password);

        Credentials ccc;
        ccc = cmanager.credentials;
        Credentials ccc2( ccc );

        ccc2 = move(ccc);
        Credentials ccc4( move(ccc2 ));

        cout<< "*** SET WAIT ***" << endl;
        cout<< APIGetter::get_wait_msec().count() << " --> ";
        APIGetter::set_wait_msec( milliseconds(1500) );
        cout<< APIGetter::get_wait_msec().count() << endl;

        cout<< "*** QUOTE DATA ***" << endl;
        quote_getters(cmanager.credentials);

        historical_getters(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        QuotesSubscription spy_quote_sub({"SPY"}, {ft::symbol, ft::last_price});
        NYSEActivesSubscription actives_sub(ActivesSubscriptionBase::DurationType::min_60);

        SharedSession ss(cmanager.credentials, account_id, callback,
                         milliseconds(3000), milliseconds(15000));

        ss->start( {spy_quote_sub, actives_sub} );

        cout<< "*** OPTION DATA ***" << endl;
        option_chain_getter(cmanager.credentials);
        option_chain_strategy_getter(cmanager.credentials);
        option_chain_analytical_getter(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        QuotesSubscription qqq_quote_sub({"QQQ"}, {ft::symbol, ft::last_size});
        ss->add_subscription(qqq_quote_sub);

        cout<< "*** MARKET DATA ***" << endl;
        instrument_info_getter(cmanager.credentials);
        market_hours_getter(cmanager.credentials);
        movers_getter(cmanager.credentials);
        this_thread::sleep_for( seconds(3) );


        try{
            ss->start(spy_quote_sub);
        }catch(StreamingException& e){
            cout<< "succesffuly caught StreamingException" << endl;
        }
        ss->stop();


        cout<< "*** ACCOUNT DATA ***" << endl;
        account_info_getter(account_id, cmanager.credentials);
        preferences_getter(account_id, cmanager.credentials);
        user_principals_getter(cmanager.credentials);
        subscription_keys_getter(account_id, cmanager.credentials);
        this_thread::sleep_for( seconds(3) );

        TimesaleFuturesSubscription sp_quote_sub( {"/ES"}, {tsft::symbol,
                                                           tsft::trade_time,
                                                           tsft::last_price } );
        ss->start( sp_quote_sub );

        cout<< "*** TRANSACTION DATA ***" << endl;
        try{
            transaction_history_getter(account_id, cmanager.credentials);
            individual_transaction_history_getter(account_id, cmanager.credentials);
        }catch( ServerError& e){
            cout<<"caught server error: " << e.what() << " (" << e.code << ")" << endl;
        }

        string in;
        cin >> in;

    }catch( tdma::APIException& e ){
        std::cerr<< "APIException: " << e.what() << std::endl;
    }catch( json::exception& e){
        std::cerr<< "json::exception: " << e.what() << std::endl;
    }catch( std::exception& e ){
        std::cerr<< "Unknown error: " << e.what() << std::endl;
    }

    return 0;
}

void
streaming(string id, Credentials& c)
{
    using namespace chrono;
    json j;

    bool timed_out = false;
    auto callback = [&]( StreamingCallbackType cb_type,
                         StreamerService service,
                         unsigned long long timestamp, json j )
        {
            if(cb_type == StreamingCallbackType::timeout){
                timed_out = true;
            }
                cout<< cb_type << endl
                    << "\t service: " << StreamerService(service) << endl
                    << "\t timestamp: " << timestamp << endl
                    << "\t content: " << j << endl
                    << endl;
        };

    /*
    {
        using ft = QuotesSubscription::FieldType;
        using dt = ActivesSubscriptionBase::DurationType;
        using oft = OptionsSubscription::FieldType;
        using fxft = LevelOneForexSubscription::FieldType;
        using foft = LevelOneFuturesOptionsSubscription::FieldType;
        using fft = LevelOneFuturesSubscription::FieldType;
        using nft = NewsHeadlineSubscription::FieldType;
        using ceft = ChartEquitySubscription::FieldType;
        using cft = ChartSubscriptionBase::FieldType;
        using tsft = TimesalesSubscriptionBase::FieldType;

        QuotesSubscription q1({"SPY"}, {ft::symbol, ft::last_price});
        //NasdaqActivesSubscription q2(dt::all_day);
        NewsHeadlineSubscription q10( {"SPY", "GOOG"}, {nft::symbol, nft::headline});
        */
        /*
        NYSEActivesSubscription q3(dt::min_60);
        OTCBBActivesSubscription q4(dt::min_10);       
        OptionActivesSubscription q5(OptionActivesSubscription::VenueType::opts,
                                     dt::min_1);
        OptionsSubscription q6( {"SPY_072718C276", "SPY_072718P276"},
                                {oft::symbol, oft::last_price, oft::delta});
                                */
   //     LevelOneFuturesSubscription q7( {"/ES", "/GC"}, {fft::symbol, fft::last_price});
/*

        LevelOneForexSubscription q8( {"EUR/USD"}, {fxft::symbol, fxft::quote_time});
        LevelOneFuturesOptionsSubscription q9( {"/ESZ92700"}, {foft::symbol, foft::open_interest});


        ChartEquitySubscription q11( {"SPY"}, {ceft::symbol, ceft::open_price});           
        //ChartForexSubscription q12( {"EUR/USD"}, {cft::symbol, cft::chart_time,
        //                                          cft::open_price, cft::high_price,
        //                                          cft::low_price, cft::close_price,
        //                                          cft::volume});
        ChartFuturesSubscription q13( {"/ZN"}, {cft::symbol, cft::high_price});      
        ChartOptionsSubscription q14( {"SPY_072718C276"}, {cft::symbol, cft::volume} );

        TimesaleEquitySubscription q15( {"EEM"}, {tsft::symbol, tsft::trade_time,
                                                 tsft::last_price } );
        TimesaleFuturesSubscription q16( {"/ES, /GC"}, {tsft::symbol, tsft::trade_time,
                                                 tsft::last_price } );
        TimesaleOptionsSubscription q17( {"SPY_072718C276"}, {tsft::symbol, tsft::trade_time,
                                                 tsft::last_price } );
        //TimesaleForexSubscription q18( {"EUR/USD"}, {tsft::symbol, tsft::trade_time,
        //                                         tsft::last_price } );



        StreamingSession *ss = StreamingSession::Create(c, id, callback,
                                                        milliseconds(3000),
                                                        milliseconds(15000));
*/
    using ft = QuotesSubscription::FieldType;
    QuotesSubscription q1({"SPY"}, {ft::symbol, ft::last_price});
    {
        SharedSession s1(c, id, callback);
        SharedSession s2(s1);
        s2 = s1;
        SharedSession s3(move(s2));
        s3 = move(s1);
        try{
            s1->start(q1);
            this_thread::sleep_for( seconds(5) );
            return;
        }catch(std::exception& e){
            cout << "SUCCESS " << e.what() << endl;
        }
        try{
            s2->start(q1);
            this_thread::sleep_for( seconds(5) );
            return;
        }catch(std::exception& e){
            cout << "SUCCESS " << e.what() << endl;
        }
        s3->start(q1);
        this_thread::sleep_for( seconds(5) );
    }

        /*
        //QuoteGetter qg(c, "SPY");

        ss->start( {q1,q10, q7} );
        for(int i = 0; i < 100; ++i){
            this_thread::sleep_for( seconds(1) );
            cout<< "*** " << i << " ***" << endl;
            if( timed_out ){
                cout << "TIMED OUT PRE"<< endl;
                try{
                    ss->start( q7 );
                    timed_out = false;
                }catch(StreamingException& e ){
                    cerr << e.what() << endl;
                }
                cout << "TIMED OUT POST" << endl;
            }
        }

        StreamingSession::Destroy(ss);

    }
*/
    //int i = 10;

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
    hpg.set_extended_hours(false);
    cout<< hpg << hpg.get() << endl << endl;

    unsigned long long end = 1528205400000;
    unsigned long long start = 1528119000000;
    //unsigned long day_msec = 86400000;
    HistoricalRangeGetter hrg(c, "SPY", FrequencyType::minute, 30,
                               start, end, true);
    cout<< hrg << hrg.get() << endl << endl;
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

}

long long
msec_since_epoch()
{
    using namespace chrono;
    auto tse = (steady_clock::now().time_since_epoch());
    return duration_cast<milliseconds>(tse).count();
}


