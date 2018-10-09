#ifndef TEST_H_
#define TEST_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define SleepFor(msec) Sleep(msec)
#else
#include <unistd.h>
#define SleepFor(msec) usleep(msec * 1000)
#endif

void print_error(int err, const char* name);

#define CHECK_AND_RETURN_ON_ERROR(err, name) \
do{ \
    print_error(err, name); \
    return (err); \
}while(0);

int Test_Getters(struct Credentials *creds, const char* acct, long wait);

int Test_Streaming(struct Credentials *creds, const char* acct);

int Test_Execution_Order_Objects();



#endif /* TEST_H_ */
