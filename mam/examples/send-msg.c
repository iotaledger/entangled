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
  trit_t msg_id[MAM_MSG_ID_SIZE];
  bundle_transactions_t *bundle = NULL;
  mam_channel_t *channel = NULL;
  retcode_t err = RC_OK;

  if (ac != 5) {
    fprintf(stderr, "usage: send <host> <port> <seed> <payload>\n");
    return EXIT_FAILURE;
  }

  if ((err = mam_api_init(&api, (tryte_t *)av[3])) != RC_OK) {
    fprintf(stderr, "mam_api_init failed with err %d\n", err);
    return EXIT_FAILURE;
  }

  // Creating channel
  {
    trits_t channel_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
    tryte_t address[NUM_TRYTES_ADDRESS];

    trits_from_str(channel_name, TEST_CHANNEL_NAME);
    if ((channel = malloc(sizeof(mam_channel_t))) == NULL) {
      fprintf(stderr, "malloc failed\n");
      return EXIT_FAILURE;
    }
    mam_channel_create(&api.prng, TEST_MSS_DEPTH, channel_name, channel);
    trits_to_trytes(channel->id, address, NUM_TRITS_ADDRESS);
    fprintf(stderr, "Address: ");
    for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
      fprintf(stderr, "%c", address[i]);
    }
    fprintf(stderr, "\n");
  }

  ERR_BIND_RETURN(mam_channel_t_set_add(api.channels, cha), ret);
  bundle_transactions_new(&bundle);

  // Writing header to bundle
  mam_example_write_header(&api, channel, bundle, msg_id);

  // Writing packet to bundle
  mam_example_write_packet(&api, channel, bundle, av[4], msg_id);

  // Sending bundle
  if ((err = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
    fprintf(stderr, "send_bundle failed with err %d\n", err);
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Bundle: ");
  for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
    fprintf(stderr, "%c",
            ((iota_transaction_t *)utarray_front(bundle))->essence.bundle[i]);
  }
  fprintf(stderr, "\n");

  // Cleanup
  {
    bundle_transactions_free(&bundle);
    mam_channel_destroy(channel);
    free(channel);
  }

  if ((err = mam_api_destroy(&api)) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed with err %d\n", err);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
