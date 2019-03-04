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

    trits_from_str(channel_name, TEST_CHANNEL_NAME);
    if ((channel = malloc(sizeof(mam_channel_t))) == NULL) {
      fprintf(stderr, "malloc failed\n");
      return EXIT_FAILURE;
    }
    mam_channel_create(&api.prng, TEST_MSS_DEPTH, channel_name, channel);
  }

  ERR_BIND_RETURN(mam_channel_t_set_add(api.channels, cha), ret);
  bundle_transactions_new(&bundle);

  // Writing header to bundle
  {
    mam_psk_t_set_t psks = NULL;

    mam_psk_t_set_add(&psks, &psk);
    mam_api_bundle_write_header(&api, channel, NULL, NULL, NULL, psks, NULL, 0,
                                bundle, msg_id);
    mam_psk_t_set_free(&psks);
  }

  // Writing packet to bundle
  {
    tryte_t *payload_trytes =
        (tryte_t *)malloc(2 * strlen(av[4]) * sizeof(tryte_t));

    ascii_to_trytes(av[4], payload_trytes);
    mam_api_bundle_write_packet(&api, channel, msg_id, payload_trytes,
                                strlen(av[4]) * 2, 0, bundle);
    free(payload_trytes);
  }

  // Sending bundle
  if ((err = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
    fprintf(stderr, "send_bundle failed with err %d\n", err);
    return EXIT_FAILURE;
  }

  fprintf(stderr, "Address: ");
  for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
    fprintf(stderr, "%c",
            ((iota_transaction_t *)utarray_front(bundle))->essence.address[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "Bundle: ");
  for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
    fprintf(stderr, "%c",
            ((iota_transaction_t *)utarray_front(bundle))->essence.bundle[i]);
  }
  fprintf(stderr, "\n");

  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / 3];

  trits_to_trytes(msg_id, msg_id_trytes, MAM_MSG_ID_SIZE);

  fprintf(stderr, "Message ID: ");
  for (size_t i = 0; i < MAM_MSG_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", msg_id_trytes[i]);
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
