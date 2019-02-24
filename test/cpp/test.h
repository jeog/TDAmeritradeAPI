
#ifndef TEST_H_
#define TEST_H_

#include <string>

#include "tdma_common.h"

extern bool use_live_connection;

long long msec_since_epoch();

void test_getters(const std::string& account_id, Credentials& creds);

void test_streaming( const std::string& account_id,
                     Credentials& c );

void test_execution_order_objects();

void
test_execute_transactions( const std::string& account_id,
                           Credentials& creds );

#endif /* TEST_H_ */
