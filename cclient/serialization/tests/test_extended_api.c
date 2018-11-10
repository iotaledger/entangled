/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include <unity/unity.h>
#include <cclient/iota_client_extended_api.h>

void test_iota_client_find_transaction_objects(void){
    retcode_t ret = iota_client_find_transaction_objects(NULL, NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT(true);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_iota_client_find_transaction_objects);

    return UNITY_END();
}