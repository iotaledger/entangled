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
  bundle_transactions_t *bundle = NULL;
  int ret = EXIT_SUCCESS;
  trit_t msg_id[MAM_MSG_ID_SIZE];

  if (ac != 5) {
    fprintf(stderr, "usage: send <host> <port> <seed> <payload>\n");
    return EXIT_FAILURE;
  }

  if (mam_api_init(&api, (tryte_t *)av[3]) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  trits_t cha_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
  trits_from_str(cha_name, TEST_CHANNEL_NAME);
  mam_channel_t *cha = malloc(sizeof(mam_channel_t));
  memset(cha, 0, sizeof(mam_channel_t));
  mam_channel_create(&api.prng, TEST_MSS_DEPTH, cha_name, cha);

  ERR_BIND_RETURN(mam_channel_t_set_add(api.channels, cha), ret);
  bundle_transactions_new(&bundle);

  {
    mam_psk_t_set_t psks = NULL;

    mam_psk_t_set_add(&psks, &psk);
    mam_api_bundle_write_header(&api, cha, NULL, NULL, NULL, psks, NULL, 0,
                                bundle, msg_id);
  }

  {
    tryte_t *payload_trytes =
        (tryte_t *)malloc(2 * strlen(av[4]) * sizeof(tryte_t));

    ascii_to_trytes(av[4], payload_trytes);
    mam_api_bundle_write_packet(&api, cha, msg_id, payload_trytes,
                                strlen(av[4]) * 2, 0, bundle);
    free(payload_trytes);
  }

  send_bundle(av[1], atoi(av[2]), bundle);
  bundle_transactions_free(&bundle);

  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
