### HTTPS Get Interface
- - -

- [Overview](#overview)
- [Certificates](#certificates)
- [Using Getter Objects](#using-getter-objects)
    - [C++](#c)
    - [C](#c-1)
    - [Python](#python)
- [Throttling](#throttling)
- [Utilities](#utilities)
    - [Option Symbols](#option-symbols)
- [Example Usage](#example-usage)
    - [C++](#c-2)
    - [C](#c-3)
    - [Python](#python-1)
- [Getter Classes](#getter-classes)
    - [QuoteGetter](#quotegetter)  
    - [QuotesGetter](#quotesgetter)  
    - [MarketHoursGetter](#markethoursgetter)  
    - [MoversGetter](#moversgetter)  
    - [HistoricalPeriodGetter](#historicalperiodgetter)  
    - [HistoricalRangeGetter](#historicalrangegetter)  
    - [OptionChainGetter](#optionchaingetter)  
    - [OptionChainStrategyGetter](#optionchainstrategygetter)  
    - [OptionChainAnalyticalGetter](#optionchainanalyticalgetter)  
    - [AccountInfoGetter](#accountinfogetter)  
    - [PreferencesGetter](#preferencesgetter)  
    - [UserPrincipalsGetter](#userprincipalsgetter)  
    - [StreamerSubscriptionKeysGetter](#streamersubscriptionkeysgetter)  
    - [TransactionHistoryGetter](#transactionhistorygetter)  
    - [IndividualTransactionHistoryGetter](#individualtransactionhistorygetter)  
    - [InstrumentInfoGetter](#instrumentinfogetter)  

*UPDATES*

- *On Jun 30 2018 API behavior was changed (by Ameritrade) to fix a bug w/ expired token messages. This will result in an ```InvalidRequest``` exception being thrown with error code 401 in older versions of the library (before commit b75586). If so upgrade to a newer version of the library.*
- - - 

### Overview

```
[C, C++]
#include "tdma_api_get.h"

[C++]
using namespace tdma;

[Python]
from tdma_api import get
```

The C++ Get Interface consists of 'Getter' objects that derive from ```APIGetter```, and related convenience functions. 

The C++ 'Getter' objects are simple 'proxies', all defined inline, that call through the ABI to the corresponding implementation objects. The implementation objects are built on top of ```conn::CurlConnection```: an object-oriented wrapper to libcurl found in curl_connect.h/cpp.

C uses a similar object-oriented approach where methods are passed a pointer to C proxy objects that are manually created and destroyed by the client.

The Python interface mirrors C++ almost exactly. 

### Certificates

Certificates are required to validate the hosts. If libcurl is built against the native ssl lib(e.g openssl on linux) or you use the pre-built dependencies for Windows(we built libcurl with -ENABLE_WINSSL) the default certificate store ***should*** take care of this for you. If this doesn't happen an ```APIExecutionException```  will be thrown with code CURLE_SSL_CACERT(60) and you'll have to use your own certificates via:
```
[C++]
void
SetCertificateBundlePath(const std::string& path)

    path :: the path to a certificate bundle file (.pem)

[C]
inline int
SetCertificateBundlePath(const char* path)

    returns -> 0 on success, error code on failure

[Python]
def auth.set_certificate_bundle_path(path):
    ...
```

There is a default 'cacert.pem' file in the base directory extracted from Firefox that you can use. 
(You can get updated versions from the [curl site](https://curl.haxx.se/docs/caextract.html).) The 
path of this file, *hard-coded during compilation*, can be found in the DEF_CERTIFICATE_BUNDLE_PATH string.

### Using Getter Objects 

Getter objects are fundamental to accessing the API. Before using you'll need to have obtained a [valid Credentials object](README.md#authentication). 

Each object sets up an underlying HTTPS/Get connection(via libcurl) using the credentials object and the relevant arguments for that particular request type. The connection sets the Keep-Alive header and will execute a request each time ```get / Get``` is called, until ```close / Close``` is called. C++ and Python getters will call ```close``` on destruction.

**Symbol strings are automatically converted to upper-case, e.g 'spy' -> 'SPY'.**

#### [C++]

1. construct a Getter object 
2. use the ```.get()``` method which returns a json object from the server OR throws an exception.

Each object has accessor methods for querying the fields passed to the constructor and updating 
those to be used in subsequent ```.get()``` call. 

If you only need to make the call once, each 'Getter' object has a similarly name 'Get' convenience function
that wraps the object and calls its ```.get()``` method once before destruction. 

To make this more concrete here is the Getter interface for individual price quotes:

```
class QuoteGetter
        : public APIGetter {
    ...
public:
    QuoteGetter( Credentials& creds, string symbol );

    string
    get_symbol() const;

    void
    set_symbol(string symbol);
    
    json /* INHERITED FROM APIGetter */
    get();
    
    void /* INHERITED FROM APIGetter */
    close();

    bool /* INHERITED FROM APIGetter */
    is_closed();
};

inline json
GetQuote(Credentials& creds, string symbol)
{ return QuoteGetter(creds, symbol).get(); }
```

#### [C]

1. define a a getter struct that will serve as the proxy instance, e.g:
```
QuoteGetter_C qg;
memset(&qg, 0, sizeof(qg));
```

2. initialize the getter object using the appropriately named ```Create``` function, e.g:
```
inline int
QuoteGetter_Create( struct Credentials *pcreds, 
                    const char* symbol,
                    QuoteGetter_C *pgetter )

    pcreds  :: a pointer to your credentials struct
    symbol  :: the symbol to get quotes for
    pgetter :: a pointer to a QuoteGetter_C struct to be populated
    returns -> 0 on success, error code on failure
```

3. use a pointer to the proxy instance to return (un-parsed) json data, e.g:
```
inline int
QuoteGetter_Get(QuoteGetter_C *getter, char **buf, size_t *n)

    getter :: pointer to the getter object created by 'Create'
    buf    :: address of a char* that will be populated by a char buffer
              *HEAP ALLOCATED VIA MALLOC*
    n      :: address of a size_t to be populated with the size of the
              char buffer (size of data + 1 for the null term)
```
Note that a heap allocated char buffer is returned. It's the caller's responsibility
to free the object with:

```
inline int
FreeBuffer( char* buf )

inline int
FreeBuffers( char** bufs, size_t n)
```

Basic use of the Get call looks like:
```
char* raw;
size_t n;
int err = QuoteGetter_Get(&qg, &raw, &n);
if( err ){
   //
}
// do something with 'raw' before you free it 
FreeBuffer( raw ); // notice we are using the char* version for a single buffer
```

To view or change the paramaters of the getter use the accessor methods, e.g:
```
inline int
QuoteGetter_GetSymbol(QuoteGetter_C *getter, char **buf, size_t *n)

inline int
QuoteGetter_SetSymbol(QuoteGetter_C *getter, const char *symbol)
```
Again, remember to free the char buffer returned by the 'GetSymbol' call.

When done you can close the object to end the connection and/or destroy it
to release the underlying resources e.g:
```
inline int
QuoteGetter_Close(QuoteGetter_C *getter)

inline int
QuoteGetter_Destroy(QuoteGetter_C *getter)
```
***Once ```Destroy``` is called any use of the getter is UNDEFINED BEHAVIOR.***

To check the state(before ```Destroy``` is called), e.g:
```
inline int
QuoteGetter_IsClosed(QuoteGetter_C *getter, int *b)
```

#### [Python]

1. construct a Getter object (tdma_api/get.py) 
2. use the ```.get()``` method which returns a parsed json object(dict, list, or None via json.loads()) from the server OR throws clib.LibraryNotLoaded or clib.CLibException.

Each object has accessor methods for querying the fields passed to the constructor and updating 
those to be used in subsequent ```.get()``` call. 

To make this more concrete here is the Getter interface for individual price quotes:

```
class QuoteGetter(_APIGetter):        
    def __init__(self, creds, symbol)    
    
    def get_symbol(self)
        
    def set_symbol(self, symbol)

    def get(self) /* INHERITED */
    
    def close(self) /* INHERITED */

    def is_closed(self) /* INHERITED */
```

### Throttling

The API docs indicate a limit of two requests per second so we implement a throttling/blocking 
mechanism **across ALL Getter objects** with a default wait of 500 milliseconds. If, for instance, we use the default
wait and make five ```.get()``` calls in immediate succession the group of calls will take
~2500 milliseconds to complete. This wait time can be accessed with:
```
    [C++]
    static chrono::milliseconds
    APIGetter::get_wait_msec();

    [C]
    inline int
    APIGetter_GetWaitMSec(unsigned long long *msec)

    [Python]
    def get.get_wait_msec()
```
```    
    [C++]
    static void
    APIGetter::set_wait_msec(chrono::milliseconds msec);

    [C]
    inline int
    APIGetter_SetWaitMSec(unsigned long long msec)

    [Python] 
    def get.set_wait_msec(msec)    
```
To check the number of milliseconds before the next ```.get()``` call can be executed(not block):
```
    [C++]
    static chrono::milliseconds
    APIGetter::wait_remaining();

    [C]
    inline int
    APIGetter_WaitRemaining(unsigned long long *msec);

    [Python]
    def get.wait_remaining()
```

This interface should not be used for streaming data, i.e. repeatedly making getter calls -  
use [StreamingSession](README_STREAMING.md) for that.

### Utilities

#### Option Symbols

To construct a standard option symbol that can be used with the quote getters:

```
[C++]
inline std::string
BuildOptionSymbol( const std::string& underlying,
                     unsigned int month,
                     unsigned int day,
                     unsigned int year,
                     bool is_call,
                     double strike )

[C]
inline int
BuildOptionSymbol( const char* underlying,
                     unsigned int month,
                     unsigned int day,
                     unsigned int year,
                     int is_call,
                     double strike,
                     char **buf,
                     size_t *n )

[Python]
def get.build_option_symbol(underlying, month, day, year, is_call, strike):
```

This is not guaranteed to work on all underlying types but generally:

```BuildOptionSymbol("SPY", 1, 17, 2020, true, 300.00) --> "SPY_011720C300"```

For Example:
```
string spy_c300 = BuildOptionSymbol("SPY", 1, 17, 2020, true, 300.00);
string spy_p250 = BuildOptionSymbol("SPY", 1, 17, 2020, false, 250.00);

QuotesGetter qg(creds, {spy_c300, spy_p250});
qg.get();
```
**If using C don't forget to call ```FreeBuffer``` on the populated 'buf' when done.**

### Example Usage 

#### [C++]
```
    #include <string>
    #include <chrono>
    #include <iostream>
    
    #include "tdma_api_get.h"
    
    using namespace tdma;
    using namespace std;
    
    ...    
    
        /* 
         * this could throw LocalCredentialsException (e.g empty access token in 
         * Credentials struct) or ValueException (e.g empty symbol string) 
         */
        QuoteGetter qg(credentials_manager.credentials, "SPY");
        
        json j;
        try{
            j = qg.get();
        }catch( InvalidRequest& e){
            // invalid symbol ?
        }catch( APIExecutionException& e ){
            // something more serious 
        }
        
        cout<< qg.get_symbol() << ' ' << j << endl;
        this_thread::sleep_for( chrono::seconds(2) );
        
        j = qg.get()
        cout<< qg.get_symbol() << ' ' << j << endl;
        this_thread::sleep_for( chrono::seconds(2) );
        
        qg.set_symbol("QQQ");
        j = qg.get();    
        cout<< qg.get_symbol() << ' ' << j << endl;
    
    ...
    
```

#### [C]
```
    #include <stdio.h>
    #include <stdlib.h>
    
    #include "tdma_api_get.h"
  
    ...    
    
         int err = 0;
         char *buf = NULL, *symbol = NULL;
         size_t n = 0;

         QuoteGetter_C qg;
         memset(&qg, 0, sizeof(QuoteGetter_C));

        if( (err = QuoteGetter_Create(creds, "SPY", &qg)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_Create");

        if( (err = QuoteGetter_Get(&qg, &buf, &n)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_Get");

        if( (err = QuoteGetter_GetSymbol(&qg, &symbol, &n)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_GetSymbol");

        if( symbol && buf )
            printf( "Get: %s - %s \n", symbol, buf);

        if(buf){
            free(buf);
            buf = NULL;
        }

        if(symbol){
            free(symbol);
            symbol = NULL;
        }

        if( (err = QuoteGetter_SetSymbol(&qg, "QQQ")) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_SetSymbol");

        if( (err = QuoteGetter_Get(&qg, &buf, &n)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_Get");

        if( (err = QuoteGetter_GetSymbol(&qg, &symbol, &n)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_GetSymbol");

        if( symbol && buf )
            printf( "Get: %s - %s \n", symbol, buf);

        if(buf){
            free(buf);
            buf = NULL;
        }

        if(symbol){
            free(symbol);
            symbol = NULL;
        }

        if( (err = QuoteGetter_Destroy(&qg)) ){
            memset(&qg, 0, sizeof(QuoteGetter_C));
            CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_Destroy");
        }

        memset(&qg, 0, sizeof(QuoteGetter_C));
    ...
    
```

#### [Python]
```
    from tdma_api import get, clib, auth
    
    ...    

    if not clib._lib:
        clib.init("path/to/lib/libTDAmeritrade.so")

    with auth.CredentialManager("path/to/creds/file", "password") as cm:
        try:
            g = get.QuoteGetter(cm.credentials, "SPY")
            assert q.credentials

            j = g.get()
            s = g.get_symbol()
            assert s == "SPY"
            print(s, j[s])

            g.set_symbol("QQQ")
            j = g.get()
            s = g.get_symbol()
            assert s == "QQQ"
            print(s, j[s])
        except clib.CLibException as e:
            print( str(e) )
            raise
    
    ...
    
```

### Getter Classes
- - -

*Only the C++ Getters are shown. The Python and C interfaces match these closely.  The C interface uses appropriately named functions to mimic the methods of the C++ classes and requires explicit use of the ```Create``` functions for construction and ```Destroy``` functions for destruction. See tdma_api_get.h for function prototypes.*

#### QuoteGetter

Retrieve a single quote for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/quotes/apis/get/marketdata/{symbol}/quotes)

**constructors**
```
QuoteGetter::QuoteGetter(Credentials& creds, string symbol);

    creds   ::  credentials struct received from RequestAccessToken / LoadCredentials 
                / CredentialsManager.credentials
    symbol  ::  (case sensitive) security symbol 

```

**methods**
```
virtual json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string 
QuoteGetter::get_symbol() const;    
```
```
void
QuoteGetter::set_symbol(string symbol);
```

**convenience function**
```
inline json
GetQuote(Credentials& creds, string symbol);
```
<br>

#### QuotesGetter

Retrieve a single quote for multiple securities. [TDAmeritrade docs.](https://developer.tdameritrade.com/quotes/apis/get/marketdata/quotes)

**constructors**
```
QuotesGetter::QuotesGetter(Credentials& creds, const set<string>& symbols);

    creds   ::  credentials struct received from RequestAccessToken / LoadCredentials 
                / CredentialsManager.credentials
    symbol  ::  (case sensitive) set of security symbols 

```

- empty symbol strings and/or symbol sets will throw ValueException
- if all symbols are removed, calling ```.get()``` will return an empty json object


**methods**
```
json 
APIGetter::get();
```
```
void
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
set<string> 
QuotesGetter::get_symbols() const;    
```
```
void
QuotesGetter::set_symbols(const set<string>& symbols);
```
```
void
QuotesGetter::add_symbol(const string& symbol);
```
```
void
QuotesGetter::remove_symbol(const string& symbol);
```
```
void
QuotesGetter::add_symbols(const set<string>& symbols);
```
```
void
QuotesGetter::remove_symbols(const set<string>& symbols);
```


**convenience function**
```
inline json
GetQuotes(Credentials& creds, const set<string>& symbols);
```
<br>

#### MarketHoursGetter

Retrieve market hours for a single market. [TDAmeritrade docs.](https://developer.tdameritrade.com/market-hours/apis/get/marketdata/{market}/hours)

**constructors**
```
MarketHoursGetter::MarketHoursGetter( Credentials& creds, 
                                      MarketType market_type,
                                      const string& date );

    creds        ::  credentials struct received from RequestAccessToken 
                     / LoadCredentials / CredentialsManager.credentials
    market_type  ::  market to get hours for
    date         ::  date string to get hours for (yyyy-MM-dd or yyyy-MM-dd'T'HH::mm::ssz)

```

**types**
```
enum class MarketType : int{
    equity,
    option,
    future,
    bond,
    forex
};
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
MarketHoursGetter::get_date() const;
```
```
MarketType
MarketHoursGetter::get_market_type() const;
```
```
void
MarketHoursGetter::set_date(const string& date);
```
```
void
MarketHoursGetter::set_market_type(MarketType market_type);
```

**convenience function**
```
inline json
GetMarketHours(Credentials& creds, MarketType market_type, const string& date);
```
<br>



#### MoversGetter

Top 10 up or down movers by value or %. [TDAmeritrade docs.](https://developer.tdameritrade.com/movers/apis/get/marketdata/{index}/movers)

**constructors**
```
MoversGetter::MoversGetter( Credentials& creds, 
                            MoversIndex index,
                            MoversDirectionType direction_type,
                            MoversChangeType change_type );

    creds          ::  credentials struct received from RequestAccessToken 
                       / LoadCredentials / CredentialsManager.credentials
    index          ::  index(compx, dji, spx) to get movers for
    direction_type ::  direction of moves
    change_type    ::  value or percent change

```

**types**
```
enum class MoversIndex : int{
    compx, /* $COMPX */
    dji, /* $DJI */
    spx /* SPX */
};
```
```
enum class MoversDirectionType : int{
    up,
    down,
    up_and_down
};
```
```
enum class MoversChangeType : int{
    value,
    percent
};
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
MoversIndex
MoversGetter::get_index() const;
```
```
MoversDirectionType
MoversGetter::get_direction_type() const;
```
```
MoversChangeType
MoversGetter::get_change_type() const
```
```
void
MoversGetter::set_index(MoversIndex index);
```
```
void
MoversGetter::set_direction_type(MoversDirectionType direction_type);
```
```
void
MoversGetter::set_change_type(MoversChangeType change_type);
```

**convenience function**
```
inline json
GetMovers( Credentials& creds,  
           MoversIndex index,
           MoversDirectionType direction_type,
           MoversChangeType change_type );
```
<br>


#### HistoricalPeriodGetter

Price history over a certain period for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/price-history/apis/get/marketdata/{symbol}/pricehistory)

**constructors**
```
HistoricalPeriodGetter::HistoricalPeriodGetter( Credentials& creds,
                                                const string& symbol,
                                                PeriodType period_type,
                                                unsigned int period,
                                                FrequencyType frequency_type,
                                                unsigned int frequency,
                                                bool extended_hours );

    creds          ::  credentials struct received from RequestAccessToken 
                       / LoadCredentials / CredentialsManager.credentials
    symbol         :: (case sensitive) security symbol
    period_type    :: type of period (day, month etc.)
    period         :: # of periods **
    frequency_type :: type of frequency (minute, daily etc.) **
    frequency      :: # of that frequency (e.g 10 or 100 minutes) **
    extended_hours :: include extended hours data

    ** these args are restricted to certain values based on other args and will
       throw ValueError if invalid, see the 'utilities' section below 

```

**types**
```
enum class PeriodType : int{
    day,
    month,
    year,
    ytd
};
```
```
enum class FrequencyType : int{
    minute,
    daily,
    weekly,
    monthly
};
```

**utilities**

*Note - The C interface exposes 2D array versions of these objects where each 'row' is terminated by at least one -1 value. See tdma_api_get.h for defintions.*

***```VALID_PERIODS_BY_PERIOD_TYPE```*** - A mapping that provides *the* set of valid # of 
periods for a particular period type. (e.g ```PeriodType::day``` allows for 1,2,3,4,5, and 10 periods)
```
const unordered_map<PeriodType, set<int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE = {
    {PeriodType::day, set<int>{1,2,3,4,5,10}},
    {PeriodType::month, set<int>{1,2,3,6}},
    {PeriodType::year, set<int>{1,2,3,5,10,15,20}},
    {PeriodType::ytd, set<int>{1}},
};
```

***```VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE```*** - A mapping that provides *the* set of valid 
frequency types for a particular period type. (e.g ```PeriodType::day``` only allows for ```FrequencyType::minute```)
```
const unordered_map<PeriodType, set<FrequencyType, EnumCompare<FrequencyType>>,
                    EnumHash<PeriodType>>
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE ={
    {PeriodType::day, {FrequencyType::minute} },
    {PeriodType::month, {FrequencyType::daily, FrequencyType::weekly} },
    {PeriodType::year, {FrequencyType::daily, FrequencyType::weekly, 
                        FrequencyType::monthly} },
    {PeriodType::ytd, { FrequencyType::weekly} },
};
```

***```VALID_FREQUENCIES_BY_FREQUENCY_TYPE```*** - A mapping that provides *the* set of valid frequency amounts for a particular frequency type. (e.g ```FrequencyType::daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
```
const unordered_map<FrequencyType, set<int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<int>{1,5,10,30}},
    {FrequencyType::daily, set<int>{1}},
    {FrequencyType::weekly, set<int>{1}},
    {FrequencyType::monthly, set<int>{1}},
};
```

- invalid frequency-to-frequency-type combinations will THROW immediately
- invalid period-to-period-type combinations will THROW immediately
- invalid frequency-type-to-period-type combinations will only THROW immediately on construction; if passed to the 'set' methods they WILL NOT THROW UNTIL ```.get()``` is called


**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
HistoricalGetterBase::get_symbol() const;
```
```
PeriodType
HistoricalPeriodGetter::get_period_type() const;
```
```
unsigned int
HistoricalPeriodGetter::get_period() const;
```
```
unsigned int 
HistoricalGetterBase::get_frequency() const;
```
```
FrequencyType
HistoricalGetterBase::get_frequency_type() const;
```
```
bool 
HistoricalGetterBase::is_extended_hours() const;  
```
```
void  
HistoricalGetterBase::set_symbol(const string& symbol);
```
```
void
HistoricalPeriodGetter::set_period( PeriodType period_type, 
                                    usigned int period );
```
```
void
HistoricalGetterBase::set_frequency( FrequencyType frequency_type,
                                       unsigned int frequency );
```
```
void 
HistoricalGetterBase::set_extended_hours(bool extended_hours);

```

**convenience function**
```
inline json
GetHistoricalPeriod( Credentials& creds,
                     const string& symbol,
                     PeriodType period_type,
                     unsigned int period,
                     FrequencyType frequency_type,
                     unsigned int frequency,
                     bool extended_hours );
```
<br>

#### HistoricalRangeGetter

Price history between a date range for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/price-history/apis/get/marketdata/{symbol}/pricehistory)

**constructors**
```
HistoricalRangeGetter::HistoricalRangeGetter( Credentials& creds,
                                              const string& symbol,                         
                                              FrequencyType frequency_type,
                                              unsigned int frequency,
                                              unsigned long long start_msec_since_epoch,
                                              unsigned long long end_msec_since_epoch,
                                              bool extended_hours );

    creds                  ::  credentials struct received from RequestAccessToken 
                               / LoadCredentials / CredentialsManager.credentials
    symbol                 :: (case sensitive) security symbol
    frequency_type         :: type of frequency (minute, daily etc.) 
    frequency              :: # of that frequency (e.g 10 or 100 minutes) **
    start_msec_since_epoch :: beginning of historical range in milliseconds since epoch***
    end_msec_since_epoch   :: end of historical range in milliseconds since epoch***
    extended_hours         :: include extended hours data

    ** these args are restricted to certain values based on other args and will
       throw ValueError if invalid, see the 'utilities' section below 

    *** the epoch is 00:00 of Jan 1 1970
```

**types**
```
enum class FrequencyType : int{
    minute,
    daily,
    weekly,
    monthly
};
```

**utilities**

*Note - The C interface exposes a 2D array version of this object where each 'row' is terminated by at least one -1 value. See tdma_api_get.h for defintions.*

***```VALID_FREQUENCIES_BY_FREQUENCY_TYPE```*** - A mapping that provides *the* set of valid 
frequency amounts for a particular frequency type. (e.g ```FrequencyType::daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
```
const unordered_map<FrequencyType, set<int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<int>{1,5,10,30}},
    {FrequencyType::daily, set<int>{1}},
    {FrequencyType::weekly, set<int>{1}},
    {FrequencyType::monthly, set<int>{1}},
};

```
- invalid frequency-to-frequency-type combinations will THROW immediately

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
HistoricalGetterBase::get_symbol() const;
```
```
unsigned int 
HistoricalGetterBase::get_frequency() const;
```
```
FrequencyType
HistoricalGetterBase::get_frequency_type() const;
```
```
PeriodType
HistoricalRangeGetter::get_end_msec_since_epoch() const;
```
```
unsigned int
HistoricalRangeGetter::get_start_msec_since_epoch() const;
```
```
bool 
HistoricalGetterBase::is_extended_hours() const;  
```
```
void  
HistoricalGetterBase::set_symbol(const string& symbol);
```
```
void
HistoricalGetterBase::set_frequency( FrequencyType frequency_type,
                                       unsigned int frequency );
```
```
void
HistoricalRangeGetter::set_end_msec_since_epoch(
    usigned long long end_msec_since_epoch
);
```
```
void
HistoricalRangeGetter::set_stat_msec_since_epoch(
    unsigned long long start_msec_since_epoch
);
```
```
void 
HistoricalGetterBase::set_extended_hours(bool extended_hours);
```

**convenience function**
```
inline json
GetHistoricalRange( Credentials& creds,
                    const string& symbol,                         
                    FrequencyType frequency_type,
                    unsigned int frequency,
                    unsigned long long start_msec_since_epoch,
                    unsigned long long end_msec_since_epoch,
                    bool extended_hours );
```
<br>


#### OptionChainGetter

Traditional Option chain for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

**constructors**
```
OptionChainGetter::OptionChainGetter( 
        Credentials& creds,
        const string& symbol,                         
        const OptionStrikes& strikes,
        OptionContractType contract_type = OptionContractType::all,
        bool include_quotes = false,
        const string& from_date = "",
        const string& to_date = "",
        OptionExpMonth exp_month = OptionExpMonth::all,
        OptionType option_type = OptionType::all 
    );

    creds          ::  credentials struct received from RequestAccessToken 
                       / LoadCredentials / CredentialsManager.credentials
    symbol         :: (case sensitive) security symbol
    strikes        :: type of strikes (see types section below)
    contract_type  :: type of contracts (put, call)
    include_quotes :: include underlying quotes
    from_date      :: beginning date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    to_date        :: end date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    exp_month      :: expiration month
    option_type    :: type of option (standard, non-standard)

```

**types**

```
enum class OptionContractType : int{
    call,
    put,
    all
};
```
```
enum class OptionRangeType : int{
    null, /* this shouldn't be used directly  */
    itm, /* in-the-money */
    ntm, /* near-the-money */
    otm, /* out-of-the-monety */
    sak, /* strikes-above-market */
    sbk, /* strikes-below-market */
    snk, /* strikes-near-market */
    all
};
```
```
enum class OptionExpMonth : int{
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dec,
    all
};
```
```
enum class OptionType : int{
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*The following types will be constructed/used by the OptionStrikes class. Use the static methods listed below.*
```
enum class OptionStrikesType : int {
    n_atm,
    single,
    range,
    none, 
};

typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;

class OptionStrikes {
    using Type = OptionStrikesType;
    Type _type;
    OptionStrikesValue _value;
    ...
};
```

**utilities**

**```OptionStrikes```** - Object used to represent one of three distinct strike 
types and related values. Create with its static method:

***```N_ATM```*** - return 'n' surrounding, at-the-money strikes.
```
    static OptionStrikes
    OptionStrikes::N_ATM(unsigned int n);
```

***```Single```*** - return a single strike at a certain price.
```
    static OptionStrikes
    OptionStrikes::Single(double price);
```

***```Range```*** - return a particular range (itm, sak etc.) of strikes.
```
    static OptionStrikes
    OptionStrikes::Range(OptionRangeType range);
```

The following instance methods can be used to get information about the object but the 
user must use the correct call or union field(first use get_type() then call the correct method or access the appropriate union field).
```
OptionStrikesType
OptionStrikes::get_type() const;
```
```
OptionStrikesValue
OptionStrikes::get_value() const;
```
```
unsigned int
OptionStrikes::get_n_atm() const;
```
```
double
OptionStrikes::get_single() const;
```
```
OptionRangeType
OptionStrikes::get_range() const;
```

**methods**

```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
OptionChainGetter::get_symbol() const;
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
string
OptionChainGetter::get_from_date() const;
```
```
string
OptionChainGetter::get_to_date() const;
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
void
OptionChainGetter::set_strikes(OptionStrikes strikes);
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```

**convenience function**
```
inline json
GetOptionChain( Credentials& creds,
                const string& symbol,
                OptionStrikes strikes,
                OptionContractType contract_type = OptionContractType::all,
                bool include_quotes = false,
                const string& from_date = "",
                const string& to_date = "",
                OptionExpMonth exp_month = OptionExpMonth::all,
                OptionType option_type = OptionType::all );
```
<br>

#### OptionChainStrategyGetter

Option chains of strategies (spreads) for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

**constructors**
```
OptionChainStrategyGetter::OptionChainStrategyGetter( 
        Credentials& creds,
        const string& symbol,  
        OptionStrategy strategy,                       
        const OptionStrikes& strikes,
        OptionContractType contract_type = OptionContractType::all,
        bool include_quotes = false,
        const string& from_date = "",
        const string& to_date = "",
        OptionExpMonth exp_month = OptionExpMonth::all,
        OptionType option_type = OptionType::all 
    );
                    

    creds          ::  credentials struct received from RequestAccessToken 
                       / LoadCredentials / CredentialsManager.credentials
    symbol         :: (case sensitive) security symbol
    strikes        :: type of strikes (see types/utilities sections below)
    strategy       :: type of strategy (see types/utilities sections below)
    contract_type  :: type of contracts (put, call)
    include_quotes :: include underlying quotes
    from_date      :: beginning date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    to_date        :: end date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    exp_month      :: expiration month
    option_type    :: type of option (standard, non-standard)

```

**types**

```
enum class OptionContractType : int{
    call,
    put,
    all
};
```
```
enum class OptionStrategyType : int{ 
    covered,
    vertical,
    calendar,
    strangle,
    straddle,
    butterfly,
    condor,
    diagonal,
    collar,
    roll
};
```
```
enum class OptionRangeType : int{
    null, /* this shouldn't be used directly  */
    itm, /* in-the-money */
    ntm, /* near-the-money */
    otm, /* out-of-the-monety */
    sak, /* strikes-above-market */
    sbk, /* strikes-below-market */
    snk, /* strikes-near-market */
    all
};
```
```
enum class OptionExpMonth : int{
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dec,
    all
};
```
```
enum class OptionType : int{
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*The following types will be constructed/used by the OptionStrikes class. Use the static methods listed below.*
```
enum class OptionStrikesType : int {
    n_atm,
    single,
    range,
    none, 
};

typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;

class OptionStrikes {
    using Type = OptionStrikesType;
    Type _type;
    OptionStrikesValue _value;
    ...
};
```

*See below for an explanation of ```OptionStrategy```*
```
class OptionStrategy {
    OptionStrategyType _strategy;
    double _spread_interval;
    ...
};

```

**utilities**

**```OptionStrikes```** - Object used to represent one of three distinct strike 
types and related values. Create with its static method:

***```N_ATM```*** - return 'n' surrounding, at-the-money strikes.
```
    static OptionStrikes
    OptionStrikes::N_ATM(unsigned int n);
```

***```Single```*** - return a single strike at a certain price.
```
    static OptionStrikes
    OptionStrikes::Single(double price);
```

***```Range```*** - return a particular range (itm, sak etc.) of strikes.
```
    static OptionStrikes
    OptionStrikes::Range(OptionRangeType range);
```

The following instance methods can be used to get information about the object but the 
user must use the correct call or union field(first use get_type() then call the correct method or access the appropriate union field).
```
OptionStrikesType
OptionStrikes::get_type() const;
```
```
OptionStrikesValue
OptionStrikes::get_value() const;
```
```
unsigned int
OptionStrikes::get_n_atm() const;
```
```
double
OptionStrikes::get_single() const;
```
```
OptionRangeType
OptionStrikes::get_range() const;
```

**```OptionStrategy```** - Object used to represent one of a number of distinct
option (spread) strategies and the spread size (in dollars), if applicable. 
Create with its static method:

``` 
static OptionStrategy
OptionStrategy::Covered(); // covered call/put
```
```
static OptionStrategy
OptionStrategy::Calendar();
```
```
static OptionStrategy
OptionStrategy::Vertical(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Strangle(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Straddle(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Butterfly(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Condor(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Diagonal(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Collar(double spread_interval=1.0);
```
```
static OptionStrategy
OptionStrategy::Roll(double spread_interval=1.0);
```

The following methods can be used to get information about the instance:

```
OptionStrategyType
OptionStrategy::get_strategy() const
```
```
double
OptionStrategy::get_spread_interval() const
```

**methods**

```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
OptionChainGetter::get_symbol() const;
```
```
OptionStrategy
OptionChainStrategyGetter::get_strategy() const;
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
string
OptionChainGetter::get_from_date() const;
```
```
string
OptionChainGetter::get_to_date() const;
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
void
OptionChainStrategyGetter::set_strategy(OptionStrategy strategy);
```
```
void
OptionChainGetter::set_strikes(OptionStrikes strikes);
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```

**convenience function**
```
inline json
GetOptionChainStrategy( Credentials& creds,
                        const string& symbol,
                        OptionStrategy strategy,
                        OptionStrikes strikes,
                        OptionContractType contract_type = OptionContractType::all,
                        bool include_quotes = false,
                        const string& from_date = "",
                        const string& to_date = "",
                        OptionExpMonth exp_month = OptionExpMonth::all,
                        OptionType option_type = OptionType::all );
```
<br>

#### OptionChainAnalyticalGetter

Option chains with analytics, for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

**constructors**
```
OptionChainAnalyticalGetter::OptionChainAnalyticalGetter( 
        Credentials& creds,
        double volatility,
        double underlying_price,
        double interest_rate,
        unsigned int days_to_exp,                       
        const OptionStrikes& strikes,
        OptionContractType contract_type = OptionContractType::all,
        bool include_quotes = false,
        const string& from_date = "",
        const string& to_date = "",
        OptionExpMonth exp_month = OptionExpMonth::all,
        OptionType option_type = OptionType::all 
    );
                    

    creds            ::  credentials struct received from RequestAccessToken 
                         / LoadCredentials / CredentialsManager.credentials
    symbol           :: (case sensitive) security symbol
    volatility       :: volatility to use in calculations
    underlying_price :: price of underlying asset to use in calculations
    interest_rate    :: interest rate to use in calculations
    days_to_exp      :: # of days to expiration to use in calculations
    strikes          :: type of strikes (see types/utilities sections below)
    strategy         :: type of strategy (see types/utilities sections below)
    contract_type    :: type of contracts (put, call)
    include_quotes   :: include underlying quotes
    from_date        :: beginning date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    to_date          :: end date of range of chains (yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz)
    exp_month        :: expiration month
    option_type      :: type of option (standard, non-standard)

```

**types**

```
enum class OptionContractType : int{
    call,
    put,
    all
};
```
```
enum class OptionRangeType : int{
    null, /* this shouldn't be used directly  */
    itm, /* in-the-money */
    ntm, /* near-the-money */
    otm, /* out-of-the-monety */
    sak, /* strikes-above-market */
    sbk, /* strikes-below-market */
    snk, /* strikes-near-market */
    all
};
```
```
enum class OptionExpMonth : int{
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dec,
    all
};
```
```
enum class OptionType : int{
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*The following types will be constructed/used by the OptionStrikes class. Use the static methods listed below.*
```
enum class OptionStrikesType : int {
    n_atm,
    single,
    range,
    none, 
};

typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;

class OptionStrikes {
    using Type = OptionStrikesType;
    Type _type;
    OptionStrikesValue _value;
    ...
};
```

**utilities**

**```OptionStrikes```** - Object used to represent one of three distinct strike 
types and related values. Create with its static method:

***```N_ATM```*** - return 'n' surrounding, at-the-money strikes.
```
    static OptionStrikes
    OptionStrikes::N_ATM(unsigned int n);
```

***```Single```*** - return a single strike at a certain price.
```
    static OptionStrikes
    OptionStrikes::Single(double price);
```

***```Range```*** - return a particular range (itm, sak etc.) of strikes.
```
    static OptionStrikes
    OptionStrikes::Range(OptionRangeType range);
```

The following instance methods can be used to get information about the object but the 
user must use the correct call or union field(first use get_type() then call the correct method or access the appropriate union field).
```
OptionStrikesType
OptionStrikes::get_type() const;
```
```
OptionStrikesValue
OptionStrikes::get_value() const;
```
```
unsigned int
OptionStrikes::get_n_atm() const;
```
```
double
OptionStrikes::get_single() const;
```
```
OptionRangeType
OptionStrikes::get_range() const;
```

**methods**

```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
OptionChainGetter::get_symbol() const;
```
```
double 
OptionChainAnalyticalGetter::get_volatility() const;
```
```
double 
OptionChainAnalyticalGetter::get_underlying_price() const;
```
```
double
OptionChainAnalyticalGetter::get_interest_rate() const;
```
```
unsigned int
OptionChainAnalyticalGetter::get_days_to_exp() const;
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
string
OptionChainGetter::get_from_date() const;
```
```
string
OptionChainGetter::get_to_date() const;
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
void
OptionChainAnalyticalGetter::set_volatility(double volatility);
```
```
void
OptionChainAnalyticalGetter::set_underlying_price(double underlying_price);
```
```
void
OptionChainAnalyticalGetter::set_interest_rate(double interest_rate);
```
```
void
OptionChainAnalyticalGetter::set_days_to_exp(unsigned int days_to_exp);
```
```
void
OptionChainGetter::set_strikes(OptionStrikes strikes);
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```

**convenience function**
```
inline json
GetOptionChainAnalytical( Credentials& creds,
                          double volatility,
                          double underlying_price,
                          double interest_rate,
                          unsigned int days_to_exp,                       
                          const OptionStrikes& strikes,
                          OptionContractType contract_type = OptionContractType::all,
                          bool include_quotes = false,
                          const string& from_date = "",
                          const string& to_date = "",
                          OptionExpMonth exp_month = OptionExpMonth::all,
                          OptionType option_type = OptionType::all  );
```
<br>

#### AccountInfoGetter

Account balances, positions, and orders. [TDAmeritrade docs.](https://developer.tdameritrade.com/account-access/apis/get/accounts/{accountId}-0)

**constructors**
```
AccountInfoGetter::AccountInfoGetter( Credentials& creds, 
                                      const string& account_id,
                                      bool positions,
                                      bool orders );

    creds      ::  credentials struct received from RequestAccessToken 
                   / LoadCredentials / CredentialsManager.credentials
    account_id ::  id string of account to get info for
    positions  ::  include positions
    orders     ::  include orders
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
AccountGetterBase::get_account_id() const;
```
```
bool
AccountInfoGetter::returns_positions() const;
```
```
bool
AccountInfoGetter::returns_orders() const
```
```
void
AccountGetterBase::set_account_id(const string& account_id);
```
```
void
AccountInfoGetter::return_positions(bool positions);
```
```
void
AccountInfoGetter::return_orders(bool orders);
```

**convenience function**
```
inline json
GetAccountInfo( Credentials& creds, 
                const string& account_id,
                bool positions,
                bool orders );
```
<br>


#### PreferencesGetter

Account preferences. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/accounts/{accountId}/preferences-0)

**constructors**
```
AccountPreferencesGetter::AccountPreferencesGetter( Credentials& creds, 
                                                    const string& account_id );

    creds      ::  credentials struct received from RequestAccessToken 
                   / LoadCredentials / CredentialsManager.credentials
    account_id ::  id string of account to get preferences for
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
AccountGetterBase::get_account_id() const;
```
```
void
AccountGetterBase::set_account_id(const string& account_id);
```

**convenience function**
```
inline json
GetAccountPreferences( Credentials& creds, const string& account_id );
```
<br>

#### UserPrincipalsGetter

User Principal details. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/userprincipals-0)

**constructors**
```
UserPrincipalsGetter::UserPrincipalsGetter( Credentials& creds, 
                                            bool streamer_subscription_keys,
                                            bool streamer_connection_info,
                                            bool preferences,
                                            bool surrogate_ids );

    creds                      ::  credentials struct received from RequestAccessToken 
                                   / LoadCredentials / CredentialsManager.credentials
    streamer_subscription_keys :: include streamer keys
    streamer_connection_info   :: include streamer connection info
    preferences                :: include streamer preferences
    surrogate_ids              :: include surrogate_ids
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
bool
UserPrincipalsGetter::returns_streamer_subscription_keys() const;
```
```
bool 
UserPrincipalsGetter::returns_streamer_connection_info() const;
```
```
bool
UserPrincipalsGetter::returns_preferences() const;
```
```
bool
UserPrincipalsGetter::returns_surrogate_ids() const;
```
```
void
UserPrincipalsGetter::return_streamer_subscription_keys(
    bool streamer_subscription_keys
);
```
```
void
UserPrincipalsGetter::return_streamer_connection_info(
    bool streamer_connection_info
);
```
```
void
UserPrincipalsGetter::return_preferences(bool preferences);
```
```
void
UserPrincipalsGetter::return_surrogate_ids(bool surrogate_ids);
```

**convenience function**
```
inline json
GetUserPrincipals( Credentials& creds, 
                   bool streamer_subscription_keys,
                   bool streamer_connection_info,
                   bool preferences,
                   bool surrogate_ids );
```
<br>

#### StreamerSubscriptionKeysGetter

Subscription keys. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/userprincipals/streamersubscriptionkeys-0)

**constructors**
```
StreamerSubscriptionKeysGetter::StreamerSubscriptionKeysGetter( 
        Credentials& creds, 
        const string& account_id 
    );

    creds      ::  credentials struct received from RequestAccessToken 
                   / LoadCredentials / CredentialsManager.credentials
    account_id ::  id string of account to get subscription keys for
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
AccountGetterBase::get_account_id() const;
```
```
void
AccountGetterBase::set_account_id(const string& account_id);
```

**convenience function**
```
inline json
GetStreamerSubscriptionKeys( Credentials& creds, const string& account_id );
```
<br>


#### TransactionHistoryGetter

Transactions within a date range. [TDAmeritrade docs.](https://developer.tdameritrade.com/transaction-history/apis/get/accounts/{accountId}/transactions-0)

**constructors**
```
TransactionHistoryGetter::TransactionHistoryGetter( 
        Credentials& creds, 
        const string& account_id 
        TransactionType transaction_type = TransactionType::all,
        const string& symbol="",
        const string& start_date="",
        const string& end_date="" 
    );

    creds            ::  credentials struct received from RequestAccessToken 
                         / LoadCredentials / CredentialsManager.credentials
    account_id       ::  id string of account to get transactions for
    transaction_type :: type of transactions (trade, dividend etc.)
    symbol           :: only transactions that match this symbol
    start_date       :: only transactions after this date (yyyy-MM-dd)
    end_date         :: only transactions before this date (yyyy-MM-dd)
```

**types**
```
enum class TransactionType : int{
    all,
    trade,
    buy_only,
    sell_only,
    cash_in_or_cash_out,
    checking,
    dividend,
    interest,
    other,
    advisor_fees
};
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
AccountGetterBase::get_account_id() const;
```
```
TransactionType
TransactionHistoryGetter::get_transaction_type() const;
```
```
string
TransactionHistoryGetter::get_symbol() const;
```
```
string
TransactionHistoryGetter::get_start_date() const;
```
```
string
TransactionHistoryGetter::get_end_date() const;
```
```
void
AccountGetterBase::set_account_id(const string& account_id);
```
```
void
TransactionHistoryGetter::set_transaction_type(TransactionType transaction_type);
```
```
void
TransactionHistoryGetter::set_symbol(const string& symbol);
```
```
void
TransactionHistoryGetter::set_start_date(const string& start_date);
```
```
void
TransactionHistoryGetter::set_end_date(const string& end_date);
```

**convenience function**
```
inline json
GetTransactionHistory( Credentials& creds, 
                       const string& account_id 
                       TransactionType transaction_type = TransactionType::all,
                       const string& symbol="",
                       const string& start_date="",
                       const string& end_date="" );    
```
<br>

#### IndividualTransactionHistoryGetter

Transactions by id. [TDAmeritrade docs.](https://developer.tdameritrade.com/transaction-history/apis/get/accounts/{accountId}/transactions/{transactionId}-0)

**constructors**
```
IndividualTransactionHistoryGetter::IndividualTransactionHistoryGetter( 
        Credentials& creds, 
        const string& account_id 
        const string& transaction_id 
    );

    creds          ::  credentials struct received from RequestAccessToken 
                       / LoadCredentials / CredentialsManager.credentials
    account_id     ::  id string of account to get transactions for
    transaction_id ::  id string of transaction 
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
AccountGetterBase::get_account_id() const;
```
```
string
IndividualTransactionHistoryGetter::get_transaction_id() const;
```
```
void
AccountGetterBase::set_account_id(const string& account_id);
```
```
void
IndividualTransactionHistoryGetter::set_transaction_type(const string& transaction_id);
```


**convenience function**
```
inline json
GetIndividualTransactionHistory( Credentials& creds, 
                                 const string& account_id 
                                 const string& transaction_id );
```
<br>

#### InstrumentInfoGetter

Instrument information. [TDAmeritrade docs.](https://developer.tdameritrade.com/instruments/apis)

**constructors**
```
InstrumentInfoGetter::InstrumentInfoGetter( Credentials& creds, 
                                            InstrumentSearchType search_type,
                                            const string& query_string );

    creds        ::  credentials struct received from RequestAccessToken 
                     / LoadCredentials / CredentialsManager.credentials
    search_type  :: type of search (by symbol_regex, cusip etc.)
    query_string :: search string
```
**types**
```
enum class InstrumentSearchType : int{
    symbol_exact,
    symbol_search,
    symbol_regex,
    description_search,
    description_regex,
    cusip
};
```

**methods**
```
json 
APIGetter::get();
```
```
void 
APIGetter::close();
```
```
bool 
APIGetter::is_closed() const;
```
```
string
InstrumentInfoGetter::get_query_string() const;
```
```
InstrumentSearchType
InstrumentInfoGetter::get_search_type() const;
```
```
void
InstrumentInfoGetter::set_query( InstrumentSearchType search_type, 
                                 const string& query_string );
```

**convenience function**
```
inline json
GetInstrumentInfo( Credentials& creds, 
                   InstrumentSearchType search_type,
                   const string& query_string );
```

