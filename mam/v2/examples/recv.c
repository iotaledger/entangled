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
#include "mam/v2/examples/common.h"

static void get_first_bundle_from_transactions(
    transaction_array_t const transactions,
    bundle_transactions_t *const bundle) {
  iota_transaction_t *tail = NULL;
  TX_OBJS_FOREACH(transactions, tail) {
    if (transaction_current_index(tail) == 0) {
      break;
    }
  }

  if (tail == NULL) {
    return;
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

static void recv_example_init_client_service(
    iota_client_service_t *const serv) {
  serv->http.path = "/";
  serv->http.content_type = "application/json";
  serv->http.accept = "application/json";
  serv->http.host = HOST;
  serv->http.port = PORT;
  serv->http.api_version = 1;
  serv->serializer_type = SR_JSON;
  iota_client_core_init(serv);
}

static void get_bundle_transactions(iota_client_service_t *const serv,
                                    flex_trit_t const *const bundle_hash,
                                    transaction_array_t *const out_tx_objs) {
  recv_example_req.approvees = NULL;
  recv_example_req.bundles = NULL;
  recv_example_req.tags = NULL;
  recv_example_req.addresses = NULL;
  hash243_queue_push(&recv_example_req.bundles, bundle_hash);
  // TODO - replace with iota_client_get_bundle when it's implemented
  retcode_t err = iota_client_find_transaction_objects(serv, &recv_example_req,
                                                       *out_tx_objs);
  if (err != RC_OK) {
    fprintf(stderr, "iota_client_find_transaction_objects failed with %d\n",
            err);
    return;
  } else {
    fprintf(stderr, "iota_client_find_transaction_objects succeeded\n");
  }

  if (utarray_len(*out_tx_objs) > 0) {
    fprintf(stderr, "number of transactions for given address: %d\n",
            utarray_len(*out_tx_objs));
  }
}

static void receive_bundle_public_channel(
    mam_api_t const *const api, flex_trit_t const *const bundle_hash) {
  retcode_t err;
  iota_client_service_t serv;
  recv_example_init_client_service(&serv);
  iota_client_extended_init();

  transaction_array_t out_tx_objs = transaction_array_new();
  get_bundle_transactions(&serv, bundle_hash, &out_tx_objs);

  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  if (out_tx_objs == NULL) {
    return;
  }

  get_first_bundle_from_transactions(out_tx_objs, bundle);

  if (bundle_transactions_size(bundle) == 0) {
    return;
  }

  flex_trit_t *packet_payload = NULL;
  err = mam_api_bundle_read_msg(api, bundle, &packet_payload);
  if (err == RC_OK) {
    fprintf(stderr, "mam_api_bundle_read_msg succeeded\n");
  } else {
    fprintf(stderr, "mam_api_bundle_read_msg failed with err: %d\n", err);
  }

  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);
  bundle_transactions_free(&bundle);
  hash243_queue_free(&recv_example_req.bundles);
  recv_example_req.bundles = NULL;
}
// TODO Merge into cclient
static void receive_bundle_psk_channel(mam_api_t const *const api,
                                       flex_trit_t const *const bundle_hash) {
  retcode_t err;
  iota_client_service_t serv;
  recv_example_init_client_service(&serv);
  iota_client_extended_init();

  transaction_array_t out_tx_objs = transaction_array_new();
  get_bundle_transactions(&serv, bundle_hash, &out_tx_objs);

  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  get_first_bundle_from_transactions(out_tx_objs, bundle);

  if (bundle_transactions_size(bundle) == 0) {
    return;
  }

  flex_trit_t *packet_payload = NULL;

  mam_psk_t_set_add(&api->psks, &psk);
  err = mam_api_bundle_read_msg(api, bundle, &packet_payload);
  if (err == RC_OK) {
    fprintf(stderr, "mam_api_bundle_read_msg succeeded\n");
  } else {
    fprintf(stderr, "mam_api_bundle_read_msg failed with err: %d\n", err);
  }

  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);
  bundle_transactions_free(&bundle);
  hash243_queue_free(&recv_example_req.bundles);
  recv_example_req.bundles = NULL;
}

int main(void) {
  mam_api_t api;
  int ret = EXIT_SUCCESS;

  if (mam_api_init(&api, SENDER_SEED) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  flex_trit_t bundle_hash_public_example[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(bundle_hash_public_example, NUM_TRITS_HASH,
                         BUNDLE_HASH, NUM_TRITS_HASH, NUM_TRYTES_BUNDLE);

  receive_bundle_public_channel(&api, bundle_hash_public_example);

  flex_trit_t bundle_hash_psk_exmaple[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(bundle_hash_psk_exmaple, NUM_TRITS_HASH,
                         BUNDLE_HASH_PSK_EXAMPLE, NUM_TRITS_HASH,
                         NUM_TRYTES_BUNDLE);

  receive_bundle_psk_channel(&api, bundle_hash_psk_exmaple);

  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
