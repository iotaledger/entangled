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

#include "cclient/iota_client_extended_api.h"
#include "common/trinary/tryte_ascii.h"
#include "mam/v2/api/api.h"
#include "mam/v2/examples/common.h"

// TODO Merge into cclient
static void send_bundle(char const *const host, uint16_t const port,
                        bundle_transactions_t *const bundle) {
  iota_client_service_t serv;
  serv.http.path = "/";
  serv.http.content_type = "application/json";
  serv.http.accept = "application/json";
  serv.http.host = host;
  serv.http.port = port;
  serv.http.api_version = 1;
  serv.serializer_type = SR_JSON;
  iota_client_core_init(&serv);
  iota_client_extended_init();

  Kerl kerl;
  init_kerl(&kerl);
  bundle_finalize(bundle, &kerl);
  transaction_array_t out_tx_objs = transaction_array_new();
  hash8019_array_p raw_trytes = hash8019_array_new();
  iota_transaction_t *curr_tx = NULL;
  flex_trit_t trits_8019[FLEX_TRIT_SIZE_8019];

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

  BUNDLE_FOREACH(bundle, curr_tx) {
    transaction_serialize_on_flex_trits(curr_tx, trits_8019);
    hash_array_push(raw_trytes, trits_8019);
  }
  iota_client_send_trytes(&serv, raw_trytes, 1, 14, NULL, true, out_tx_objs);
  // TODO uncomment
  // hash_array_free(raw_trytes);
  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);
}

int main(int ac, char **av) {
  mam_api_t api;
  bundle_transactions_t *bundle = NULL;
  int ret = EXIT_SUCCESS;
  trit_t msg_id[MAM2_MSG_ID_SIZE];

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
