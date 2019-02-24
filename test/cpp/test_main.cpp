
#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>

#include "test.h"
#include "json.hpp"

using namespace tdma;
using namespace std;

void test_option_symbol_builder();

bool use_live_connection = true;

int main(int argc, char* argv[])
{
    using namespace chrono;

   if (argc < 4 ) {
        cerr << "invalid # of args" << endl;
        cerr << "  args: [account id] [path to credentials filed] [password]" << endl;
        return 1;
    }
    
    cout<< argv[0] << endl << argv[1] << endl << argv[2] << endl << argv[3] << endl;
    std::string account_id(argv[1]);
    std::string creds_path(argv[2]);
    std::string password(argv[3]);

    if (account_id.empty() || creds_path.empty() || password.empty()) {        
        cerr << "invalid args" << endl;
        cerr << "  args: [account id] [path to credentials filed] [password]" << endl;
        return 1;        
    }

    if( argc > 4 && strcmp(argv[4], "--no-live-connect") == 0){
        cout<< argv[4] << endl;
        use_live_connection = false;
    }

    {
        CredentialsManager cmanager(creds_path, password);

        Credentials ccc;
        ccc = cmanager.credentials;
        Credentials ccc2( ccc );

        ccc2 = move(ccc);
        Credentials ccc4( move(ccc2 ));

        cout<< "*** [BEGIN] TEST OPTION SYMBOL BUILDER [BEGIN] ***" << endl;
        test_option_symbol_builder();
        cout<< "*** [END] TEST OPTION SYMBOL BUILDER [END] ***" << endl << endl;

        cout<< "*** [BEGIN] TEST EXECUTION ORDER OBJECTS [BEGIN] ***" << endl;
        test_execution_order_objects();
        cout<< "*** [END] TEST EXECUTION ORDER OBJECTS [END] ***" << endl << endl;
      
        // THIS SENDS LIVE ORDERS
        //cout<< "*** [BEGIN] TEST EXECUTION TRANSACTIONS [BEGIN] ***" << endl;
        //test_execute_transactions(account_id, cmanager.credentials);
        //cout<< "*** [END] TEST EXECUTION TRANSACTIONS [END] ***" << endl << endl;
        // THIS SENDS LIVE ORDERS

        cout<< "*** [BEGIN] TEST GETTERS [BEGIN] ***" << endl;
        test_getters(account_id, cmanager.credentials);
        cout<< "*** [END] TEST GETTERS [END] ***" << endl << endl;

        cout<< "*** [BEGIN] TEST STREAMING [BEGIN] ***" << endl;
        test_streaming(account_id, cmanager.credentials);

        cout<< "*** [END] TEST STREAMING [END] ***" << endl << endl;       
    }

    cout<< endl << "*** SUCCESS ***" << endl;
    return 0;
}

void test_option_symbol_builder()
{
    auto is_good = [](std::string sym, std::string underlying, unsigned int month,
                      unsigned int day, unsigned int year, bool is_call,
                      double strike){
        std::string o = tdma::BuildOptionSymbol(underlying, month, day, year,
                                                is_call, strike);
        if( o != sym )
            throw std::runtime_error(
                "option strings don't match(" + o + "," + sym + ")"
                );
    };

    auto is_bad= [](std::string underlying, unsigned int month, unsigned int day,
                    unsigned int year, bool is_call, double strike){
        string o;
        try{
            o = tdma::BuildOptionSymbol(underlying, month, day, year,
                                                    is_call, strike);
        }catch(tdma::APIException& e ){
            cout<< "successfully caught exception: " << e << endl;
            return;
        }
        throw std::runtime_error("failed to catch exception for: " + o);
    };

    is_good("SPY_010118C300", "Spy",1,1,2018,true,300);
    is_good("SPY_123199P200", "SPY",12,31,2099,false,200);
    is_good("A_021119P1.5", "a",2,11,2019,false,1.5);
    is_good("A_021119P1.5", "A",2,11,2019,false,1.500);
    is_good("ABCDEF_110121C99.999", "abcdEF",11,1,2021,true,99.999);
    is_good("ABCDEF_110121C99.999", "ABCDEF",11,1,2021,true,99.99900);
    is_good("ABCDEF_110121C99", "ABCDEF",11,1,2021,true,99.0);
    is_good("ABCDEF_110121C99.001", "ABCDEF",11,1,2021,true,99.001);

    is_bad("",1,1,2018,true,300);
    is_bad("SPY_",1,1,2018,true,300);
    is_bad("spy_",1,1,2018,false,300);
    is_bad("_SPY",1,1,2018,true,300);
    is_bad("SP_Y",1,1,2018,true,300);
    is_bad("SPY",0,1,2018,true,300);
    is_bad("SPY",13,1,2018,true,300);
    is_bad("SPY",1,0,2018,true,300);
    is_bad("SPY",1,32,2018,true,300);
    is_bad("SPY",1,31,999,true,300);
    is_bad("SPY",1,31,10001,true,300);
    is_bad("SPY",1,31,18,true,300);
    is_bad("SPY",1,31,2018,true,0.0);
    is_bad("SPY",1,31,2018,true,-100.0);
}

long long
msec_since_epoch()
{
    using namespace chrono;
    auto tse = (steady_clock::now().time_since_epoch());
    return duration_cast<milliseconds>(tse).count();
}


