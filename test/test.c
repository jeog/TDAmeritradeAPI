#include "tdma_api_get.h"
#include "tdma_api_streaming.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define SleepFor(msec) Sleep(msec)
#else
#include <unistd.h>
#define SleepFor(msec) usleep(msec * 1000)
#endif

#define CHECK_AND_RETURN_ON_ERROR(err, name) \
do{ \
    fprintf(stderr, "error(%i): " name "\n", (err)); \
    return (err); \
}while(0);

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
Test_Streaming(struct Credentials *creds, const char* acct);

int main(int argc, char* argv[])
{
    int err = 0;
    unsigned long long w = 0, w2 =0;

    struct Credentials creds;
    memset(&creds, 0, sizeof(struct Credentials));

    if (argc < 4 ) {
        printf( "invalid # of args\n");
        printf( "  args: [account id] [path to credentials filed] [password]\n" );
        return 1;
    }

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

    if( (err = LoadCredentials( argv[2], argv[3], &creds )) )
        CHECK_AND_RETURN_ON_ERROR(err, "LoadCredentials");

    err = Test_Streaming(&creds, argv[1]);
    if( err )
        return err;

    err = Test_QuoteGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_QuotesGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_MoversGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_MarketHoursGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_HistoricalPeriodGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_HistoricalRangeGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_OptionChainGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_OptionChainStrategyGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_OptionChainAnalyticalGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_AccountInfoGetter(&creds, argv[1]);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_PreferencesGetter(&creds, argv[1]);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_StreamerSubscriptionKeysGetter(&creds, argv[1]);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_TransactionHistoryGetter(&creds, argv[1]);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_IndividualTransactionHistoryGetter(&creds, argv[1]);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_UserPrincipalsGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    err = Test_InstrumentInfoGetter(&creds);
    if( err )
        return err;
    SleepFor(2000);

    if( (err = StoreCredentials( argv[2], argv[3], &creds)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StoreCredentials");

    return 0;
}

void
streaming_callback( int cb_type, int ss_type, unsigned long long ts, const char* msg)
{
    int err;
    char *buf = NULL;
    size_t n;

    if( (err = StreamingCallbackType_to_string(cb_type, &buf, &n)) ){
        fprintf(stderr, "error(%i): StreamingCallbackType_to_string\n", err);
        return;
    }
    printf("\t Callback Type: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    switch( ss_type ){
    case StreamerServiceType_QUOTE:
        if(ss_type != TYPE_ID_SUB_QUOTES){
            fprintf(stderr, "Service type and type id don't match(%i,%i) \n",
                     ss_type, TYPE_ID_SUB_QUOTES);
            exit(TDMA_API_TYPE_ERROR);
        }
        break;
    case StreamerServiceType_ACTIVES_NASDAQ:
        if(ss_type != TYPE_ID_SUB_ACTIVES_NASDAQ){
            fprintf(stderr, "Service type and type id don't match(%i,%i) \n",
                     ss_type, TYPE_ID_SUB_ACTIVES_NASDAQ);
            exit(TDMA_API_TYPE_ERROR);
        }
        break;
    case StreamerServiceType_TIMESALE_FUTURES:
        if(ss_type != TYPE_ID_SUB_TIMESALE_FUTURES){
            fprintf(stderr, "Service type and type id don't match(%i,%i) \n",
                     ss_type, TYPE_ID_SUB_TIMESALE_FUTURES);
            exit(TDMA_API_TYPE_ERROR);
        }
        break;
    default:
        break;
    }

    if( (err = StreamerServiceType_to_string(cb_type, &buf, &n)) ){
        fprintf(stderr, "error(%i): StreamingServiceType_to_string\n", err);
        return;
    }
    printf("\t Streamer Service: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    printf("\t TimeStamp: %llu \n", ts);
    printf("\t Message: %s \n", msg);
}


int
test_quotes_subscription(QuotesSubscription_C* sub )
{
    int err = 0;
    const char* symbols1[] = {"SPY", "QQQ", "EEM", "XLF", "XLE"};
    QuotesSubscriptionField fields1[] = {QuotesSubscriptionField_symbol,
                                         QuotesSubscriptionField_bid_price,
                                         QuotesSubscriptionField_ask_price,
                                          QuotesSubscriptionField_last_price};
    if( (err = QuotesSubscription_Create(symbols1, 5, fields1, 4, sub)))
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_Create");

    StreamerServiceType sst;
    char* buf = NULL;
    size_t n;

    if( (err = QuotesSubscription_GetService(sub, &sst)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_GetService");

    if( (err = StreamerServiceType_to_string(sst, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerServiceType_to_string");

    printf("Service: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    if( sst != StreamerServiceType_QUOTE ){
        fprintf(stderr, "QuoteSubscription: bad StreamerServiceType (%i) \n",
                (int)sst);
        return -1;
    }


    if( (err = QuotesSubscription_GetCommand(sub, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_GetCommand");

    printf("Command: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    char **buffers;

    if( (err = QuotesSubscription_GetSymbols(sub, &buffers, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_GetSymbols");

    printf("%s", "Symbols: ");
    size_t i;
    for(i = 0; i < n; ++i){
        size_t ii;
        int no_match = 1;
        for(ii = 0; ii < n; ++ii){
            if( strcmp(buffers[i], symbols1[ii]) == 0 ){
                no_match = 0;
                break;
            }
        }
        if( no_match ){
            fprintf(stderr, "QuoteSubscription: bad symbols \n");
            FreeBuffers(buffers, n);
            return -1;
        }
        printf("%s ", buffers[i]);
    }
    printf("%s", "\n");

    if( buffers ){
        FreeBuffers(buffers, n);
        buf = NULL;
    }

    QuotesSubscriptionField *fields = NULL;

    if( (err = QuotesSubscription_GetFields(sub, &fields, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_GetFields");

    printf("%s", "Fields: ");
    for(i = 0; i < n; ++i){
        int f = fields[i];
        size_t ii;
        int no_match = 1;
        for(ii = 0; ii < n; ++ii){
            if( f == fields1[ii] ){
                no_match = 0;
                break;
            }
        }
        if( no_match ){
            fprintf(stderr, "QuotesSubscription: bad field (%i,%i) \n",
                     f, fields1[ii]);
             FreeFieldsBuffer((int*)fields);
             return -1;
        }
        size_t n2;
        if( (err = QuotesSubscriptionField_to_string(f, &buf, &n2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscriptionField_to_string");

        printf("%s ", buf);
        if( buf ){
            FreeBuffer(buf);
            buf = NULL;
        }
    }
    printf("%s", "\n");

    if( buffers ){
        FreeFieldsBuffer((int*)fields);
        buffers = NULL;
    }
    return 0;
}

int
test_timesale_futures_subscription(TimesaleFuturesSubscription_C* sub )
{
    int err = 0;
    const char* symbols1[] = {"/ES"};
    TimesaleSubscriptionField fields1[] = {TimesaleSubscriptionField_symbol,
                                          TimesaleSubscriptionField_last_price};
    if( (err = TimesaleFuturesSubscription_Create(symbols1, 1, fields1, 2, sub)))
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_Create");

    StreamerServiceType sst;
    char* buf = NULL;
    size_t n;

    if( (err = TimesaleFuturesSubscription_GetService(sub, &sst)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_GetService");

    if( (err = StreamerServiceType_to_string(sst, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerServiceType_to_string");

    printf("Service: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    if( sst != StreamerServiceType_TIMESALE_FUTURES ){
        fprintf(stderr, "TimesaleFuturesSubscription: bad StreamerServiceType (%i) \n",
                (int)sst);
        return -1;
    }


    if( (err = TimesaleFuturesSubscription_GetCommand(sub, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_GetCommand");

    printf("Command: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    char **buffers;

    if( (err = TimesaleFuturesSubscription_GetSymbols(sub, &buffers, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_GetSymbols");

    printf("%s", "Symbols: ");
    size_t i;
    for(i = 0; i < n; ++i){
        size_t ii;
        int no_match = 1;
        for(ii = 0; ii < n; ++ii){
            if( strcmp(buffers[i], symbols1[ii]) == 0 ){
                no_match = 0;
                break;
            }
        }
        if( no_match ){
            fprintf(stderr, "QuoteSubscription: bad symbols \n");
            FreeBuffers(buffers, n);
            return -1;
        }
        printf("%s ", buffers[i]);
    }
    printf("%s", "\n");

    if( buffers ){
        FreeBuffers(buffers, n);
        buf = NULL;
    }

    TimesaleSubscriptionField *fields = NULL;

    if( (err = TimesaleFuturesSubscription_GetFields(sub, &fields, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_GetFields");

    printf("%s", "Fields: ");
    for(i = 0; i < n; ++i){
        int f = fields[i];
        size_t ii;
        int no_match = 1;
        for(ii = 0; ii < n; ++ii){
            if( f == fields1[ii] ){
                no_match = 0;
                break;
            }
        }
        if( no_match ){
            fprintf(stderr, "TimesaleFuturesSubscription: bad field (%i,%i) \n",
                     f, fields1[ii]);
             FreeFieldsBuffer((int*)fields);
             return -1;
        }
        size_t n2;
        if( (err = TimesaleSubscriptionField_to_string(f, &buf, &n2)) )
            CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscriptionField_to_string");

        printf("%s ", buf);
        if( buf ){
            FreeBuffer(buf);
            buf = NULL;
        }
    }
    printf("%s", "\n");

    if( buffers ){
        FreeFieldsBuffer((int*)fields);
        buffers = NULL;
    }
    return 0;
}


int
test_nasdaq_actives_subscription(NasdaqActivesSubscription_C* sub )
{
    int err = 0;
    DurationType dt = DurationType_min_60;
    if( (err = NasdaqActivesSubscription_Create(dt, sub)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_Create");

    StreamerServiceType sst;
    char* buf = NULL;
    size_t n;

    if( (err = NasdaqActivesSubscription_GetService(sub, &sst)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_GetService");

    if( (err = StreamerServiceType_to_string(sst, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerServiceType_to_string");

    printf("Service: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    if( sst != StreamerServiceType_ACTIVES_NASDAQ ){
        fprintf(stderr, "NasdaqActivesSubscription: bad StreamerServiceType (%i) \n",
                (int)sst);
        return -1;
    }

    if( (err = NasdaqActivesSubscription_GetCommand(sub, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_GetCommand");

    printf("Command: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    DurationType dt2;
    if( (err = NasdaqActivesSubscription_GetDuration(sub, &dt2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_GetDuration");

    if( dt2 != dt ){
        fprintf(stderr, "NasdaqActivesSubscription: bad duration (%i) \n",
                (int)dt);
        return -1;
    }
    if( (err = DurationType_to_string(dt2, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "DurationType_to_string");

    printf("Duration: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    return 0;
}
int
Test_Streaming(struct Credentials* c, const char* account_id)
{
    char *buf = NULL;
    size_t n;
    int code;

    QuotesSubscription_C q1 ;
    int err = test_quotes_subscription(&q1);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "QuotesSubscription error(%i): %s", code, buf);
        if( buf ){
            FreeBuffer(buf);
            n = 0;
            code = 0;
        }
        return -1;
    }

    if( q1.type_id != TYPE_ID_SUB_QUOTES ){
        fprintf(stderr, "invalid type id (%i,%i) \n", q1.type_id,
                 TYPE_ID_SUB_QUOTES);
        return -1;
    }

    TimesaleFuturesSubscription_C q2;
    err = test_timesale_futures_subscription(&q2);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "TimesaleFuturesSubscription error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
        return -1;
    }

    if( q2.type_id != TYPE_ID_SUB_TIMESALE_FUTURES ){
        fprintf(stderr, "invalid type id (%i,%i) \n", q2.type_id,
                 TYPE_ID_SUB_TIMESALE_FUTURES );
        return -1;
    }

    NasdaqActivesSubscription_C q3;
    err = test_nasdaq_actives_subscription(&q3);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "NasdaqActivesSubscription error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
        return -1;
    }

    if( q3.type_id != TYPE_ID_SUB_ACTIVES_NASDAQ ){
        fprintf(stderr, "invalid type id (%i,%i) \n", q3.type_id,
                 TYPE_ID_SUB_ACTIVES_NASDAQ);
        return -1;
    }

    // CREATE SESSION 1
    StreamingSession_C ss;
    if( (err = StreamingSession_Create(c, account_id, streaming_callback, &ss)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Create");

    StreamingSubscription_C* subs[] = {(StreamingSubscription_C*)&q1};
    int results[] = {-1};

    // START SESSION 1
    if( (err = StreamingSession_Start(&ss, subs, 1, results)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Start");

    printf("Results: %i\n", results[0]);

    StreamingSubscription_C* subs2[] = {(StreamingSubscription_C*)&q2,
                                        (StreamingSubscription_C*)&q3};
    int results2[] = {-1,-1};

    // ADD TO SESSION 1
    if( (err = StreamingSession_AddSubscriptions(&ss, subs2, 2, results2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_AddSubscriptions");

    printf("Results: %i, %i, \n", results2[0], results2[1]);

    SleepFor(10000);

    // TRY TO RE-START SESSION 1 /W ACTIVE SESSION 1 (should get error)
    err = StreamingSession_Start(&ss, subs, 1, NULL);
    if( err != TDMA_API_STREAM_ERROR ){
        fprintf(stderr, "%s", "failed to get error on 2nd start");
        return -1;
    }

    // CREATE SESSION 2 (should be ok)
    StreamingSession_C ss2;
    if( (err = StreamingSession_Create(c, account_id, streaming_callback, &ss2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Create #2");

    // TRY TO START SESSION 2 /W ACTIVE SESSION 1 (should get error)
    err = StreamingSession_Start(&ss2, subs, 1, NULL);
    if( err != TDMA_API_STREAM_ERROR ){
        fprintf(stderr, "%s", "failed to get error for multiple active");
        return -1;
    }

    // STOP SESSION 1
    if( (err = StreamingSession_Stop(&ss)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Stop");

    results2[0] = -1;
    results2[1] = -1;

    SleepFor(5000);

    // RE-START SESSION 1
    if( (err = StreamingSession_Start(&ss, subs2, 2, results2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Start #2");

    printf("Results: %i, %i, \n", results2[0], results2[1]);

    SleepFor(5000);

    // STOP SESSION 1
    if( (err = StreamingSession_Stop(&ss)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Stop #2");


    StreamingSubscription_C* subs3[] = {(StreamingSubscription_C*)&q2,
                                        (StreamingSubscription_C*)&q2,
                                        (StreamingSubscription_C*)&q3};

    // START SESSION 2 (ignore results)
    if( (err = StreamingSession_Start(&ss2, subs3, 3, NULL)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Start #3");

    SleepFor(10000);

    // DESTROY SESSION 1 (already stopped)
    if( (err = StreamingSession_Destroy(&ss)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Destroy");

    // DESTROY SESSION 2 (should stop, then destroy)
    if( (err = StreamingSession_Destroy(&ss2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamingSession_Destroy");

    SleepFor(3000);

    err = QuotesSubscription_Destroy(&q1);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "QuotesSubscription_Destroy error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
    }

    err = NasdaqActivesSubscription_Destroy(&q3);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "NasdaqActivesSubscription_Destroy error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
    }

    err = StreamingSubscription_Destroy((StreamingSubscription_C*)(&q2)); // generic
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "StreamingSubscription_Destroy error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
    }

    return 0;

}


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




