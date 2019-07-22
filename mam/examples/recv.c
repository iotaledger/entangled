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
  bool is_last_packet;

  if (ac < 4 || ac > 5) {
    fprintf(stderr, "usage: recv <host> <port> <bundle> <chid> (optional)\n");
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

  if (receive_bundle(av[1], atoi(av[2]), (tryte_t *)av[3], bundle) != RC_OK) {
    return EXIT_FAILURE;
  }

  mam_psk_t_set_add(&api.psks, &psk);
  if (ac == 5) {
    mam_api_add_trusted_channel_pk(&api, (tryte_t *)av[4]);
  }

  if (mam_api_bundle_read(&api, bundle, &payload_trytes, &payload_size, &is_last_packet) == RC_OK) {
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

  return ret;
}
