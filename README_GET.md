### HTTPS Get Interface
- - -

- [Overview](#overview)
- [Certificates](#certificates)
- [Using Getter Objects](#using-getter-objects)
    - [C++](#c)
    - [C](#c-1)
    - [Python](#python)
    - [Java](#java)
- [Throttling](#throttling)
- [Example Usage](#example-usage)
    - [C++](#c-2)
    - [C](#c-3)
    - [Python](#python-1)
    - [Java](#java-1)
- [Getter Classes](#getter-classes)
    - [QuoteGetter](#quotegetter)  
    - [QuotesGetter](#quotesgetter)  
    - [MarketHoursGetter](#markethoursgetter)  
    - [MoversGetter](#moversgetter)  
    - [HistoricalPeriodGetter](#historicalperiodgetter)  ***\*UPDATED\****
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
    - [OrderGetter](#ordergetter)  ***\*NEW\****
    - [OrdersGetter](#ordersgetter)  ***\*NEW\****

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

[Java]
// for illustrative purposes (import individual classes as needed)
import io.github.jeog.tdameritradeapi.get.*; 
```

The C++ Get Interface consists of 'Getter' objects - all derived from ```APIGetter``` and defined inline - that call through the ABI to the corresponding implementation objects. The implementation objects are built on top of ```conn::CurlConnection```, an object-oriented wrapper to libcurl found in curl_connect.h/cpp.

C uses a similar object-oriented approach where functions are passed a pointer to proxy objects that are manually created and destroyed by the client.

The Python and Java interfaces mirror C++ almost exactly. 

### Certificates

Certificates are required to validate the hosts. If libcurl is built against the native ssl lib(e.g openssl on linux) or you use the pre-built dependencies for Windows(we built libcurl with -ENABLE_WINSSL) the default certificate store ***should*** take care of this for you. If this doesn't happen a ```ConnectException```  will be thrown in C++ (CLibException in Python and Java; the related error code in C) with code CURLE_SSL_CACERT(60). If so you'll have to use your own certificates via:
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

[Java]
public class Auth{
    ...
    public static void
    setCertificateBundlePath(String path) throws CLibException;
    ...
}
```

There is a default 'cacert.pem' file in the base directory extracted from Firefox that you can use. 
(You can get updated versions from the [curl site](https://curl.haxx.se/docs/caextract.html).) The 
path of this file, *hard-coded during compilation*, can be found in the DEF_CERTIFICATE_BUNDLE_PATH string.

### Using Getter Objects 

Getter objects are fundamental to accessing the API and require a valid [Credentials object](README.md#authentication). 

Each object sets up an underlying HTTPS/Get connection on construction. The connection sets the Keep-Alive header and will execute a request each time ```get()/Get()``` is called, until ```close()/Close()``` is called. C++, Java, and Python getters will call ```close()``` on destruction. Symbol strings are automatically converted to upper-case.

#### [C++]

1. Construct a Getter object 
2. Use the ```.get()``` method which returns a json object from the server OR throws an exception.
3. Use the objects's accessor methods for querying the fields passed to the constructor and/or updating those to be used in subsequent ```.get()``` calls. 

    To make this more concrete here is the Getter interface for individual price quotes:

    ```
    class QuoteGetter
            : public APIGetter {
        ...
    public:
        QuoteGetter( Credentials& creds, const string& symbol );

        string
        get_symbol() const;

        void
        set_symbol(const string& symbol);
        
        json /* INHERITED FROM APIGetter */
        get();
        
        void /* INHERITED FROM APIGetter */
        close();

        bool /* INHERITED FROM APIGetter */
        is_closed();
    };
    ```

#### [C]

1. Define a a getter struct that will serve as the proxy instance, e.g:
    ```
    QuoteGetter_C qg = {0,0};
    ```

2. Initialize the getter object using the appropriately named ```Create``` function, e.g:
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

3. Use a pointer to the proxy instance to return (un-parsed) json data, e.g:
    ```
    inline int
    QuoteGetter_Get(QuoteGetter_C *pgetter, char **buf, size_t *n)

        pgetter :: pointer to the getter object created by 'Create'
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

4. To view or change the paramaters of the getter use the accessor methods, e.g:
    ```
    inline int
    QuoteGetter_GetSymbol(QuoteGetter_C *pgetter, char **buf, size_t *n)

    inline int
    QuoteGetter_SetSymbol(QuoteGetter_C *pgetter, const char *symbol)
    ```
    Again, remember to free the char buffer returned by the 'GetSymbol' call.

5. When done you can close the object to end the connection and/or destroy it
    to release the underlying resources e.g:
    ```
    inline int
    QuoteGetter_Close(QuoteGetter_C *pgetter)

    inline int
    QuoteGetter_Destroy(QuoteGetter_C *pgetter)
    ```
    Once ```Destroy``` is called any use of the getter is UNDEFINED BEHAVIOR.

    To check the state(before ```Destroy``` is called), e.g:
    ```
    inline int
    QuoteGetter_IsClosed(QuoteGetter_C *pgetter, int *b)
    ```

##### Stateless Convenience Function

Each object has a similarly named stand-alone function for making a single get call(e.g ```GetQuote(...)```, ```GetAccountInfo(...)```). This function calls Create, Get, and Destroy, populating a char buffer, as above.

#### [Python]

1. Construct a Getter object (tdma_api/get.py) 
2. Use the ```.get()``` method which returns a parsed json object(dict, list, or None via json.loads()) from the server OR throws clib.CLibException.
3. Use the object's accessor methods for querying the fields passed to the constructor and updating 
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

#### [Java]

1. Construct a Getter object (package ```io.github.jeog.tdameritradeapi.get```)
    - you can use Getter objects in a 'try-with-resources' block to close the connection immediately when done
2. Use the ```.get()``` method which returns an `Object` that is either of type ```JSONObject``` or ```JSONArray``` OR throws ```TDameritradeAPI.CLibException```
3. Use the object's accessor methods for querying the fields passed to the constructor and updating 
those to be used in subsequent ```.get()``` calls. 

    To make this more concrete here is the Getter interface for individual price quotes:

    ```
    public class QuoteGetter extends APIGetter{

        public QuoteGetter( Credentials creds, String symbol ) throws  CLibException;

        public String
        getSymbol() throws  CLibException;
        
        public void
        setSymbol(String symbol) throws  CLibException;

        public Object
        get() throws  CLibException; /* INHERITED */
        
        @Override
        public void
        close() throws  CLibException /* INHERITED */
        
        public boolean
        isClosed() throws  CLibException /* INHERITED */
    }
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

    [Java]
    public class APIGetter {
        ...
        static public long getWaitMSec() throws  CLibException;
        ...
    }
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

    [Java]
    public class APIGetter {
        ...
        static public void setWaitMSec( long msec ) throws  CLibException;
        ...
    }
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

    [Java]
    public class APIGetter {
        ...
        static public long waitRemaining() throws  CLibException;
        ...
    }
```

This interface should not be used for streaming data, i.e. repeatedly making getter calls -  
use [StreamingSession](README_STREAMING.md) for that.

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
        }catch( ConnectException& e ){
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

         QuoteGetter_C qg = {0,0};

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

#### [Java]
```
    import io.github.jeog.tdameritradeapi.TDAmeritradeAPI;
    import io.github.jeog.tdameritradeapi.TDAmeritradeAPI.CLibException;
    import io.github.jeog.tdameritradeapi.Auth.CredentialsManager; 
    import io.github.jeog.tdameritradeapi.get.QuoteGetter;

    import org.json.JSONObject;

    ...

        TDAmeritradeAPI.init(libPath);
        
        try( CredentialsManager cm = new CredentialsManager(credsPath, credsPassword) ){
        
            try( QuoteGetter qGetter = new QuoteGetter(cm.getCredentials(), "SPY") ){

                JSONObject j = (JSONObject)qGetter.get(); // we know its JSONObject in this case
                System.out.println( qGetter.getSymbol() + ": " + j.toString(4) )

                qGetter.setSymbol("QQQ");
                j = (JSONObject)qGetter.get(); 
                System.out.println( qGetter.getSymbol() + ": " + j.toString(4) )  

            }catch( TDAmeritradeAPI.CLibException exc ){
                // ERROR
            }
        }

```

### Getter Classes
- - -

C++ Getter classes and the equivalent C interfaces are outlined below.  

The C interface uses appropriately named functions to mimic the methods of the C++ classes. It requires explicit use of the ```Create``` functions for construction and ```Destroy``` functions for destruction. 

The Python and Java interfaces match C++ almost exactly. 

#### QuoteGetter

Retrieve a single quote for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/quotes/apis/get/marketdata/{symbol}/quotes)

##### [C++]

**constructors**
```
QuoteGetter::QuoteGetter(Credentials& creds, const string& symbol);

    creds   ::  credentials struct received from RequestAccessToken / LoadCredentials 
                / CredentialsManager.credentials
    symbol  ::  (case sensitive) security symbol 

```
**types**
```
class QuoteGetter : public APIGetter;
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
QuoteGetter::set_symbol(const string& symbol);
```

##### [C]

**types**
```
struct QuoteGetter_C;
```

**functions**
```
static inline int
QuoteGetter_Create(struct Credentials *pcreds, 
                   const char* symbol, 
                   QuoteGetter_C *pgetter);
```
```
static inline int
QuoteGetter_Destroy(QuoteGetter_C *pgetter);
```
```
static inline int
QuoteGetter_Get(QuoteGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
QuoteGetter_Close(QuoteGetter_C *pgetter);
```
```
static inline int
QuoteGetter_IsClosed(QuoteGetter_C *pgetter, int*b);
```
```
static inline int
QuoteGetter_GetSymbol(QuoteGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int
QuoteGetter_SetSymbol(QuoteGetter_C *pgetter, const char* symbol);
```
```
static inline int 
GetQuote(structCredentials *pcreds, const char* symbol, char **buf, size_t *n);
```
<br>

#### QuotesGetter

Retrieve a single quote for multiple securities. [TDAmeritrade docs.](https://developer.tdameritrade.com/quotes/apis/get/marketdata/quotes)

##### [C++]

**constructors**
```
QuotesGetter::QuotesGetter(Credentials& creds, const set<string>& symbols);

    creds   ::  credentials struct received from RequestAccessToken / LoadCredentials 
                / CredentialsManager.credentials
    symbol  ::  (case sensitive) set of security symbols 

```

- empty symbol strings and/or symbol sets will throw ValueException
- if all symbols are removed, calling ```.get()``` will return an empty json object

**types**
```
class QuotesGetter : public APIGetter;
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

##### [C]

**types**
```
struct QuotesGetter_C;
```

**functions**
```
static inline int
QuotesGetter_Create(struct Credentials *pcreds, 
                    const char** symbols,
                    size_t nsymbols,
                    QuotesGetter_C *pgetter);
```
```
static inline int
QuotesGetter_Destroy(QuotesGetter_C *pgetter);
```
```
static inline int
QuotesGetter_Get(QuotesGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
QuotesGetter_Close(QuotesGetter_C *pgetter);
```
```
static inline int
QuotesGetter_IsClosed(QuotesGetter_C *pgetter, int*b);
```
```
static inline int
QuotesGetter_GetSymbols(QuotesGetter_C *pgetter, char ***buf, size_t *n);
```
```
static inline int
QuotesGetter_SetSymbols(QuotesGetter_C *pgetter, 
                        const char** 
                        symbols, size_t nsymbols);
```
```
static inline int
QuotesGetter_AddSymbol(QuotesGetter_C *pgetter, const char* symbol);
```
```
static inline int
QuotesGetter_RemoveSymbols(QuotesGetter_C *pgetter, const char* symbol);
```
```
static inline int
QuotesGetter_AddSymbols(QuotesGetter_C *pgetter, 
                        const char **symbols, 
                        size_t nsymbols);
```
```
static inline int
QuotesGetter_RemoveSymbols(QuotesGetter_C *pgetter, 
                           const char **symbols, 
                           size_t nsymbols);
```
```
static inline int
GetQuotes(struct Credentials *pcreds, 
          const char** symbols, 
          size_t nysmbols, 
          char **buf, 
          size_t *n);
```
<br>

#### MarketHoursGetter

Retrieve market hours for a single market. [TDAmeritrade docs.](https://developer.tdameritrade.com/market-hours/apis/get/marketdata/{market}/hours)

##### [C++]

**constructors**
```
MarketHoursGetter::MarketHoursGetter( Credentials& creds, 
                                      MarketType market_type,
                                      const string& date );

    creds        ::  credentials struct received from RequestAccessToken 
                     / LoadCredentials / CredentialsManager.credentials
    market_type  ::  market to get hours for
    date         ::  date string to get hours for* 
                     
    * yyyy-MM-dd or yyyy-MM-dd'T'HH::mm::ssz

```

**types**
```
class MarketHoursGetter : public APIGetter;
```
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
void
MarketHoursGetter::set_date(const string& date);
```
```
MarketType
MarketHoursGetter::get_market_type() const;
```
```
void
MarketHoursGetter::set_market_type(MarketType market_type);
```

##### [C]

**types**
```
struct MarketHoursGetter_C;
```
```
enum MarketType {
    MarketType_equity,
    MarketType_option,
    MarketType_future,
    MarketType_bond,
    MarketType_forex
};
```

**functions**
```
static inline int
MarketHoursGetter_Create( struct Credentials *pcreds,
                          MarketType market_type,
                          const char* date,
                          MarketHoursGetter_C *pgetter );
```
```
static inline int
MarketHoursGetter_Destroy(MarketHoursGetter_C *pgetter);
```
```
static inline int
MarketHoursGetter_Get(MarketHoursGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
MarketHoursGetter_Close(MarketHoursGetter_C *pgetter);
```
```
static inline int
MarketHoursGetter_IsClosed(MarketHoursGetter_C *pgetter, int *b);
```
```
static inline int
MarketHoursGetter_GetMarketType( MarketHoursGetter_C *pgetter, 
                                 MarketType *market_type );
```
```
static inline int
MarketHoursGetter_SetMarketType( MarketHoursGetter_C *pgetter, 
                                 MarketType market_type );
```
```
static inline int
MarketHoursGetter_GetDate( MarketHoursGetter_C *pgetter, char **buf, size_t *n );
```
```
static inline int
MarketHoursGetter_SetDate( MarketHoursGetter_C *pgetter, const char* date );
```
```
static inline int
GetMarketHours( struct Credentials *pcreds,
                MarketType market_type,
                const char* date,
                char **buf,
                size_t *n );
```
<br>


#### MoversGetter

Top 10 up or down movers by value or %. [TDAmeritrade docs.](https://developer.tdameritrade.com/movers/apis/get/marketdata/{index}/movers)

##### [C++]

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
class MoversGetter : public APIGetter;
```
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
void
MoversGetter::set_index(MoversIndex index);
```
```
MoversDirectionType
MoversGetter::get_direction_type() const;
```
```
void
MoversGetter::set_direction_type(MoversDirectionType direction_type);
```
```
MoversChangeType
MoversGetter::get_change_type() const
```
```
void
MoversGetter::set_change_type(MoversChangeType change_type);
```

##### [C]

**types**
```
struct MoversGetter_C;
```
```
enum MoversIndex {
    MoversIndex_compx, /* $COMPX */
    MoversIndex_dji,  /* $DJI */
    MoversIndex_spx   /* SPX */
};
```
```
enum MoversDirectionType {
    MoversDirectionType_up,
    MoversDirectionType_down,
    MoversDirectionType_up_and_down
};
```
```
enum MoversChangeType {
    MoversChangeType_value,
    MoversChangeType_percent
};
```

**functions**
```
static inline int
MoversGetter_Create( struct Credentials *pcreds,
                     MoversIndex index,
                     MoversDirectionType direction_type,
                     MoversChangeType change_type,
                     MoversGetter_C *pgetter );
```
```
static inline int
MoversGetter_Destroy( MoversGetter_C *pgetter)
{ return MoversGetter_Destroy_ABI(pgetter, 0); };
```
```
static inline int
MoversGetter_Get(MoversGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
MoversGetter_Close(MoversGetter_C *pgetter);
```
```
static inline int
MoversGetter_IsClosed(MoversGetter_C *pgetter, int *b);
```
```
static inline int
MoversGetter_GetIndex( MoversGetter_C *pgetter, MoversIndex *index);
```
```
static inline int
MoversGetter_SetIndex( MoversGetter_C *pgetter,  MoversIndex index);
```
```
static inline int
MoversGetter_GetDirectionType( MoversGetter_C *pgetter,
                               MoversDirectionType *direction_type );
```
```
static inline int
MoversGetter_SetDirectionType( MoversGetter_C *pgetter,
                               MoversDirectionType direction_type);
```
```
static inline int
MoversGetter_GetChangeType( MoversGetter_C *pgetter,
                            MoversChangeType *change_type );
```
```
static inline int
MoversGetter_SetChangeType( MoversGetter_C *pgetter,
                            MoversChangeType change_type);
```
```
static inline int
GetMovers( struct Credentials *pcreds,
           MoversIndex index,
           MoversDirectionType direction_type,
           MoversChangeType change_type,
           char **buf,
           size_t *n );
```
<br>

#### HistoricalPeriodGetter

Price history for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/price-history/apis/get/marketdata/{symbol}/pricehistory)

Control the size and granularity of the historical period using the following fields:
- period_type - *e.g day*
- period - *e.g 3 (days)*
- frequency_type - *e.g minute*
- frequency - *e.g 5 (minutes)*

By default the period will be 'anchored' at an end point of **yesterday** (excluding today's data). To change this anchor point set the optional field:
- msec_since_epoch - *e.g -1512108000000 to start on the 12/01/2017 bar*

How these five parameters interact (what are valid combinations, what throws and when etc.)
can be somewhat complex so **please read the detailed comments** in the C++ *constructors* and *utilities* sections below.

##### [C++]

**constructors**
```
HistoricalPeriodGetter::HistoricalPeriodGetter( Credentials& creds,
                                                const string& symbol,
                                                PeriodType period_type,
                                                unsigned int period,
                                                FrequencyType frequency_type,
                                                unsigned int frequency,
                                                bool extended_hours,
                                                long long msec_since_epoch = 0 );

    creds            ::  credentials struct received from RequestAccessToken 
                         / LoadCredentials / CredentialsManager.credentials
    symbol           :: (case sensitive) security symbol
    period_type      :: type of period (day, month etc.)
    period           :: # of periods **
    frequency_type   :: type of frequency (minute, daily etc.) *
    frequency        :: # of that frequency (e.g 10 or 100 minutes) *
    extended_hours   :: include extended hours data
    msec_since_epoch :: milliseconds since epoch of period anchor **

    * these args are restricted to certain values based on other args and will
      throw ValueError if invalid, see the 'utilities' section below 

    ** msec_since_epoch represents both the datetime that will serve to 
       anchor/bound the period and the position of the period with respect to it.

       > 0 : milliseconds since epoch to END of period; the period will
             end at the last full candle before this datetime

       < 0 : -1 * milliseconds since epoch to START of period; the period
             will begin at the first full candle after this datetime  

       = 0 : maintain/revert to default behavior(anchor end to yesterday,
             ignoring todays candles; pass a current or future value to 
             include today)    

       e.g 

       (-1512108000000) w/ a monthly frequency will start the period 
       with the 12/01/17 monthly candle  

       (1512108000000) w/ a monthly frequency will end the period 
       with the 11/01/17 monthly candle         
```

**types**
```
class HistoricalPeriodGetter : public HistoricalGetterBase;
```
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

```VALID_PERIODS_BY_PERIOD_TYPE``` - A mapping that provides *the* set of valid # of 
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

```VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE``` - A mapping that provides *the* set of valid 
frequency types for a particular period type. (e.g ```PeriodType::day``` only allows for ```FrequencyType::minute```)
```
const unordered_map<PeriodType, set<FrequencyType, EnumCompare<FrequencyType>>,
                    EnumHash<PeriodType>>
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE ={
    {PeriodType::day, {FrequencyType::minute} },
    {PeriodType::month, {FrequencyType::daily, FrequencyType::weekly} },
    {PeriodType::year, {FrequencyType::daily, FrequencyType::weekly, 
                        FrequencyType::monthly} },
    {PeriodType::ytd, {FrequencyType::daily, FrequencyType::weekly} },
};
```

```VALID_FREQUENCIES_BY_FREQUENCY_TYPE``` - A mapping that provides *the* set of valid frequency amounts for a particular frequency type. (e.g ```FrequencyType::daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
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
void  
HistoricalGetterBase::set_symbol(const string& symbol);
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
void
HistoricalGetterBase::set_frequency( FrequencyType frequency_type,
                                       unsigned int frequency );
```
```
bool 
HistoricalGetterBase::is_extended_hours() const;  
```
```
void 
HistoricalGetterBase::set_extended_hours(bool extended_hours);
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
void
HistoricalPeriodGetter::set_period( PeriodType period_type, 
                                    usigned int period );
```
```
long long
HistoricalPeriodGetter::get_msec_since_epoch() const;
```
```
void
HistoricalPeriodGetter::set_msec_since_epoch( long long msec_since_epoch );
```

##### [C]

**types**
```
struct HistoricalPeriodGetter_C;
```
```
enum PeriodType {
    PeriodType_day,
    PeriodType_month,
    PeriodType_year,
    PeriodType_ytd
};
```
```
enum FrequencyType {
    FrequencyType_minute,
    FrequencyType_daily,
    FrequencyType_weekly,
    FrequencyType_monthly
};
```
**utilities**

```VALID_PERIODS_BY_PERIOD_TYPE``` - A mapping that provides *the* set of valid # of 
periods for a particular period type. (e.g ```PeriodType_day``` allows for 1,2,3,4,5, and 10 periods)
```
static const int
VALID_PERIODS_BY_PERIOD_TYPE[PeriodType_ytd + 1][8] = {
    {1,2,3,4,5,10,-1,-1},
    {1,2,3,6,-1,-1,-1,-1},
    {1,2,3,5,10,15,10,-1},
    {1,-1,-1,-1,-1,-1,-1,-1},
};
```

```VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE``` - A mapping that provides *the* set of valid 
frequency types for a particular period type. (e.g ```PeriodType_day``` only allows for ```FrequencyType_minute```)
```
static const FrequencyType
VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[PeriodType_ytd + 1][4] = {
    {FrequencyType_minute, -1, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, -1, -1},
    {FrequencyType_daily, FrequencyType_weekly, FrequencyType_monthly, -1},
    {FrequencyType_daily, FrequencyType_weekly, -1, -1}
};
```

```VALID_FREQUENCIES_BY_FREQUENCY_TYPE``` - A mapping that provides *the* set of valid frequency amounts for a particular frequency type. (e.g ```FrequencyType_daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
```
static const int
VALID_FREQUENCIES_BY_FREQUENCY_TYPE[FrequencyType_monthly + 1][5] = {
    {1, 5, 10, 30, -1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1}
};
```

- invalid frequency-to-frequency-type combinations will return error code immediately
- invalid period-to-period-type combinations will return error code immediately
- invalid frequency-type-to-period-type combinations will only return error code immediately from 'Create' function; if passed to the 'Set' methods it will not return an error code until the '_Get' function is called

**functions**
```
static inline int
HistoricalPeriodGetter_Create( struct Credentials *pcreds,
                               const char* symbol,
                               int period_type,
                               unsigned int period,
                               int frequency_type,
                               unsigned int frequency,
                               int extended_hours,
                               long long msec_since_epoch,
                               HistoricalPeriodGetter_C *pgetter );
```
```
static inline int
HistoricalPeriodGetter_Destroy( HistoricalPeriodGetter_C *pgetter );
```
```
static inline int
HistoricalPeriodGetter_Get(HistoricalPeriodGetter_C *pgetter, 
                           char** buf,  
                           size_t *n);
```
```
static inline int
HistoricalPeriodGetter_Close(HistoricalPeriodGetter_C *pgetter);
```
```
static inline int
HistoricalPeriodGetter_IsClosed(HistoricalPeriodGetter_C *pgetter, int *b);
```
```
static inline int
HistoricalPeriodGetter_GetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  char **buf,
                                  size_t *n );
```
```
static inline int
HistoricalPeriodGetter_SetSymbol( HistoricalPeriodGetter_C *pgetter,
                                  const char *symbol );
```
```
static inline int
HistoricalPeriodGetter_GetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     unsigned int *frequency );
```
```
static inline int
HistoricalPeriodGetter_GetFrequencyType( HistoricalPeriodGetter_C *pgetter,
                                         FrequencyType *frequency_type );
```
```
static inline int
HistoricalPeriodGetter_SetFrequency( HistoricalPeriodGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency );
```
```
static inline int
HistoricalPeriodGetter_IsExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                        int *is_extended_hours );
```
```
static inline int
HistoricalPeriodGetter_SetExtendedHours( HistoricalPeriodGetter_C *pgetter,
                                         int is_extended_hours );
```
```
static inline int
HistoricalPeriodGetter_GetPeriodType( HistoricalPeriodGetter_C *pgetter,
                                      PeriodType *period_type );
```
```
static inline int
HistoricalPeriodGetter_GetPeriod( HistoricalPeriodGetter_C *pgetter,
                                  unsigned int *period );
```
```
static inline int
HistoricalPeriodGetter_SetPeriod( HistoricalPeriodGetter_C *pgetter,
                                  PeriodType period_type,
                                  unsigned int period );
```
```
static inline int
HistoricalPeriodGetter_SetMSecSinceEpoch( HistoricalPeriodGetter_C *pgetter,
                                          long long msec_since_epoch );
```
```
static inline int
HistoricalPeriodGetter_GetMSecSinceEpoch( HistoricalPeriodGetter_C *pgetter,
                                          long long *msec_since_epoch );
```
```
static inline int
GetHistoricalPeriod( struct Credentials *pcreds,
                     const char* symbol,
                     int period_type,
                     unsigned int period,
                     int frequency_type,
                     unsigned int frequency,
                     int extended_hours,
                     long long msec_since_epoch,
                     char **buf,
                     size_t *n );
```
<br>

#### HistoricalRangeGetter

Price history between a date range for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/price-history/apis/get/marketdata/{symbol}/pricehistory)

##### [C++]

**constructors**
```
HistoricalRangeGetter::HistoricalRangeGetter( 
        Credentials& creds,
        const string& symbol,                         
        FrequencyType frequency_type,
        unsigned int frequency,
        unsigned long long start_msec_since_epoch,
        unsigned long long end_msec_since_epoch,
        bool extended_hours 
    );

    creds                  ::  credentials struct received from RequestAccessToken 
                               / LoadCredentials / CredentialsManager.credentials
    symbol                 :: (case sensitive) security symbol
    frequency_type         :: type of frequency (minute, daily etc.) 
    frequency              :: # of that frequency (e.g 10 or 100 minutes) *
    start_msec_since_epoch :: start of historical range in milliseconds since epoch**
    end_msec_since_epoch   :: end of historical range in milliseconds since epoch**
    extended_hours         :: include extended hours data

    * these args are restricted to certain values based on other args and will
       throw ValueError if invalid, see the 'utilities' section below 

    ** the epoch is 00:00 of Jan 1 1970
```

**types**
```
class HistoricalRangeGetter : public HistoricalGetterBase;
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

```VALID_FREQUENCIES_BY_FREQUENCY_TYPE``` - A mapping that provides *the* set of valid 
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
void  
HistoricalGetterBase::set_symbol(const string& symbol);
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
void
HistoricalGetterBase::set_frequency( FrequencyType frequency_type,
                                       unsigned int frequency );
```
```
bool 
HistoricalGetterBase::is_extended_hours() const;  
```
```
void 
HistoricalGetterBase::set_extended_hours(bool extended_hours);
```
```
PeriodType
HistoricalRangeGetter::get_end_msec_since_epoch() const;
```
```
void
HistoricalRangeGetter::set_end_msec_since_epoch(
    usigned long long end_msec_since_epoch
);
```
```
unsigned int
HistoricalRangeGetter::get_start_msec_since_epoch() const;
```
```
void
HistoricalRangeGetter::set_stat_msec_since_epoch(
    unsigned long long start_msec_since_epoch
);
```


##### [C]

**types**
```
struct HistoricalRangeGetter_C;
```
```
enum FrequencyType {
    FrequencyType_minute,
    FrequencyType_daily,
    FrequencyType_weekly,
    FrequencyType_monthly
};
```
**utilities**

```VALID_FREQUENCIES_BY_FREQUENCY_TYPE``` - A mapping that provides *the* set of valid frequency amounts for a particular frequency type. (e.g ```FrequencyType_daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
```
static const int
VALID_FREQUENCIES_BY_FREQUENCY_TYPE[FrequencyType_monthly + 1][5] = {
    {1, 5, 10, 30, -1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1},
    {1, -1, -1, -1 ,-1}
};
```
- invalid frequency-to-frequency-type combinations will return error code immediately

**functions**
```
static inline int
HistoricalRangeGetter_Create( struct Credentials *pcreds,
                               const char* symbol,
                               FrequencyType frequency_type,
                               unsigned int frequency,
                               unsigned long long start_msec_since_epoch,
                               unsigned long long end_msec_since_epoch,
                               int extended_hours,
                               HistoricalRangeGetter_C *pgetter );
```
```
static inline int
HistoricalRangeGetter_Destroy( HistoricalRangeGetter_C *pgetter );
```
```
static inline int
HistoricalRangeGetter_Get( HistoricalRangeGetter_C *pgetter, 
                           char** buf,  
                           size_t *n);
```
```
static inline int
HistoricalRangeGetter_Close(HistoricalRangeGetter_C *pgetter);
```
```
static inline int
HistoricalRangeGetter_IsClosed(HistoricalRangeGetter_C *pgetter, int *b);
```
```
static inline int
HistoricalRangeGetter_GetSymbol( HistoricalRangeGetter_C *pgetter,
                                  char **buf,
                                  size_t *n );
```
```
static inline int
HistoricalRangeGetter_SetSymbol( HistoricalRangeGetter_C *pgetter,
                                  const char *symbol );
```
```
static inline int
HistoricalRangeGetter_GetFrequency( HistoricalRangeGetter_C *pgetter,
                                    unsigned int *frequency );
```
```
static inline int
HistoricalRangeGetter_GetFrequencyType( HistoricalRangeGetter_C *pgetter,
                                        FrequencyType *frequency_type );
```
```
static inline int
HistoricalRangeGetter_SetFrequency( HistoricalRangeGetter_C *pgetter,
                                     FrequencyType frequency_type,
                                     unsigned int frequency );
```
```
static inline int
HistoricalRangeGetter_IsExtendedHours( HistoricalRangeGetter_C *pgetter,
                                       int *is_extended_hours );
```
```
static inline int
HistoricalRangeGetter_SetExtendedHours( HistoricalRangeGetter_C *pgetter,
                                        int is_extended_hours );
```
```
static inline int
HistoricalRangeGetter_GetEndMSecSinceEpoch( HistoricalRangeGetter_C *pgetter,
                                            unsigned long long *end_msec );
```
```
static inline int
HistoricalRangeGetter_SetEndMSecSinceEpoch( HistoricalRangeGetter_C *pgetter,
                                            unsigned long long end_msec );
```
```
static inline int
HistoricalRangeGetter_GetStartMSecSinceEpoch( HistoricalRangeGetter_C *pgetter,
                                              unsigned long long *start_msec );
```
```
static inline int
HistoricalRangeGetter_SetStartMSecSinceEpoch( HistoricalRangeGetter_C *pgetter,
                                              unsigned long long start_msec );
```
```
static inline int
GetHistoricalRange( struct Credentials *pcreds,
                    const char* symbol,
                    FrequencyType frequency_type,
                    unsigned int frequency,
                    unsigned long long start_msec_since_epoch,
                    unsigned long long end_msec_since_epoch,
                    int extended_hours,
                    char **buf,
                    size_t *n );
```

<br>



#### OptionChainGetter

Traditional Option chain for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

##### [C++]

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
    from_date      :: start date of range of chains*
    to_date        :: end date of range of chains*
    exp_month      :: expiration month
    option_type    :: type of option (standard, non-standard)

    * yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz

```

**types**
```
class OptionChainGetter : public APIGetter;
```
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
```
enum class OptionStrikesType : int {
    n_atm,
    single,
    range,
    none, 
};
```
```
typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;
```
```
class OptionStrikes {
    using Type = OptionStrikesType;
    Type _type;
    OptionStrikesValue _value;
    ...
};
```

**utilities**

**```OptionStrikes```** - Class used to represent one of three distinct strike 
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
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
void
OptionChainGetter::set_strikes(const OptionStrikes& strikes);
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
string
OptionChainGetter::get_from_date() const;
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
string
OptionChainGetter::get_to_date() const;
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```

##### [C]

**types**
```
struct OptionChainGetter_C;
```
```
enum OptionContractType {
    OptionContractType_call,
    OptionContractType_put,
    OptionContractType_all
};
```
```
enum OptionRangeType {
    OptionRangeType_null, /* this shouldn't be used directly  */
    OptionRangeType_itm, /* in-the-money */
    OptionRangeType_ntm, /* near-the-money */
    OptionRangeType_otm, /* out-of-the-monety */
    OptionRangeType_sak, /* strikes-above-market */
    OptionRangeType_sbk, /* strikes-below-market */
    OptionRangeType_snk, /* strikes-near-market */
    OptionRangeType_all
};
```
```
enum OptionExpMonth {
    OptionExpMonth_jan,
    OptionExpMonth_feb,
    OptionExpMonth_mar,
    OptionExpMonth_apr,
    OptionExpMonth_may,
    OptionExpMonth_jun,
    OptionExpMonth_jul,
    OptionExpMonth_aug,
    OptionExpMonth_sep,
    OptionExpMonth_oct,
    OptionExpMonth_nov,
    OptionExpMonth_dec,
    OptionExpMonth_all
};
```
```
enum OptionType {
    OptionType_s, /* standard */
    OptionType_ns, /* non-standard */
    OptionType_all
};
```
```
enum OptionStrikesType {
    OptionStrikesType_n_atm,
    OptionStrikesType_single,
    OptionStrikesType_range,
    OptionStrikesType_none, /* don't use */
};
```
```
typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;
```

- the OptionStrikesValue field used should match the OptionStrikesType

**functions**
```
static inline int
OptionChainGetter_Create( struct Credentials *pcreds,
                          const char* symbol,
                          OptionStrikesType strikes_type,
                          OptionStrikesValue strikes_value,
                          OptionContractType contract_type,
                          int include_quotes,
                          const char* from_date,
                          const char* to_date,
                          OptionExpMonth exp_month,
                          OptionType option_type,
                          OptionChainGetter_C *pgetter );
```
```
static inline int
OptionChainGetter_Destroy(OptionChainGetter_C *pgetter );
```
```
static inline int
OptionChainGetter_Get( OptionChainGetter_C *pgetter, char** buf,  size_t *n);
```
```
static inline int
OptionChainGetter_Close(OptionChainGetter_C *pgetter);
```
```
static inline int
OptionChainGetter_IsClosed(OptionChainGetter_C *pgetter, int *b);
```
```
static inline int
OptionChainGetter_GetSymbol(OptionChainGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int
OptionChainGetter_SetSymbol(OptionChainGetter_C *pgetter, const char *symbol);
```
```
static inline int
OptionChainGetter_GetStrikes( OptionChainGetter_C *pgetter, 
                              OptionStrikesType *strikes_type, 
                              OptionStrikesValue *strikes_value );
```
```
static inline int
OptionChainGetter_SetStrikes( OptionChainGetter_C *pgetter, 
                              OptionStrikesType strikes_type, 
                              OptionStrikesValue strikes_value );
```
```
static inline int
OptionChainGetter_GetContractType( OptionChainGetter_C *pgetter, 
                                   OptionContractType *contract_type );
```
```
static inline int
OptionChainGetter_SetContractType( OptionChainGetter_C *pgetter, 
                                   OptionContractType contract_type );
```
```
static inline int
OptionChainGetter_IncludesQuotes( OptionChainGetter_C *pgetter, 
                                  int *includes_quotes );
```
```
static inline int
OptionChainGetter_IncludeQuotes( OptionChainGetter_C *pgetter, int include_quotes );
```
```
static inline int
OptionChainGetter_GetFromDate(OptionChainGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int
OptionChainGetter_SetFromDate(OptionChainGetter_C *pgetter, const char *date);
```
```
static inline int
OptionChainGetter_GetToDate(OptionChainGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int
OptionChainGetter_SetToDate(OptionChainGetter_C *pgetter, const char *date);
```
```
static inline int
OptionChainGetter_GetExpMonth( OptionChainGetter_C *pgetter, 
                               OptionExpMonth *exp_month );
```
```
static inline int
OptionChainGetter_SetExpMonth( OptionChainGetter_C *pgetter, 
                               OptionExpMonth exp_month );
```
```
static inline int
OptionChainGetter_GetOptionType( OptionChainGetter_C *pgetter, 
                                 OptionType *option_type );
```
```
static inline int
OptionChainGetter_SetOptionType( OptionChainGetter_C *pgetter, 
                                 OptionType option_type );
```
```
static inline int
GetOptionChain( struct Credentials *pcreds,
                const char* symbol,
                OptionStrikesType strikes_type,
                OptionStrikesValue strikes_value,
                OptionContractType contract_type,
                int include_quotes,
                const char* from_date,
                const char* to_date,
                OptionExpMonth exp_month,
                OptionType option_type,
                char **buf,
                size_t *n );
```

<br>


#### OptionChainStrategyGetter

Option chains of strategies (spreads) for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

##### [C++]

**constructors**
```
OptionChainStrategyGetter::OptionChainStrategyGetter( 
        Credentials& creds,
        const string& symbol,  
        const OptionStrategy& strategy,                       
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
    from_date      :: start date of range of chains*
    to_date        :: end date of range of chains* 
    exp_month      :: expiration month
    option_type    :: type of option (standard, non-standard)

    * yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz

```

**types**
```
class OptionChainStrategyGetter : public OptionChainGetter;
```
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
```
enum class OptionStrikesType : int {
    n_atm,
    single,
    range,
    none, 
};
```
```
typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;
```
```
class OptionStrikes {
    using Type = OptionStrikesType;
    Type _type;
    OptionStrikesValue _value;
    ...
};
```
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
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
void
OptionChainGetter::set_strikes(const OptionStrikes& strikes);
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
string
OptionChainGetter::get_from_date() const;
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
string
OptionChainGetter::get_to_date() const;
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```
```
OptionStrategy
OptionChainStrategyGetter::get_strategy() const;
```
```
void
OptionChainStrategyGetter::set_strategy(const OptionStrategy& strategy);
```


##### [C]

**types**
```
struct OptionChainStrategyGetter_C;
```
```
enum OptionContractType {
    OptionContractType_call,
    OptionContractType_put,
    OptionContractType_all
};
```
```
enum OptionStrategyType { 
    OptionStrategyType_covered,
    OptionStrategyType_vertical,
    OptionStrategyType_calendar,
    OptionStrategyType_strangle,
    OptionStrategyType_straddle,
    OptionStrategyType_butterfly,
    OptionStrategyType_condor,
    OptionStrategyType_diagonal,
    OptionStrategyType_collar,
    OptionStrategyType_roll
}
```
```
enum OptionRangeType {
    OptionRangeType_null, /* this shouldn't be used directly  */
    OptionRangeType_itm, /* in-the-money */
    OptionRangeType_ntm, /* near-the-money */
    OptionRangeType_otm, /* out-of-the-monety */
    OptionRangeType_sak, /* strikes-above-market */
    OptionRangeType_sbk, /* strikes-below-market */
    OptionRangeType_snk, /* strikes-near-market */
    OptionRangeType_all
};
```
```
enum OptionExpMonth {
    OptionExpMonth_jan,
    OptionExpMonth_feb,
    OptionExpMonth_mar,
    OptionExpMonth_apr,
    OptionExpMonth_may,
    OptionExpMonth_jun,
    OptionExpMonth_jul,
    OptionExpMonth_aug,
    OptionExpMonth_sep,
    OptionExpMonth_oct,
    OptionExpMonth_nov,
    OptionExpMonth_dec,
    OptionExpMonth_all
};
```
```
enum OptionType {
    OptionType_s, /* standard */
    OptionType_ns, /* non-standard */
    OptionType_all
};
```
```
enum OptionStrikesType {
    OptionStrikesType_n_atm,
    OptionStrikesType_single,
    OptionStrikesType_range,
    OptionStrikesType_none, /* don't use */
};
```
```
typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;
```

- the OptionStrikesValue field used should match the OptionStrikesType


**functions**
```
static inline int
OptionChainStrategyGetter_Create( struct Credentials *pcreds,
                                  const char* symbol,
                                  OptionStrategyType strategy_type,
                                  double spread_interval,
                                  OptionStrikesType strikes_type,
                                  OptionStrikesValue strikes_value,
                                  OptionContractType contract_type,
                                  int include_quotes,
                                  const char* from_date,
                                  const char* to_date,
                                  OptionExpMonth exp_month,
                                  OptionType option_type,
                                  OptionChainStrategyGetter_C *pgetter );
```
```
static inline int
OptionChainStrategyGetter_Destroy( OptionChainStrategyGetter_C *pgetter );
```
```
static inline int
OptionChainStrategyGetter_Get( OptionChainStrategyGetter_C *pgetter, 
                               char** buf,  
                               size_t *n );
```
```
static inline int
OptionChainStrategyGetter_Close(OptionChainStrategyGetter_C *pgetter);
```
```
static inline int
OptionChainStrategyGetter_IsClosed(OptionChainStrategyGetter_C *pgetter, int *b);
```
```
static inline int
OptionChainStrategyGetter_GetSymbol( OptionChainStrategyGetter_C *pgetter, 
                                     char **buf, 
                                     size_t *n );
```
```
static inline int
OptionChainStrategyGetter_SetSymbol( OptionChainStrategyGetter_C *pgetter, 
                                     const char *symbol );
```
```
static inline int
OptionChainStrategyGetter_GetStrikes( OptionChainStrategyGetter_C *pgetter, 
                                      OptionStrikesType *strikes_type, 
                                      OptionStrikesValue *strikes_value );
```
```
static inline int
OptionChainStrategyGetter_SetStrikes( OptionChainStrategyGetter_C *pgetter, 
                                      OptionStrikesType strikes_type, 
                                      OptionStrikesValue strikes_value );
```
```
static inline int
OptionChainStrategyGetter_GetContractType( OptionChainStrategyGetter_C *pgetter, 
                                           OptionContractType *contract_type );
```
```
static inline int
OptionChainStrategyGetter_SetContractType( OptionChainStrategyGetter_C *pgetter, 
                                           OptionContractType contract_type );
```
```
static inline int
OptionChainStrategyGetter_IncludesQuotes( OptionChainStrategyGetter_C *pgetter, 
                                          int *includes_quotes );
```
```
static inline int
OptionChainStrategyGetter_IncludeQuotes( OptionChainStrategyGetter_C *pgetter, 
                                         int include_quotes );
```
```
static inline int
OptionChainStrategyGetter_GetFromDate( OptionChainStrategyGetter_C *pgetter, 
                                       char **buf, 
                                       size_t *n );
```
```
static inline int
OptionChainStrategyGetter_SetFromDate( OptionChainStrategyGetter_C *pgetter, 
                                       const char *date );
```
```
static inline int
OptionChainStrategyGetter_GetToDate( OptionChainStrategyGetter_C *pgetter, 
                                     char **buf, 
                                     size_t *n );
```
```
static inline int
OptionChainStrategyGetter_SetToDate( OptionChainStrategyGetter_C *pgetter, 
                                     const char *date );
```
```
static inline int
OptionChainStrategyGetter_GetExpMonth( OptionChainStrategyGetter_C *pgetter, 
                                       OptionExpMonth *exp_month );
```
```
static inline int
OptionChainStrategyGetter_SetExpMonth( OptionChainStrategyGetter_C *pgetter, 
                                       OptionExpMonth exp_month );
```
```
static inline int
OptionChainStrategyGetter_GetOptionType( OptionChainStrategyGetter_C *pgetter, 
                                         OptionType *option_type );
```
```
static inline int
OptionChainStrategyGetter_SetOptionType( OptionChainStrategyGetter_C *pgetter, 
                                         OptionType option_type );
```
```
static inline int
OptionChainStrategyGetter_GetStrategy( OptionChainStrategyGetter_C *pgetter,
                                       OptionStrategyType *strategy_type,
                                       double *spread_interval );
```
```
static inline int
OptionChainStrategyGetter_SetStrategy( OptionChainStrategyGetter_C *pgetter,
                                        OptionStrategyType strategy_type,
                                        double spread_interval );
```
```
static inline int
GetOptionChainStrategy( struct Credentials *pcreds,
                        const char* symbol,
                        OptionStrategyType strategy_type,
                        double spread_interval,
                        OptionStrikesType strikes_type,
                        OptionStrikesValue strikes_value,
                        OptionContractType contract_type,
                        int include_quotes,
                        const char* from_date,
                        const char* to_date,
                        OptionExpMonth exp_month,
                        OptionType option_type,
                        char **buf,
                        size_t *n );
```

<br>


#### OptionChainAnalyticalGetter

Option chains with analytics, for a single security. [TDAmeritrade docs.](https://developer.tdameritrade.com/option-chains/apis/get/marketdata/chains)

##### [C++]

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
    from_date        :: start date of range of chains*
    to_date          :: end date of range of chains*
    exp_month        :: expiration month
    option_type      :: type of option (standard, non-standard)

    * yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz

```

**types**
```
class OptionChainAnalyticalGetter : public OptionChainGetter;
```
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
void
OptionChainGetter::set_symbol(const string& symbol);
```
```
OptionStrikes
OptionChainGetter::get_strikes() const;
```
```
void
OptionChainGetter::set_strikes(const OptionStrikes& strikes);
```
```
OptionContractType
OptionChainGetter::get_contract_type() const;
```
```
void
OptionChainGetter::set_contract_type(OptionContractType contract_type);
```
```
bool
OptionChainGetter::includes_quotes() const;
```
```
void
OptionChainGetter::include_quotes(bool includes_quotes);
```
```
string
OptionChainGetter::get_from_date() const;
```
```
void
OptionChainGetter::set_from_date(const string& from_date);
```
```
string
OptionChainGetter::get_to_date() const;
```
```
void
OptionChainGetter::set_to_date(const string& to_date);
```
```
OptionExpMonth
OptionChainGetter::get_exp_month() const;
```
```
void
OptionChainGetter::set_exp_month(OptionExpMonth exp_month);
```
```
OptionType
OptionChainGetter::get_option_type() const
```
```
void
OptionChainGetter::set_option_type(OptionType option_type);
```
```
double 
OptionChainAnalyticalGetter::get_volatility() const;
```
```
void
OptionChainAnalyticalGetter::set_volatility(double volatility);
```
```
double 
OptionChainAnalyticalGetter::get_underlying_price() const;
```
```
void
OptionChainAnalyticalGetter::set_underlying_price(double underlying_price);
```
```
double
OptionChainAnalyticalGetter::get_interest_rate() const;
```
```
void
OptionChainAnalyticalGetter::set_interest_rate(double interest_rate);
```
```
unsigned int
OptionChainAnalyticalGetter::get_days_to_exp() const;
```
```
void
OptionChainAnalyticalGetter::set_days_to_exp(unsigned int days_to_exp);
```

##### [C]

**types**
```
struct OptionChainAnalyticalGetter_C;
```
```
enum OptionContractType {
    OptionContractType_call,
    OptionContractType_put,
    OptionContractType_all
};
```
```
enum OptionRangeType {
    OptionRangeType_null, /* this shouldn't be used directly  */
    OptionRangeType_itm, /* in-the-money */
    OptionRangeType_ntm, /* near-the-money */
    OptionRangeType_otm, /* out-of-the-monety */
    OptionRangeType_sak, /* strikes-above-market */
    OptionRangeType_sbk, /* strikes-below-market */
    OptionRangeType_snk, /* strikes-near-market */
    OptionRangeType_all
};
```
```
enum OptionExpMonth {
    OptionExpMonth_jan,
    OptionExpMonth_feb,
    OptionExpMonth_mar,
    OptionExpMonth_apr,
    OptionExpMonth_may,
    OptionExpMonth_jun,
    OptionExpMonth_jul,
    OptionExpMonth_aug,
    OptionExpMonth_sep,
    OptionExpMonth_oct,
    OptionExpMonth_nov,
    OptionExpMonth_dec,
    OptionExpMonth_all
};
```
```
enum OptionType {
    OptionType_s, /* standard */
    OptionType_ns, /* non-standard */
    OptionType_all
};
```
```
enum OptionStrikesType {
    OptionStrikesType_n_atm,
    OptionStrikesType_single,
    OptionStrikesType_range,
    OptionStrikesType_none, /* don't use */
};
```
```
typedef union {
    unsigned int _n_atm;
    double _single;
    OptionRangeType _range;
} OptionStrikesValue;
```

- the OptionStrikesValue field used should match the OptionStrikesType


**functions**
```
static inline int
OptionChainAnalyticalGetter_Create( struct Credentials *pcreds,
                                    const char* symbol,
                                    double volatility,
                                    double underlying_price,
                                    double interest_rate,
                                    unsigned int days_to_exp,
                                    OptionStrikesType strikes_type,
                                    OptionStrikesValue strikes_value,
                                    OptionContractType contract_type,
                                    int include_quotes,
                                    const char* from_date,
                                    const char* to_date,
                                    OptionExpMonth exp_month,
                                    OptionType option_type,
                                    OptionChainAnalyticalGetter_C *pgetter );
```
```
static inline int
OptionChainAnalyticalGetter_Destroy( OptionChainAnalyticalGetter_C *pgetter );
```
```
static inline int
OptionChainAnalyticalGetter_Get( OptionChainAnalyticalGetter_C *pgetter, 
                                 char** buf,  
                                 size_t *n );
```
```
static inline int
OptionChainAnalyticalGetter_Close(OptionChainAnalyticalGetter_C *pgetter);
```
```
static inline int
OptionChainAnalyticalGetter_IsClosed(OptionChainAnalyticalGetter_C *pgetter, int *b);
```
```
static inline int
OptionChainAnalyticalGetter_GetSymbol( OptionChainAnalyticalGetter_C *pgetter, 
                                       char **buf, 
                                       size_t *n );
```
```
static inline int
OptionChainAnalyticalGetter_SetSymbol( OptionChainAnalyticalGetter_C *pgetter, 
                                       const char *symbol );
```
```
static inline int
OptionChainAnalyticalGetter_GetStrikes( OptionChainAnalyticalGetter_C *pgetter, 
                                        OptionStrikesType *strikes_type, 
                                        OptionStrikesValue *strikes_value );
```
```
static inline int
OptionChainAnalyticalGetter_SetStrikes( OptionChainAnalyticalGetter_C *pgetter, 
                                        OptionStrikesType strikes_type, 
                                        OptionStrikesValue strikes_value );
```
```
static inline int
OptionChainAnalyticalGetter_GetContractType( OptionChainAnalyticalGetter_C *pgetter, 
                                             OptionContractType *contract_type );
```
```
static inline int
OptionChainAnalyticalGetter_SetContractType( OptionChainAnalyticalGetter_C *pgetter, 
                                             OptionContractType contract_type );
```
```
static inline int
OptionChainAnalyticalGetter_IncludesQuotes( OptionChainAnalyticalGetter_C *pgetter, 
                                            int *includes_quotes );
```
```
static inline int
OptionChainAnalyticalGetter_IncludeQuotes( OptionChainAnalyticalGetter_C *pgetter, 
                                           int include_quotes );
```
```
static inline int
OptionChainAnalyticalGetter_GetFromDate( OptionChainAnalyticalGetter_C *pgetter, 
                                         char **buf, 
                                         size_t *n );
```
```
static inline int
OptionChainAnalyticalGetter_SetFromDate( OptionChainAnalyticalGetter_C *pgetter, 
                                         const char *date );
```
```
static inline int
OptionChainAnalyticalGetter_GetToDate( OptionChainAnalyticalGetter_C *pgetter, 
                                       char **buf, 
                                       size_t *n );
```
```
static inline int
OptionChainAnalyticalGetter_SetToDate( OptionChainAnalyticalGetter_C *pgetter, 
                                       const char *date );
```
```
static inline int
OptionChainAnalyticalGetter_GetExpMonth( OptionChainAnalyticalGetter_C *pgetter, 
                                         OptionExpMonth *exp_month );
```
```
static inline int
OptionChainAnalyticalGetter_SetExpMonth( OptionChainAnalyticalGetter_C *pgetter, 
                                         OptionExpMonth exp_month );
```
```
static inline int
OptionChainAnalyticalGetter_GetOptionType( OptionChainAnalyticalGetter_C *pgetter, 
                                           OptionType *option_type );
```
```
static inline int
OptionChainAnalyticalGetter_SetOptionType( OptionChainAnalyticalGetter_C *pgetter, 
                                           OptionType option_type );
```
```
static inline int
OptionChainAnalyticalGetter_GetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double *volatility
    );
```
```
static inline int
OptionChainAnalyticalGetter_SetVolatility(
    OptionChainAnalyticalGetter_C *pgetter,
    double volatility
    );
```
```
static inline int
OptionChainAnalyticalGetter_GetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double *underlying_price
    );
```
```
static inline int
OptionChainAnalyticalGetter_SetUnderlyingPrice(
    OptionChainAnalyticalGetter_C *pgetter,
    double underlying_price
    );
```
```
static inline int
OptionChainAnalyticalGetter_GetInterestRate(
    OptionChainAnalyticalGetter_C *pgetter,
    double *interest_rate
    )
```
```
static inline int
OptionChainAnalyticalGetter_SetInterestRate( 
    OptionChainAnalyticalGetter_C *pgetter,
    double interest_rate
    );
```
```
static inline int
OptionChainAnalyticalGetter_GetDaysToExp( OptionChainAnalyticalGetter_C *pgetter,
                                          unsigned int *days_to_exp );
```
```
static inline int
OptionChainAnalyticalGetter_SetDaysToExp( OptionChainAnalyticalGetter_C *pgetter,
                                          unsigned int days_to_exp );
```
```
static inline int
GetOptionChainAnalytical( struct Credentials *pcreds,
                          const char* symbol,
                          double volatility,
                          double underlying_price,
                          double interest_rate,
                          unsigned int days_to_exp,
                          OptionStrikesType strikes_type,
                          OptionStrikesValue strikes_value,
                          OptionContractType contract_type,
                          int include_quotes,
                          const char* from_date,
                          const char* to_date,
                          OptionExpMonth exp_month,
                          OptionType option_type,
                          char **buf,
                          size_t *n );
```

<br>

#### AccountInfoGetter

Account balances, positions, and orders. [TDAmeritrade docs.](https://developer.tdameritrade.com/account-access/apis/get/accounts/{accountId}-0)

##### [C++]

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
**types**
```
class AccountInfoGetter : public AccountGetterBase;
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
```
bool
AccountInfoGetter::returns_positions() const;
```
```
void
AccountInfoGetter::return_positions(bool positions);
```
```
bool
AccountInfoGetter::returns_orders() const
```
```
void
AccountInfoGetter::return_orders(bool orders);
```

##### [C]

**types**
```
struct AccountInfoGetter_C;
```

**functions**
```
static inline int
AccountInfoGetter_Create( struct Credentials *pcreds,
                          const char* account_id,
                          int positions,
                          int orders,
                          AccountInfoGetter_C *pgetter );
```
```
static inline int
AccountInfoGetter_Destroy( AccountInfoGetter_C *pgetter);
```
```
static inline int
AccountInfoGetter_Get(AccountInfoGetter *pgetter, char** buf, size_t *n);
```
```
static inline int
AccountInfoGetter_Close(AccountInfoGetter *pgetter);
```
```
static inline int
AccountInfoGetter_IsClosed(AccountInfoGetter_C *pgetter, int *b);
```
```
static inline int 
AccountInfoGetter_GetAccountId(AccountInfoGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int 
AccountInfoGetter_SetAccountId(AccountInfoGetter_C *pgetter, const char *symbol);
```
```
static inline int
AccountInfoGetter_ReturnsPositions( AccountInfoGetter_C *pgetter, 
                                    int *returns_positions );
```
```
static inline int
AccountInfoGetter_ReturnPositions( AccountInfoGetter_C *pgetter,
                                   int return_positions );
```
```
static inline int
AccountInfoGetter_ReturnsOrders( AccountInfoGetter_C *pgetter, 
                                 int *returns_orders );
```
```
static inline int
AccountInfoGetter_ReturnOrders( AccountInfoGetter_C *pgetter, 
                                int return_orders )
```
```
static inline int
GetAccountInfo( struct Credentials *pcreds,
                const char* account_id,
                int positions,
                int orders,
                char **buf,
                size_t *n );
```
<br>



#### PreferencesGetter

Account preferences. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/accounts/{accountId}/preferences-0)

##### [C++]

**constructors**
```
PreferencesGetter::PreferencesGetter( Credentials& creds, 
                                      const string& account_id );

    creds      ::  credentials struct received from RequestAccessToken 
                   / LoadCredentials / CredentialsManager.credentials
    account_id ::  id string of account to get preferences for
```
**types**
```
class PreferencesGetter : public AccountGetterBase;
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

##### [C]

**types**
```
struct PreferencesGetter_C;
```

**functions**
```
static inline int
PreferencesGetter_Create( struct Credentials *pcreds,
                          const char* account_id,
                          PreferencesGetter_C *pgetter );
```
```
static inline int
PreferencesGetter_Destroy( PreferencesGetter_C *pgetter );
```
```
static inline int
PreferencesGetter_Get( PreferencesGetter *pgetter, char** buf, size_t *n );
```
```
static inline int
PreferencesGetter_Close(PreferencesGetter *pgetter);
```
```
static inline int
PreferencesGetter_IsClosed(PreferencesGetter_C *pgetter, int *b);
```
```
static inline int 
PreferencesGetter_GetAccountId(PreferencesGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int 
PreferencesGetter_SetAccountId(PreferencesGetter_C *pgetter, const char *symbol);
```
```
static inline int
GetPreferences( struct Credentials *pcreds,
                const char* account_id,
                char **buf,
                size_t *n );
```
<br>

#### UserPrincipalsGetter

User Principal details. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/userprincipals-0)

##### [C++]
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
**types**
```
class UserPrincipalsGetter : public APIGetter;
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
void
UserPrincipalsGetter::return_streamer_subscription_keys(
    bool streamer_subscription_keys
);
```
```
bool 
UserPrincipalsGetter::returns_streamer_connection_info() const;
```
```
void
UserPrincipalsGetter::return_streamer_connection_info(
    bool streamer_connection_info
);
```
```
bool
UserPrincipalsGetter::returns_preferences() const;
```
```
void
UserPrincipalsGetter::return_preferences(bool preferences);
```
```
bool
UserPrincipalsGetter::returns_surrogate_ids() const;
```
```
void
UserPrincipalsGetter::return_surrogate_ids(bool surrogate_ids);
```

##### [C]

**types**
```
struct UserPrincipalsGetter_C;
```

**functions**
```
static inline int
UserPrincipalsGetter_Create( struct Credentials *pcreds,
                             int streamer_subscription_keys,
                             int streamer_connection_info,
                             int preferences,
                             int surrogate_ids,
                             UserPrincipalsGetter_C *pgetter);
```
```
static inline int
UserPrincipalsGetter_Destroy(UserPrincipalsGetter_C *pgetter);
```
```
static inline int
UserPrincipalsGetter_Get(UserPrincipalsGetter *pgetter, char** buf, size_t *n);
```
```
static inline int
UserPrincipalsGetter_Close(UserPrincipalsGetter *pgetter);
```
```
static inline int
UserPrincipalsGetter_IsClosed(UserPrincipalsGetter_C *pgetter, int *b);
```
```
static inline int
UserPrincipalsGetter_ReturnsSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                              int *returns_subscription_keys );
```
```
static inline int
UserPrincipalsGetter_ReturnSubscriptionKeys( UserPrincipalsGetter_C *pgetter,
                                             int return_subscription_keys);
```
```
static inline int
UserPrincipalsGetter_ReturnsConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                            int *returns_connection_info );
```
```
static inline int
UserPrincipalsGetter_ReturnConnectionInfo( UserPrincipalsGetter_C *pgetter,
                                           int return_connection_info );
```
```
static inline int
UserPrincipalsGetter_ReturnsPreferences( UserPrincipalsGetter_C *pgetter,
                                         int *returns_preferences);
```
```
static inline int
UserPrincipalsGetter_ReturnPreferences( UserPrincipalsGetter_C *pgetter,
                                        int return_preferences );
```
```
static inline int
UserPrincipalsGetter_ReturnsSurrogateIds( UserPrincipalsGetter_C *pgetter,
                                          int *returns_surrogate_ids );
```
```
static inline int
UserPrincipalsGetter_ReturnSurrogateIds(  UserPrincipalsGetter_C *pgetter,
                                          int return_surrogate_ids );
```
```
static inline int
GetUserPrincipals( struct Credentials *pcreds,
                   int streamer_subscription_keys,
                   int streamer_connection_info,
                   int preferences,
                   int surrogate_ids,
                   char **buf,
                   size_t *n );

```
<br>

#### StreamerSubscriptionKeysGetter

Subscription keys. [TDAmeritrade docs.](https://developer.tdameritrade.com/user-principal/apis/get/userprincipals/streamersubscriptionkeys-0)

##### [C++]

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
**types**
```
class StreamerSubscriptionKeysGetter : AccountGetterBase;
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

##### [C]

**types**
```
struct StreamerSubscriptionKeysGetter_C;
```

**functions**
```
static inline int
StreamerSubscriptionKeysGetter_Create( struct Credentials *pcreds,
                                       const char* account_id,
                                       StreamerSubscriptionKeysGetter_C *pgetter );
```
```
static inline int
StreamerSubscriptionKeysGetter_Destroy(StreamerSubscriptionKeysGetter_C *pgetter);
```
```
static inline int
StreamerSubscriptionKeysGetter_Get( StreamerSubscriptionKeysGetter_C *pgetter, 
                                    char** buf, 
                                    size_t *n );
```
```
static inline int
StreamerSubscriptionKeysGetter_Close(StreamerSubscriptionKeysGetter_C *pgetter);
```
```
static inline int
StreamerSubscriptionKeysGetter_IsClosed(
    StreamerSubscriptionKeysGetter_C *pgetter, 
    int *b
);
```
```
static inline int 
StreamerSubscriptionKeysGetter_GetAccountId(
    StreamerSubscriptionKeysGetter_C *pgetter, 
    char **buf, 
    size_t *n
);
```
```
static inline int 
StreamerSubscriptionKeysGetter_SetAccountId(
    StreamerSubscriptionKeysGetter_C *pgetter, 
    const char *symbol
);
```
```
static inline int
GetStreamerSubscriptionKeys( struct Credentials *pcreds,
                             const char* account_id,
                             char **buf,
                             size_t *n );

```
<br>


#### TransactionHistoryGetter

Transactions within a date range. [TDAmeritrade docs.](https://developer.tdameritrade.com/transaction-history/apis/get/accounts/{accountId}/transactions-0)

##### [C++]

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
class TransactionHistoryGetter : public AccountGetterBase;
```
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
void
AccountGetterBase::set_account_id(const string& account_id);
```
```
TransactionType
TransactionHistoryGetter::get_transaction_type() const;
```
```
void
TransactionHistoryGetter::set_transaction_type(TransactionType transaction_type);
```
```
string
TransactionHistoryGetter::get_symbol() const;
```
```
void
TransactionHistoryGetter::set_symbol(const string& symbol);
```
```
string
TransactionHistoryGetter::get_start_date() const;
```
```
void
TransactionHistoryGetter::set_start_date(const string& start_date);
```
```
string
TransactionHistoryGetter::get_end_date() const;
```
```
void
TransactionHistoryGetter::set_end_date(const string& end_date);
```

##### [C]

**types**
```
struct TransactionHistoryGetter_C;
```
```
enum TransactionType {
    TransactionType_all,
    TransactionType_trade,
    TransactionType_buy_only,
    TransactionType_sell_only,
    TransactionType_cash_in_or_cash_out,
    TransactionType_checking,
    TransactionType_dividend,
    TransactionType_interest,
    TransactionType_other,
    TransactionType_advisor_fees
};
```

**functions**
```
static inline int
TransactionHistoryGetter_Create( struct Credentials *pcreds,
                                 const char* account_id,
                                 TransactionType transaction_type,
                                 const char* symbol,
                                 const char* start_date,
                                 const char* end_date,
                                 TransactionHistoryGetter_C *pgetter );
```
```
static inline int
TransactionHistoryGetter_Destroy( TransactionHistoryGetter_C *pgetter );
```
```
static inline int
TransactionHistoryGetter_Get( TransactionHistoryGetter_C *pgetter, 
                              char** buf, 
                              size_t *n );
```
```
static inline int
TransactionHistoryGetter_Close(TransactionHistoryGetter_C *pgetter);
```
```
static inline int
TransactionHistoryGetter_IsClosed(TransactionHistoryGetter_C *pgetter,  int *b);
```
```
static inline int 
TransactionHistoryGetter_GetAccountId( TransactionHistoryGetter_C *pgetter, 
                                       char **buf, 
                                       size_t *n );
```
```
static inline int 
TransactionHistoryGetter_SetAccountId( TransactionHistoryGetter_C *pgetter, 
                                       const char *symbol );
```
```
static inline int
TransactionHistoryGetter_GetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType *transaction_type );
```
```
static inline int
TransactionHistoryGetter_SetTransactionType( TransactionHistoryGetter_C *pgetter,
                                             TransactionType transaction_type );
```
```
static inline int
TransactionHistoryGetter_GetSymbol( TransactionHistoryGetter_C *pgetter,
                                    char **buf,
                                    size_t *n );
```
```
static inline int
TransactionHistoryGetter_SetSymbol( TransactionHistoryGetter_C *pgetter,
                                    const char* symbol );
```
```
static inline int
TransactionHistoryGetter_GetStartDate( TransactionHistoryGetter_C *pgetter,
                                       char **buf,
                                       size_t *n );
```
```
static inline int
TransactionHistoryGetter_SetStartDate( TransactionHistoryGetter_C *pgetter,
                                       const char* start_date );
```
```
static inline int
TransactionHistoryGetter_GetEndDate( TransactionHistoryGetter_C *pgetter,
                                     char **buf,
                                     size_t *n );
```
```
static inline int
TransactionHistoryGetter_SetEndDate( TransactionHistoryGetter_C *pgetter,
                                     const char* end_date );
```
```
static inline int
GetTransactionHistory( struct Credentials *pcreds,
                       const char* account_id,
                       TransactionType transaction_type,
                       const char* symbol,
                       const char* start_date,
                       const char* end_date,
                       char **buf,
                       size_t *n );

```
<br>


#### IndividualTransactionHistoryGetter

Transactions by id. [TDAmeritrade docs.](https://developer.tdameritrade.com/transaction-history/apis/get/accounts/{accountId}/transactions/{transactionId}-0)

##### [C++]

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
**types**
```
class IndividualTransactionHistoryGetter : public AccountGetterBase;
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
```
string
IndividualTransactionHistoryGetter::get_transaction_id() const;
```
```
void
IndividualTransactionHistoryGetter::set_transaction_id(const string& transaction_id);
```

##### [C]

**types**
```
struct IndividualTransactionHistoryGetter_C;
```

**functions**
```
static inline int
IndividualTransactionHistoryGetter_Create(
    struct Credentials *pcreds,
    const char* account_id,
    const char* transaction_id,
    IndividualTransactionHistoryGetter_C *pgetter );
```
```
static inline int
IndividualTransactionHistoryGetter_Destroy(
    IndividualTransactionHistoryGetter_C *pgetter
    )
```
```
static inline int
IndividualTransactionHistoryGetter_Get( 
    IndividualTransactionHistoryGetter_C *pgetter, 
    char** buf, 
    size_t *n 
);
```
```
static inline int
IndividualTransactionHistoryGetter_Close(
    IndividualTransactionHistoryGetter_C *pgetter
);
```
```
static inline int
IndividualTransactionHistoryGetter_IsClosed(
    IndividualTransactionHistoryGetter_C *pgetter, 
    int *b
);
```
```
static inline int 
IndividualTransactionHistoryGetter_GetAccountId(
    IndividualTransactionHistoryGetter_C *pgetter, 
    char **buf, 
    size_t *n
);
```
```
static inline int 
IndividualTransactionHistoryGetter_SetAccountId(
    IndividualTransactionHistoryGetter_C *pgetter, 
    const char *symbol
);
```
```
static inline int
IndividualTransactionHistoryGetter_GetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    char **buf,
    size_t *n );
```
```
static inline int
IndividualTransactionHistoryGetter_SetTransactionId(
    IndividualTransactionHistoryGetter_C *pgetter,
    const char* transaction_id );
```
```
static inline int
GetIndividualTransactionHistory( struct Credentials *pcreds,
                                 const char* account_id,
                                 const char* transaction_id,
                                 char **buf,
                                 size_t *n );

```
<br>

#### InstrumentInfoGetter

Instrument information. [TDAmeritrade docs.](https://developer.tdameritrade.com/instruments/apis)

##### [C++]

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
class InstrumentInfoGetter : public APIGetter;
```
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

##### [C]

**types**
```
struct InstrumentInfoGetter_C;
```
```
enum InstrumentSearchType {
    InstrumentSearchType_symbol_exact,
    InstrumentSearchType_symbol_search,
    InstrumentSearchType_symbol_regex,
    InstrumentSearchType_description_search,
    InstrumentSearchType_description_regex,
    InstrumentSearchType_cusip
};
```
**functions**
```
static inline int
InstrumentInfoGetter_Create( struct Credentials *pcreds,
                             InstrumentSearchType search_type,
                             const char* query_string,
                             InstrumentInfoGetter_C *pgetter );
```
```
static inline int
InstrumentInfoGetter_Destroy(InstrumentInfoGetter_C *pgetter);
```
```
static inline int
InstrumentInfoGetter_Get(InstrumentInfoGetter_C *pgetter,  char** buf,  size_t *n);
```
```
static inline int
InstrumentInfoGetter_Close(InstrumentInfoGetter_C *pgetter);
```
```
static inline int
InstrumentInfoGetter_IsClosed(InstrumentInfoGetter_C *pgetter, int *b);
```
```
static inline int
InstrumentInfoGetter_GetSearchType( InstrumentInfoGetter_C *pgetter,
                                    InstrumentSearchType *search_type );
```
```
static inline int
InstrumentInfoGetter_GetQueryString( InstrumentInfoGetter_C *pgetter,
                                     char **buf,
                                     size_t *n );
```
```
static inline int
InstrumentInfoGetter_SetQuery( InstrumentInfoGetter_C *pgetter,
                               InstrumentSearchType search_type,
                               const char* query_string );
```
```
static inline int
GetInstrumentInfo( struct Credentials *pcreds,
                   InstrumentSearchType search_type,
                   const char* query_string,
                   char **buf,
                   size_t *n );

```
<br>


#### OrderGetter

Order information by order id. [TDAmeritrade docs.](https://developer.tdameritrade.com/account-access/apis/get/accounts/{accountId}/orders/{orderId}-0)

##### [C++]

**constructors**
```
OrderGetter::OrderGetter( Credentials& creds, 
                          const string& account_id 
                          const string& order_id  );

    creds       ::  credentials struct received from RequestAccessToken 
                    / LoadCredentials / CredentialsManager.credentials
    account_id  ::  id string of account to get order for
    order_id    ::  id string of order
```
**types**
```
class OrderGetter : public AccountGetterBase;
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
```
string
OrderGetter::get_order_id() const;
```
```
void
OrderGetter::set_order_id(const string& order_id);
```


##### [C]

**types**
```
struct OrderGetter_C;
```

**functions**
```
static inline int
OrderGetter_Create( struct Credentials *pcreds,
                    const char* account_id,
                    const char* order_id,
                    OrderGetter_C *pgetter );
```
```
static inline int
OrderGetter_Destroy(OrderGetter_C *pgetter);
```
```
static inline int
OrderGetter_Get(OrderGetter_C *pgetter,  char** buf, size_t *n);
```
```
static inline int
OrderGetter_Close(OrderGetter_C *pgetter);
```
```
static inline int
OrderGetter_IsClosed(OrderGetter_C *pgetter, int *b);
```
```
static inline int 
OrderGetter_GetAccountId(OrderGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int 
OrderGetter_SetAccountId(OrderGetter_C *pgetter, const char *symbol);
```
```
static inline int
OrderGetter_GetOrderId(OrderGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int
OrderGetter_SetOrderId(OrderGetter_C *pgetter, const char *order_id);
```
```
static inline int
GetOrder( struct Credentials *pcreds,
          const char* account_id,
          const char* order_id,
          char **buf,
          size_t *n );

```
<br>

#### OrdersGetter

Search for orders of a certain type, within a date/time range. [TDAmeritrade docs.](https://developer.tdameritrade.com/account-access/apis/get/accounts/{accountId}/orders-0)

##### [C++]

**constructors**
```
OrdersGetter::OrdersGetter( Credentials& creds, 
                            const string& account_id 
                            unsigned int nmax_results,
                            const std::string& from_entered_time,
                            const std::string& to_entered_time,
                            OrderStatusType order_status_type );

    creds             :: credentials struct received from RequestAccessToken 
                          / LoadCredentials / CredentialsManager.credentials
    account_id        :: id string of account to get orders for
    nmax_results      :: max number of (order) results to return
    from_entered_time :: time(<= 60 days ago) before which no orders will be returned*
    to_entered_time   :: time(<= 60 days ago) after which no orders will be returned*
    order_status_type :: status type of orders to return

    * yyyy-MM-dd or yyyy-MM-dd'T'HH:mm:ssz
```

**types**
```
class OrdersGetter : public AccountGetterBase;
```
```
enum class OrderStatusType : int{
    AWAITING_PARENT_ORDER,
    AWAITING_CONDITION,
    AWAITING_MANUAL_REVIEW,
    ACCEPTED,
    AWAITING_UR_OUT,
    PENDING_ACTIVATION,
    QUEUED,
    WORKING,
    REJECTED,
    PENDING_CANCEL,
    CANCELED,
    PENDING_REPLACE,
    REPLACED,
    FILLED,
    EXPIRED
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
void
AccountGetterBase::set_account_id(const string& account_id);
```
```
unsigned int
OrdersGetter::get_nmax_results() const;
```
```
void
OrdersGetter::set_nmax_results(unsigned int nmax_results);
```
```
string
OrdersGetter::get_from_entered_time() const;
```
```
void
OrdersGetter::set_from_entered_time(const std::string& from_entered_time);
```
```
string
OrdersGetter::get_to_entered_time() const;
```
```
void
OrdersGetter::set_to_entered_time(const std::string& to_entered_time);
```
```
OrderStatusType
OrdersGetter::get_order_status_type() const;
```
```
void
OrdersGetter::set_order_status_type(OrderStatusType order_status_type);
```

##### [C]

**types**
```
struct OrdersGetter_C;
```
```
enum OrderStatusType {
    OrderStatusType_AWAITING_PARENT_ORDER,
    OrderStatusType_AWAITING_CONDITION,
    OrderStatusType_AWAITING_MANUAL_REVIEW,
    OrderStatusType_ACCEPTED,
    OrderStatusType_AWAITING_UR_OUT,
    OrderStatusType_PENDING_ACTIVATION,
    OrderStatusType_QUEUED,
    OrderStatusType_WORKING,
    OrderStatusType_REJECTED,
    OrderStatusType_PENDING_CANCEL,
    OrderStatusType_CANCELED,
    OrderStatusType_PENDING_REPLACE,
    OrderStatusType_REPLACED,
    OrderStatusType_FILLED,
    OrderStatusType_EXPIRED
};

```
**functions**
```
static inline int
OrdersGetter_Create( struct Credentials *pcreds,
                     const char* account_id,
                     unsigned int nmax_results,
                     const char* from_entered_time,
                     const char* to_entered_time,
                     OrderStatusType order_status_type,
                     OrdersGetter_C *pgetter );
```
```
static inline int
OrdersGetter_Destroy(OrdersGetter_C *pgetter );
```
```
static inline int
OrdersGetter_Get(OrdersGetter_C *pgetter,  char** buf, size_t *n);
```
```
static inline int
OrdersGetter_Close(OrdersGetter_C *pgetter);
```
```
static inline int
OrdersGetter_IsClosed(OrdersGetter_C *pgetter, int *b);
```
```
static inline int 
OrdersGetter_GetAccountId(OrdersGetter_C *pgetter, char **buf, size_t *n);
```
```
static inline int 
OrdersGetter_SetAccountId(OrdersGetter_C *pgetter, const char *symbol);
```
```
static inline int
OrdersGetter_GetNMaxResults(OrdersGetter_C *pgetter, unsigned int *nmax_results);
```
```
static inline int
OrdersGetter_SetNMaxResults(OrdersGetter_C *pgetter, unsigned int nmax_results);
```
```
static inline int
OrdersGetter_GetFromEnteredTime(OrdersGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
OrdersGetter_SetFromEnteredTime( OrdersGetter_C *pgetter,
                                 const char* from_entered_time );
```
```
static inline int
OrdersGetter_GetToEnteredTime(OrdersGetter_C *pgetter, char** buf, size_t *n);
```
```
static inline int
OrdersGetter_SetToEnteredTime( OrdersGetter_C *pgetter,
                               const char* to_entered_time );
```
```
static inline int
OrdersGetter_GetOrderStatusType( OrdersGetter_C *pgetter,
                                 OrderStatusType *order_status_type );
```
```
static inline int
OrdersGetter_SetOrderStatusType( OrdersGetter_C *pgetter,
                                 OrderStatusType order_status_type );
```
```
static inline int
GetOrders( struct Credentials *pcreds,
           const char* account_id,
           unsigned int nmax_results,
           const char* from_entered_time,
           const char* to_entered_time,
           OrderStatusType order_status_type,
           char **buf,
           size_t *n );

```
<br>
