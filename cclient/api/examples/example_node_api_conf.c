/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_node_api_conf(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  get_node_api_conf_res_t node_conf = {};

  if ((ret = iota_client_get_node_api_conf(s, &node_conf)) == RC_OK) {
    printf("maxFindTransactions: %" PRIu32 " \n", node_conf.max_find_transactions);
    printf("maxRequestsList: %" PRIu32 " \n", node_conf.max_requests_list);
    printf("maxGetTrytes: %" PRIu32 " \n", node_conf.max_get_trytes);
    printf("maxBodyLength: %" PRIu32 " \n", node_conf.max_body_length);
    printf("milestoneStartIndex: %" PRIu32 " \n", node_conf.milestone_start_index);
    printf("testNet: %s\n", node_conf.test_net ? "true" : "false");

  } else {
    printf("Error: %s", error_2_string(ret));
  }
}
