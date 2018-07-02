/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#ifndef TDMA_API_STREAMING_H
#define TDMA_API_STREAMING_H

#include <map>
#include <set>
#include <thread>
#include <memory>

#include "_common.h"
#include "websocket_connect.h"
#include "threadsafe_hashmap.h"
#include "tdma_common.h"

namespace tdma{

class DLL_SPEC_ StreamerService{    
public:
    /* NOTE as we add these need to update the constructor in streaming.cpp
            and the to_string overload in params.cpp */
    enum class type : unsigned int{
        NONE,                       /* NULL FOR INTERNAL/CALLBACK USE */
        //ACCT_ACTIVITY,            /* NOT IMPLEMENTED YET */
        ADMIN,
        ACTIVES_NASDAQ,
        ACTIVES_NYSE,
        ACTIVES_OTCBB,
        ACTIVES_OPTIONS,
        //FOREX_BOOK,               /* NOT DOCUMENTED */
        //FUTURES_BOOK,             /* NOT DOCUMENTED */
        //LISTED_BOOK,              /* NOT DOCUMENTED */
        //NASDAQ_BOOK,              /* NOT DOCUMENTED */
        //OPTIONS_BOOK,             /* NOT DOCUMENTED */
        //FUTURES_OPTION_BOOK,      /* NOT DOCUMENTED */
        CHART_EQUITY,
        //CHART_FOREX,              /* NOT WORKING */
        CHART_FUTURES,
        CHART_OPTIONS,
        //CHART_HISTORY_FUTURES,    /* NOT IMPLEMENTED YET */
        QUOTE,
        LEVELONE_FUTURES,
        LEVELONE_FOREX,
        LEVELONE_FUTURES_OPTIONS,
        OPTION,
        //LEVELTWO_FUTURES,         /* NOT DOCUMENTED */
        NEWS_HEADLINE,
        //NEWS_STORY,               /* NOT DOCUMENTED */
        //NEWS_HEADLINE_LIST,       /* NOT DOCUMENTED */
        //STREAMER_SERVER,          /* NOT DOCUMENTED - OLD HTTP VERSION ?*/
        TIMESALE_EQUITY,
        TIMESALE_FUTURES,
        //TIMESALE_FOREX,           /* NOT WORKING */
        TIMESALE_OPTIONS
    };

    explicit StreamerService(std::string service_name);

    explicit StreamerService(unsigned int i)
        : _service( static_cast<type>(i) ) {}
    
    StreamerService(type service_type)
        : _service(service_type) {}

    StreamerService(nullptr_t)
        : _service(type::NONE) {}

    operator type () const { return _service; }  

private:
    type _service;
};


enum class AdminCommandType : unsigned int{
    LOGIN,
    LOGOUT,
    QOS
};

enum class QOSType : unsigned int {
    express,   /* 500 ms */
    real_time, /* 750 ms */
    fast,      /* 1000 ms DEFAULT */
    moderate,  /* 1500 ms */
    slow,      /* 3000 ms */
    delayed    /* 5000 ms */
};

//DLL_SPEC_ std::string
//to_string(const StreamerService::type& service);

DLL_SPEC_ std::string
to_string(const StreamerService& service);

DLL_SPEC_ std::string
to_string(const AdminCommandType& command);

DLL_SPEC_ std::string
to_string(const QOSType& qos);

using std::to_string;

//DLL_SPEC_ std::ostream&
//operator<<(std::ostream& out, const StreamerService::type& service);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const StreamerService& service);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const AdminCommandType& command);

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const QOSType& qos);

struct StreamerCredentials{
    std::string user_id;
    std::string token;
    std::string company;
    std::string segment;
    std::string cd_domain;
    std::string user_group;
    std::string access_level;
    bool authorized;
    long long timestamp;
    std::string app_id;
    std::string acl;
};

struct StreamerInfo{
    StreamerCredentials credentials;
    std::string credentials_encoded;
    std::string url;
    std::string primary_acct_id;

    void
    encode_credentials();
};


class DLL_SPEC_ StreamingSubscription{
    StreamerService::type _service;
    std::string _command;
    std::map<std::string, std::string> _parameters;
protected:
    StreamingSubscription( StreamerService::type service,
                           const std::string& command,
                           const std::map<std::string, std::string>& paramaters );

public:
    static
    std::string
    encode_symbol(std::string symbol);

    StreamerService::type
    get_service() const
    { return _service; }

    std::string
    get_command() const
    { return _command; }

    std::map<std::string, std::string>
    get_parameters() const
    { return _parameters; }
};


class DLL_SPEC_ SubscriptionBySymbolBase
        : public StreamingSubscription {
    std::set<std::string> _symbols;

    template<typename F>
    static std::map<std::string, std::string>
    build_paramaters( const std::set<std::string>& symbols,
                      const std::set<F>& fields );

public:
    std::set<std::string>
    get_symbols() const
    { return _symbols; }

protected:
    template<typename F>
    SubscriptionBySymbolBase( StreamerService::type service,
                              std::string command,
                              const std::set<std::string>& symbols,
                              const std::set<F>& fields )
        :
            StreamingSubscription(service, command,
                                  build_paramaters(symbols, fields)),
            _symbols(symbols)
        {
        }
};


class DLL_SPEC_ QuotesSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        bid_price,
        ask_price,
        last_price,
        bid_size,
        ask_size,
        ask_id,
        bid_id,
        total_volume,
        last_size,
        trade_time,
        quote_time,
        high_price,
        low_price,
        bid_tick,
        close_price,
        exchange_id,
        marginable,
        shortable,
        island_bid,
        island_ask,
        island_volume,
        quote_day,
        trade_day,
        volatility,
        description,
        last_id,
        digits,
        open_price,
        net_change,
        high_52_week,
        low_52_week,
        pe_ratio,
        dividend_amount,
        dividend_yeild,
        island_bid_size,
        island_ask_size,
        nav,
        fund_price,
        exchanged_name,
        dividend_date,
        regular_market_quote,
        regular_market_trade,
        regular_market_last_price,
        regular_market_last_size,
        regular_market_trade_time,
        regular_market_trade_day,
        regular_market_net_change,
        security_status,
        mark,
        quote_time_as_long,
        trade_time_as_long,
        regular_market_trade_time_as_long
    };

private:
    std::set<FieldType> _fields;

public:
    QuotesSubscription( const std::set<std::string>& symbols,
                        const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ ActivesSubscriptionBase
        : public StreamingSubscription {
public:
    enum class DurationType : unsigned int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };

private:
    std::string _venue;
    DurationType _duration;

protected:
    ActivesSubscriptionBase(StreamerService::type service,
                            std::string venue,
                            DurationType duration);
public:
    DurationType
    get_duration() const
    { return _duration; }
};

std::string
to_string(const ActivesSubscriptionBase::DurationType& duration);

std::ostream&
operator<<( std::ostream& out,
            const ActivesSubscriptionBase::DurationType& duration );


class DLL_SPEC_ NasdaqActivesSubscription
        : public ActivesSubscriptionBase {
public:
    NasdaqActivesSubscription(DurationType duration)
        : ActivesSubscriptionBase(StreamerService::type::ACTIVES_NASDAQ,
                                  "NASDAQ", duration)
    {}
};

class DLL_SPEC_ NYSEActivesSubscription
        : public ActivesSubscriptionBase {
public:
    NYSEActivesSubscription(DurationType duration)
        : ActivesSubscriptionBase(StreamerService::type::ACTIVES_NYSE,
                                  "NYSE", duration)
    {}
};

class DLL_SPEC_ OTCBBActivesSubscription
        : public ActivesSubscriptionBase {
public:
    OTCBBActivesSubscription(DurationType duration)
        : ActivesSubscriptionBase(StreamerService::type::ACTIVES_OTCBB,
                                  "OTCBB", duration)
    {}
};

class DLL_SPEC_ OptionActivesSubscription
        : public ActivesSubscriptionBase {
public:
    enum class VenueType : unsigned int {
        opts,
        calls,
        puts,
        opts_desc,
        calls_desc,
        puts_desc
    };

private:
    VenueType _venue;

public:
    OptionActivesSubscription(VenueType venue, DurationType duration);

    VenueType
    get_venue() const
    { return _venue; }
};

std::string
to_string(const OptionActivesSubscription::VenueType& venue);

std::ostream&
operator<<(std::ostream& out, const OptionActivesSubscription::VenueType& venue);


class DLL_SPEC_ OptionsSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        description,
        bid_price,
        ask_price,
        last_price,
        high_price,
        low_price,
        close_price,
        total_volume,
        open_interest,
        volatility,
        quote_time,
        trade_time,
        money_intrinsic_value,
        quote_day,
        trade_day,
        expiration_year,
        multiplier,
        digits,
        open_price,
        bid_size,
        ask_size,
        last_size,
        net_change,
        strike_price,
        contract_type,
        underlying,
        expiration_month,
        deliverables,
        time_value,
        expiration_day,
        days_to_expiration,
        delta,
        gamma,
        theta,
        vega,
        rho,
        security_status,
        theoretical_option_value,
        underlying_price,
        uv_expiration_type,
        mark,
    };

private:
    std::set<FieldType> _fields;

public:
    OptionsSubscription( const std::set<std::string>& symbols,
                         const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ LevelOneFuturesSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        bid_price,
        ask_price,
        last_price,
        bid_size,
        ask_size,
        ask_id,
        bid_id,
        total_volume,
        last_size,
        quote_time,
        trade_time,
        high_price,
        low_price,
        close_price,
        exchange_id,
        description,
        last_id,
        open_price,
        net_change,
        future_percent_change,
        exchange_name,
        security_status,
        open_interest,
        mark,
        tick,
        tick_amount,
        product,
        future_price_format,
        future_trading_hours,
        future_is_tradable,
        future_multiplier,
        future_is_active,
        future_settlement_price,
        future_active_symbol,
        future_expiration_date
    };

private:
    std::set<FieldType> _fields;

public:
    LevelOneFuturesSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ LevelOneForexSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        bid_price,
        ask_price,
        last_price,
        bid_size,
        ask_size,
        total_volume,
        last_size,
        quote_time,
        trade_time,
        high_price,
        low_price,
        close_price,
        exchange_id,
        description,
        open_price,
        net_change,
        percent_change,
        exchange_name,
        digits,
        security_status,
        tick,
        tick_amount,
        product,
        trading_hours,
        is_tradable,
        market_maker,
        high_52_week,
        low_52_week,
        mark
    };

private:
    std::set<FieldType> _fields;

public:
    LevelOneForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ LevelOneFuturesOptionsSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        bid_price,
        ask_price,
        last_price,
        bid_size,
        ask_size,
        ask_id,
        bid_id,
        total_volume,
        last_size,
        quote_time,
        trade_time,
        high_price,
        low_price,
        close_price,
        exchange_id,
        description,
        last_id,
        open_price,
        net_change,
        future_percent_change,
        exchange_name,
        security_status,
        open_interest,
        mark,
        tick,
        tick_amount,
        product,
        future_price_format,
        future_trading_hours,
        future_is_tradable,
        future_multiplier,
        future_is_active,
        future_settlement_price,
        future_active_symbol,
        future_expiration_date
    };

private:
    std::set<FieldType> _fields;

public:
    LevelOneFuturesOptionsSubscription( const std::set<std::string>& symbols,
                                        const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ NewsHeadlineSubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        error_code,
        story_datetime,
        headline_id,
        status,
        headline,
        story_id,
        count_for_keyword,
        keyword_array,
        is_host,
        story_source
    };

private:
    std::set<FieldType> _fields;

public:
    NewsHeadlineSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


// TODO implement ADD command
class DLL_SPEC_ ChartEquitySubscription
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        open_price,
        high_price,
        low_price,
        close_price,
        volume,
        sequence,
        chart_time,
        chart_day
    };

private:
    std::set<FieldType> _fields;

public:
    ChartEquitySubscription( const std::set<std::string>& symbols,
                             const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


class DLL_SPEC_ ChartSubscriptionBase
        : public SubscriptionBySymbolBase {
public:
    enum class FieldType : unsigned int {
        symbol,
        chart_time,
        open_price,
        high_price,
        low_price,
        close_price,
        volume
    };

private:
    std::set<FieldType> _fields;

protected:
    ChartSubscriptionBase( StreamerService::type service,
                           const std::set<std::string>& symbols,
                           const std::set<FieldType>& fields );

    std::set<FieldType>
    get_fields() const
    { return _fields; }
};


/*
 * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
 *
class DLL_SPEC_ ChartForexSubscription
        : public ChartSubscriptionBase {   
public:
    ChartForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : ChartSubscriptionBase( StreamerService::type::CHART_FOREX,
                                 symbols, fields )
    {}
};
*/

class DLL_SPEC_ ChartFuturesSubscription
        : public ChartSubscriptionBase {
public:
    ChartFuturesSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields )
        : ChartSubscriptionBase( StreamerService::type::CHART_FUTURES,
                                 symbols, fields )
    {}
};


class DLL_SPEC_ ChartOptionsSubscription
        : public ChartSubscriptionBase {
public:
    ChartOptionsSubscription( const std::set<std::string>& symbols,
                              const std::set<FieldType>& fields )
        : ChartSubscriptionBase( StreamerService::type::CHART_OPTIONS,
                                 symbols, fields )
    {}
};


class DLL_SPEC_ TimesalesSubscriptionBase
        : public SubscriptionBySymbolBase {
public:
    enum FieldType : unsigned int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };

private:
    std::set<FieldType> _fields;

public:
    std::set<FieldType>
    get_fields() const
    { return _fields; }

protected:
    TimesalesSubscriptionBase( StreamerService::type service,
                               const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields );
};


class DLL_SPEC_ TimesaleEquitySubscription
        : public TimesalesSubscriptionBase {
public:
    TimesaleEquitySubscription( const std::set<std::string>& symbols,
                                const std::set<FieldType>& fields )
        : TimesalesSubscriptionBase( StreamerService::type::TIMESALE_EQUITY,
                                    symbols, fields)
    {}
};

/*
 *  * NOT WORKING - EUR/USD repsonse: error 22, msg 'Bad command formatting'
class DLL_SPEC_ TimesaleForexSubscription
        : public TimesalesSubscriptionBase {
public:
    TimesaleForexSubscription( const std::set<std::string>& symbols,
                               const std::set<FieldType>& fields )
        : TimesalesSubscriptionBase( StreamerService::type::TIMESALE_FOREX,
                                    symbols, fields)
    {}
};
*/

class DLL_SPEC_ TimesaleFuturesSubscription
        : public TimesalesSubscriptionBase {
public:
    TimesaleFuturesSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields )
        : TimesalesSubscriptionBase( StreamerService::type::TIMESALE_FUTURES,
                                    symbols, fields)
    {}
};

class DLL_SPEC_ TimesaleOptionsSubscription
        : public TimesalesSubscriptionBase {
public:
    TimesaleOptionsSubscription( const std::set<std::string>& symbols,
                                 const std::set<FieldType>& fields )
        : TimesalesSubscriptionBase( StreamerService::type::TIMESALE_OPTIONS,
                                    symbols, fields)
    {}
};


enum class StreamingCallbackType : unsigned int {
    listening_start,
    listening_stop,
    data,
    notify,
    timeout,
    error
};

DLL_SPEC_ std::string
to_string(const StreamingCallbackType& callback_type);

using std::to_string;

DLL_SPEC_ std::ostream&
operator<<(std::ostream& out, const StreamingCallbackType& callback_type);

/* 
 * CALLBACK passed to StreamingSession:
 *
 * callback(StreamingCallbackType, service, timestamp, json data);
 *
 *   listening_start - listening thread has started, other args empty
 *
 *   listening_stop - listening thread has stopped (w/o error), other args empty
 *
 *   data - json data of StreamerService::type type returned from server
 *          with timestamp
 *
 *   notify - notify response indicating some 'urgent' message from server
 *
 *   timeout - listening thread has timed out, connection closed/reset,
 *             other args empty
 *
 *   error - error or exception in listening thread, connection closed/reset,
 *           json = {{"error": error message}}
 *
 * All callback types except 'data' have ServiceType::type::None
 *
 * DO NOT CALL BACK INTO StreamingSession from inside the callback.
 * DO NOT TRY TO STOP OR RESTART from inside the callback.
 * DO NOT BLOCK THE THREAD from inside the callback.
 *
 */
typedef std::function<void(StreamingCallbackType cb_type, StreamerService,
                           unsigned long long, json)> streaming_cb_ty;


class DLL_SPEC_ StreamingSession{
public:
    static const std::string VERSION; // = "1.0"
    static const std::chrono::milliseconds MIN_TIMEOUT; // 1000;
    static const std::chrono::milliseconds MIN_LISTENING_TIMEOUT; // 10000;
    static const std::chrono::milliseconds LOGOUT_TIMEOUT; // 1000
    static const std::chrono::milliseconds DEF_CONNECT_TIMEOUT; // 3000
    static const std::chrono::milliseconds DEF_LISTENING_TIMEOUT; // 30000
    static const std::chrono::milliseconds DEF_SUBSCRIBE_TIMEOUT; // 1500

private:
    class AdminSubscription
            : public StreamingSubscription {
    public:
        AdminSubscription( AdminCommandType command,
                           const std::map<std::string, std::string>& params = {})
            :
                StreamingSubscription( StreamerService::type::ADMIN,
                                       to_string(command), params )
        {}
    };

    struct PendingResponse{
        typedef std::function<void(int, std::string, std::string,
                                   unsigned long long, int, std::string)>
        response_cb_ty;

        static const response_cb_ty CALLBACK_TO_COUT;

        int request_id;
        std::string service;
        std::string command;
        response_cb_ty callback;        

        PendingResponse( int request_id,
                         const std::string& service,
                         const std::string& command,
                         response_cb_ty callback = nullptr );

        PendingResponse();

    };


    class StreamingRequest{
        StreamerService::type _service;
        std::string _command;
        std::string _account_id;
        std::string _source_id;
        int _request_id;
        std::map<std::string, std::string> _parameters;

    public:
        StreamingRequest( StreamerService::type service,
                          const std::string& command,
                          const std::string& account_id,
                          const std::string& source_id,
                          int request_id,
                          const std::map<std::string, std::string>& paramaters );

        StreamingRequest( const StreamingSubscription& subscription,
                          const std::string& account_id,
                          const std::string& source_id,
                          int request_id );

        json
        to_json() const;
    };


    class StreamingRequests{
        std::vector<StreamingRequest> _requests;

    public:
        StreamingRequests( std::initializer_list<StreamingRequest> requests )
            : _requests( requests )
        {}

        StreamingRequests( const std::vector<StreamingSubscription>& subscriptions,
                           const std::string& account_id,
                           const std::string& source_id,
                           const std::vector<int>& request_ids );

        json
        to_json() const;

    };

    static std::set<std::string> active_primary_accounts;
    static std::set<std::string> active_accounts;

    StreamerInfo _streamer_info;
    std::string _account_id;
    std::unique_ptr<conn::WebSocketClient> _client;
    streaming_cb_ty _callback;
    std::chrono::milliseconds _connect_timeout;
    std::chrono::milliseconds _listening_timeout;
    std::chrono::milliseconds _subscribe_timeout;
    std::thread _listener_thread;
    std::string _server_id;
    int _next_request_id;
    bool _logged_in;
    bool _listening;
    QOSType _qos;
    unsigned long long _last_heartbeat;
    ThreadSafeHashMap<int, PendingResponse> _responses_pending;
    bool _request_response_to_cout;

    class ListenerThreadTarget{
        static const std::string RESPONSE_TO_REQUEST;
        static const std::string RESPONSE_NOTIFY;
        static const std::string RESPONSE_SNAPSHOT;
        static const std::string RESPONSE_DATA;

        StreamingSession *_ss;

        class Timeout : public StreamingException {};

        void
        exec();

        void
        parse(const std::string& responses);

        void
        parse_response_to_request(const json& response);

        void
        parse_response_notify(const json& responses);

        void
        parse_response_snapshot(const json& response);

        void
        parse_response_data(const json& response);

    public:
        ListenerThreadTarget( StreamingSession *ss )
            : _ss(ss) {}

        void
        operator()();
    };

    bool
    _login();

    bool
    _logout();

    void
    _start_listener_thread();

    void
    _stop_listener_thread();

    void
    _reset();

    void
    _subscribe( const std::vector<StreamingSubscription>& subscriptions,
                 PendingResponse::response_cb_ty callback = nullptr );

    StreamingSession( const StreamerInfo& streamer_info,
                        const std::string& account_id,
                        streaming_cb_ty callback,
                        std::chrono::milliseconds connect_timeout,
                        std::chrono::milliseconds listening_timeout,
                        std::chrono::milliseconds subscribe_timeout,
                        bool request_response_to_cout );

    virtual
    ~StreamingSession();

public:

    static StreamingSession*
    Create( Credentials& creds,
             const std::string& account_id,
             streaming_cb_ty callback,
             std::chrono::milliseconds connect_timeout=DEF_CONNECT_TIMEOUT,
             std::chrono::milliseconds listening_timeout=DEF_LISTENING_TIMEOUT,
             std::chrono::milliseconds subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT,
             bool request_response_to_cout = true );

    static void
    Destroy( StreamingSession* session );

    StreamingSession( const StreamingSession& ) = delete;

    StreamingSession&
    operator=( const StreamingSession& ) = delete;

    std::deque<bool> // success/fails in the order passed
    start(const std::vector<StreamingSubscription>& subscriptions);

    bool 
    start(const StreamingSubscription& subscription)
    { return start( std::vector<StreamingSubscription>{subscription} )[0]; }

    void
    stop();

    std::deque<bool> // success/fails in the order passed
    add_subscriptions(const std::vector<StreamingSubscription>& subscriptions);

    bool 
    add_subscription(const StreamingSubscription& subscription)
    { return add_subscriptions(std::vector<StreamingSubscription>{subscription})[0]; }

    QOSType
    get_qos() const
    { return _qos; }

    bool
    set_qos(const QOSType& qos);
};


class DLL_SPEC_ SharedSession{
    struct Deleter{
        void
        operator()(StreamingSession *s)
        { StreamingSession::Destroy(s); }
    };

    std::shared_ptr<StreamingSession> _s;

public:
    SharedSession( Credentials& creds,
                     const std::string& account_id,
                     streaming_cb_ty callback,
                     std::chrono::milliseconds connect_timeout
                         = StreamingSession::DEF_CONNECT_TIMEOUT,
                     std::chrono::milliseconds listening_timeout
                         = StreamingSession::DEF_LISTENING_TIMEOUT,
                     std::chrono::milliseconds subscribe_timeout
                         = StreamingSession::DEF_SUBSCRIBE_TIMEOUT,
                     bool request_response_to_cout = true );

    StreamingSession*
    operator->();
};


template<typename F>
std::map<std::string, std::string>
SubscriptionBySymbolBase::build_paramaters(
        const std::set<std::string>& symbols,
        const std::set<F>& fields
        )
{
    std::vector<std::string> symbols_enc;
    for(auto& s : symbols){
        symbols_enc.emplace_back( StreamingSubscription::encode_symbol(s) );
    }

    std::vector<std::string> fields_str(fields.size());
    std::transform( fields.begin(), fields.end(), fields_str.begin(),
               [](F f){
                   return to_string(static_cast<unsigned int>(f));
               } );

    return { {"fields", util::join(fields_str, ',')},
             {"keys", util::join(symbols_enc, ',')} };
}


} /* amtd */

#endif // TDMA_API_STREAMING_H
