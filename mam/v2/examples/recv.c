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

static tryte_t BUNDLE_HASH[NUM_TRYTES_BUNDLE] =
    "YKJJHKXLXWHIBNSHZTLEXOOHJXHKWIGGSIGDVKFSCKQZOISJXTN9JPCZGAH9KWJXIKKESSQICC"
    "DEWKZD9";

static tryte_t CH_ID[NUM_TRYTES_ADDRESS] =
    "99999999999999999999999999999ND9999999999999999999999999999999999999999999"
    "9999999";

static void get_first_bundle_from_transactions(
    transaction_array_t const transactions,
    bundle_transactions_t *const bundle) {
  iota_transaction_t *tail;
  TX_OBJS_FOREACH(transactions, tail) {
    if (transaction_current_index(tail) == 0) {
      break;
    }
  }

  bundle_transactions_add(bundle, tail);

  iota_transaction_t *curr_tx;
  iota_transaction_t prev;
  prev = *tail;
  TX_OBJS_FOREACH(transactions, curr_tx) {
    if (transaction_current_index(curr_tx) ==
            (transaction_current_index(&prev) + 1) &&
        (memcmp(transaction_hash(curr_tx), transaction_trunk(&prev),
                FLEX_TRIT_SIZE_243) == 0)) {
      bundle_transactions_add(bundle, curr_tx);
      prev = *curr_tx;
    }
  }
}
// TODO Merge into cclient
static void receive_bundle(mam_api_t const *const api,
                           flex_trit_t const *const ch_id,
                           flex_trit_t const *const bundle_hash) {
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

  transaction_array_t out_tx_objs = transaction_array_new();

  find_transactions_req_t req;
  req.approvees = NULL;
  req.bundles = NULL;
  req.tags = NULL;
  req.addresses = NULL;
  hash243_queue_push(&req.bundles, bundle_hash);
  // TODO - replace with iota_client_get_bundle when it's implemented
  retcode_t err =
      iota_client_find_transaction_objects(&serv, &req, out_tx_objs);
  if (err != RC_OK) {
    fprintf(stderr, "iota_client_find_transaction_objects failed with %d\n",
            err);
    return;
  } else {
    fprintf(stderr, "iota_client_find_transaction_objects succeeded\n");
  }

  if (utarray_len(out_tx_objs) > 0) {
    fprintf(stderr, "number of transactions for given address: %d\n",
            utarray_len(out_tx_objs));
  }

  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  get_first_bundle_from_transactions(out_tx_objs, bundle);

  flex_trit_t *packet_payload = NULL;
  err = mam_api_bundle_read_msg(api, ch_id, bundle, &packet_payload);
  if (err == RC_OK) {
    fprintf(stderr, "mam_api_bundle_read_msg succeeded\n");
  } else {
    fprintf(stderr, "mam_api_bundle_read_msg failed with err: %d\n", err);
  }

  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);
  bundle_transactions_free(&bundle);
  hash243_queue_free(&req.bundles);
}

int main(void) {
  mam_api_t api;
  int ret = EXIT_SUCCESS;

  if (mam_api_init(&api, SENDER_SEED) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(bundle_hash, NUM_TRITS_HASH, BUNDLE_HASH,
                         NUM_TRITS_HASH, NUM_TRYTES_BUNDLE);

  flex_trit_t ch_id[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(ch_id, NUM_TRITS_ADDRESS, CH_ID, NUM_TRITS_ADDRESS,
                         NUM_TRYTES_ADDRESS);
  receive_bundle(&api, ch_id, bundle_hash);
  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
