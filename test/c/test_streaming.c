#include "tdma_api_streaming.h"

#include "test.h"

/* include/_streaming.h */
const int TYPE_ID_SUB_QUOTES = 1;
const int TYPE_ID_SUB_OPTIONS = 2;
const int TYPE_ID_SUB_LEVEL_ONE_FUTURES = 3;
const int TYPE_ID_SUB_LEVEL_ONE_FOREX = 4;
const int TYPE_ID_SUB_LEVEL_ONE_FUTURES_OPTIONS = 5;
const int TYPE_ID_SUB_NEWS_HEADLINE = 6;
const int TYPE_ID_SUB_CHART_EQUITY = 7;
const int TYPE_ID_SUB_CHART_FOREX = 8; // not working
const int TYPE_ID_SUB_CHART_FUTURES = 9;
const int TYPE_ID_SUB_CHART_OPTIONS = 10;
const int TYPE_ID_SUB_TIMESALE_EQUITY = 11;
const int TYPE_ID_SUB_TIMESALE_FOREX = 12; // not working
const int TYPE_ID_SUB_TIMESALE_FUTURES = 13;
const int TYPE_ID_SUB_TIMESALE_OPTIONS = 14;
const int TYPE_ID_SUB_ACTIVES_NASDAQ = 15;
const int TYPE_ID_SUB_ACTIVES_NYSE = 16;
const int TYPE_ID_SUB_ACTIVES_OTCBB = 17;
const int TYPE_ID_SUB_ACTIVES_OPTION = 18;

const int TYPE_ID_STREAMING_SESSION = 100;

void
streaming_callback( int cb_type,
                    int ss_type,
                    unsigned long long ts,
                    const char* msg )
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

    if( (err = StreamerServiceType_to_string(ss_type, &buf, &n)) ){
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
    QuotesSubscription_C sub_;
    const char* symbols1a[] = {"SPY", "QQQ"};
    QuotesSubscriptionField fields1a[] = {QuotesSubscriptionField_symbol};
    if( (err = QuotesSubscription_Create( symbols1a, 2, fields1a, 1,
                                          CommandType_UNSUBS, &sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_Create");

    if( (err = QuotesSubscription_Copy(&sub_, sub) ) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_Copy");

    if( (err = QuotesSubscription_Destroy(&sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_Destroy (COPY");


    const char* symbols1b[] = {};
    if( (err = QuotesSubscription_SetSymbols(sub, symbols1b, 0)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_SetSymbols");

    const char* symbols1[] = {"SPY", "QQQ", "EEM", "XLF", "XLE"};
    if( (err = QuotesSubscription_SetSymbols(sub, symbols1, 5)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_SetSymbols");


    QuotesSubscriptionField fields1b[] = {};
    if( (err = QuotesSubscription_SetFields(sub, fields1b, 0)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_SetFields");

    QuotesSubscriptionField fields1[] = {QuotesSubscriptionField_symbol,
                                         QuotesSubscriptionField_bid_price,
                                         QuotesSubscriptionField_ask_price,
                                          QuotesSubscriptionField_last_price};
    if( (err = QuotesSubscription_SetFields(sub, fields1, 4)) )
            CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_SetFields");


    if( (err = QuotesSubscription_SetCommand(sub, CommandType_SUBS)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_SetCommand");

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

    CommandType cmnd;
    if( (err = QuotesSubscription_GetCommand(sub, &cmnd)) )
        CHECK_AND_RETURN_ON_ERROR(err, "QuotesSubscription_GetCommand");

    if( (err = CommandType_to_string(cmnd, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CommandType_to_string" );

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
    TimesaleFuturesSubscription_C sub_;
    const char* symbols1a[] = {"/ES", "/GC"};
    TimesaleSubscriptionField fields1a[] = {TimesaleSubscriptionField_symbol};
    if( (err = TimesaleFuturesSubscription_Create(symbols1a, 2, fields1a, 1,
                                                  CommandType_ADD, &sub_)))
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_Create");

    if( (err = TimesaleFuturesSubscription_Copy(&sub_, sub) ) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_Copy");

    if( (err = TimesaleFuturesSubscription_Destroy(&sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_Destroy (COPY");


    const char* symbols1[] = {"/ES"};
    if( (err = TimesaleFuturesSubscription_SetSymbols(sub, symbols1, 1)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_SetSymbols");

    TimesaleSubscriptionField fields1[] = {TimesaleSubscriptionField_symbol,
                                          TimesaleSubscriptionField_last_price};
    if( (err = TimesaleFuturesSubscription_SetFields(sub, fields1, 2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_SetFields");


    if( (err = TimesaleFuturesSubscription_SetCommand(sub, CommandType_SUBS)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_SetCommand");

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


    CommandType cmnd;
    if( (err = TimesaleFuturesSubscription_GetCommand(sub, &cmnd)) )
        CHECK_AND_RETURN_ON_ERROR(err, "TimesaleFuturesSubscription_GetCommand");

    if( (err = CommandType_to_string(cmnd, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CommandType_to_string" );

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
    NasdaqActivesSubscription_C sub_;
    DurationType dt1 = DurationType_min_5;
    if( (err = NasdaqActivesSubscription_Create(dt1, CommandType_VIEW, &sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_Create");

    if( (err = NasdaqActivesSubscription_Copy(&sub_, sub) ) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_Copy");

    if( (err = NasdaqActivesSubscription_Destroy(&sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_Destroy (COPY");


    DurationType dt = DurationType_min_60;
    if( (err = NasdaqActivesSubscription_SetDuration(sub, dt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaActivesSubscription_SetDuration");

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

    CommandType cmnd;
    if( (err = NasdaqActivesSubscription_GetCommand(sub, &cmnd)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_GetCommand");

    if( (err = CommandType_to_string(cmnd, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CommandType_to_string" );

    if( strcmp(buf, "VIEW") ){
        fprintf(stderr, "NasdaqActivesSubscription: bad command (%s) \n", buf);
        return -1;
    }

    printf("Command: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    if( (err = NasdaqActivesSubscription_SetCommand(sub, CommandType_SUBS)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_SetCommand");

    if( (err = NasdaqActivesSubscription_GetCommand(sub, &cmnd)) )
        CHECK_AND_RETURN_ON_ERROR(err, "NasdaqActivesSubscription_GetCommand");

    if( (err = CommandType_to_string(cmnd, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CommandType_to_string" );

    if( strcmp(buf, "SUBS") ){
        fprintf(stderr, "NasdaqActivesSubscription: bad command (%s) \n ", buf);
        return -1;
    }

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
test_option_actives_subscription(OptionActivesSubscription_C* sub)
{
    int err = 0;
    OptionActivesSubscription_C sub_;
    DurationType dt1 = DurationType_min_5;
    VenueType vt1 = VenueType_calls;

    if( (err = OptionActivesSubscription_Create(dt1, vt1, CommandType_UNSUBS, &sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_Create");

    if( (err = OptionActivesSubscription_Copy(&sub_, sub) ) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_Copy");

    if( (err = OptionActivesSubscription_Destroy(&sub_)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_Destroy (COPY");


    DurationType dt = DurationType_min_60;
    if( (err = OptionActivesSubscription_SetDuration(sub, dt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_SetDuration");

    VenueType vt = VenueType_opts;
    if( (err = OptionActivesSubscription_SetVenue(sub, vt)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_SetVenue");

    if( (err = OptionActivesSubscription_SetCommand(sub, CommandType_SUBS)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_SetCommand");

    StreamerServiceType sst;
    char* buf = NULL;
    size_t n;

    if( (err = OptionActivesSubscription_GetService(sub, &sst)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_GetService");

    if( (err = StreamerServiceType_to_string(sst, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StreamerServiceType_to_string");

    printf("Service: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    if( sst != StreamerServiceType_ACTIVES_OPTIONS ){
        fprintf(stderr, "OptionActivesSubscription: bad StreamerServiceType (%i) \n",
                (int)sst);
        return -1;
    }

    CommandType cmnd;
    if( (err = OptionActivesSubscription_GetCommand(sub, &cmnd)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_GetCommand");

    if( (err = CommandType_to_string(cmnd, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CommandType_to_string" );

    if( strcmp(buf, "SUBS") ){
        fprintf(stderr, "OptionActivesSubscription: bad command (%s) \n ", buf);
        return -1;
    }

    printf("Command: %s \n", buf);
    if( buf ){
        FreeBuffer(buf);
        buf = NULL;
    }

    DurationType dt2;
    if( (err = OptionActivesSubscription_GetDuration(sub, &dt2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_GetDuration");

    if( dt2 != dt ){
        fprintf(stderr, "OptionActivesSubscription: bad duration (%i) \n",
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

    VenueType vt2;
    if( (err = OptionActivesSubscription_GetVenue(sub, &vt2)) )
        CHECK_AND_RETURN_ON_ERROR(err, "OptionActivesSubscription_GetVenue");

    if( vt2 != vt ){
        fprintf(stderr, "OptionActivesSubscription: bad venue (%i) \n",
                (int)vt);
        return -1;
    }
    if( (err = VenueType_to_string(vt2, &buf, &n)) )
        CHECK_AND_RETURN_ON_ERROR(err, "VenueType_to_string");

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

    OptionActivesSubscription_C q4;
    err = test_option_actives_subscription(&q4);
    if( err ){
        LastErrorMsg(&buf, &n);
        LastErrorCode(&code);
        fprintf(stderr, "OptionActivesSubscription error(%i): %s", code, buf);
        if( buf ){
              FreeBuffer(buf);
              n = 0;
              code = 0;
          }
        return -1;
    }

    if( q4.type_id != TYPE_ID_SUB_ACTIVES_OPTION ){
        fprintf(stderr, "invalid type id (%i,%i) \n", q4.type_id,
                 TYPE_ID_SUB_ACTIVES_OPTION);
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

    err = StreamingSubscription_Destroy((StreamingSubscription_C*)(&q4)); // generic
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


