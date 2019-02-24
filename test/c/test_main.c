
#include "test.h"
#include "tdma_api_streaming.h"

int
Test_BuildOptionSymbol(void);


int main(int argc, char* argv[])
{
    int err = 0;

    struct Credentials creds;
    memset(&creds, 0, sizeof(struct Credentials));

    if (argc < 4 ) {
        printf( "invalid # of args\n");
        printf( "  args: [account id] [path to credentials filed] [password]\n" );
        return 1;
    }

    printf("*** [BEGIN] TEST OPTION SYMBOL BUILDER [BEGIN] ***\n");
    err = Test_BuildOptionSymbol();
    if( err ){
        printf("\n *** [ERROR] TEST OPTION SYMBOL BUILDER [ERROR] ***\n");
        StoreCredentials( argv[2], argv[3], &creds);
        return err;
    }
    printf("\n *** [END] TEST OPTION SYMBOL BUILDER [END] ***\n\n");

    printf("*** [BEGIN] TEST EXECUTION ORDER OBJECTS [BEGIN] ***\n");
    err = Test_Execution_Order_Objects();
    if( err ){
        printf("\n *** [ERROR] TEST EXECUTION ORDER OBJECTS [ERROR] ***\n");
        return err;
    }
    printf("\n *** [END] TEST EXECUTION ORDER OBJECTS [END] ***\n\n");

    if( (err = LoadCredentials( argv[2], argv[3], &creds )) )
        CHECK_AND_RETURN_ON_ERROR(err, "LoadCredentials");

    printf("*** [BEGIN] TEST GETTERS [BEGIN] ***\n");
    err = Test_Getters(&creds, argv[1], 1500);
    if( err ){
        printf("\n *** [ERROR] TEST GETTERS [ERROR] ***\n");
        StoreCredentials( argv[2], argv[3], &creds);
        return err;
    };
    printf("\n *** [END] TEST GETTERS[END] ***\n\n");

    printf("*** [BEGIN] TEST STREAMING [BEGIN] ***\n");
    err = Test_Streaming(&creds, argv[1]);
    if( err ){
        printf("\n *** [ERROR] TEST STREAMING [ERROR] ***\n");
        StoreCredentials( argv[2], argv[3], &creds);
        return err;
    }
    printf("\n *** [END] TEST STREAMING [END] ***\n\n");

    if( (err = StoreCredentials( argv[2], argv[3], &creds)) )
        CHECK_AND_RETURN_ON_ERROR(err, "StoreCredentials");

    if( (err = CloseCredentials(&creds)) )
        CHECK_AND_RETURN_ON_ERROR(err, "CloseCredentials");

    return 0;
}

int cmp_option_symbol(const char* sym, const char* u, unsigned int m,
                        unsigned int d, unsigned int y, int c, double s)
{
    int err = 0;
    char* buf = NULL;
    size_t n;
    if( (err = BuildOptionSymbol(u,m,d,y,c,s, &buf, &n)) ){
        fprintf(stderr, "BuildOptionSymbol failed for '%s' with error %i \n",
                sym, err);
        return -1;
    }

    if( !buf ){
        fprintf(stderr, "BuildOptionSymbol failed for %s, null buffer \n", sym);
        return -1;
    }

    if( strcmp(sym, buf) ){
        fprintf(stderr, "BuildOptionSymbol failed for %s, no match \n", sym);
        free(buf);
        return -1;
    }

    free(buf);
    return 0;

}

int
Test_BuildOptionSymbol(void)
{
    cmp_option_symbol("SPY_010118C300", "SPY",1,1,2018,1,300);
    cmp_option_symbol("SPY_123199P200", "SPY",12,31,2099,0,200);
    cmp_option_symbol("A_021119P1.5", "A",2,11,2019,0,1.5);
    cmp_option_symbol("A_021119P1.5", "A",2,11,2019,0,1.500);
    cmp_option_symbol("ABCDEF_110121C99.999", "ABCDEF",11,1,2021, 1,99.999);
    cmp_option_symbol("ABCDEF_110121C99.999", "ABCDEF",11,1,2021, 1,99.99900);
    cmp_option_symbol("ABCDEF_110121C99", "ABCDEF",11,1,2021,1, 99.0);
    cmp_option_symbol("ABCDEF_110121C99.001", "ABCDEF",11,1,2021, 1,99.001);

    int t = 1;
    char* buf;
    size_t n;
    if( BuildOptionSymbol("",1,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY_",1,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY_",1,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("_SPY",1,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SP_Y",1,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",0,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",13,1,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,0,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,32,2018,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,31,999,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,31,10001,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,31,18,1,300,&buf,&n) == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,31,2018,1,0.0,&buf,&n)  == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    }
    if( BuildOptionSymbol("SPY",1,31,2018,1,-100.0,&buf,&n)  == 0){
        fprintf(stderr, "BuildOptionSymbol did not fail for test # %i \n", t++);
        return -1;
    };

    return 0;
}

void
print_error(int err, const char* name)
{
    char *buf = NULL;
    size_t n;
    fprintf(stderr, "error(%i): %s \n", err, name);
    int err2 = LastErrorMsg(&buf, &n);
    if( !err2 )
        fprintf(stderr, "last error msg: %s \n", buf);
    if( buf )
        FreeBuffer(buf);

}
