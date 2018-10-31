#include <iostream>

#include "test.h"

#include "tdma_api_streaming.h"

using namespace tdma;
using namespace std;

void
callback( int cb_type,
          int ss_type,
          unsigned long long timestamp,
          const char* msg )
{
    cout<< to_string(static_cast<StreamingCallbackType>(cb_type)) << endl
        << "\t service: "
        << to_string(static_cast<StreamerServiceType>(ss_type)) << endl
        << "\t timestamp: " << timestamp << endl
        << "\t content: " << json::parse(string(msg)) << endl << endl;
};


template<typename S>
void display_sub( S& sub,
                  typename enable_if<is_base_of<SubscriptionBySymbolBase, S>
                      ::value, S>::type* _ = nullptr )
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
void display_sub( S& sub,
                  typename enable_if<is_base_of<ActivesSubscriptionBase, S>
                      ::value && !is_same<S,OptionActivesSubscription>::value>
                          ::type* _ = nullptr )
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
test_sub_fields_symbols( S& sub,
                         std::string name,
                         std::set<std::string> symbols,
                         std::set<F> fields,
                         StreamerServiceType sst,
                         std::string command="SUBS" )
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
test_sub_duration( S& sub,
                   std::string name,
                   DurationType dt,
                   StreamerServiceType sst,
                   std::string command="SUBS" )
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
test_sub_duration_venue( S& sub,
                         std::string name,
                         DurationType dt,
                         VenueType vt,
                         StreamerServiceType sst,
                         std::string command="SUBS" )
{
    if( vt != sub.get_venue() )
        throw std::runtime_error(name + ": bad venue");
    test_sub_duration(sub, name, dt, sst, command);
}

void
test_streaming(const string& account_id, Credentials& c)
{
    using namespace chrono;
    json j;

    using ft = QuotesSubscription::FieldType;
    using oft = OptionsSubscription::FieldType;
    using fxft = LevelOneForexSubscription::FieldType;
    using foft = LevelOneFuturesOptionsSubscription::FieldType;
    using fft = LevelOneFuturesSubscription::FieldType;
    using nft = NewsHeadlineSubscription::FieldType;
    using ceft = ChartEquitySubscription::FieldType;
    using cft = ChartSubscriptionBase::FieldType;
    using tsft = TimesaleSubscriptionBase::FieldType;


    set<string> symbols1 = {"spy"};
    set<ft> fields1 = {ft::symbol, ft::last_price};
    QuotesSubscription q1(symbols1, fields1);
    display_sub(q1);
    test_sub_fields_symbols(q1, "QuotesSubscription", {"SPY"}, fields1,
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
