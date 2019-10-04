### Streaming Interface
- - -
- [Overview](#overview)
- [StreamingSession](#streamingsession)
    - [Callback](#callback)
        - [Args](#callback-args)
        - [Summary Table](#summary)
    - [Start](#start)
    - [Stop](#stop)
    - [Add](#add)
    - [QOS](#qos)
    - [Destroy](#destroy)
- [Subscriptions](#subscriptions)
    - [Managed Subscriptions](#managed-subscriptions)
        - [Symbol / Field ](#symbol--field)
        - [Duration / Venue ](#symbol--field)
        - [Commands](#commands)
    - [Raw Subscription](#raw-subscription)
    - [Copy](#copy)
    - [Destroy](#destroy-1)
- [Example Usage](#example-usage)
    - [C++](#c)
    - [C](#c-1)
    - [Python](#python)
    - [Java](#java)
- [Subscription Classes](#subscription-classes)  
    - [Managed Subscription Classes](#managed-subscription-classes)  
        - [QuotesSubscription](#quotessubscription)  
        - [OptionsSubscription](#optionssubscription)  
        - [LevelOneFuturesSubscription](#levelonefuturessubscription)  
        - [LevelOneForexSubscription](#leveloneforexsubscription)  
        - [LevelOneFuturesOptionsSubscription](#levelonefuturesoptionssubscription)  
        - [NewsHeadlineSubscription](#newsheadlinesubscription)  
        - [ChartEquitySubscription](#chartequitysubscription)  
        - [ChartFuturesSubscription](#chartfuturessubscription)  
        - [ChartOptionsSubscription](#chartoptionssubscription)  
        - [TimesaleEquitySubscription](#timesaleequitysubscription)  
        - [TimesaleFuturesSubscription](#timesalefuturessubscription)  
        - [TimesaleOptionsSubscription](#timesaleoptionssubscription)  
        - [NasdaqActivesSubscription](#nasdaqactivessubscription)  
        - [NYSEActivesSubscription](#nyseactivessubscription)  
        - [OTCBBActivesSubscription](#otcbbactivessubscription)  
        - [OptionActivesSubscription](#optionactivessubscription)  
        - [AcctActivitySubscription](#acctactivitysubscription) ***\*NEW\****
    - [RawSubscription](#rawsubscription)  

*UPDATES*

- *On Oct 4 2019 a major bug was fixed that was causing streaming sessions(created with different credentials) to spontaneously close with a notification message about only having one session per user name and password. It's recommended to use commits later than 6ca8c8*

- - -

### Overview

```
[C, C++]
#include "tdma_api_streaming.h"

[C++]
using namespace tdma;

[Python]
from tdma_api import stream

[Java]
// for illustrative purposes (import individual classes as needed)
import io.github.jeog.tdameritradeapi.stream.*; 
```

Streaming functionality in C++ is provided via ```StreamingSession``` which calls through the ABI to its corresponding implementation object. That object, ```StreamingSessionImpl```, is built on top of ```WebSocketClient``` using the uWebSocket *(src/uWebSocket/)* library. 

C uses a similar object-oriented approach where methods are passed a pointer to proxy objects that are manually created and destroyed by the client. 

The Python interface uses ```stream.StreamingSession```, and the Java interface uses ```io.github.jeog.tdameritrade.stream.StreamingSession```, both of which mirror C++ almost exactly. (see below)

Once created, streaming objects are passed subscription objects for the particular streaming services desired.

### StreamingSession

To create a new session the authenticated user will pass their Credentials object (as they did for the [HTTPS Get Interface](README_GET.md)), a callback function, an option account ID, and some optional timeout args to the static factory.

```
[C++]
static shared_ptr<StreamingSession>
StreamingSession::Create( 
        Credentials& creds,     
        streaming_cb_ty callback,
        std::string account_id="",
        std::chrono::milliseconds connect_timeout=DEF_CONNECT_TIMEOUT,
        std::chrono::milliseconds listening_timeout=DEF_LISTENING_TIMEOUT,
        std::chrono::milliseconds subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT 
        );

    creds             ::  credentials struct received from RequestAccessToken 
                          / LoadCredentials / CredentialsManager.credentials
    callback          ::  callback for when notifications, data etc. 
    account_id        ::  account to use (defaults to primary account)
    connect_timeout   ::  milliseconds to wait for a connection
    listening_timeout ::  milliseconds to wait for any response from server
    subscribe_timeout ::  milliseconds to wait for a subscription response 
```

***Note - each time a session is created a single HTTPS/Get request for the account's streamer information is made.***

The C interface expects a pointer to a StreamingSession_C struct to be 
populated with a generic pointer to the underlying C++ object. Once this object
is initialized you will use it in all subsequent (C) calls, until it is destroyed.


```
[C]
typedef struct{
    void *obj;
    int type_id;
    void *ctx; // reserved
} StreamingSession_C;

[C]
inline int
StreamingSession_Create( struct Credentials *pcreds,
                         streaming_cb_ty callback,
                         StreamingSession_C *psession ); // <-populated on success
    ...
    returns -> 0 on success, error code on failure

[C]
inline int
StreamingSession_CreateEx( struct Credentials *pcreds,
                           streaming_cb_ty callback,
                           const char* account_id,
                           unsigned long connect_timeout,
                           unsigned long listening_timeout,
                           unsigned long subscribe_timeout                         
                           StreamingSession_C *psession ); // <-populated on success
    ...
    returns -> 0 on success, error code on failure
```

The Python interface uses the ```stream.StreamingSession``` class directly.
```
class StreamingSession:
    def __init__( self, creds, callback, 
                  account_id=None,
                  connect_timeout=DEF_CONNECT_TIMEOUT,
                  listening_timeout=DEF_LISTENING_TIMEOUT,
                  subscribe_timeout=DEF_SUBSCRIBE_TIMEOUT ):

```
As does the java interface:
```
public class StreamingSession implements AutoCloseable {
    ...
    public StreamingSession( Credentials creds, Callback callback, long connectTimeout,
            long listeningTimeout, long subscribeTimeout ) throws CLibException;
    
    public StreamingSession( Credentials creds, Callback callback) throws CLibException;
    ...
}
```

In Java you can use the session in a 'try-with-resources' block to insure proper shutdown of the connection:
```
try( StreamingSession session = new StreamingSession(credentials, callback) ){
    //
}catch( CLibException exc ){
    //
}
```

#### Callback 

The primary means of signaling changes in session state AND returning data to the user is the callback function. 

```
[C, C++]
typedef void(*streaming_cb_ty)(int, int, unsigned long long, const char*);

[Python]
def my_callback(a,b,c,d):
    ...

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public static interface Callback {
        public void 
        call(int serviceType, int callbackType, long timestamp, String data);
    ...
    }
}

```

- **DO NOT** call into StreamingSession(i.e use its methods) from inside the callback  
- **DO NOT** block the callback thread for extended periods  
- **DO NOT** assume exceptions will get handled higher up (assume program termination)  
- **DO NOT** assume the memory const char* points at will be valid after callback returns(use ```strdup```, ```strcpy```, or ```std::string(...)``` to copy)

##### Callback Args 

In order for the callback to work across the ABI for C, C++, Python, and Java code it casts
certain values to native types. It's the client's responsibility to parse responses.

1. The first argument to the callback will be ```StreamingCallbackType``` as an int:
    ```
    [C++]
    enum class StreamingCallbackType : int {
        listening_start,  /* 0 */
        listening_stop,   /* 1 */
        data,             /* 2 */
        request_response, /* 3 */
        notify,           /* 4 */
        timeout,          /* 5 */
        error             /* 6 */
    }

    [C]
    enum StreamingCallbackType {
        StreamingCallbackType_listening_start,
        StreamingCallbackType_listening_stop,
        StreamingCallbackType_data,
        StreamingCallbackType_request_response,
        StreamingCallbackType_notify,
        StreamingCallbackType_timeout,
        StreamingCallbackType_error
    }

    [Python]
    CALLBACK_TYPE_LISTENING_START = 0
    CALLBACK_TYPE_LISTENING_STOP = 1
    CALLBACK_TYPE_DATA = 2
    CALLBACK_TYPE_REQUEST_RESPONSE = 3
    CALLBACK_TYPE_NOTIFY = 4
    CALLBACK_TYPE_TIMEOUT = 5
    CALLBACK_TYPE_ERROR = 6

    [Java]
    public class StreamingSession implements AutoCloseable {
        ...
        public enum CallbackType implements CLib.ConvertibleEnum {
            LISTENING_START(0),
            LISTENING_STOP(1),
            DATA(2),
            REQUEST_RESPONSE(3),
            NOTIFY(4),
            TIMEOUT(5),
            ERROR(6);   
            ...
        }
        ...
    }
    ```

    - ***```listening_start``` ```listening_stop```*** - are simple signals about the listening state of the session and *should* occur after you call ```start``` and ```stop```, respectively.

    - ***```request_response```*** - indicates a response from the server for a particular request e.g LOGIN or set SUBS. The 4th arg will contain a json string of relevant fields of the form ```{"request_id":<id>,"command":<command>, "code":<code> , "message":<message>}```

    - ***```error```*** - indicates some type of error/exception state has propagated up from the listening thread and caused it to close. The 4th arg will be a json string of the form ```{"error": <error message>}```. This error is generated from the client side but may be in response to something on the server side(e.g. unparsable json).

    - ***```timeout```*** - indicates the listening thread hasn't received a message in *listening_timeout* milliseconds (defaults to 30000) and has shutdown. You'll need to restart the session ***from the original thread*** or destroy it.

    - ***```notify```*** - indicates a heartbeat(every 10 seconds) OR some type of 'urgent' message from the server. The actual message will be in json form and passed to the 4th arg. This message may indicate a condition that will close the streaming session from the server side. The heartbeat message will contain a millisecond timestamp and be of the form ```{"heartbeat":"1565322739463"}```. [***Earlier versions of of ```notify``` ignored the heartbeat - last used in commit b2d88d (Aug 8 2019)***]

    - ***```data```*** - will be the bulk of the callbacks and contain the subscribed-to data (see below).


2. The second argument will contain the ```StreamerServiceType``` of the data or server response, as an int:

    ```
    DECL_C_CPP_TDMA_ENUM(StreamerServiceType, 1, 29,
        BUILD_ENUM_NAME( NONE ),
        BUILD_ENUM_NAME( QUOTE ),
        BUILD_ENUM_NAME( OPTION ),
        BUILD_ENUM_NAME( LEVELONE_FUTURES ),
        BUILD_ENUM_NAME( LEVELONE_FOREX ),
        BUILD_ENUM_NAME( LEVELONE_FUTURES_OPTIONS ),
        BUILD_ENUM_NAME( NEWS_HEADLINE ),
        BUILD_ENUM_NAME( CHART_EQUITY ),
        BUILD_ENUM_NAME( CHART_FOREX ), /* NOT WORKING */
        BUILD_ENUM_NAME( CHART_FUTURES ),
        BUILD_ENUM_NAME( CHART_OPTIONS ),
        BUILD_ENUM_NAME( TIMESALE_EQUITY ),
        BUILD_ENUM_NAME( TIMESALE_FOREX ), /* NOT WORKING */
        BUILD_ENUM_NAME( TIMESALE_FUTURES ),
        BUILD_ENUM_NAME( TIMESALE_OPTIONS ),
        BUILD_ENUM_NAME( ACTIVES_NASDAQ ),
        BUILD_ENUM_NAME( ACTIVES_NYSE ),
        BUILD_ENUM_NAME( ACTIVES_OTCBB ),
        BUILD_ENUM_NAME( ACTIVES_OPTIONS ),
        BUILD_ENUM_NAME( ADMIN ), 
        /*
         * EVERYTHING BELOW HERE DOES NOT HAVE A CORRESPONDING
         * MANAGED SUBSCRIPTION - TRY USING 'RawSubscription'
         */
        BUILD_ENUM_NAME( ACCT_ACTIVITY ),
        BUILD_ENUM_NAME( CHART_HISTORY_FUTURES ),
        BUILD_ENUM_NAME( FOREX_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( FUTURES_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( LISTED_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( NASDAQ_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( OPTIONS_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( FUTURES_OPTIONS_BOOK ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( NEWS_STORY ), /* NOT DOCUMENTED BY TDMA */
        BUILD_ENUM_NAME( NEWS_HEADLINE_LIST ), /* NOT DOCUMENTED BY TDMA */

        /* in case something gets returned we're not aware of - DON'T USE */
        BUILD_ENUM_NAME( UNKNOWN )
    );
    ```

    These macros expand like so:
    ```
    [C++]
    enum class StreamerServiceType : int{
        NONE,
        QUOTE,
        OPTION,
        ...        
    }

    [C]
    enum StreamerServiceType {
        StreamerServiceType_NONE,
        StreamerServiceType_QUOTE,
        StreamerServiceType_OPTION,
        ...
    }
    ```

    Python defines its own constant values:

    ```
    [Python]
    SERVICE_TYPE_NONE = 0
    SERVICE_TYPE_QUOTE = 1
    SERVICE_TYPE_OPTION = 2
    SERVICE_TYPE_LEVELONE_FUTURES = 3
    SERVICE_TYPE_LEVELONE_FOREX = 4
    SERVICE_TYPE_LEVELONE_FUTURES_OPTIONS = 5
    SERVICE_TYPE_NEWS_HEADLINE = 6
    SERVICE_TYPE_CHART_EQUITY = 7
    SERVICE_TYPE_CHART_FOREX = 8 # NOT WORKING (SERVER SIDE)
    SERVICE_TYPE_CHART_FUTURES = 9
    SERVICE_TYPE_CHART_OPTIONS = 10 
    SERVICE_TYPE_TIMESALE_EQUITY = 11
    SERVICE_TYPE_TIMESALE_FOREX = 12 # NOT WORKING (SERVER SIDE)
    SERVICE_TYPE_TIMESALE_FUTURES = 13
    SERVICE_TYPE_TIMESALE_OPTIONS = 14
    SERVICE_TYPE_ACTIVES_NASDAQ = 15
    SERVICE_TYPE_ACTIVES_NYSE = 16
    SERVICE_TYPE_ACTIVES_OTCBB = 17
    SERVICE_TYPE_ACTIVES_OPTIONS = 18 
    SERVICE_TYPE_ADMIN = 19 # NOT MANAGED
    SERVICE_TYPE_ACCT_ACTIVITY = 20 # NOT MANAGED
    SERVICE_TYPE_CHART_HISTORY_FUTURES = 21 # NOT MANAGED
    SERVICE_TYPE_FOREX_BOOK = 22 # NOT MANAGED
    SERVICE_TYPE_FUTURES_BOOK = 23 # NOT MANAGED
    SERVICE_TYPE_LISTED_BOOK = 24 # NOT MANAGED
    SERVICE_TYPE_NASDAQ_BOOK = 25 # NOT MANAGED
    SERVICE_TYPE_OPTIONS_BOOK = 26 # NOT MANAGED
    SERVICE_TYPE_FUTURES_OPTION_BOOK = 27 # NOT MANAGED
    SERVICE_TYPE_NEWS_STORY = 28 # NOT MANAGED
    SERVICE_TYPE_NEWS_HEADLINE_LIST = 29 # NOT MANAGED
    SERVICE_TYPE_UNKNOWN = 30 # **DON'T PASS TO INTERFACE**
    ```

    Java provides an enum within the class:
    ```
    [Java]
    public class StreamingSession implements AutoCloseable {
        ...
        public enum ServiceType implements CLib.ConvertibleEnum {
            NONE(0), // *DONT PASS*
            QUOTE(1),
            OPTION(2),
            LEVELONE_FUTURES(3),
            LEVELONE_FOREX(4),
            LEVELONE_FUTURES_OPTIONS(5),
            NEWS_HEADLINE(6),
            CHART_EQUITY(7),
            CHART_FOREX(8), // NOT WORKING (SERVER SIDE)
            CHART_FUTURES(9),
            CHART_OPTIONS(10), 
            TIMESALE_EQUITY(11),
            TIMESALE_FOREX(12), // NOT WORKING (SERVER SIDE)
            TIMESALE_FUTURES(13),
            TIMESALE_OPTIONS(14),
            ACTIVES_NASDAQ(15),
            ACTIVES_NYSE(16),
            ACTIVES_OTCBB(17),
            ACTIVES_OPTIONS(18), 
            ADMIN(19), // NOT MANAGED
            ACCT_ACTIVITY(20), // NOT MANAGED
            CHART_HISTORY_FUTURES(21), // NOT MANAGED
            FOREX_BOOK(22), // NOT MANAGED
            FUTURES_BOOK(23), // NOT MANAGED
            LISTED_BOOK(24), // NOT MANAGED
            NASDAQ_BOOK(25), // NOT MANAGED
            OPTIONS_BOOK(26), // NOT MANAGED
            FUTURES_OPTIONS_BOOK(27), // NOT MANAGED
            NEWS_STORY(28), // NOT MANAGED
            NEWS_HEADLINE_LIST(29), // NOT MANAGED
            UNKNOWN(30); // *DONT PASS*
            }
            ...
        }
    }
    ```

3. The third argument is a timestamp from the server in milliseconds since the epoch that
is (currently) only relevant for certain callbacks. 

4. The fourth argument is a json string (C/C++/Java) containing admin/error info OR the actual raw data returned from the server. 
    - C++ users can use ```json::parse(string(data))``` 
    - Java users can use ```(new org.json.JSONTokener(data)).nextValue()``` and check if the returned Object is an ```org.json.JSONObject``` or ```org.json.JSONArray```. A data response returns a json array so, alternatively, the ```org.json.JSONArray``` constructor can be used directly. 
    - The python callback will automatically convert the string to a list, dict, or None via json.loads().

The json structure of the response will be dependent on the callback and service type. In order to understand how to parse the object you'll need to refer to the relevant section in [Ameritrade's Streaming documentation](https://developer.tdameritrade.com/content/streaming-data).

##### Summary
StreamingCallbackType | StreamingService  | timestamp   | json 
----------------------|-------------------|-------------|-----
```listening_start``` | ```NONE```      | 0           | {}
```listening_stop```  | ```NONE```      | 0           | {}
```data```            | *YES*           | *YES*       | *StreamingService dependent*
```request_response```| *YES*           | *YES*       | {"request_id":12, "command":"SUBS", "code":0, "message":"msg from server"}
```notify```          | ```NONE```      | 0           | *Message Type dependent*
```notify```          | ```NONE```      | 0           | {"heartbeat":"1565322739463"}
```timeout```         | ```NONE```      | 0           | {}
```error```           | ```NONE```      | 0           | {"error":"error message"}

#### Start

Once a Session is created it needs to be started and different services need to be subscribed to.  Starting a session will automatically try to log the user in. In order to start, three conditions must be met:

1. No other Sessions with the same Primary Account ID can be active. An active session is one that's been started and not stopped. 
2. It must have at least one subscription. (Subscription objects are explained in the [Subscriptions Section](#subscriptions).)
3. ALL subscriptions must use 'CommandType' SUBS. (The default value, see the [Subscriptions Section](#subscriptions) ) 

If not the start call will throw ```StreamingException``` (C++), ```TDAmeritradeAPI.CLibException``` (Java), ```clib.CLibException``` (Python), or return ```TDMA_API_STREAM_ERROR``` (C).

```
[C++]
bool
StreamingSession::start(const StreamingSubscription& subscription);

[C++]
deque<bool> 
StreamingSession::start(const vector<StreamingSubscription>& subscriptions);

[C]
inline int
StreamingSession_Start( StreamingSession_C *psession,
                        StreamingSubscription_C **subs,
                        size_t nsubs,
                        int *results_buffer );

[Python]
def stream.StreamingSession.start(self, *subscriptions):

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public List<Boolean>
    start( List<StreamingSubscription> subscriptions ) throws CLibException;

    public boolean
    start( StreamingSubscription subscription ) throws CLibException;
    ...
}

```


The C++ methods take a single subscription object or a vector of different ones. Both return the success/failure state of each subscription in the order they were passed. Errors result in a ```StreamingException```.

The Java methods take a single subscription object or a list of different ones. Both return the success/failure state of each subscription in the order they were passed. Errors result in a ```TDAmeritradeAPI.CLibException```.

The Python method takes one or more subscription objects, returning a list of bools to indicate success/failure of each. Other errors result in a ```clib.CLibException```.

The C method takes an array of subscription proxy objects (cast to their generic 'base' type pointer) and returns the success/failure state of each subscription in a 'results_buffer'
array. This int array must be allocated by the caller to at least the size of the 'subs' array, or the arg can be set to NULL to ignore results.

#### Stop

Stopping a session will log the user out and remove all subscriptions.

```
[C++]
void
StreamingSession::stop();

[C]
inline int
StreamingSession_Stop( StreamingSession_C *psession );

[Python]
def stream.StreamingSession.stop(self):

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public void stop() throws CLibException
    ...
}
```

#### Add 

Subscriptions can only be added **to a started session**. If you try to add a subscription to a stopped session it will throw ```StreamingException``` (C++), ```clib.CLibException``` (Python), ```TDAmeritradeAPI.CLibException``` (Java), or return ```TDMA_API_STREAM_ERROR``` (C). The return value(s) or populated results buffer(C) indicate the success/failure of each subscription.

```
[C++]
std::deque<bool> 
StreamingSession::add_subscriptions(const vector<StreamingSubscription>& subscription);

[C++]
bool
StreamingSession::add_subscription(const StreamingSubscription& subscription);

[C]
inline int
StreamingSession_AddSubscriptions( StreamingSession_C *psession, 
                                   StreamingSubscription_C **subs,
                                   size_t nsubs,
                                   int *results_buffer ); 

[Python]
def stream.StreamingSession.add_subscriptions(self, *subscriptions):

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public List<Boolean>
    add( List<StreamingSubscription> subscriptions ) throws CLibException;
    
    public boolean
    add( StreamingSubscription subscription ) throws CLibException;
    ...
}
```

You can add multiple subscription instances but instances of the same type will override older/preceding ones.

To update a subscription you can use ADD, VIEW, or UNSUBS commands. (see below)

To replace a subscription you can override it by adding a new subscription of
the same type(this appears to work but haven't tried every possible combination so
there's no guarantee) or using the UNSUBS command followed by the SUBS command or by stopping the session and then restarting with new subscriptions. It's best to avoid doing the latter because the session object will go through 
a somewhat costly life-cycle:
```
    STOP
        ._stop_listener_thread()
        ._logout() 
        ._client->close() 
    START
        ._client->connect() 
        ._login() 
        ._start_listener_thread()
        ._subscribe()
```
It should also be considered poor practice to continually create and tear-down connections with the server.

#### QOS

To get or set the update latency(quality-of-service) of the connection use:

```
[C++]
QOSType
StreamingSession::get_qos() const;

[C]
inline int
StreamingSession_GetQOS( StreamingSession_C *psession, QOSType *qos);

[Python]
def stream.StreamingSession.get_qos(self):

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public QOSType getQOS() throws CLibException 
    ...
}
```
```
[C++]
bool
StreamingSession::set_qos(const QOSType& qos);

[C+]
inline int
StreamingSession_SetQOS( StreamingSession_C *psession, QOSType qos, int *result);

[Python]
def stream.StreamingSession.set_qos(self, qos):

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public boolean setQOS( QOSType qos ) throws CLibException 
    ...
}
```
```
[C++]
enum class QOSType : int {
    express,   /* 500 ms */
    real_time, /* 750 ms */
    fast,      /* 1000 ms DEFAULT */
    moderate,  /* 1500 ms */
    slow,      /* 3000 ms */
    delayed    /* 5000 ms */
};

[C]
enum QOSType {
    QOSType_express,   /* 500 ms */
    QOSType_real_time, /* 750 ms */
    QOSType_fast,      /* 1000 ms DEFAULT */
    QOSType_moderate,  /* 1500 ms */
    QOSType_slow,      /* 3000 ms */
    QOSType_delayed    /* 5000 ms */
};

[Python]
QOS_EXPRESS = 0 
QOS_REAL_TIME = 1
QOS_FAST = 2
QOS_MODERATE = 3
QOS_SLOW = 4
QOS_DELAYED = 5

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public enum QOSType implements CLib.ConvertibleEnum {
        EXPRESS(0),
        REAL_TIME(1),
        FAST(2),
        MODERATE(3),
        SLOW(4),
        DELAYED(5);  
        ...
    }
    ...
}
```

#### Destroy

When completely done, the session should be destroyed. The C++ shared_ptr, Java, and Python class will do this for you(assuming there aren't any other references to the object). 

In C you'll need to use:
```
[C]
inline int
StreamingSession_Destroy( StreamingSession_C *psession);
```

Destruction will stop the session first, logging the user out. Using the proxy object after this point results in ***UNDEFINED BEHAVIOR***.

### Subscriptions

Subscriptions in C++, Python, and Java are managed using classes that derive from ```StreamingSubscription```, ```stream._StreamingSubscription```, and ```io.github.jeog.tdameritradeapi.stream.StreamingSubscription```, respectively. The [Subscription Classes section](README_STREAMING.md#subscription-classes) below describes the interfaces for each of the classes.

Subscriptions in C are managed using proxies(simple C structs) that contain a generic pointer to the underlying C++ subscription object and can be passed to calls that mimic the methods of the underlying C++ object.

Currently there  are 'managed' subscriptions which handle most of the details for you and a 'raw' subscription
which allows for any combination of strings for the service type, command type, and parameters. 
There are no managed subscriptions for services not documented by TDMA so you can use a raw
subscription to attempt to access/experiment with them.

#### Managed Subscriptions

There are two basic types of managed subscription objects:

##### Symbol / Field 

- ```QuotesSubscription```
- ```OptionsSubscription```
- ```LevelOneFuturesSubscription```
- ```LevelOneForexSubscription```
- ```LevelOneFuturesOptionsSubscription```
- ```NewsHeadlineSubscription```
- ```ChartEquitySubscription```
- ```ChartFuturesSubscription```
- ```ChartOptionsSubscription```
- ```TimesaleFuturesSubscription```
- ```TimesaleEquitySubscription```
- ```TimesaleOptionsSubscription```

These use a combination of security symbols/strings and field numbers representing what type of data to return. In C/C++ these field numbers are found in the 'Field' suffixed enums:

- ```QuotesSubscriptionField```
- ```OptionsSubscriptionField```
- ```LevelOneFuturesSubscriptionField```
- ```LevelOneForexSubscriptionField```
- ```LevelOneFuturesOptionsSubscriptionField```
- ```NewsHeadlineSubscriptionField```
- ```ChartEquitySubscriptionField```
- ```ChartSubscriptionField```
- ```TimesaleSubscriptionField```

See the [Subscription Classes Section](#subscription-classes) for the subscription interfaces and enum definitions.

In Python these fields are defined in the subscription class (or its base class) and are all of the form FIELD_[].

In Java these fields are defined as enums named 'FieldType', inside the relevant Subscription class.

Some objects share a Field enum. For instance, ```TimesaleEquitySubscription``` and
```TimesaleOptionsSubscription``` use fields from ```enum TimesaleSubscriptionField```. In Python and Java these fields are defined in a shared base class. 

To create a subscription you'll pass symbol strings AND values from the appropriate
enum, e.g:
```
[C++]
class QuotesSubscription
        : public SubscriptionBySymbolBase {
    using FieldType = QuotesSubscriptionField;
    ...
    QuotesSubscription( const std::set<std::string>& symbols,
                        const std::set<FieldType>& fields,
                        CommandType command = CommandType::SUBS );
}

[C]
int
QuotesSubscription_Create(const char** symbols, 
                          size_t nsymbols, 
                          QuotesSubscriptionField *fields,
                          size_t nfields,
                          CommandType command,
                          QuotesSubscription_C *psub); // <-- to be populated on success

[Python]
class QuotesSubscription(_SubscriptionBySymbolBase):
    def __init__(self, symbols, fields, command=COMMAND_TYPE_SUBS): 

[Java]
public class QuotesSubscription extends SubscriptionBySymbolBase {
    ...
    public QuotesSubscription( Set<String> symbols, Set<FieldType> fields, CommandType command) 
            throws CLibException;
    
    public QuotesSubscription( Set<String> symbols, Set<FieldType> fields) throws CLibException;
    ...
}
```

The respective base classes define:
```

[C++]
std::set<std::string>
SubscriptionBySymbolBase::get_symbols() const

void
SubscriptionBySymbolBase::set_symbols(const std::set<std::string>& symbols);

[Python]
def stream._SubscriptionBySymbolBase.get_symbols():

def stream._SubscriptionBySymbolBase.set_symbols( symbols ):

[Java]
public abstract class SubscriptionBySymbolBase extends ManagedSubscriptionBase {
    ...
    public void
    setSymbols(Set<String> symbols) throws CLibException;
    
    public Set<String>
    getSymbols() throws CLibException;
    ...
}
```

In C you'll use the appropraitely named calls, e.g:
```
[C]
int
QuotesSubscription_GetSymbols( QuotesSubscription_C *psub, 
                               char ***bufers, 
                               size_t *n );

int
QuotesSubscription_SetSymbols( QuotesSubscription_C *psub, 
                               const char **buffers, 
                               size_t n );
...

```
Remember, its the client's repsonsibility to free the buffers populated by the 'Get' calls:
```
[C]
inline int
FreeBuffers( char** buffers, size_t n);
```

To access the fields, e.g:
```
[C++]
std::set<FieldType> 
QuotesSubscription::get_fields() const

void
QuotesSubscription::set_fields(const std::set<FieldType>& fields);

[Python]
def stream._SubscriptionBySymbolBase.get_fields():

def stream._SubscriptionBySymbolBase.set_fields( fields ):

[Java]
public class QuotesSubscription extends SubscriptionBySymbolBase {
    public void
    setFields( Set<FieldType> fields ) throws CLibException;
    
    public Set<FieldType>
    getFields() throws CLibException;
}
```

In C you'll use the appropriately named calls, e.g:
```
[C]
int
QuotesSubscription_GetFields( QuotesSubscription_C *psub, 
                              QuotesSubscriptionField **fields, 
                              size_t *n);

...

int
QuotesSubscription_SetFields( QuotesSubscription_C *psub, 
                              QuotesSubscriptionField *fields, 
                              size_t n);

...
```
Remember, its the client's repsonsibility to free the buffers returned from the 'Get' calls (cast to int*):
```
[C]
inline int
FreeFieldsBuffer( int* buffer );
```

##### Duration / Venue 

- NasdaqActivesSubscription
- NYSEActivesSubscription
- OTCBBActivesSubscription
- OptionActivesSubscription

Duration/Venue objects are similar except they use DurationType and/or VenueType enum values as arguments, e.g:
```
[C++]
class OptionActivesSubscription
        : public ActivesSubscriptionBase {

    OptionActivesSubscription( VenueType venue, 
                               DurationType duration,
                               CommandType command = CommandType::SUBS );
}

[C]
int
OptionActivesSubscription_Create( VenueType venue,
                                  DurationType duration_type, 
                                  CommandType command,                    
                                  OptionActivesSubscription_C *psub);

[Python]
class OptionActivesSubscription(_ActivesSubscriptionBase):
    def __init__(self, venue, duration, command=COMMAND_TYPE_SUBS):

[Java]
public class OptionActivesSubscription extends ActivesSubscriptionBase {
    ...
    public OptionActivesSubscription( VenueType venue, DurationType duration, CommandType command)
            throws CLibException;

    public OptionActivesSubscription( VenueType venue, DurationType duration) throws CLibException;
    ...
}
```

See the [Subscription Classes Section](#subscription-classes) for more details.

#### Commands 

Each subscription has an associated ```CommandType``` enum that defaults to 'SUBS'.

```
[C++]
enum class CommandType : int {
    SUBS,
    UNSUBS,
    ADD,
    VIEW,
    LOGIN, /* don't use */
    LOGOUT, /* don't use */
    QOS /* don't use */
};

[C]
enum CommandType{
    CommandType_SUBS,
    CommandType_UNSUBS,
    CommandType_ADD,
    CommandType_VIEW,
    CommandType_LOGIN, /* don't use */
    CommandType_LOGOUT, /* don't use */
    CommandType_QOS /* don't use */
};

[Python]

COMMAND_TYPE_SUBS = 0
COMMAND_TYPE_UNSUBS = 1
COMMAND_TYPE_ADD = 2
COMMAND_TYPE_VIEW = 3

[Java]
public class StreamingSession implements AutoCloseable {
    ...
    public enum CommandType implements CLib.ConvertibleEnum {
        SUBS(0),
        UNSUBS(1),
        ADD(2),
        VIEW(3);
        ...
    }
    ...
}
```

- **SUBS** : subscribe to a new type of Streamer Service. If you already have an active subscription(of the current subscription/service type) it will be overriden.

- **UNSUBS**: unsubscribe to all, or part, of an active subscription.

- **ADD** : add to an active subscription, e.g add a symbol to a QuotesSubscription

- **VIEW** : update the fields returned by the an active subscription, e.g add QuotesSubscription::total_volume to a subscription that currently returns QuotesSubscription::last_price

*(LOGIN, LOGOUT, and QOS are only used behind the scenese for implementing the related functionality and will return an error code(C) or throw(C++/Python) if passed to a Subscription object.)*

Different commands allow for different combinations of symbols, fields, durations etc. and the accepted values are too complex to outline. Some general rules:

- Symbol/Field subscriptions don't need to include fields in an UNSUBS command. The symbols passed will be unsubscribed to. Passing no symbols will result in unsubscribing to all symbols for the currently active Streamer Service.

- Symbol/Field subscriptions don't need to include symbols in a VIEW command. Simply include the updated fields you want returned.

If you want to update a subscription using an UNSUBS/ADD/VIEW command a safe technique is to take the original subscription object used to initiate the subscription, change the command type with ```set_command``` and update the other parameters, e.g if using ADD use ```set_symbols``` or VIEW use ```set_fields```.

#### Raw Subscription

A raw subscription allows you to pass your own service type, command type and parameters. Some of the service types enumerated in ```StreamerServiceType``` are not documented by TDMA and are, for the time being, not part of the managed subscription interface. As they are undocumented you may have to email TDMA or use the old API docs, and experiment, to find the acceptable parameters to use.

If you manage to successfully access a service that is not included in the service type enum
the ```request_response``` callback will return the 'int' value of ```StreamerServiceType::UNKNOWN```.

You can replicate a managed subcription with a raw one, but trying to compare them will return false.

An example for subscribing to the 'NASDAQ_BOOK' service :

```
[C++]
std::string service("NASDAQ_BOOK");
std::string command("SUBS");
std::map<std::string, std::string> parameters{
    {"keys","GOOG,APPL"}, 
    {"fields", "0,1,2"}
};

RawSubscription sub(service, command, parameters); 

[C]
RawSubscription_C sub = {0,0};

const char* service = "NASDAQ_BOOK";
const char* command = "SUBS";
KeyValPair parameters[] = {
    {"keys","GOOG,APPL"}, 
    {"fields", "0,1,2"}
};

if( (err = RawSubscription_Create(service, command, parameters, 2, &sub) ){
    // ERROR
}
 
[Python]
sub = stream.RawSubscription( "NASDAQ_BOOK", "SUBS",  
                              { "keys":"GOOG,APPL", "fields":"0,1,2" } )

[Java]
Map<String,String> parameters = new HashMap<String, String>();
subParams.put("keys", "GOOG,APPL");
subParams.put("fields", "0,1,2"); 
RawSubscription sub = new RawSubscription("NASDAQ_BOOK", "SUBS", parameters);

```

Remember, for the C interface it's the client's repsonsibility to free the buffers populated by the 'Get' calls:
```
inline int
FreeBuffer( char* buffer );

inline int
FreeKeyValBuffer( KeyValPair* pkeyvals, size_t n );
```

#### Copy

C++ Subscription objects have copy semantics implemented:
```
QuotesSubscription( const QuotesSubscription& sub ) const;

QuotesSubscription&
operator=( const QuotesSubscription& sub ) const;
```

The C interface has copy constructor functions:
```
int QuotesSubscription_Copy( QuotesSubscription_C* from,
                             QuotesSubscription_C* to )
``` 
**IMPORTANT - the 'to' arg should point at an UNITIALIZED proxy object(before 'Create...' is called on it). Copying into an already created/initialized object will cause a memory leak.**

Python only copies a reference to class instances by default but a deep_copy method is available that will make a copy of the python instance and underlying C object:
```
def stream._StreamingSubscription.deep_copy(self);
```

Currently Java doesn't provide a method for deep copy.

#### Destroy

When done with a subscription it should be destroyed. This is done automatically
in C++ and Python by the object's destructor, and in Java by a 'finalize' method in the JNA struct's 'autoAllocate' method.

In C either cast the proxy object to StreamingSubscription_C*  and use the generic 'Destroy' call:
```
inline int
StreamingSubscription_Destroy( StreamingSubscription_C *sub );
```

.. or use the more type-safe specific version, e.g:
```
inline int
TimesaleEquitySubscription_Destroy( TimesaleEquitySubscription_C *sub ):
```
Using the proxy object after this point results in ***UNDEFINED BEHAVIOR***.

### Example Usage 

#### [C++]
```
    #include <string>  
    #include <iostream>
    #include <chrono>
    
    #include "tdma_api_streaming.h"
    
    using namespace tdma;
    using namespace std;

    ...
 
        set<string> symbols = {"SPY", "QQQ"};

        set<TimesaleSubscriptionField> fields = { 
            TimesaleSubscriptionField::symbol,
            TimesaleSubscriptionField::trade_time, 
            TimesaleSubscriptionField::last_price                 
        } 

        auto sub1 = TimesaleEquitySubscription(symbols, fields);

        //assert symbols == sub1.get_symbols()
        symbols = sub1.get_symbols();

        //assert fields == sub1.get_fields();
        fields = sub1.get_fields();
        
        auto sub2 = NasdaqActivesSubscription( DurationType::min_60 );

        auto cb = [](int cbt, int sst, unsigned long long ts, const char* s)
        {
            auto cb_type = static_cast<StreamingCallbackType>(cbt);
            auto ss_type = static_cast<StreamerServiceType(sst);
            cout<< "CALLBACK: " << to_string(cb_type) << endl
                << "\t service: " << to_string(ss_type) << endl
                << "\t timestamp: " << ts << endl
                << "\t content: " << json::parse(string(s)) << endl 
                << endl;
        }

        {
            std::shared_ptr<StreamingSession> session = 
                StreamingSession::Create( credentials_manager.credentials, cb );

            bool result = session->start( sub1 );
            //assert result
            this_thread::sleep_for( chrono::seconds(5) );

            result = session->add_subscription( sub2 );
            //assert result
            this_thread::sleep_for( chrono::seconds(5) );

            // add a symbol to subscription 1         
            sub1.set_command( CommandType::ADDS );
            sub1.set_symbols( {"IWM"} );
            result = session->add_subscription( sub1 );          

            // unsubscribe to subscription 2
            sub2.set_command( CommandType::UNSUBS );
            result = session->add_subscription( sub2 );

            session->stop(); // clears subscriptions as well
            this_thread::sleep_for( chrono::seconds(5) );

            /*
             * Note to restart using the old subs we need to reset the command type 
             * back to SUBS or the 'start' call will throw
             */
            sub1.set_command( CommandType::SUBS );        
            sub2.set_command( CommandType::SUBS );    
            auto results = session->start( {sub1, sub2} ); 
            // assert each result
            this_thread::sleep_for( chrono::seconds(5) );

        } /* session goes out of scope:
           *    1) smart pointer calls destructor on proxy object
           *    2) proxy object's smart ptr calls destroy through the ABI
           *    3) destroy method calls the implementation objects destructor
           *    4) that destructor calls stop() and closes the session/connection
           */
    
    ...
```

#### [C]
```
    #include <stdio.h>
    #include <stdlib.h>
    
    #include "tdma_api_streaming.h"
  
    void
    callback(int cb_type, int ss_type, unsigned long long ts, const char* s){
        //
    }

    ...    

        int err = 0;

        // our proxy objects
        TimesaleEquitySubscription_C sub1; 
        NasdaqActivesSubscription_C sub2;

        const char* symbols[] = {"SPY", "QQQ"} 
        TimesaleSubscription fields[] = {      
            TimesaleSubscriptionField_symbol,
            TimesaleSubscriptionField_trade_time,
            TimesaleSubscriptionField_last_price
        };

        // create the first subscription
        err = TimesaleEquitySubscription( symbols, 2, fields, 3, &sub1 );
        if( err ){
            //
        }

        // create the second
        err = NasdaqActivesSubscription( DurationType_min_60, &sub2 );
        if( err ){
            //
        }
        
        TimesaleSubscriptionField *sub1_fields = NULL;
        char **sub1_symbols = NULL;
        size_t n = 0;

        // get fields from the first (notice the name is of the 'base' type)
        err = TimesaleSubscription_GetFields( &sub1, &sub1_fields, &n) )  
        if ( err ){
          //
        }

        // when done w/ the fields free them
        if( sub1_fields ){
            FreeFieldsBuffer( (int*)sub1_fields );
            sub1_fields = NULL;
            n = 0;
        }

        // get symbols from the first (notice the name is of the actual type)
        err = TimesaleEquitySubscription_GetSymbols( &sub1, &sub1_symbols, &n) )  
        if ( err ){
          //
        }

        // when done w/ the symbols free them
        if( sub1_symbols ){
            FreeBuffers( sub1_symbols, n );
            sub1_symbols = NULL;
            n = 0;
        }


        // streaming session proxy
        StreamingSession_C session;
    
        // create a session using a pointer to the Credentials struct
        err = StreamingSession_Create( pcreds, &callback, &session );
        if( err ){
            //
        }

        // combine subscriptions, casting each sub to its 'base' pointer
        StreamingSubscription_C* subs[] = {
            (StreamingSubscription_C*)&sub1, 
            (StreamingSubscription_C*)&sub2, 
        };

        // results buffer
        int results[] = {-1,-1};        

        // start session w/ an array of subscriptions
        err = StreamingSession_Start( &session, subs, 2, results );
        if( err ) {
            //
        }

        // To update sub1 use:
        //   TimesaleSubscription_SetCommand
        //   TimesaleSubscription_SetSymbols
        //   TimesaleSubscription_SetFields

        // we no longer need the subscriptions so lets destroy them
        for(int i = 0; i < (sizeof(subs)/sizeof(StreamingSubscription_C*)); ++i){
            err = StreamingSubscription_Destroy( subs[i] );
            if( err ){
                //
            }
            subs[i] = NULL;
        }

        // StreamingSession_AddSubscriptions (if we want more subs)
        // StreamingSession_SetQOS (if we want to change latency)

        // stop the session
        err = StreamingSession_Stop( &session );
        if( err ){
            //
        }

        // and finally, destroy it
        err = StreamingSession_Destroy( &session );
        if( err ){
            //
        }                

    ...
```

#### [Python]
```
    import time
    from tdma_api import clib, auth, stream
    
    ...    

    if not clib._lib:
        clib.init("path/to/lib/libTDAmeritrade.so")

    def callback(cb, ss, ts, msg):
        cb_str = stream.callback_type_to_str(cb)
        ss_str = stream.service_type_to_str(ss)
        print("--CALLBACK" + "-" * 70)
        print("  Type      ::", cb_str.ljust(16))
        print("  Service   ::", ss_str.ljust(25))
        print("  TimeStamp ::", str(ts))
        if cb == stream.CALLBACK_TYPE_DATA:   
            print("  Data     ::", ss_str)
                if type(msg) is dict:
                    for k, v in msg.items():
                        print( "   ", k, str(v) )
                elif type(msg) is list:
                    for v in msg:
                        print( "   ", str(k) ) 
                else:
                    print( "   ", str(msg) )
        else:
            print("  Message  ::", str(msg))
        print("-" * 80)

    with auth.CredentialManager("path/to/creds/file", "password") as cm:
        try:
            # CREATE A SUBSCRIPTION
            QSUB = stream.QuotesSubscription
            symbols = ('SPY', 'QQQ', 'IWM')    
            fields = (QSUB.FIELD_SYMBOL, QSUB.FIELD_BID_PRICE, QSUB.FIELD_ASK_PRICE)
            qsub = QSUB(symbols, fields)   

            assert set(qsub.get_symbols()) == set(symbols)         

            # CREATE A SESSION
            session = stream.StreamingSession(cm.credentials, callback)

            # START THE SESSION
            assert session.start(qsub)
            assert session.is_active()

            # DECREASE THE LATENCY
            assert session.set_qos(stream.QOS_REAL_TIME)
            assert session.get_qos() == stream.QOS_REAL_TIME
            time.sleep(10)

            # CREATE A SECOND SUBSCRIPTION
            OPSUB = stream.OptionsSubscription
            symbols = ["SPY_081718C286", "SPY_081718P286"]
            fields = [OPSUB.FIELD_DELTA, OPSUB.FIELD_GAMMA, OPSUB.FIELD_VEGA]
            opsub = OPSUB(symbols, fields)

            assert OPSUB.FIELD_DELTA in opsub.get_fields()

            # CREATE A THIRD SUBSCRIPTION
            ASUB = stream.OptionActivesSubscription
            asub = ASUB( venue=ASUB.VENUE_TYPE_PUTS_DESC,
                         duration=ASUB.DURATION_TYPE_MIN_60 )

            assert asub.get_duration() == ASUB.DURATION_TYPE_MIN_60

            # ADD THEM TO THE SESSION
            assert all( session.add_subscriptions(opsub, asub) )
            time.sleep(10)

            # REMOVE 'IWM' FROM SUBSCRIPTION 1
            qsub.set_symbols( ['IWM'] )
            qsub.set_command( COMMAND_TYPE_UNSUBS )
            assert session.add_subscription(qsub)

            # UPDATE FIELDS IN SUBSCRIPTION 1
            qsub.set_symbols( ('SPY', 'QQQ') )
            qsub.set_fields( (QSUB.FIELD_SYMBOL, QSUB.FIELD_LAST_PRICE) )
            qsub.set_command( COMMAND_TYPE_VIEW )
            assert session.add_subscription(qsub)    

            # ADD SYMBOL TO SUBSCRIPTION 1
            qsub.set_symbols( ['TLT'] )
            qsub.set_command( COMMAND_TYPE_ADD )
            assert session.add_subscription(qsub)

            # UNSUBSCRIBE TO ALL SYMBOLS IN SUBSCRIPTION 1
            qsub.set_symbols( [] )
            qsub.set_command( COMMAND_TYPE_UNSUBS )
            assert session.add_subscription(qsub)

            # STOP THE SESSION
            session.stop()
            assert not session.is_active()
      
        except clib.CLibException as e:
            print( str(e) )
            raise
    
    ...
```

#### [Java]
```
    import io.github.jeog.tdameritradeapi.Auth; 
    import io.github.jeog.tdameritradeapi.Auth.CredentialsManager;
    import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
    import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
    import io.github.jeog.tdameritradeapi.stream.StreamingSession;
    import io.github.jeog.tdameritradeapi.stream.TimesaleEquitySubscription;

    import org.json.JSONObject;

    ... 

        static class MyCallback implements StreamingSession.Callback{        
            public void 
            call(int callbackType, int serviceType, long timestamp, String data) {       
         
                String msg = String.format("[service=%d, callback=%d, timestamp=%d]", 
                        serviceType, callbackType, timestamp);                        
                System.out.println("CALLBACK " + msg);
                
                if( CallbackType.fromInt(callbackType).equals(CallbackType.DATA) ) 
                {
                    try {
                        String d = data.trim();
                        
                        // probably can just use 'new JSONArray(d)' but for thoroughness...
                        if( d.startsWith("[") ) {
                            JSONArray j = new JSONArray(d);
                            for( int i = 0; i < j.length(); ++i ) {
                                System.out.println( j.getJSONObject(i).toString() );
                            }
                        }else if( d.startsWith("{") ) {
                            JSONObject j = new JSONObject(d);
                            System.out.println( j.toString(4) );
                        }else {
                            System.err.println("* unrecognizable json *");
                        }
                      
                        System.out.println("* * *");
                    }catch( JSONException exc ) {
                        System.err.println("JSON exc: " + exc.getMessage());
                        System.err.println("  " + data);
                    }
                }
            }        
            }

    ...

        TDAmeritradeAPI.init(libPath);

        try( new CredentialsManager cm = new CredentialsManager(credsPath, credsPassword) ){

            // (prepare to) use equity time & sales using ALL available fields
            TimesaleEquitySubscription sub1 = new TimesaleEquitySubscription( 
                new HashSet<String>(Arrays.asList("SPY","QQQ"))
            );

            // (prepare to) use level 1 futures bid,ask,last data for Gold and 10yr Note futures
            Set<String> lofSymbols = new HashSet<String>(Arrays.asList("/GC","/ZN"));
            Set<LevelOneFuturesSubscription.FieldType> lofFields = 
                new HashSet<LevelOneFuturesSubscription.FieldType>();
            lofFields.add( LevelOneFuturesSubscription.FieldType.BID_PRICE );
            lofFields.add( LevelOneFuturesSubscription.FieldType.ASK_PRICE );
            lofFields.add( LevelOneFuturesSubscription.FieldType.LAST_PRICE );
            LevelOneFuturesSubscription sub2 = new LevelOneFuturesSubscription(lofSymbols, lofFields);
            
            // going to pass the two subscriptions when we start the session
            List<StreamingSubscription> subcriptions = 
                new ArrayList<StreamingSubscription>( Arrays.asList(sub1, sub2) );   

            // create the session object    
            try( StreamingSession session = new StreamingSession(cm.getCredentials(), new MyCallback()) ){

                // start it with both subscriptions
                List<Boolean> results = session.start( subscriptions );
                for( Boolean result : results ){
                    // check if succesful
                }

                // (prepare to) drop subscription 1
                sub1.setCommand( StreamingSession.CommandType.UNSUBS );
            
                // (prepare to) change fields in subscription 2
                lofFields.clear();
                lofFields.add( LevelOneFuturesSubscription.FieldType.TRADE_TIME );
                lofFields.add( LevelOneFuturesSubscription.FieldType.LAST_SIZE );            
                sub2.setCommand( StreamingSession.CommandType.VIEW );
                sub2.setFields( lofFields );

                // execute the changes/updates
                results = session.add( subscriptions ); // 'subscriptions' list has refs to sub1 and sub2
                for( Boolean result : results ){
                    // check if succesful
                }

                // (perpare to) add symbol to subscription 2
                lofSymbols.add( "/CL" );
                sub2.setSymbols(lofSymbols);
                sub2.setCommand( StreamingSession.CommandType.ADD );
        
                // execute the change/update
                boolean result = session.add( sub2 );
                if( !result ){
                    // check if succesful
                }

            } // .stop() called for us when exiting the try-with-resources block

        }catch( CLibException exc ){
            // ERROR
        }

```

### Subscription Classes
- - -

Only the C++ Subscriptions are detailed below. The Python, Java, and C interfaces match these almost exactly. 

The C interface uses appropriately named functions to mimic the methods of the C++ classes and requires explicit use of the ```Create``` functions for construction and ```Destroy``` functions for destruction. See tdma_api_streaming.h for function prototypes.*

#### Managed Subscription Classes

*The C++ enum types are shown below; The C versions prepend the enum type name and an underscore to the value name, e.g*
```
[C++]
QuotesSubscriptionField field = QuotesSubscriptionField::last_price;
//or
QuotesSubscriptionField field = QuotesSubscription::FieldType::last_price;

[C]
QuotesSubscriptionField field = QuotesSubscriptionField_last_price;
```

The Python versions are defined as class variables of the particular subcription object in the form of FIELD_[], e.g:
```
[Python]
class stream.QuotesSubscription(_SubscriptionBySymbolBase):
    ...
    FIELD_SYMBOL = 0
    FIELD_BID_PRICE = 1
    FIELD_ASK_PRICE = 2
    FIELD_LAST_PRICE = 3
    ...
```

The Java versions are defined as enums inside the subscription class, e.g:
```
[Java]
public class QuotesSubscription extends SubscriptionBySymbolBase {

    public enum FieldType implements CLib.ConvertibleEnum {
        SYMBOL(0),
        BID_PRICE(1),
        ASK_PRICE(2),
        ...
    }
    ...
}
```

#### QuotesSubscription

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
QuotesSubscription( const set<string>& symbols, 
                    const set<FieldType>& fields,
                    CommandType command = CommandType::SUBS );

    symbols :: symbols to retrieve quote data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
    command :: control the subscription type
```
**types**
```
enum class QuotesSubscriptionField : int {
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

using FieldType = QuotesSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
QuotesSubscription::get_fields() const;
```
```
void
QuotesSubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### OptionsSubscription

Streaming option data that calls back when changed.

**constructors**
```
OptionsSubscription::OptionsSubscription( const set<string>& symbols, 
                                          const set<FieldType>& fields,
                                          CommandType command = CommandType::SUBS );

    symbols :: symbols to retrieve quote data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
    command :: control the subscription type
```
**types**
```
enum class OptionsSubscriptionField : int {
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

using FieldType = OptionsSubscriptionField;        

```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
OptionsSubscription::get_fields() const;
```
```
void
OptionsSubscription::set_fields( const set<FieldType>& fields );
```
<br>


#### LevelOneFuturesSubscription

Streaming futures data that calls back when changed.

**constructors**
```
LevelOneFuturesSubscription::LevelOneFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
    command :: control the subscription type
```
**types**
```
enum class LevelOneFuturesSubscriptionField : int {
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

using FieldType = LevelOneFuturesSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
LevelOneFuturesSubscription::get_fields() const;
```
```
void
LevelOneFuturesSubscription::set_fields( const set<FieldType>& fields );
```
<br>


#### LevelOneForexSubscription

Streaming forex data that calls back when changed.

**constructors**
```
LevelOneForexSubscription::LevelOneForexSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
    command :: control the subscription type
```
**types**
```
enum class LevelOneForexSubscriptionField : int {
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

using FieldType = LevelOneForexSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
LevelOneForexSubscription::get_fields() const;
```
```
void
LevelOneForexSubscription::set_fields( const set<FieldType>& fields );
```
<br>


#### LevelOneFuturesOptionsSubscription

Streaming futures-options data calls back when changed.

**constructors**
```
LevelOneFuturesOptionsSubscription::LevelOneFuturesOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve level-one data for
    fields  :: fields of data to retrieve (bid_price, quote_time etc.)
    command :: control the subscription type
```
**types**
```
enum class LevelOneFuturesOptionsSubscriptionField : int {
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
    
using FieldType = LevelOneFuturesOptionsSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
LevelOneFuturesOptionsSubscription::get_fields() const;
```
```
void
LevelOneFuturesOptionsSubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### NewsHeadlineSubscription

Streaming news headlines as a sequence.

**constructors**
```
NewsHeadlineSubscription::NewsHeadlineSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve news headlines for
    fields  :: fields of each headline to return (headline, story_source etc.)
    command :: control the subscription type
```

**types**
```
enum class NewsHeadlineSubscriptionField : int {
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

using FieldType = NewsHeadlineSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
NewsHeadlineSubscription::get_fields() const;
```
```
void
NewsHeadlineSubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### ChartEquitySubscription
Streaming 1-minute OHLCV equity values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartEquitySubscription::ChartEquitySubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum class ChartEquitySubscriptionField :int {
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

using FieldType = ChartEquitySubscriptionField;
```
***methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
ChartEquitySubscription::get_fields() const;
```
```
void
ChartEquitySubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### ChartFuturesSubscription
Streaming 1-minute OHLCV futures values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartFuturesSubscription::ChartFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum class ChartSubscriptionField : int {
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

using FieldType = ChartSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
ChartFuturesSubscription::get_fields() const;
```
```
void
ChartFuturesSubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### ChartOptionsSubscription
Streaming 1-minute OHLCV option values as a sequence. The bar falls on the 0 
second and includes data between 0 and 59 seconds. (e.g 9:30 bar is 9:30:00 -> 9:30:59)

**constructors**
```
ChartOptionsSubscription::ChartOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS 
    );

    symbols :: symbols to retrieve ohlcv values for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum class ChartSubscriptionField : int {
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

using FieldType = ChartSubscriptionField;
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
ChartOptionsSubscription::get_fields() const;
```
```
void
ChartOptionsSubscription::set_fields( const set<FieldType>& fields );
```
<br>


#### TimesaleEquitySubscription
Streaming time & sales equity trades as a sequence.

**constructors**
```
TimesaleEquitySubscription::TimesaleEquitySubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum TimesaleSubscriptionField : int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };

using FieldType = TimesaleSubscriptionField
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
TimesaleEquitySubscription::get_fields() const;
```
```
void
TimesaleEquitySubscription::set_fields( const set<FieldType>& fields );
```
<br>

#### TimesaleFuturesSubscription
Streaming time & sales futures trades as a sequence.

**constructors**
```
TimesaleFuturesSubscription::TimesaleFuturesSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum TimesaleSubscriptionField : int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };

using FieldType = TimesaleSubscriptionField
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
TimesaleFuturesSubscription::get_fields() const;
```
```
void
TimesaleFuturesSubscription::set_fields( const set<FieldType>& fields );
```
<br>


#### TimesaleOptionsSubscription
Streaming time & sales option trades as a sequence.

**constructors**
```
TimesaleOptionsSubscription::TimesaleOptionsSubscription( 
        const set<string>& symbols, 
        const set<FieldType>& fields,
        CommandType command = CommandType::SUBS 
    );

    symbols :: symbols to retrieve time & sales data for
    fields  :: fields to retrieve 
    command :: control the subscription type
```
**types**
```
enum TimesaleSubscriptionField : int{
        symbol,
        trade_time,
        last_price,
        last_size,
        last_sequence
    };

using FieldType = TimesaleSubscriptionField
```
**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
CommandType
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
set<string>
SubscriptionBySymbolBase::get_symbols() const;
```
```
void
SubscriptionBySymbolBase::set_symbols( const set<string>& symbols );
```
```
set<FieldType>
TimesaleOptionsSubscription::get_fields() const;
```
```
void
TimesaleOptionsSubscription::set_fields( const set<FieldType>& fields );
```
<br>



#### NasdaqActivesSubscription

Most active NASDAQ traded securies for various durations.

**constructors**
```
NasdaqActivesSubscription::NasdaqActivesSubscription(
    DurationType duration,
    CommandType command = CommandType::SUBS
);

    duration :: period over which to return actives
    command :: control the subscription type
```

**types**
```
enum class DurationType : int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
```

**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
string
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
```
void
ActivesSubscriptionBase::set_duration( DurationType duration );
```
<br>

#### NYSEActivesSubscription

Most active NYSE traded securies for various durations.

**constructors**
```
NYSEActivesSubscription::NYSEActivesSubscription(
    DurationType duration,
    CommandType command = CommandType::SUBS
);

    duration :: period over which to return actives
    command :: control the subscription type
```

**types**
```
enum class DurationType : int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
};
```

**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
string
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
```
void
ActivesSubscriptionBase::set_duration( DurationType duration );
```
<br>

#### OTCBBActivesSubscription

Most active OTCBB traded securies for various durations.

**constructors**
```
OTCBBActivesSubscription::OTCBBActivesSubscription(
    DurationType duration,
    CommandType command = CommandType::SUBS
);

    duration :: period over which to return actives
    command :: control the subscription type
```

**types**
```
enum class DurationType : int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
```

**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
string
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
```
void
ActivesSubscriptionBase::set_duration( DurationType duration );
```
<br>


#### OptionActivesSubscription

Most active Options of various types and durations traded.

**constructors**
```
OptionActivesSubscription::OptionActivesSubscription( 
    VenueType venue,
    DurationType duration,
    CommandType command = CommandType::SUBS 
);

    venue    :: type of option (put, call-desc)
    duration :: period over which to return actives
    command :: control the subscription type

```

**types**
```
enum class DurationType : int {
        all_day,
        min_60,
        min_30,
        min_10,
        min_5,
        min_1
    };
```
```
enum class VenueType : int {
        opts,
        calls,
        puts,
        opts_desc,
        calls_desc,
        puts_desc    
};
```

**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
string
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```
```
DurationType
ActivesSubscriptionBase::get_duration() const;
```
```
VenueType
OptionActivesSubscription::get_venue() const;
```
```
void
OptionActivesSubscription::set_venue( VenueType venue );
```
<br>

#### RawSubscription

Custom subscription objects.

**constructors**
```
RawSubscription::RawSubscription( 
    const std::string& service,
    const std::string& command,
    const std::map<std::string, std::string>& parameters
);

    service    :: service type to access
    command    :: control the subscription type
    parameters :: control the type of data returned
```

**methods**
```
std::string
RawSubscription::get_service_str() const;
```
```
void
RawSubscription::set_service_str(const std::string& service);
```
```
std::string
RawSubscription::get_command_str() const;
```
```
void
RawSubscription::set_command_str(const std::string& command);
```
```
std::map<std::string, std::string>
RawSubscription::get_parameters() const;
```
```
void
RawSubscription::set_parameters( std::map<std::string, std::string>& parameters );
```
<br>

#### AcctActivitySubscription

Order & Position updates. Returned data is an array of JSON; successfull responses
will contain message data as a string of XML. (Sorry, this is what they return!)

See the python ```ParseResponseData``` docstring (below in utilities secion) 
for a basic explanation of the structure of returned data. For more information see 
[section 6.0 ACCT_ACTIVITY of the TDMA docs](https://developer.tdameritrade.com/content/streaming-data).


**constructors**
```
AcctActivitySubscription::AcctActivitySubscription( 
    CommandType command = CommandType::SUBS
);
   
    command    :: control the subscription type (SUBS or UNSUBS)  
```

**methods**
```
StreamerServiceType
ManagedSubscription::get_service() const;
```
```
string
ManagedSubscription::get_command() const;
```
```
void
ManagedSubscription::set_command(CommandType command);
```

**utilities**

Python provides a static parse call for converting the returned JSON/XML to python objects.

The docstring may be helpful if you're trying to parse the JSON/XML from another language.

```
    @staticmethod
    def AcctActivitySubscription.ParseResponseData(data):
        """Convert responses containing XML to a list-of-dict-of-[dict|str|None]

        The 'data' 4th arg of the callback should be passed ONLY when the callback
        type is of CALLBACK_TYPE_DATA and the service type is of 
        SERVICE_TYPE_ACCT_ACTIVITY. 

        'data' should be of form:
        [ 
            {"1":"<account>", "2":"SUBSCRIBED", "3":"",
            {"1":"<account>", "2":"<message_type>", "3":"<message_data xml></>",
            {"1":"<account>", "2":"ERROR", "3":"error message",
            {"1":"<account>", "2":"<message_type>", "3":"<message_data xml></>",
            ...
        ]

        Returns an array of dicts of one of the following forms:
                                
        1) {"account":"<account>", "message_type":"SUBSCRIBED", "message_data":""}
        2) {"account":"<account>", "message_type":"ERROR", "message_data":"error message"}
        3) {"account":"<account>", "message_type":"<message_type>", "message_data":dict() }

        Form 1 is returned initially on subscription, form 3 contains valid responses 
        and its message_type is one of the strs represented by the MSG_TYPE_[] constants. 
        "message_data" is a dict of (hierarchical) key/val (tag/text) pairs pulled from the 
        returned XML. 

        NOTES:
            1) the returned dict excludes the top-level message-type tag 
               (e.g 'OrderCancelRequestMessage')
            2) all data are of type str or None      
            3) if xml (unexpectedly) has 'text' and child nodes the text will be added to the 
               dict with key '#text'
            4) earlier versions returned a flattend dict(changed because of naming collisions)

        e.g 

        >> def callback(cb, ss, t, data):
        >>   if cb == stream.CALLBACK_TYPE_DATA and ss == stream.SERVICE_TYPE_ACCT_ACTIVITY:
        >>      for resp in AcctActivitySubscription.ParseResponseData(data):
        >>          msg_ty = resp['message_type']
        >>          if msg_ty == MSG_TYPE_SUBSCRIBED:
        >>              print("SUBCSRIBED")
        >>          elif msg_ty == MSG_TYPE_ERROR:
        >>              print("ERROR: ", resp['message_data'])
        >>          else:
        >>              print("ACCT ACTIVITY RESPONSE: %s" % resp['message_type'])
        >>              print( json.dumps(resp['message_data'], indent=4) )
        >>
        >> ... (on callback) ...
        >>
        >> ACCT ACTIVITY RESPONSE: OrderEntryRequest
        >> {
        >>     "ActivityTimestamp": "2019-09-22T19:41:25.582-05:00",
        >>     "LastUpdated": "2019-09-22T19:41:25.582-05:00",
        >>     ...
        >>     "Order": {
        >>         "OpenClose": "Open",
        >>         "Security": {
        >>             "CUSIP": "0SPY..JI90250000",
        >>             "SecurityType": "Call Option",
        >>             "Symbol": "SPY_101819C250"
        >>         },
        >>         ...
        >>    ...
        >> } 
        >>                 
        """          
        
```
