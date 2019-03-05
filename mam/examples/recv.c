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

#include "mam/examples/common.h"

int main(int ac, char **av) {
  mam_api_t api;
  int ret = EXIT_SUCCESS;
  tryte_t *payload_trytes = NULL;
  size_t payload_size = 0;
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  if (ac != 4) {
    fprintf(stderr, "usage: recv <host> <port> <bundle>\n");
    return EXIT_FAILURE;
  }

  if (mam_api_init(&api, (tryte_t *)DUMMY_SEED) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  receive_bundle(av[1], atoi(av[2]), (tryte_t *)av[3], bundle);

  mam_psk_t_set_add(&api.psks, &psk);

  if (mam_api_bundle_read_msg(&api, bundle, &payload_trytes, &payload_size) ==
      RC_OK) {
    if (payload_trytes == NULL || payload_size == 0) {
      fprintf(stderr, "No payload\n");
    } else {
      char *payload = calloc(payload_size * 2 + 1, sizeof(char));

      trytes_to_ascii(payload_trytes, payload_size, payload);
      fprintf(stderr, "Payload: %s\n", payload);
      free(payload);
    }
  } else {
    fprintf(stderr, "mam_api_bundle_read_msg failed\n");
  }

  bundle_transactions_free(&bundle);

  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
