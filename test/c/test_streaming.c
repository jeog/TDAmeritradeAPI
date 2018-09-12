#include "tdma_api_streaming.h"

#include "test.h"

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
    if( (err = StreamingSession_Create(c, streaming_callback, &ss)) )
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
    if( (err = StreamingSession_Create(c, streaming_callback, &ss2)) )
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


