#include "tdma_api_get.h"

#include "test.h"

#include <time.h>

int
Test_QuoteGetter(struct Credentials *creds);

int
Test_QuotesGetter(struct Credentials *creds);

int
Test_MarketHoursGetter(struct Credentials *creds);

int
Test_MoversGetter(struct Credentials *creds);

int
Test_HistoricalPeriodGetter(struct Credentials *creds);

int
Test_HistoricalRangeGetter(struct Credentials *creds);

int
Test_OptionChainGetter(struct Credentials *creds);

int
Test_OptionChainStrategyGetter(struct Credentials *creds);

int
Test_OptionChainAnalyticalGetter(struct Credentials *creds);

int
Test_AccountInfoGetter(struct Credentials *creds, const char* acct);

int
Test_PreferencesGetter(struct Credentials *creds, const char* acct);

int
Test_StreamerSubscriptionKeysGetter(struct Credentials *creds, const char* acct);

int
Test_TransactionHistoryGetter(struct Credentials *creds, const char* acct);

int
Test_IndividualTransactionHistoryGetter(struct Credentials *creds, const char* acct);

int
Test_UserPrincipalsGetter(struct Credentials *creds);

int
Test_InstrumentInfoGetter(struct Credentials *creds);

int
Test_OrderGetters(struct Credentials *creds, const char* acct);

int
Test_Getters(struct Credentials *creds, const char* acct, long wait)
{
    int err = 0;
    unsigned long long w = 0, w2 =0;

    if( (err = APIGetter_GetDefWaitMSec(&w)) )
        CHECK_AND_RETURN_ON_ERROR(err, "APIGetter_GetDefWaitMSec");

    printf( "default wait sec: %llu \n", w);

    if( (err = APIGetter_GetWaitMSec(&w)) )
        CHECK_AND_RETURN_ON_ERROR(err, "APIGetter_GetWaitMSec");

    if( (err = APIGetter_SetWaitMSec( 1500 )) )
        CHECK_AND_RETURN_ON_ERROR(err, "APIGetter_SetWaitMSec");

    if( (err = APIGetter_GetWaitMSec(&w2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "APIGetter_GetWaitMSec");

    printf( "change wait sec: %llu --> %llu \n", w, w2 );

    err = Test_QuoteGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_QuotesGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_MoversGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_MarketHoursGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_HistoricalPeriodGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_HistoricalRangeGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_OptionChainGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_OptionChainStrategyGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_OptionChainAnalyticalGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_AccountInfoGetter(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_PreferencesGetter(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_StreamerSubscriptionKeysGetter(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_TransactionHistoryGetter(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_IndividualTransactionHistoryGetter(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_UserPrincipalsGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_InstrumentInfoGetter(creds);
    if( err )
        return err;
    SleepFor(wait);

    err = Test_OrderGetters(creds, acct);
    if( err )
        return err;
    SleepFor(wait);

    return err;
}

/* include/_get.h */
const int TYPE_ID_GETTER_QUOTE = 1;
const int TYPE_ID_GETTER_QUOTES = 2;
const int TYPE_ID_GETTER_MARKET_HOURS = 3;
const int TYPE_ID_GETTER_MOVERS = 4;
const int TYPE_ID_GETTER_HISTORICAL_PERIOD = 5;
const int TYPE_ID_GETTER_HISTORICAL_RANGE = 6;
const int TYPE_ID_GETTER_OPTION_CHAIN = 7;
const int TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY = 8;
const int TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL = 9;
const int TYPE_ID_GETTER_ACCOUNT_INFO = 10;
const int TYPE_ID_GETTER_PREFERENCES = 11;
const int TYPE_ID_GETTER_SUBSCRIPTION_KEYS = 12;
const int TYPE_ID_GETTER_TRANSACTION_HISTORY = 13;
const int TYPE_ID_GETTER_IND_TRANSACTION_HISTORY = 14;
const int TYPE_ID_GETTER_ORDER = 15;
const int TYPE_ID_GETTER_ORDERS = 16;
const int TYPE_ID_GETTER_USER_PRINCIPALS = 17;
const int TYPE_ID_GETTER_INSTRUMENT_INFO = 18;

int
Test_QuoteGetter(struct Credentials* creds)
{
     int err = 0;
     char *buf = NULL, *symbol = NULL;
     size_t n = 0;

     QuoteGetter_C qg;
     memset(&qg, 0, sizeof(QuoteGetter_C));

    if( (err = QuoteGetter_Create(creds, "SPY", &qg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuoteGetter_Create");

    if( qg.type_id != TYPE_ID_GETTER_QUOTE ){
        fprintf(stderr, "invalid type id (%i,%i) \n", qg.type_id,
                 TYPE_ID_GETTER_QUOTE);
        return -1;
    }

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
    return 0;
}

int
Test_QuotesGetter(struct Credentials* creds)
{
    int err = 0;
    char *buf = NULL;
    size_t nsymbols = 0;
    size_t ndata = 0;

    QuotesGetter_C qsg;
    memset(&qsg, 0, sizeof(QuotesGetter_C));

    const char* symbols_in[] = {"SPY", "QQQ"};
    const char* symbols_in2[] = {"EEM", "QQQ", "GOOG"};
    char **symbols_out;

    if( (err = QuotesGetter_Create(creds, symbols_in, 2, &qsg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_Create");

    if( qsg.type_id != TYPE_ID_GETTER_QUOTES ){
        fprintf(stderr, "invalid type id (%i,%i) \n", qsg.type_id,
                 TYPE_ID_GETTER_QUOTES);
        return -1;
    }
    if( (err = QuotesGetter_GetSymbols(&qsg, &symbols_out, &nsymbols)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_GetSymbol");


    if( (err = QuotesGetter_Get(&qsg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_Get");


    if( symbols_out ){
        printf("Symbols: ");
        int i;
        for(i = 0; i < nsymbols; ++ i){
            if(symbols_out[i]){
                printf("%s ", symbols_out[i]);
                free(symbols_out[i]);
            }
        }
        printf("\n");
        free(symbols_out);
        symbols_out = NULL;
    }

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }


    if( (err = QuotesGetter_SetSymbols(&qsg, symbols_in2, 3)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_SetSymbols");

    if( (err = QuotesGetter_Get(&qsg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_Get");

    if( (err = QuotesGetter_GetSymbols(&qsg, &symbols_out, &nsymbols)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_GetSymbols");

    if( symbols_out ){
        printf("Symbols: ");
        int i;
        for(i = 0; i < nsymbols; ++ i){
            if(symbols_out[i]){
                printf("%s ", symbols_out[i]);
                free(symbols_out[i]);
            }
        }
        printf("\n");
        free(symbols_out);
        symbols_out = NULL;
    }

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = QuotesGetter_RemoveSymbols(&qsg, symbols_in2, 3)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_RemoveSymbols");

    if( (err = QuotesGetter_AddSymbols(&qsg, symbols_in, 2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_AddSymbols");

    if( (err = QuotesGetter_GetSymbols(&qsg, &symbols_out, &nsymbols)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_GetSymbols");

    if( symbols_out ){
        printf("Symbols: ");
        int i;
        for(i = 0; i < nsymbols; ++ i){
            if(symbols_out[i]){
                printf("%s ", symbols_out[i]);
                free(symbols_out[i]);
            }
        }
        printf("\n");
        free(symbols_out);
        symbols_out = NULL;
    }

    if( (err = QuotesGetter_Get(&qsg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    return 0;
}

int
Test_MarketHoursGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    MarketType mt;
    size_t ndata = 0;
    char* str;
    size_t n;

    MarketHoursGetter_C mhg;
    memset(&mhg, 0, sizeof(MarketHoursGetter_C));

    if( (err = MarketHoursGetter_Create(creds, MarketType_bond, "2019-07-04", &mhg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_Create");

    if( mhg.type_id != TYPE_ID_GETTER_MARKET_HOURS ){
        fprintf(stderr, "invalid type id (%i,%i) \n", mhg.type_id,
                 TYPE_ID_GETTER_MARKET_HOURS);
        return -1;
    }
    if( (err = MarketHoursGetter_GetMarketType(&mhg, &mt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_GetMarketType");

    MarketType_to_string(mt, &str, &n);
    if( str ){
        printf("MarketType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = MarketHoursGetter_GetDate(&mhg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_GetDate");

    if( str ){
        printf("Date: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = MarketHoursGetter_Get(&mhg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = MarketHoursGetter_SetMarketType(&mhg, MarketType_equity) ))
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_SetMarketType");

    if( (err = MarketHoursGetter_SetDate(&mhg, "2019-07-05") ))
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_SetDate");

    if( (err = MarketHoursGetter_GetMarketType(&mhg, &mt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_GetMarketType");

    MarketType_to_string(mt, &str, &n);
    if( str ){
        printf("MarketType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = MarketHoursGetter_GetDate(&mhg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_GetDate");

    if( str ){
        printf("Date: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = MarketHoursGetter_Get(&mhg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MarketHoursGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }


    return 0;
}

int
Test_MoversGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    MoversIndex mi;
    MoversDirectionType mdt;
    MoversChangeType mct;
    size_t ndata = 0;

    MoversGetter_C mg;
    memset(&mg, 0, sizeof(MoversGetter_C));

    if( (err = MoversGetter_Create(creds, MoversIndex_spx, MoversDirectionType_up,
                                   MoversChangeType_value, &mg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_Create");

    if( mg.type_id != TYPE_ID_GETTER_MOVERS ){
        fprintf(stderr, "invalid type id (%i,%i) \n", mg.type_id,
                 TYPE_ID_GETTER_MOVERS);
        return -1;
    }

    if( (err = MoversGetter_GetIndex(&mg, &mi)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetIndex");

    if( (err = MoversGetter_GetDirectionType(&mg, &mdt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetDirectionType");

    if( (err = MoversGetter_GetChangeType(&mg, &mct)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetChangeType");

    char* str;
    size_t n;
    MoversIndex_to_string(mi, &str, &n);
    if( str ){
        printf("MoversIndex: %s \n", str);
        free(str);
        str = NULL;
    }

    MoversDirectionType_to_string(mdt, &str, &n);
    if( str ){
        printf("MoversDirectionType: %s \n", str);
        free(str);
        str = NULL;
    }

    MoversChangeType_to_string(mct, &str, &n);
    if( str ){
        printf("MoversChangeType: %s \n", str);
        free(str);
        str = NULL;
    }


    if( (err = MoversGetter_Get(&mg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = MoversGetter_SetIndex(&mg, MoversIndex_compx)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_SetIndex");

    if( (err = MoversGetter_SetDirectionType(&mg, MoversDirectionType_up_and_down)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_SetDirectionType");

    if( (err = MoversGetter_SetChangeType(&mg, MoversChangeType_percent)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_SetChangeType");

    if( (err = MoversGetter_GetIndex(&mg, &mi)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetIndex");

    if( (err = MoversGetter_GetDirectionType(&mg, &mdt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetDirectionType");

    if( (err = MoversGetter_GetChangeType(&mg, &mct)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_GetChangeType");

    MoversIndex_to_string(mi, &str, &n);
    if( str ){
        printf("MoversIndex: %s \n", str);
        free(str);
        str = NULL;
    }

    MoversDirectionType_to_string(mdt, &str, &n);
    if( str ){
        printf("MoversDirectionType: %s \n", str);
        free(str);
        str = NULL;
    }

    MoversChangeType_to_string(mct, &str, &n);
    if( str ){
        printf("MoversChangeType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = MoversGetter_Get(&mg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "MoversGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    return 0;
}

int
Test_HistoricalPeriodGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;

    HistoricalPeriodGetter_C hpg;
    memset(&hpg, 0, sizeof(HistoricalPeriodGetter_C));

    PeriodType pt = PeriodType_day;
    unsigned int period = VALID_PERIODS_BY_PERIOD_TYPE[pt][0];
    FrequencyType ft = VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][0];
    unsigned int freq = VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][1];

    if( (err = HistoricalPeriodGetter_Create(creds, "SPY", pt, period, ft,
                                             freq, 1, &hpg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_Create");


    if( hpg.type_id != TYPE_ID_GETTER_HISTORICAL_PERIOD ){
        fprintf(stderr, "invalid type id (%i,%i) \n", hpg.type_id,
                 TYPE_ID_GETTER_HISTORICAL_PERIOD);
        return -1;
    }

    PeriodType pt2;
    unsigned int period2;
    FrequencyType ft2;
    unsigned int freq2;
    char *symbol;
    size_t n;
    int is_ext;

    if( (err = HistoricalPeriodGetter_GetSymbol(&hpg, &symbol, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetSymbol");

    if( (err = HistoricalPeriodGetter_GetFrequency(&hpg, &freq2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetFrequency");

    if( (err = HistoricalPeriodGetter_GetFrequencyType(&hpg, &ft2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetFrequencyType");

    if( (err = HistoricalPeriodGetter_IsExtendedHours(&hpg, &is_ext)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_IsExtendedHours");

    if( (err = HistoricalPeriodGetter_GetPeriodType(&hpg, &pt2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetPeriodType");

    if( (err = HistoricalPeriodGetter_GetPeriod(&hpg, &period2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetPeriod");

    if( symbol ){
        printf("Symbol: %s \n", symbol);
        free(symbol);
        symbol = NULL;
    }

    char* str;
    PeriodType_to_string(pt2, &str, &n);
    if( str ){
        printf("PeriodType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Period: %i \n", period2);

    FrequencyType_to_string(ft2, &str, &n);
    if( str ){
        printf("FrequencyType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Frequency: %i \n", freq2);
    printf("Extended Hours: %i \n", is_ext);


    if( (err = HistoricalPeriodGetter_Get(&hpg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    pt = PeriodType_month;
    period = VALID_PERIODS_BY_PERIOD_TYPE[pt][3];
    ft = VALID_FREQUENCY_TYPES_BY_PERIOD_TYPE[pt][0];
    freq = VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][0];

    if( (err = HistoricalPeriodGetter_SetSymbol(&hpg, "QQQ")) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_SetSymbol");

    if( (err = HistoricalPeriodGetter_SetFrequency(&hpg, ft, freq)) ) //
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_SetFrequency");

    // test for error on SET
    if( HistoricalPeriodGetter_SetFrequency(&hpg, ft, 99) == 0){
        fprintf(stderr, "HistoricalPeriodGetter_SetFrequency did not fail (%i, %i) \n",
                ft, 99);
        return -1;
    }

    if( (err = HistoricalPeriodGetter_SetPeriod(&hpg, pt, period)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_SetPeriod");

    if( (err = HistoricalPeriodGetter_SetExtendedHours(&hpg, 0)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_SetExtendedHours");

    /* */

    if( (err = HistoricalPeriodGetter_GetSymbol(&hpg, &symbol, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetSymbol");

    if( (err = HistoricalPeriodGetter_GetFrequency(&hpg, &freq2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetFrequency");

    if( (err = HistoricalPeriodGetter_GetFrequencyType(&hpg, &ft2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetFrequencyType");

    if( (err = HistoricalPeriodGetter_IsExtendedHours(&hpg, &is_ext)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_IsExtendedHours");

    if( (err = HistoricalPeriodGetter_GetPeriodType(&hpg, &pt2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetPeriodType");

    if( (err = HistoricalPeriodGetter_GetPeriod(&hpg, &period2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_GetPeriod");

    if( symbol ){
        printf("Symbol: %s \n", symbol);
        free(symbol);
        symbol = NULL;
    }

    PeriodType_to_string(pt2, &str, &n);
    if( str ){
        printf("PeriodType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Period: %i \n", period2);

    FrequencyType_to_string(ft2, &str, &n);
    if( str ){
        printf("FrequencyType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Frequency: %i \n", freq2);
    printf("Extended Hours: %i \n", is_ext);


    if( (err = HistoricalPeriodGetter_Get(&hpg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalPeriodGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    // sets error state but shouldn't fail until GET
    if( (err = HistoricalPeriodGetter_SetFrequency(&hpg, FrequencyType_minute, 1))){
        fprintf(stderr, "HistoricalPeriodGetter_SetFrequency should not have "
                 "failed until GET (%i) \n", err);
        return -1;
    }

    if( HistoricalPeriodGetter_Get(&hpg, &buf, &ndata) != TDMA_API_VALUE_ERROR ){
        fprintf(stderr, "HistoricalPeriodGetter_Get did not return value error. \n");
        if( buf ){
            free(buf);
            buf = NULL;
        }
        return -1;
    }


    return 0;
}


int
Test_HistoricalRangeGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;

    HistoricalRangeGetter_C hpg;
    memset(&hpg, 0, sizeof(HistoricalRangeGetter_C));

    FrequencyType ft = FrequencyType_minute;
    unsigned int freq = VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][3];
    unsigned long long end = 1528205400000;
    unsigned long long start = 1528119000000;

    if( (err = HistoricalRangeGetter_Create(creds, "SPY",ft,
                                             freq, start, end, 1, &hpg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_Create");


    if( hpg.type_id != TYPE_ID_GETTER_HISTORICAL_RANGE ){
        fprintf(stderr, "invalid type id (%i,%i) \n", hpg.type_id,
                 TYPE_ID_GETTER_HISTORICAL_RANGE);
        return -1;
    }

    FrequencyType ft2;
    unsigned int freq2;
    char *symbol;
    size_t n;
    int is_ext;
    unsigned long long end2, start2;

    if( (err = HistoricalRangeGetter_GetSymbol(&hpg, &symbol, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetSymbol");

    if( (err = HistoricalRangeGetter_GetFrequency(&hpg, &freq2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetFrequency");

    if( (err = HistoricalRangeGetter_GetFrequencyType(&hpg, &ft2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetFrequencyType");

    if( (err = HistoricalRangeGetter_IsExtendedHours(&hpg, &is_ext)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_IsExtendedHours");

    if( (err = HistoricalRangeGetter_GetStartMSecSinceEpoch(&hpg, &start2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetStartMSecSinceEpoch");

    if( (err = HistoricalRangeGetter_GetEndMSecSinceEpoch(&hpg, &end2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetEndMSecSinceEpoch");

    if( symbol ){
        printf("Symbol: %s \n", symbol);
        free(symbol);
        symbol = NULL;
    }

    char* str;
    FrequencyType_to_string(ft2, &str, &n);
    if( str ){
        printf("FrequencyType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Frequency: %i \n", freq2);
    printf("Extended Hours: %i \n", is_ext);
    printf("Start MSec: %llu \n", start2);
    printf("End MSec: %llu \n", end2);


    if( (err = HistoricalRangeGetter_Get(&hpg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }


    ft = FrequencyType_daily;
    freq = VALID_FREQUENCIES_BY_FREQUENCY_TYPE[ft][0];
    start -= (10 * 86400000);
    end -= 86400000;

    if( (err = HistoricalRangeGetter_SetSymbol(&hpg, "QQQ")) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_SetSymbol");

    if( (err = HistoricalRangeGetter_SetFrequency(&hpg, ft, freq)) ) //
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_SetFrequency");

    // test for error on SET
    if( HistoricalRangeGetter_SetFrequency(&hpg, ft, 99) == 0){
        fprintf(stderr, "SetFrequency did not fail as expected (%i, %i) \n",
                ft, 99);
        return -1;
    }

    if( (err = HistoricalRangeGetter_SetExtendedHours(&hpg, 0)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_SetExtendedHours");

    if( (err = HistoricalRangeGetter_SetStartMSecSinceEpoch(&hpg, start)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_SetStartMSecSinceEpoch");

    if( (err = HistoricalRangeGetter_SetEndMSecSinceEpoch(&hpg, end)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_SetEndMSecSinceEpoch");

    /* */

    if( (err = HistoricalRangeGetter_GetSymbol(&hpg, &symbol, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetSymbol");

    if( (err = HistoricalRangeGetter_GetFrequency(&hpg, &freq2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetFrequency");

    if( (err = HistoricalRangeGetter_GetFrequencyType(&hpg, &ft2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetFrequencyType");

    if( (err = HistoricalRangeGetter_IsExtendedHours(&hpg, &is_ext)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_IsExtendedHours");

    if( (err = HistoricalRangeGetter_GetStartMSecSinceEpoch(&hpg, &start2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetStartMSecSinceEpoch");

    if( (err = HistoricalRangeGetter_GetEndMSecSinceEpoch(&hpg, &end2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "HistoricalRangeGetter_GetEndMSecSinceEpoch");

    if( symbol ){
        printf("Symbol: %s \n", symbol);
        free(symbol);
        symbol = NULL;
    }

    FrequencyType_to_string(ft2, &str, &n);
    if( str ){
        printf("FrequencyType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Frequency: %i \n", freq2);
    printf("Extended Hours: %i \n", is_ext);
    printf("Start MSec: %llu \n", start2);
    printf("End MSec: %llu \n", end2);


    // not exactly sure why this is rejected by server
    if( (err = HistoricalRangeGetter_Get(&hpg, &buf, &ndata)) ){
        fprintf(stderr, "HistoricalRangeGetter_Get ERROR(%i)", err);
    }

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    return 0;
}

int
Test_OptionChainGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;
    char* str;
    size_t n;

    OptionChainGetter_C ocg;
    memset(&ocg, 0, sizeof(OptionChainGetter_C));

    OptionStrikesType ost = OptionStrikesType_single;
    OptionStrikesValue osv; osv.single = 70.00;
    OptionContractType oct = OptionContractType_call;
    int includes_quotes = 1;
    const char* from_date = "2018-07-24";
    const char* to_date = "2018-09-24";
    OptionExpMonth oep = OptionExpMonth_aug;
    OptionType ot = OptionType_s;


    if( (err = OptionChainGetter_Create(creds, "KORS", ost, osv, oct,
                                        includes_quotes, from_date, to_date,
                                        oep, ot, &ocg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_Create");


    if( ocg.type_id != TYPE_ID_GETTER_OPTION_CHAIN ){
        fprintf(stderr, "invalid type id (%i,%i) \n", ocg.type_id,
                 TYPE_ID_GETTER_OPTION_CHAIN);
        return -1;
    }

    OptionStrikesType ost2;
    OptionStrikesValue osv2;
    OptionContractType oct2;
    int includes_quotes2;
    char* from_date2;
    char* to_date2;
    OptionExpMonth oep2;
    OptionType ot2;

    if( (err = OptionChainGetter_GetSymbol(&ocg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetSymbol");

    if( (err = OptionChainGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetStrikesInfo");

    if( (err = OptionChainGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetContractType");

    if( (err = OptionChainGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_IncludesQuotes");

    if( (err = OptionChainGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetFromeDate");

    if( (err = OptionChainGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetToDate");

    if( (err = OptionChainGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetExpMonth");

    if( (err = OptionChainGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %lf \n", osv2.single);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = OptionChainGetter_Get(&ocg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    ost = OptionStrikesType_n_atm;
    osv.n_atm =  2;
    oct = OptionContractType_all;
    includes_quotes = 0;
    oep = OptionExpMonth_jan;
    ot = OptionType_all;

    if( (err = OptionChainGetter_SetSymbol(&ocg, "SPY")) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetSymbol");

    if( (err = OptionChainGetter_SetStrikes(&ocg, ost, osv)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetStrikesInfo");

    if( (err = OptionChainGetter_SetContractType(&ocg, oct)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetContractType");

    if( (err = OptionChainGetter_IncludeQuotes(&ocg, includes_quotes)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_IncludeQuotes");

    if( (err = OptionChainGetter_SetFromDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetFromeDate");

    if( (err = OptionChainGetter_SetToDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetToDate");

    if( (err = OptionChainGetter_SetExpMonth(&ocg, oep)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetExpMonth");

    if( (err = OptionChainGetter_SetOptionType(&ocg, ot)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_SetOptionType");

    if( (err = OptionChainGetter_GetSymbol(&ocg, &str, &n)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetSymbol");

    if( (err = OptionChainGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetStrikesInfo");

    if( (err = OptionChainGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetContractType");

    if( (err = OptionChainGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_IncludesQuotes");

    if( (err = OptionChainGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetFromeDate");

    if( (err = OptionChainGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetToDate");

    if( (err = OptionChainGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetExpMonth");

    if( (err = OptionChainGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %i \n", osv2.n_atm);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = OptionChainGetter_Get(&ocg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = OptionChainGetter_Destroy(&ocg) ))
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainGetter_Destroy");

    return 0;
}


int
Test_OptionChainStrategyGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;
    char* str;
    size_t n;

    OptionChainStrategyGetter_C ocg;
    memset(&ocg, 0, sizeof(OptionChainStrategyGetter_C));

    OptionStrategyType ostrat = OptionStrategyType_vertical;
    double ospread = 1.00;
    OptionStrikesType ost = OptionStrikesType_single;
    OptionStrikesValue osv; osv.single = 70.00;
    OptionContractType oct = OptionContractType_call;
    int includes_quotes = 1;
    const char* from_date = "2018-07-24";
    const char* to_date = "2018-09-24";
    OptionExpMonth oep = OptionExpMonth_aug;
    OptionType ot = OptionType_s;


    if( (err = OptionChainStrategyGetter_Create(creds, "KORS", ostrat, ospread,
                                                ost, osv, oct, includes_quotes,
                                                from_date, to_date, oep, ot, &ocg)) ){
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_Create");
    }


    if( ocg.type_id != TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY ){
        fprintf(stderr, "invalid type id (%i,%i) \n", ocg.type_id,
                 TYPE_ID_GETTER_OPTION_CHAIN_STRATEGY);
        return -1;
    }

    OptionStrategyType ostrat2;
    double ospread2;
    OptionStrikesType ost2;
    OptionStrikesValue osv2;
    OptionContractType oct2;
    int includes_quotes2;
    char* from_date2;
    char* to_date2;
    OptionExpMonth oep2;
    OptionType ot2;

    if( (err = OptionChainStrategyGetter_GetSymbol(&ocg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetSymbol");

    if( (err = OptionChainStrategyGetter_GetStrategy(&ocg, &ostrat2, &ospread2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetStrategy");

    if( (err = OptionChainStrategyGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetStrikesInfo");

    if( (err = OptionChainStrategyGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetContractType");

    if( (err = OptionChainStrategyGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_IncludesQuotes");

    if( (err = OptionChainStrategyGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetFromeDate");

    if( (err = OptionChainStrategyGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetToDate");

    if( (err = OptionChainStrategyGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetExpMonth");

    if( (err = OptionChainStrategyGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionStrategyType_to_string(ostrat2, &str, &n);
    if( str ){
        printf("OptionStrategyType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("Strategy Spread: %lf \n", ospread2);

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %lf \n", osv2.single);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    //if( (err = OptionChainStrategyGetter_Get(&ocg, &buf, &ndata)) )
    //    CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    ostrat = OptionStrategyType_calendar;
    ospread = 0.0; // TODO *****************************************
    ost = OptionStrikesType_n_atm;
    osv.n_atm =  2;
    oct = OptionContractType_all;
    includes_quotes = 0;
    oep = OptionExpMonth_jan;
    ot = OptionType_all;

    if( (err = OptionChainStrategyGetter_SetSymbol(&ocg, "SPY")) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetSymbol");

    if( (err = OptionChainStrategyGetter_SetStrategy(&ocg, ostrat, ospread)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetStrategy");

    if( (err = OptionChainStrategyGetter_SetStrikes(&ocg, ost, osv)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetStrikesInfo");

    if( (err = OptionChainStrategyGetter_SetContractType(&ocg, oct)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetContractType");

    if( (err = OptionChainStrategyGetter_IncludeQuotes(&ocg, includes_quotes)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_IncludeQuotes");

    if( (err = OptionChainStrategyGetter_SetFromDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetFromeDate");

    if( (err = OptionChainStrategyGetter_SetToDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetToDate");

    if( (err = OptionChainStrategyGetter_SetExpMonth(&ocg, oep)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetExpMonth");

    if( (err = OptionChainStrategyGetter_SetOptionType(&ocg, ot)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_SetOptionType");

    if( (err = OptionChainStrategyGetter_GetSymbol(&ocg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetSymbol");

    if( (err = OptionChainStrategyGetter_GetStrategy(&ocg, &ostrat2, &ospread2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetStrategy");

    if( (err = OptionChainStrategyGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetStrikesInfo");

    if( (err = OptionChainStrategyGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetContractType");

    if( (err = OptionChainStrategyGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_IncludesQuotes");

    if( (err = OptionChainStrategyGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetFromeDate");

    if( (err = OptionChainStrategyGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetToDate");

    if( (err = OptionChainStrategyGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetExpMonth");

    if( (err = OptionChainStrategyGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionStrategyType_to_string(ostrat2, &str, &n);
    if( str ){
        printf("OptionStrategyType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("Strategy Spread: %lf \n", ospread2);

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %i \n", osv2.n_atm);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = OptionChainStrategyGetter_Get(&ocg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = OptionChainStrategyGetter_Destroy(&ocg) ))
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainStrategyGetter_Destroy");

    return 0;
}



int
Test_OptionChainAnalyticalGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;
    char* str;
    size_t n;

    OptionChainAnalyticalGetter_C ocg;
    memset(&ocg, 0, sizeof(OptionChainAnalyticalGetter_C));

    double vol = 20.00;
    double up = 70.00;
    double ir =3.00;
    unsigned int exp = 100;
    OptionStrikesType ost = OptionStrikesType_single;
    OptionStrikesValue osv; osv.single = 70.00;
    OptionContractType oct = OptionContractType_call;
    int includes_quotes = 1;
    const char* from_date = "2018-07-24";
    const char* to_date = "2018-09-24";
    OptionExpMonth oep = OptionExpMonth_aug;
    OptionType ot = OptionType_s;


    if( (err = OptionChainAnalyticalGetter_Create(creds, "KORS", vol, up, ir, exp,
                                                ost, osv, oct, includes_quotes,
                                                from_date, to_date, oep, ot, &ocg)) ){
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_Create");
    }


    if( ocg.type_id != TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL ){
        fprintf(stderr, "invalid type id (%i,%i) \n", ocg.type_id,
                 TYPE_ID_GETTER_OPTION_CHAIN_ANALYTICAL);
        return -1;
    }

    double vol2;
    double up2;
    double ir2;
    unsigned int exp2;
    OptionStrikesType ost2;
    OptionStrikesValue osv2;
    OptionContractType oct2;
    int includes_quotes2;
    char* from_date2;
    char* to_date2;
    OptionExpMonth oep2;
    OptionType ot2;

    if( (err = OptionChainAnalyticalGetter_GetSymbol(&ocg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetSymbol");

    if( (err = OptionChainAnalyticalGetter_GetVolatility(&ocg, &vol2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetVolatility");

    if( (err = OptionChainAnalyticalGetter_GetUnderlyingPrice(&ocg, &up2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetUnderlyingPirce");

    if( (err = OptionChainAnalyticalGetter_GetInterestRate(&ocg, &ir2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetInterestRate");

    if( (err = OptionChainAnalyticalGetter_GetDaysToExp(&ocg, &exp2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetDaysToExp");

    if( (err = OptionChainAnalyticalGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetStrikesInfo");

    if( (err = OptionChainAnalyticalGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetContractType");

    if( (err = OptionChainAnalyticalGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_IncludesQuotes");

    if( (err = OptionChainAnalyticalGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetFromeDate");

    if( (err = OptionChainAnalyticalGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetToDate");

    if( (err = OptionChainAnalyticalGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetExpMonth");

    if( (err = OptionChainAnalyticalGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Volatility: %lf \n", vol2);
    printf("Underlying Price: %lf \n", up2);
    printf("Interest Rate: %lf \n", ir2);
    printf("Days To Exp: %i \n", exp2);

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %lf \n", osv2.single);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = OptionChainAnalyticalGetter_Get(&ocg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    vol = 50.00;
    up = 100.00;
    ir = 10.00;
    exp = 300;
    ost = OptionStrikesType_n_atm;
    osv.n_atm =  2;
    oct = OptionContractType_all;
    includes_quotes = 0;
    oep = OptionExpMonth_jan;
    ot = OptionType_all;

    if( (err = OptionChainAnalyticalGetter_SetSymbol(&ocg, "SPY")) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetSymbol");

    if( (err = OptionChainAnalyticalGetter_SetVolatility(&ocg, vol)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetVolatility");

    if( (err = OptionChainAnalyticalGetter_SetUnderlyingPrice(&ocg, up)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetUnderlyingPirce");

    if( (err = OptionChainAnalyticalGetter_SetInterestRate(&ocg, ir)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetInterestRate");

    if( (err = OptionChainAnalyticalGetter_SetDaysToExp(&ocg, exp)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetDaysToExp");

    if( (err = OptionChainAnalyticalGetter_SetStrikes(&ocg, ost, osv)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetStrikesInfo");

    if( (err = OptionChainAnalyticalGetter_SetContractType(&ocg, oct)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetContractType");

    if( (err = OptionChainAnalyticalGetter_IncludeQuotes(&ocg, includes_quotes)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_IncludeQuotes");

    if( (err = OptionChainAnalyticalGetter_SetFromDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetFromeDate");

    if( (err = OptionChainAnalyticalGetter_SetToDate(&ocg, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetToDate");

    if( (err = OptionChainAnalyticalGetter_SetExpMonth(&ocg, oep)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetExpMonth");

    if( (err = OptionChainAnalyticalGetter_SetOptionType(&ocg, ot)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_SetOptionType");

    if( (err = OptionChainAnalyticalGetter_GetSymbol(&ocg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetSymbol");

    if( (err = OptionChainAnalyticalGetter_GetVolatility(&ocg, &vol2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetVolatility");

    if( (err = OptionChainAnalyticalGetter_GetUnderlyingPrice(&ocg, &up2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetUnderlyingPirce");

    if( (err = OptionChainAnalyticalGetter_GetInterestRate(&ocg, &ir2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetInterestRate");

    if( (err = OptionChainAnalyticalGetter_GetDaysToExp(&ocg, &exp2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetDaysToExp");

    if( (err = OptionChainAnalyticalGetter_GetStrikes(&ocg, &ost2, &osv2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetStrikesInfo");

    if( (err = OptionChainAnalyticalGetter_GetContractType(&ocg, &oct2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetContractType");

    if( (err = OptionChainAnalyticalGetter_IncludesQuotes(&ocg, &includes_quotes2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_IncludesQuotes");

    if( (err = OptionChainAnalyticalGetter_GetFromDate(&ocg, &from_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetFromeDate");

    if( (err = OptionChainAnalyticalGetter_GetToDate(&ocg, &to_date2, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetToDate");

    if( (err = OptionChainAnalyticalGetter_GetExpMonth(&ocg, &oep2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetExpMonth");

    if( (err = OptionChainAnalyticalGetter_GetOptionType(&ocg, &ot2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_GetOptionType");

    if( str ){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Volatility: %lf \n", vol2);
    printf("Underlying Price: %lf \n", up2);
    printf("Interest Rate: %lf \n", ir2);
    printf("Days To Exp: %iu \n", exp2);

    OptionStrikesType_to_string(ost2, &str, &n);
    if( str ){
        printf("OptionStrikesType: %s \n", str);
        free(str);
        str = NULL;
    }
    printf("OptionStrikesValue: %i \n", osv2.n_atm);

    OptionContractType_to_string(oct2, &str, &n);
    if( str ){
        printf("OptionContractType: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Includes Quotes: %i \n", includes_quotes2);

    if( from_date2 ){
        printf("From Date: %s \n", from_date2);
        free(from_date2);
        from_date2 = NULL;
    }

    if( to_date2 ){
        printf("To Date: %s \n", to_date2);
        free(to_date2);
        to_date2 = NULL;
    }

    OptionExpMonth_to_string(oep2, &str, &n);
    if( str ){
        printf("OptionExpMonth: %s \n", str);
        free(str);
        str = NULL;
    }

    OptionType_to_string(ot2, &str, &n);
    if( str ){
        printf("OptionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = OptionChainAnalyticalGetter_Get(&ocg, &buf, &ndata)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_Get");

    if( buf) {
        printf("Get: %s \n", buf);
        free(buf);
        buf = NULL;
    }

    if( (err = OptionChainAnalyticalGetter_Destroy(&ocg) ))
        CHECK_AND_RETURN_ON_ERROR(err, "OptionChainAnalyticalGetter_Destroy");

    return 0;
}

int
Test_AccountInfoGetter(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    AccountInfoGetter_C aig;
    memset(&aig, 0, sizeof(AccountInfoGetter_C));

    int rpos = -1;
    int rord = -1;

    if( (err = AccountInfoGetter_Create(creds, acct, 1, 1, &aig)) )
        CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_Create");

    if( aig.type_id != TYPE_ID_GETTER_ACCOUNT_INFO ){
        fprintf(stderr, "invalid type id (%i,%i) \n", aig.type_id,
                 TYPE_ID_GETTER_ACCOUNT_INFO );
        return -1;
    }

    if( (err = AccountInfoGetter_GetAccountId(&aig, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_GetAccountId");

    if( (err = AccountInfoGetter_ReturnsPositions(&aig, &rpos)) )
        CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnsPositions");

    if( (err = AccountInfoGetter_ReturnsOrders(&aig, &rord)) )
        CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnsOrders");

    if(str){
        printf("Account ID: %s \n", str);
        free(str);
        str = NULL;
    }

    printf("Returns Positions: %i \n", rpos);
    printf("Returns Orders: %i \n", rord);

   if( (err = AccountInfoGetter_Get(&aig, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }


   if( (err = AccountInfoGetter_SetAccountId(&aig, "BAD_ACCOUNT_ID")) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_SetSymbol");

   if( (err = AccountInfoGetter_ReturnPositions(&aig, 0)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnPositions");

   if( (err = AccountInfoGetter_ReturnOrders(&aig, 0)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnsOrders");

   if( (err = AccountInfoGetter_GetAccountId(&aig, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_GetAccountId");

   if( (err = AccountInfoGetter_ReturnsPositions(&aig, &rpos)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnsPositions");

   if( (err = AccountInfoGetter_ReturnsOrders(&aig, &rord)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_ReturnsOrders");

   if(str){
       printf("Account ID: %s \n", str);
       free(str);
       str = NULL;
   }

   printf("Returns Positions: %i \n", rpos);
   printf("Returns Orders: %i \n", rord);

   if( (err = AccountInfoGetter_Destroy(&aig)) )
       CHECK_AND_RETURN_ON_ERROR(err, "AccountInfoGetter_Destroy");

   return 0;
}

int
Test_PreferencesGetter(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    PreferencesGetter_C pg;
    memset(&pg, 0, sizeof(PreferencesGetter_C));

    if( (err = PreferencesGetter_Create(creds, acct, &pg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_Create");

    if( pg.type_id != TYPE_ID_GETTER_PREFERENCES ){
        fprintf(stderr, "invalid type id (%i,%i) \n", pg.type_id,
                 TYPE_ID_GETTER_PREFERENCES);
        return -1;
    }

    if( (err = PreferencesGetter_GetAccountId(&pg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_GetAccountId");

    if(str){
        printf("Account ID: %s \n", str);
        free(str);
        str = NULL;
    }

   if( (err = PreferencesGetter_Get(&pg, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }


   if( (err = PreferencesGetter_SetAccountId(&pg, "BAD_ACCOUNT_ID")) )
       CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_SetSymbol");

   if( (err = PreferencesGetter_GetAccountId(&pg, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_GetAccountId");

   if(str){
       printf("Account ID: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = PreferencesGetter_Destroy(&pg)) )
       CHECK_AND_RETURN_ON_ERROR(err, "PreferencesGetter_Destroy");

   return 0;
}

int
Test_StreamerSubscriptionKeysGetter(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    StreamerSubscriptionKeysGetter_C sskg;
    memset(&sskg, 0, sizeof(StreamerSubscriptionKeysGetter_C));

    if( (err = StreamerSubscriptionKeysGetter_Create(creds, acct, &sskg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_Create");

    if( sskg.type_id != TYPE_ID_GETTER_SUBSCRIPTION_KEYS ){
        fprintf(stderr, "invalid type id (%i,%i) \n", sskg.type_id,
                 TYPE_ID_GETTER_SUBSCRIPTION_KEYS );
        return -1;
    }

    if( (err = StreamerSubscriptionKeysGetter_GetAccountId(&sskg, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_GetAccountId");

    if(str){
        printf("Account ID: %s \n", str);
        free(str);
        str = NULL;
    }

   if( (err = StreamerSubscriptionKeysGetter_Get(&sskg, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }


   if( (err = StreamerSubscriptionKeysGetter_SetAccountId(&sskg, "BAD_ACCOUNT_ID")) )
       CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_SetSymbol");

   if( (err = StreamerSubscriptionKeysGetter_GetAccountId(&sskg, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_GetAccountId");

   if(str){
       printf("Account ID: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = StreamerSubscriptionKeysGetter_Destroy(&sskg)) )
       CHECK_AND_RETURN_ON_ERROR(err, "StreamerSubscriptionKeysGetter_Destroy");

   return 0;
}

int
Test_TransactionHistoryGetter(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    TransactionHistoryGetter_C th;
    memset(&th, 0, sizeof(TransactionHistoryGetter_C));

    TransactionType tt = TransactionType_trade;
    const char* sym1 = "SPY";
    const char* sd1 = "2018-01-01";
    const char* se1 = "2018-06-01";

    if( (err = TransactionHistoryGetter_Create(creds, acct, tt, sym1, sd1, se1, &th)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_Create");

    if( th.type_id != TYPE_ID_GETTER_TRANSACTION_HISTORY ){
        fprintf(stderr, "invalid type id (%i,%i) \n", th.type_id,
                 TYPE_ID_GETTER_TRANSACTION_HISTORY);
        return -1;
    }

    if( (err = TransactionHistoryGetter_GetAccountId(&th, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetAccountId");

    if(str){
        printf("Account ID: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = TransactionHistoryGetter_GetTransactionType(&th, &tt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetTransactionType");

    TransactionType_to_string(tt, &str, &n);
    if( str ){
        printf("TransactionType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = TransactionHistoryGetter_GetSymbol(&th, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetSymbol");

    if(str){
        printf("Symbol: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = TransactionHistoryGetter_GetStartDate(&th, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetStartDate");

    if(str){
        printf("StartDate: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = TransactionHistoryGetter_GetEndDate(&th, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetEndDate");

    if(str){
        printf("EndDate: %s \n", str);
        free(str);
        str = NULL;
    }

   if( (err = TransactionHistoryGetter_Get(&th, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }

   if( (err = TransactionHistoryGetter_SetTransactionType(&th, TransactionType_all)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_SetTransactionType");

   if( (err = TransactionHistoryGetter_SetSymbol(&th, "")) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_SetSymbol");

   if( (err = TransactionHistoryGetter_SetStartDate(&th, "")) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_SetStartDate");

   if( (err = TransactionHistoryGetter_SetEndDate(&th, "")) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_SetEndDate");


   if( (err = TransactionHistoryGetter_GetTransactionType(&th, &tt)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetTransactionType");

   TransactionType_to_string(tt, &str, &n);
   if( str ){
       printf("TransactionType: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = TransactionHistoryGetter_GetSymbol(&th, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetSymbol");

   if(str){
       printf("Symbol: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = TransactionHistoryGetter_GetStartDate(&th, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetStartDate");

   if(str){
       printf("StartDate: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = TransactionHistoryGetter_GetEndDate(&th, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_GetEndDate");

   if(str){
       printf("EndDate: %s \n", str);
       free(str);
       str = NULL;
   }

  if( (err = TransactionHistoryGetter_Get(&th, &buf, &ndata)) )
      CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_Get");

  if( buf ){
      printf( "Get: %s \n", buf);
      free(buf);
      buf = NULL;
  }

   if( (err = TransactionHistoryGetter_Destroy(&th)) )
       CHECK_AND_RETURN_ON_ERROR(err, "TransactionHistoryGetter_Destroy");

   return 0;
}

int
Test_IndividualTransactionHistoryGetter(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    IndividualTransactionHistoryGetter_C ith;
    memset(&ith, 0, sizeof(IndividualTransactionHistoryGetter_C));

    if( (err = IndividualTransactionHistoryGetter_Create(creds, acct, "093432432", &ith)) )
        CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_Create");

    if( ith.type_id != TYPE_ID_GETTER_IND_TRANSACTION_HISTORY ){
        fprintf(stderr, "invalid type id (%i,%i) \n", ith.type_id,
                 TYPE_ID_GETTER_IND_TRANSACTION_HISTORY);
        return -1;
    }

    if( (err = IndividualTransactionHistoryGetter_GetAccountId(&ith, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_GetAccountId");

    if(str){
        printf("Account ID: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = IndividualTransactionHistoryGetter_GetTransactionId(&ith, &str, &n) ) )
        CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_GetTransactionId");

    if(str){
        printf("Transaction ID: %s \n", str);
        free(str);
        str = NULL;
    }

   if( !(err = IndividualTransactionHistoryGetter_Get(&ith, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "Failed to Return Error "
           "IndividualTransactionHistoryGetter_Get");
   err = 0;

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }

   if( (err = IndividualTransactionHistoryGetter_SetTransactionId(&ith, "111111111")) )
       CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_SetTransactionId");

   if( (err = IndividualTransactionHistoryGetter_SetAccountId(&ith, "BAD_ACCOUNT")) )
       CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_SetAccountId");


   if( (err = IndividualTransactionHistoryGetter_GetTransactionId(&ith, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_GetTransactionId");

   if( str ){
       printf("Transaction ID: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = IndividualTransactionHistoryGetter_GetAccountId(&ith, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_GetAccountId");

   if(str){
       printf("Account ID: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = IndividualTransactionHistoryGetter_Destroy(&ith)) )
       CHECK_AND_RETURN_ON_ERROR(err, "IndividualTransactionHistoryGetter_Destroy");

   return 0;
}

int
Test_UserPrincipalsGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL;
    size_t ndata = 0;

    UserPrincipalsGetter_C upg;
    memset(&upg, 0, sizeof(UserPrincipalsGetter_C));

    int skeys = -1;
    int ci = -1;
    int prefs = -1;
    int sids = -1;

    if( (err = UserPrincipalsGetter_Create(creds, 1,1,1,1, &upg)) )
        CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_Create");

    if( upg.type_id != TYPE_ID_GETTER_USER_PRINCIPALS ){
        fprintf(stderr, "invalid type id (%i,%i) \n", upg.type_id,
                 TYPE_ID_GETTER_USER_PRINCIPALS );
        return -1;
    }

    if( (err = UserPrincipalsGetter_ReturnsSubscriptionKeys(&upg, &skeys)) )
        CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsSubscriptionKeys");

    if( (err = UserPrincipalsGetter_ReturnsConnectionInfo(&upg, &ci)) )
        CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsConnectionInfo");

    if( (err = UserPrincipalsGetter_ReturnsPreferences(&upg, &prefs)) )
        CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsPreferences");

    if( (err = UserPrincipalsGetter_ReturnsSurrogateIds(&upg, &sids)) )
        CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsSurrogateIds");

    printf("Subscription Keys: %i \n", skeys);
    printf("Connection Info: %i \n", ci);
    printf("Preferences: %i \n", prefs);
    printf("Surrogate IDs: %i \n", sids);

   if( (err = UserPrincipalsGetter_Get(&upg, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }

   skeys = ci = prefs = sids = 0;

   if( (err = UserPrincipalsGetter_ReturnSubscriptionKeys(&upg, skeys)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnSubscriptionKeys");

   if( (err = UserPrincipalsGetter_ReturnConnectionInfo(&upg, ci)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnConnectionInfo");

   if( (err = UserPrincipalsGetter_ReturnPreferences(&upg, prefs)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnPreferences");

   if( (err = UserPrincipalsGetter_ReturnSurrogateIds(&upg, sids)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnSurrogateIds");

   skeys = ci = prefs = sids = -1;

   if( (err = UserPrincipalsGetter_ReturnsSubscriptionKeys(&upg, &skeys)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsSubscriptionKeys");

   if( (err = UserPrincipalsGetter_ReturnsConnectionInfo(&upg, &ci)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsConnectionInfo");

   if( (err = UserPrincipalsGetter_ReturnsPreferences(&upg, &prefs)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsPreferences");

   if( (err = UserPrincipalsGetter_ReturnsSurrogateIds(&upg, &sids)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_ReturnsSurrogateIds");

   printf("Subscription Keys: %i \n", skeys);
   printf("Connection Info: %i \n", ci);
   printf("Preferences: %i \n", prefs);
   printf("Surrogate IDs: %i \n", sids);

   if( (err = UserPrincipalsGetter_Get(&upg, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }


   if( (err = UserPrincipalsGetter_Destroy(&upg)) )
       CHECK_AND_RETURN_ON_ERROR(err, "UserPrincipalsGetter_Destroy");

   return 0;
}

int
Test_InstrumentInfoGetter(struct Credentials *creds)
{
    int err = 0;
    char *buf = NULL, *str = NULL;
    size_t n = 0, ndata = 0;

    InstrumentInfoGetter_C iig;
    memset(&iig, 0, sizeof(InstrumentInfoGetter_C));

    InstrumentSearchType ist;

    if( (err = InstrumentInfoGetter_Create(creds, InstrumentSearchType_cusip,
                                            "78462F103", &iig)) )
        CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_Create");

    if( iig.type_id != TYPE_ID_GETTER_INSTRUMENT_INFO ){
        fprintf(stderr, "invalid type id (%i,%i) \n", iig.type_id,
                 TYPE_ID_GETTER_INSTRUMENT_INFO);
        return -1;
    }

    if( (err = InstrumentInfoGetter_GetSearchType(&iig, &ist)) )
        CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_GetSearchType");

    InstrumentSearchType_to_string(ist, &str, &n);
    if( str ){
        printf("InstrumentSearchType: %s \n", str);
        free(str);
        str = NULL;
    }

    if( (err = InstrumentInfoGetter_GetQueryString(&iig, &str, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_GetQueryString");

    if(str){
        printf("QueryString: %s \n", str);
        free(str);
        str = NULL;
    }

   if( (err = InstrumentInfoGetter_Get(&iig, &buf, &ndata)) )
       CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_Get");

   if( buf ){
       printf( "Get: %s \n", buf);
       free(buf);
       buf = NULL;
   }

   if( (err = InstrumentInfoGetter_SetQuery(&iig, InstrumentSearchType_symbol_regex,
                                               "GOOGL?")) )
       CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_Get");

   if( (err = InstrumentInfoGetter_GetSearchType(&iig, &ist)) )
       CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_GetSearchType");

   InstrumentSearchType_to_string(ist, &str, &n);
   if( str ){
       printf("InstrumentSearchType: %s \n", str);
       free(str);
       str = NULL;
   }

   if( (err = InstrumentInfoGetter_GetQueryString(&iig, &str, &n)) )
       CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_GetQueryString");

   if(str){
       printf("QueryString: %s \n", str);
       free(str);
       str = NULL;
   }

  if( (err = InstrumentInfoGetter_Get(&iig, &buf, &ndata)) )
      CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_Get");

  if( buf ){
      printf( "Get: %s \n", buf);
      free(buf);
      buf = NULL;
  }

   if( (err = InstrumentInfoGetter_Destroy(&iig)) )
       CHECK_AND_RETURN_ON_ERROR(err, "InstrumentInfoGetter_Destroy");

   return 0;
}

void
build_order_date(char* buf, int off)
{

    time_t tt_now;
    time(&tt_now);

    struct tm t= *gmtime(&tt_now);
    t.tm_mday -= off;
    mktime(&t);

    sprintf(buf, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    buf[10] = 0;
}


void
find_order_ids(char*buf, size_t n, unsigned long long **order_ids, int *cnt)
{
    char oid[20];
    const char* key = "\"orderId\" : ";
    size_t klen = strlen(key);
    if( n < klen )
        return;
    size_t i = 0;
    for( ; i <= n-klen; ++i){
        if( strncmp(key, &buf[i], klen) == 0){
            i += klen;
            char *tmp = buf + i;
            memset(oid, 0, 20);
            int ii = 0;
            while( *tmp != ','){
                if( *tmp == 0 )
                    return;
                if( *tmp >= '0' && *tmp <= '9')
                    oid[ii++] = *tmp;
                ++tmp;
            }
            (*cnt)++;
            *order_ids = realloc(*order_ids, sizeof(unsigned long long) * (*cnt));
            (*order_ids)[(*cnt)-1] = strtoull(oid, NULL, 10);
        }
    }
}


int
Test_OrderGetters(struct Credentials *creds, const char* acct)
{
    int err = 0;
    char *buf = NULL;
    size_t n = 0;
    OrdersGetter_C og = {0,0};

    char fromdate1[11], todate1[11], fromdate2[11], todate2[11];
    build_order_date(fromdate1, 59);
    build_order_date(todate1, 0);
    build_order_date(fromdate2, 20);
    build_order_date(todate2, 1);

    if( (err = OrdersGetter_Create(creds, acct, 10, fromdate1, todate1,
                                   OrderStatusType_CANCELED, &og)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Create");

    if( og.type_id != TYPE_ID_GETTER_ORDERS ){
        fprintf(stderr, "invalid type id (%i,%i) \n", og.type_id,
                 TYPE_ID_GETTER_ORDERS);
        return -1;
    }

    if( (err = OrdersGetter_GetAccountId(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetAccountId");

    if( strcmp(buf, acct) ){
        fprintf(stderr, "invalid account id (%s,%s) \n", buf, acct);
        free(buf);
        return -1;
    }
    printf("Account ID: %s \n", buf);
    free(buf);
    buf = NULL;

    unsigned int nmax = 0;
    if( (err = OrdersGetter_GetNMaxResults(&og, &nmax)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetNMaxResults");

    if( nmax != 10 ){
        fprintf(stderr, "invalid nmax_results (%u, %u) \n", nmax, 10);
        return -1;
    }
    printf("NMaxResults: %u \n", nmax);

    if( (err = OrdersGetter_GetFromEnteredTime(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetFromEnteredTime");

    if( strcmp(buf, fromdate1) ){
        fprintf(stderr, "invalid from date (%s,%s) \n", buf, fromdate1);
        free(buf);
        return -1;
    }
    printf("FromTime: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_GetToEnteredTime(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetToEnteredTime");

    if( strcmp(buf, todate1) ){
        fprintf(stderr, "invalid to date (%s,%s) \n", buf, todate1);
        free(buf);
        return -1;
    }
    printf("ToTime: %s \n", buf);
    free(buf);
    buf = NULL;

    OrderStatusType ost;
    if( (err = OrdersGetter_GetOrderStatusType(&og, &ost)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetOrderStatusType");

    if( ost != OrderStatusType_CANCELED ){
        fprintf(stderr, "invalid order status type (%i, %i) \n", ost,
                 OrderStatusType_CANCELED);
        return -1;
    }

    OrderStatusType_to_string(ost, &buf, &n);
    printf("OrderStatusType: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_Get(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Get");
    printf("Orders: %s \n", buf);

    unsigned long long *order_ids=NULL;
    int cnt = 0;
    find_order_ids(buf, n, &order_ids, &cnt);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_SetOrderStatusType(&og, OrderStatusType_QUEUED)) ){
        free(order_ids);
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetOrderStatusType ");
    }

    if( (err = OrdersGetter_Get(&og, &buf, &n)) ){
        free(order_ids);
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Get");
    }
    printf("Orders: %s \n", buf);

    find_order_ids(buf, n, &order_ids, &cnt);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_SetOrderStatusType(&og, OrderStatusType_WORKING)) ){
        free(order_ids);
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetOrderStatusType (2)");
    }

    if( (err = OrdersGetter_Get(&og, &buf, &n)) ){
        free(order_ids);
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Get");
    }
    printf("Orders: %s \n", buf);

    find_order_ids(buf, n, &order_ids, &cnt);
    free(buf);
    buf = NULL;

    unsigned long long order_id1 = 0, order_id2 = 0;
    if( cnt > 0 )
        order_id1 = order_ids[0];
    if( cnt > 1 )
        order_id2 = order_ids[1];
    free(order_ids);

//
    if( (err = OrdersGetter_SetNMaxResults(&og, 1)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetNMaxResults");

    if( (err = OrdersGetter_SetFromEnteredTime(&og, fromdate2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetFromEnteredTime");

    if( (err = OrdersGetter_SetToEnteredTime(&og, todate2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetToEnteredTime");

    if( (err = OrdersGetter_SetOrderStatusType(&og, OrderStatusType_REJECTED)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_SetOrderStatusType (3)");


    if( (err = OrdersGetter_GetAccountId(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetAccountId");

    if( strcmp(buf, acct) ){
        fprintf(stderr, "invalid account id (%s,%s) \n", buf, acct);
        free(buf);
        return -1;
    }
    printf("Account ID: %s \n", buf);
    free(buf);
    buf = NULL;

    nmax = 0;
    if( (err = OrdersGetter_GetNMaxResults(&og, &nmax)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetNMaxResults (2)");

    if( nmax != 1 ){
        fprintf(stderr, "invalid nmax_results (%u, %u) \n", nmax, 1);
        return -1;
    }
    printf("NMaxResults: %u \n", nmax);

    if( (err = OrdersGetter_GetFromEnteredTime(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetFromEnteredTime (2)");

    if( strcmp(buf, fromdate2) ){
        fprintf(stderr, "invalid from date (%s,%s) \n", buf, fromdate2);
        free(buf);
        return -1;
    }
    printf("FromTime: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_GetToEnteredTime(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetToEnteredTime (2)");

    if( strcmp(buf, todate2) ){
        fprintf(stderr, "invalid to date (%s,%s) \n", buf, todate2);
        free(buf);
        return -1;
    }
    printf("ToTime: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_GetOrderStatusType(&og, &ost)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_GetOrderStatusType (2)");

    if( ost != OrderStatusType_REJECTED ){
        fprintf(stderr, "invalid order status type (%i, %i) \n", ost,
                 OrderStatusType_REJECTED );
        return -1;
    }

    OrderStatusType_to_string(ost, &buf, &n);
    printf("OrderStatusType: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_Get(&og, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Get");
    printf("Orders: %s \n", buf);
    free(buf);
    buf = NULL;

    if( (err = OrdersGetter_Destroy(&og)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OrdersGetter_Destroy");

    if( cnt == 0 ){
        printf(" NO CANCELED/QUEUE/WORKING ORDERS OVER LAST 59 DAYS - "
                " SKIP OrderGetter (non-error) tests\n");

        OrderGetter_C o = {0,0};
        if( (err = OrderGetter_Create(creds, acct, "000000000", &o)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Create");

        if( o.type_id != TYPE_ID_GETTER_ORDER ){
           fprintf(stderr, "invalid type id (%i,%i) \n", o.type_id,
                    TYPE_ID_GETTER_ORDER);
           return -1;
       }

        if( OrderGetter_Get(&o, &buf, &n) !=  TDMA_API_REQUEST_ERROR ){
            fprintf(stderr, "OrderGetter_Get failed to return REQUEST_ERROR\n");
            return -1;
        }

        if( (err = OrderGetter_Destroy(&o)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Destroy");

    }else{

        OrderGetter_C o = {0,0};
        char id1_str[20];
        memset(id1_str, 0, 20);
        sprintf(id1_str, "%llu", order_id1);

        if( (err = OrderGetter_Create(creds, acct, id1_str, &o)) )
            CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Create (2)");

        if( o.type_id != TYPE_ID_GETTER_ORDER ){
           fprintf(stderr, "invalid type id (%i,%i) \n", o.type_id,
                    TYPE_ID_GETTER_ORDER);
           return -1;
       }

       if( (err = OrderGetter_GetAccountId(&o, &buf, &n)) )
           CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_GetAccountId");

       if( strcmp(buf, acct) ){
           fprintf(stderr, "invalid account id (%s,%s) \n", buf, acct);
           free(buf);
           return -1;
       }
       printf("Account ID: %s \n", buf);
       free(buf);
       buf = NULL;

       if( (err = OrderGetter_GetOrderId(&o, &buf, &n)) )
           CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_GetOrderId");

       if( strcmp(buf, id1_str) ){
           fprintf(stderr, "invalid order id (%s,%s) \n", buf, id1_str);
           free(buf);
           return -1;
       }
       printf("Order ID: %s \n", buf);
       free(buf);
       buf = NULL;

       if( (err = OrderGetter_Get(&o, &buf, &n)) )
           CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Get");

       printf("Order %llu: %s \n", order_id1, buf);
       free(buf);
       buf = NULL;

       if( order_id2 != 0 ){
           char id2_str[20];
           memset(id2_str, 0, 20);
           sprintf(id2_str, "%llu", order_id2);

           if( (err = OrderGetter_SetOrderId(&o, id2_str)) )
               CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_SetOrderId (2)");

           if( (err = OrderGetter_GetOrderId(&o, &buf, &n)) )
               CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_GetOrderId (2)");

           if( strcmp(buf, id2_str) ){
               fprintf(stderr, "invalid order id (%s,%s) \n", buf, id2_str);
               free(buf);
               return -1;
           }
           printf("Order ID: %s \n", buf);
           free(buf);
           buf = NULL;

           if( (err = OrderGetter_Get(&o, &buf, &n)) )
               CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Get (2)");

           printf("Order %llu: %s \n", order_id2, buf);
           free(buf);
           buf = NULL;

       }

       if( (err = OrderGetter_SetOrderId(&o, "000000000")) )
           CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_SetOrderId (3)");

       if( OrderGetter_Get(&o, &buf, &n) !=  TDMA_API_REQUEST_ERROR ){
           fprintf(stderr, "OrderGetter_Get failed to return REQUEST_ERROR\n");
           return -1;
       }

       if( (err = OrderGetter_Destroy(&o)) )
           CHECK_AND_RETURN_ON_ERROR(err, "OrderGetter_Destroy");
    }


    return err;
}



