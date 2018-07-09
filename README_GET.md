### HTTPS Get Interface
- - -

- [Overview](#overview)
    - [Certificates](#certificates)
    - [Using Getter Objects](#using-getter-objects)
- [Throttling](#throttling)
- [Example Usage](#example-usage)
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

- *On Jun 30 2018 API behavior was changed (by Ameritrade) to fix a bug w/ expired token messages. This will result in an ```InvalidRequest``` exception being thrown with error code 401 in older versions of the library (commit eb59c1 and earlier). If so upgrade to a newer version of the library.*
- - - 

#### Overview

```
#include "tdma_api_get.h"

using namespace tdma;
```

The Get Interface consists of 'Getter' objects that derive from ```APIGetter```, and related convenience functions. ```APIGetter``` is built on ```conn::CurlConnection```: an object-oriented wrapper to libcurl  in curl_connect.h/cpp.

##### Certificates

Certificates are required to validate the hosts. If libcurl is built against the native ssl lib(e.g openssl on linux) or you use the pre-built dependencies for Windows(we built libcurl with -ENABLE_WINSSL) the default certificate store ***should*** take care of this for you. If this doesn't happen an ```APIExecutionException```  will be thrown with code CURLE_SSL_CACERT(60) and you'll have to use your own certificates via:
```
void
SetCertificateBundlePath(const std::string& path)

    path :: the path to a certificate bundle file (.pem)
```

There is a default 'cacert.pem' file in the base directory extracted from Firefox that you can use. 
(You can get updated versions from the [curl site](https://curl.haxx.se/docs/caextract.html).) The 
path of this file, *hard-coded during compilation*, can be found in the DEF_CERTIFICATE_BUNDLE_PATH string.

##### Using Getter Objects

1. construct a Getter object passing a reference to the Credentials struct and the 
relevant arguments.
2. use the ```.get()``` method which returns a json object from the server OR throws an exception.

```.get()``` can be called more than once for new data until its ```.close()``` method or destructor is called.

Each object has accessor methods for querying the fields passed to the constructor and updating 
those to be used in subsequent ```.get()``` call. 

If you only need to make the call once, each 'Getter' object has a similarly name 'Get' function
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
};

inline json
GetQuote(Credentials& creds, string symbol)
{ return QuoteGetter(creds, symbol).get(); }
```

#### Throttling

The API docs indicate a limit of two requests per second so we implement a throttling/blocking 
mechanism **across ALL Getter objects** with a default wait of 500 milliseconds. If, for instance, we use the default
wait and make five ```.get()``` calls in immediate succession the group of calls will take
~2500 milliseconds to complete. This wait time can be accessed with:
```
    static chrono::milliseconds
    APIGetter::get_wait_msec();
```
```    
    static void
    APIGetter::set_wait_msec(chrono::milliseconds msec);
```

This interface should not be used for streaming data, i.e. repeatedly making getter calls -  
use [StreamingSession](README_STREAMING.md) for that.

#### Example Usage
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


### Getter Classes
- - -


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
json 
APIGetter::get();
```
```
void 
APIGetter::close();
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
set<string> 
QuotesGetter::get_symbols() const;    
```
```
void
QuotesGetter::set_symbols(const set<string>& symbols);
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
enum class MarketType : unsigned int{
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
enum class MoversIndex : unsigned int{
    compx, /* $COMPX */
    dji, /* $DJI */
    spx /* SPX */
};
```
```
enum class MoversDirectionType : unsigned int{
    up,
    down,
    up_and_down
};
```
```
enum class MoversChangeType : unsigned int{
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
enum class PeriodType : unsigned int {
    day,
    month,
    year,
    ytd
};
```
```
enum class FrequencyType : unsigned int {
    minute,
    daily,
    weekly,
    monthly
};
```

**utilities**

***```VALID_PERIODS_BY_PERIOD_TYPE```*** - A mapping that provides *the* set of valid # of 
periods for a particular period type. (e.g ```PeriodType::day``` allows for 1,2,3,4,5, and 10 periods)
```
const unordered_map<PeriodType, set<unsigned int>, EnumHash<PeriodType>>
VALID_PERIODS_BY_PERIOD_TYPE = {
    {PeriodType::day, set<unsigned int>{1,2,3,4,5,10}},
    {PeriodType::month, set<unsigned int>{1,2,3,6}},
    {PeriodType::year, set<unsigned int>{1,2,3,5,10,15,20}},
    {PeriodType::ytd, set<unsigned int>{1}},
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
const unordered_map<FrequencyType, set<unsigned int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<unsigned int>{1,5,10,30}},
    {FrequencyType::daily, set<unsigned int>{1}},
    {FrequencyType::weekly, set<unsigned int>{1}},
    {FrequencyType::monthly, set<unsigned int>{1}},
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
HistoricalPeriodGetter::set_frequency( FrequencyType frequency_type,
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
enum class FrequencyType : unsigned int {
    minute,
    daily,
    weekly,
    monthly
};
```

**utilities**

***```VALID_FREQUENCIES_BY_FREQUENCY_TYPE```*** - A mapping that provides *the* set of valid 
frequency amounts for a particular frequency type. (e.g ```FrequencyType::daily``` only allows for a frequency amount of 1, in other words: you can get data over some period one day at a time, but not 2,3 etc. days at a time)
```
const unordered_map<FrequencyType, set<unsigned int>, EnumHash<FrequencyType>>
VALID_FREQUENCIES_BY_FREQUENCY_TYPE = {
    {FrequencyType::minute, set<unsigned int>{1,5,10,30}},
    {FrequencyType::daily, set<unsigned int>{1}},
    {FrequencyType::weekly, set<unsigned int>{1}},
    {FrequencyType::monthly, set<unsigned int>{1}},
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
HistoricalRangeGetter::set_frequency( FrequencyType frequency_type,
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
enum class OptionContractType : unsigned int {
    call,
    put,
    all
};
```
```
enum class OptionRangeType : unsigned int {
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
enum class OptionExpMonth : unsigned int {
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
enum class OptionType : unsigned int {
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*(See 'utilities' for an explanation of ```OptionStrikes```.)*
```
class OptionStrikes {
public:
    enum class Type : unsigned int {
        n_atm,
        single,
        range
    };
private:
    union {
        unsigned int _n_atm;
        double _single;
        OptionRangeType _range;
    };
    Type _type;
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
user must use the correct call (first use get_type() then call the correct method).
```
Type
OptionStrikes::get_type() const;
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
enum class OptionContractType : unsigned int {
    call,
    put,
    all
};
```
```
enum class OptionStrategyType : unsigned int { 
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
enum class OptionRangeType : unsigned int {
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
enum class OptionExpMonth : unsigned int {
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
enum class OptionType : unsigned int {
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*(See 'utilities' for an explanation of ```OptionStrikes```.)*
```
class OptionStrikes {
public:
    enum class Type : unsigned int {
        n_atm,
        single,
        range
    };
private:
    union {
        unsigned int _n_atm;
        double _single;
        OptionRangeType _range;
    };
    Type _type;
    ...
};
```

*(See 'utilities' for an explanation of ```OptionStrategy```)*
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

The following instance methods can be used to get information about the instance but 
it's the user's responsibility to use the correct method (first use get_type() 
then call the corresponding method).
```
Type
OptionStrikes::get_type() const;
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
enum class OptionContractType : unsigned int {
    call,
    put,
    all
};
```
```
enum class OptionRangeType : unsigned int {
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
enum class OptionExpMonth : unsigned int {
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
enum class OptionType : unsigned int {
    s, /* standard */
    ns, /* non-standard */
    all
};
```

*(See 'utilities' for an explanation of ```OptionStrikes```.)*
```
class OptionStrikes {
public:
    enum class Type : unsigned int {
        n_atm,
        single,
        range
    };
private:
    union {
        unsigned int _n_atm;
        double _single;
        OptionRangeType _range;
    };
    Type _type;
    ...
};
```

**utilities**

**```OptionStrikes```** - Object used to represent one of three distinct strike 
types and related values. Create the desired type with its static method:

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

The following instance methods can be used to get information about the instance but 
it's the user's responsibility to use the correct method (first use get_type() 
then call the corresponding method).
```
Type
OptionStrikes::get_type() const;
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
enum class TransactionType : unsigned int {
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
enum class InstrumentSearchType : unsigned int{
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

