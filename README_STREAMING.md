### Streaming Interface
- - -

```
#include "tdma_api_streaming.h"

using namespace tdma;
```

Streaming functionality is implemented with ```StreamingSession``` *(streaming/streaming_session.cpp)* on top of ```WebSocketClient``` *(websocket_connect.cpp)*, built using the uWebSocket *(src/uWebSocket/)* library.

Each authenticated user can create one (and only one) StreamingSession instance:
```
static StreamingSession*
Create( Credentials& creds,
        const std::string& account_id,
        streaming_cb_ty callback,
        std::chrono::milliseconds connect_timeout=DEF_CONNECT_TIMEOUT,
        std::chrono::milliseconds listening_timeout=DEF_LISTENING_TIMEOUT,
        std::chrono::milliseconds subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT,
        bool request_response_to_cout = true );

    creds                    ::  credentials struct received from RequestAccessToken 
                                 / LoadCredentials / CredentialsManager.credentials
    account_id               ::  id string of account to get transactions for
    callback                 ::  callback for when session receives data, times
                                 out etc. (see below)
    connect_timeout          ::  milliseconds to wait for a connection
    listening_timeout        ::  milliseconds to wait for any response from server
    subscribe_timeout        ::  milliseconds to wait for a subscription response ( see below)
    request_response_to_cout ::  print login/logout/subscription response info to 
                                 stdout when received
```
The authenticated user will pass their Credentials struct and account id (as
they did for the [HTTPS Get Interface](README_GET.md)), a callback function, and
some optional timeout and output args.

When completely done the session should be destroyed to log the user out.
```
static void
StreamingSession::Destroy( StreamingSession* session );
```

A safer way to manage the session:
```
class SharedSession{
    ...
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
```
```SharedSession``` takes the same args as ```::Create``` but it manages the 
underlying session much like a ```shared_ptr``` would by allowing for copy/move/assignment
and calling ```::Destroy``` when there are no more references to it.

Use the '->' operator to access the session's methods:
```
{
    SharedSession ss(c, id, cb);
    ss->start(subscription);

    SharedSession ss2 = move(ss); // moves the session
    try{
        ss->stop
    }catch(StreamingException& e){
        // the session has moved to ss2 so the -> operator will throw
    }
} // on leaving the block the session held by ss2 will be automatically destroyed
```

#### Callback 

The primary means of signaling changes in session state (start-->stop, errors, 
timeouts etc.) and returning data to the user is via the callback function. As you'll 
see, the current design is a bit confusing and error prone so expect changes. 
*(We may shift to a safer thread-safe queue approach in the future.)*

```
typedef std::function<void(StreamingCallbackType cb_type, StreamerService,
                           unsigned long long, json)> streaming_cb_ty;
```

**DO NOT** call back into StreamingSession from inside the callback (e.g ```.start()```)  
**DO NOT** block the callback thread for extended periods  
**DO NOT** assume exceptions will get handled higher up (assume program termination)  

The first argument to the callback will be the callback type:
```
enum class StreamingCallbackType : unsigned int {
    listening_start,
    listening_stop,
    data,
    notify,
    timeout,
    error
```

***```::listening_start``` ```::listening_stop```*** - are simple signals about the 
listening state of the session and *should* occur after you call ```.start()```
and ```.stop()```, respectively

***```::error```*** - indicates some type of error/exception state that has propagated
up from the listening thread and caused it to close. The 4th arg will be a json
object of the form ```{{"error": <error message>}}```

***```::timeout```*** - indicates the listening thread hasn't received a message in 
*listening_timeout* milliseconds (defaults to 30000) and has shutdown. You'll need 
to restart the session ***from the original thread*** or destroy it.

***```::notify```*** - indicates some type of 'urgent' message from the server. The 
actual message will be in json form and passed to the 4th arg.

***```::data```*** - will be the bulk of the callbacks and contain the subscribed-to data (see below).


The second argument will contain the service type of the data (for ```::data``` type only):

    ```
    class StreamerService{    
    public:
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
        ...
    };
    ```

The third argument is a timestamp from the server in milliseconds since the epoch that
is (currently) only relevant for ```::data``` callbacks. 

The fourth argument is a json object containing the actual data. Its structure is 
dependent on the service type. In order to understand how to parse the object you'll 
need to refer to the relevant section in [Ameritrade's Streaming documentation](https://developer.tdameritrade.com/content/streaming-data) and the [json library documentation](https://github.com/nlohmann/json).

##### Summary
StreamingCallbackType   | StreamingService  | timestamp   | json 
------------------------|-------------------|-------------|-----
```::listening_start``` | ```::NONE```      | 0           | {}
```::listening_stop```  | ```::NONE```      | 0           | {}
```::data```            | *YES*             | *YES*       | *StreamingService dependent*
```::notify```          | ```::NONE```      | 0           | *Message Type dependent*
```::timeout```         | ```::NONE```      | 0           | {}
```::error```           | ```::NONE```      | 0           | {{"error", "error message"}}

#### Start / Stop

Once ```StreamingSession``` is instantiated it needs to be started and 
different services need to be subscribed to.  In order to start
it must have at least one subscription (see below). 

```
deque<bool> 
StreamingSession::start(const vector<StreamingSubscription>& subscriptions);
```
```
bool
StreamingSession::start(const StreamingSubscription& subscriptions);
```

These methods return the success/failure state of each subscription in the
order they were passed. Other errors result in a ```StreamingException```.

To stop a session:

```
void
StreamingSession::stop();
```

#### Subscriptions

Subscriptions are managed using subscription objects that derive from ```StreamingSubscription```.
The [Subscription Classes section](README_STREAMING.md#subscription-classes) below describes the interfaces for each of the classes.

Each ```StreamerService::type``` has a corresponding 'Subscription' object that is passed the relavent fields for that particular service. Remember, a ```StreamerService``` object is passed as the 2nd arg of the (data) callback and to indicate which subscription you are receiving data for.
```
    StreamerService::type::ACTIVES_NASDAQ    <--> NasdaqActivesSubscription
    StreamerService::type::LEVEL_ONE_FUTURES <--> LevelOneFuturesSubscription
    ...
```


Many of the objects contain enum definitions that consist of the available arguments
for construction. For example, if we want the inside bid/ask for emini S&P500 and gold 
futures we'd instantiate the following subscription:
```
    using ft = LevelOneFuturesSubscription::FieldType;

    set<ft> fields{ft::symbol, ft::bid_price, ft::ask_price};
    set<string> symbols{"/ES", "/GC"};
 
    LevelOneFuturesSubscription sub(symbols, fields);

    // now we'd pass 'sub' to .start() or .add_subscription()
```

To add subscriptions **to a started session**:

```
std::deque<bool> 
StreamingSession::add_subscriptions(const vector<StreamingSubscription>& subscription);

bool
StreamingSession::add_subscription(const StreamingSubscription& subscription);
```

If you try to add a subscription to a stopped session it will throw ```StreamingException```

You can add multiple subscription instances but instances of the same type ***usually***
override older/preceding ones.

To replace a subscription you can override it by adding a new subscription of
the same type(this appears to work but haven't tried every possible combination so
there's no guarantee) or by stopping the session with ```.stop()``` and then restarting 
with new subscriptions. 

It's best to avoid doing this frequently because the session object will go through 
a somewhat costly life-cycle:
```
    .stop() 
        ._stop_listener_thread()
        ._logout() 
        ._client->close() 
    .start() 
        ._client->connect() 
        ._login() 
        ._start_listener_thread()
        ._subscribe()
```

#### Quality-Of-Service

To get/set the update latency(qos) of the connection use:

```
QOSType
StreamingSession::get_qos() const;
```
```
bool
StreamingSession::set_qos(const QOSType& qos);
```
```
enum class QOSType : unsigned int {
    express,   /* 500 ms */
    real_time, /* 750 ms */
    fast,      /* 1000 ms DEFAULT */
    moderate,  /* 1500 ms */
    slow,      /* 3000 ms */
    delayed    /* 5000 ms */
};
```

#### Subscription Classes
- - -

##### QuotesSubscription

Streaming market data that calls back when changed.
- Listed (NYSE, AMEX, Pacific quotes and trades)
- NASDAQ (quotes and trades)
- OTCBB (quotes and trades)
- Pinks (quotes only)
- Mutual Funds (no quotes)
- Indicies (trades only)
- Indicators

**constructors**
```
QuotesSubscription( const set<string>& symbols, const set<FieldType>& fields );

    symbols :: symbols to retrieve quote data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
```
**types**
```
class QuotesSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
QuotesSubscription::get_fields() const;
```
- - -

##### OptionsSubscription

Streaming option data that calls back when changed.

**constructors**
```
OptionsSubscription::OptionsSubscription( const set<string>& symbols, 
                                          const set<FieldType>& fields );

    symbols :: symbols to retrieve quote data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
```
**types**
```
class OptionsSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
OptionsSubscription::get_fields() const;
```
- - -


##### LevelOneFuturesSubscription

Streaming futures data that calls back when changed.

**constructors**
```
LevelOneFuturesSubscription::LevelOneFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
```
**types**
```
class LevelOneFuturesSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
LevelOneFuturesSubscription::get_fields() const;
```
- - -


##### LevelOneForexSubscription

Streaming forex data that calls back when changed.

**constructors**
```
LevelOneForexSubscription::LevelOneForexSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
```
**types**
```
class LevelOneForexSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
LevelOneForexSubscription::get_fields() const;
```
- - -


##### LevelOneFuturesOptionsSubscription

Streaming futures-options data calls back when changed.

**constructors**
```
LevelOneFuturesOptionsSubscription::LevelOneFuturesOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
```
**types**
```
class LevelOneFuturesOptionsSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
LevelOneFuturesOptionsSubscription::get_fields() const;
```
- - -



##### ChartEquitySubscription
Streaming 1-minute OHLCV equity values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartEquitySubscription::ChartEquitySubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
```
**types**
```
class ChartEquitySubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
ChartEquitySubscription::get_fields() const;
```
- - -

##### ChartFuturesSubscription
Streaming 1-minute OHLCV futures values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartFuturesSubscription::ChartFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
```
**types**
```
class ChartFuturesSubscription
        : public ChartSubscriptionBase {
public:
    /* INHERITED FROM ChartSubscriptionBase */
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
ChartSubscriptionBase::get_fields() const;
```
- - -

##### ChartOptionsSubscription
Streaming 1-minute OHLCV option values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartOptionsSubscription::ChartOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
```
**types**
```
class ChartOptionsSubscription
        : public ChartSubscriptionBase {
public:
    /* INHERITED FROM ChartSubscriptionBase */
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
ChartSubscriptionBase::get_fields() const;
```
- - -


##### TimesaleEquitySubscription
Streaming time & sales equity trades as a sequence.

**constructors**
```
TimesaleEquitySubscription::TimesaleEquitySubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
```
**types**
```

class TimesaleEquitySubscription
        : public TimesalesSubscriptionBase {
public:
    /* INHERITED FROM TimesalesSubscriptionBase */
    enum FieldType : unsigned int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
TimesalesSubscriptionBase::get_fields() const;
```
- - -

##### TimesaleFuturesSubscription
Streaming time & sales futures trades as a sequence.

**constructors**
```
TimesaleFuturesSubscription::TimesaleFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
```
**types**
```

class TimesaleFuturesSubscription
        : public TimesalesSubscriptionBase {
public:
    /* INHERITED FROM TimesalesSubscriptionBase */
    enum FieldType : unsigned int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
TimesalesSubscriptionBase::get_fields() const;
```
- - -


##### TimesaleOptionsSubscription
Streaming time & sales option trades as a sequence.

**constructors**
```
TimesaleOptionsSubscription::TimesaleOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
```
**types**
```

class TimesaleOptionsSubscription
        : public TimesalesSubscriptionBase {
public:
    /* INHERITED FROM TimesalesSubscriptionBase */
    enum FieldType : unsigned int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
TimesalesSubscriptionBase::get_fields() const;
```
- - -



##### NasdaqActivesSubscription

Most active NASDAQ traded securies for various durations.

**constructors**
```
NasdaqActivesSubscription::NasdaqActivesSubscription(DurationType duration);

    duration :: period over which to return actives
```

**types**
```
class NasdaqActivesSubscription
        : public ActivesSubscriptionBase {
public:
    /* INHERITED FROM ActivesSubscriptionBase */
    enum class DurationType : unsigned int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
...
};
```

**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
- - -

##### NYSEActivesSubscription

Most active NYSE traded securies for various durations.

**constructors**
```
NYSEActivesSubscription::NYSEActivesSubscription(DurationType duration);

    duration :: period over which to return actives
```

**types**
```
class NYSEActivesSubscription
        : public ActivesSubscriptionBase {
public:
    /* INHERITED FROM ActivesSubscriptionBase */
    enum class DurationType : unsigned int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
...
};
```

**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
- - -

##### OTCBBActivesSubscription

Most active OTCBB traded securies for various durations.

**constructors**
```
OTCBBActivesSubscription::OTCBBActivesSubscription(DurationType duration);

    duration :: period over which to return actives
```

**types**
```
class OTCBBActivesSubscription
        : public ActivesSubscriptionBase {
public:
    /* INHERITED FROM ActivesSubscriptionBase */
    enum class DurationType : unsigned int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
...
};
```

**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
- - -


##### OptionActivesSubscription

Most active Options of various types and durations traded.

**constructors**
```
OptionActivesSubscription::OptionActivesSubscription( VenueType venue,
                                                      DurationType duration );

    venue    :: type of option (put, call-desc)
    duration :: period over which to return actives

```

**types**
```
class OptionActivesSubscription
        : public ActivesSubscriptionBase {
public:
    /* INHERITED FROM ActivesSubscriptionBase */
    enum class DurationType : unsigned int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };

    enum class VenueType : unsigned int {
        opts,
        calls,
        puts,
        opts_desc,
        calls_desc,
        puts_desc    
    };
...
};
```

**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
```
VenueType
OptionActivesSubscription::get_venue() const;
```
- - -



##### NewsHeadlineSubscription

Streaming news headlines as a sequence.

**constructors**
```
NewsHeadlineSubscription::NewsHeadlineSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields 
    );

    symbols :: symbols to retrieve news headlines for
    fields  :: fields of each headline to return (headline, story_source etc.)
```

**types**
```
class NewsHeadlineSubscription
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
    ...
};
```
**methods**
```
StreamerService::type
StreamingSubscription::get_service() const;
```
```
string
StreamingSubscription::get_command() const;
```
```
map<string, string>
StreamingSubscription::get_parameters() const;
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
set<FieldType>
NewsHeadlineSubscription::get_fields() const;
```
- - -

