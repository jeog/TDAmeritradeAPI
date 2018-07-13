#include "tdma_api_get.h"

#include <stdio.h>
#include <stdlib.h>


#define CHECK_AND_RETURN_ON_ERROR(err, name) \
do{ \
    fprintf(stderr, "error(%i): " name "\n", (err)); \
    return (err); \
}while(0);


int main(int argc, char* argv[])
{
    int err = 0;
    unsigned long long w = 0, w2 =0;
    char *buf = NULL, *symbol = NULL;
    size_t n = 0;

    struct Credentials creds;
    QuoteGetter_C qg;

    memset(&creds, 0, sizeof(struct Credentials));
    memset(&qg, 0, sizeof(QuoteGetter_C));

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

    if( (err = QuoteGetter_Create(&creds, "SPY", &qg)) )
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

    if( (err = StoreCredentials( argv[2], argv[3], &creds)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StoreCredentials");

    return 0;
}
