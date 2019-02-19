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
#include "mam/v2/api/api.h"

static tryte_t SENDER_SEED[81] =
    "SENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSEND"
    "ERSEED9";
#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_MSS_DEPTH 1
#define HOST "173.249.44.234"
#define PORT 14265

// TODO Merge into cclient
static void send_bundle(bundle_transactions_t *const bundle) {
  iota_client_service_t serv;
  serv.http.path = "/";
  serv.http.content_type = "application/json";
  serv.http.accept = "application/json";
  serv.http.host = HOST;
  serv.http.port = PORT;
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
  BUNDLE_FOREACH(bundle, curr_tx) {
    for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
      fprintf(stderr, "%c", curr_tx->essence.address[i]);
    }
    fprintf(stderr, "\n");
    transaction_serialize_on_flex_trits(curr_tx, trits_8019);
    hash_array_push(raw_trytes, trits_8019);
  }
  iota_client_send_trytes(&serv, raw_trytes, 1, 14, NULL, true, out_tx_objs);
  hash_array_free(raw_trytes);
  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);
}

int main(void) {
  mam_api_t api;
  bundle_transactions_t *bundle = NULL;
  int ret = EXIT_SUCCESS;

  if (mam_api_init(&api, SENDER_SEED) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  trits_t cha_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
  trits_from_str(cha_name, TEST_CHANNEL_NAME);
  mam_channel_t *cha = malloc(sizeof(mam_channel_t));
  memset(cha, 0, sizeof(mam_channel_t));
  mam_channel_create(&api.prng, TEST_MSS_DEPTH, cha_name, cha);

  bundle_transactions_new(&bundle);
  mam_api_bundle_write_header(&api, cha, NULL, NULL, NULL, NULL, NULL, 0,
                              bundle);
  send_bundle(bundle);
  bundle_transactions_free(&bundle);

  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
