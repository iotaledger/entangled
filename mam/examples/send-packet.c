/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "mam/examples/send-common.h"

int main(int ac, char **av) {
  mam_api_t api;
  bundle_transactions_t *bundle = NULL;
  retcode_t ret = RC_OK;

  if (ac != 7) {
    fprintf(stderr,
            "usage: send-packet <host> <port> <seed> <msg_id> <payload> "
            "<last_packet>\n");
    return EXIT_FAILURE;
  }

  if (strcmp(av[6], "yes") && strcmp(av[6], "no")) {
    fprintf(stderr, "Arg <last_packet> should be \"yes\" or \"no\" only\n");
    return EXIT_FAILURE;
  }

  // Loading or creating MAM API
  if ((ret = mam_api_load(MAM_FILE, &api, NULL, 0)) == RC_UTILS_FAILED_TO_OPEN_FILE) {
    if ((ret = mam_api_init(&api, (tryte_t *)av[3])) != RC_OK) {
      fprintf(stderr, "mam_api_init failed with err %d\n", ret);
      return EXIT_FAILURE;
    }
  } else if (ret != RC_OK) {
    fprintf(stderr, "mam_api_load failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  bundle_transactions_new(&bundle);

  // Writing packet to bundle
  {
    trit_t msg_id[MAM_MSG_ID_SIZE];

    bool last_packet = strcmp(av[6], "yes") == 0;
    trytes_to_trits((tryte_t *)av[4], msg_id, strlen(av[4]));
    if ((ret = mam_example_write_packet(&api, bundle, av[5], msg_id, last_packet)) != RC_OK) {
      fprintf(stderr, "mam_example_write_packet failed with err %d\n", ret);
      return EXIT_FAILURE;
    }
  }

  // Sending bundle
  if ((ret = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
    fprintf(stderr, "send_bundle failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Saving and destroying MAM API
  if ((ret = mam_api_save(&api, MAM_FILE, NULL, 0)) != RC_OK) {
    fprintf(stderr, "mam_api_save failed with err %d\n", ret);
  }
  if ((ret = mam_api_destroy(&api)) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Cleanup
  { bundle_transactions_free(&bundle); }

  return EXIT_SUCCESS;
}
