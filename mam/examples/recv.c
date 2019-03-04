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

#include "cclient/api/extended/extended_api.h"
#include "common/trinary/tryte_ascii.h"
#include "mam/api/api.h"
#include "mam/examples/common.h"

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
static void recv_example_init_client_service(iota_client_service_t *const serv,
                                             char const *const host,
                                             uint16_t const port) {
  serv->http.path = "/";
  serv->http.content_type = "application/json";
  serv->http.accept = "application/json";
  serv->http.host = host;
  serv->http.port = port;
  serv->http.api_version = 1;
  serv->serializer_type = SR_JSON;
  iota_client_core_init(serv);
}

static transaction_array_t get_bundle_transactions(
    iota_client_service_t *const serv, flex_trit_t const *const bundle_hash) {
  flex_trit_t bundle_hash_flex[FLEX_TRIT_SIZE_243];
  transaction_array_t out_tx_objs = transaction_array_new();
  recv_example_req.approvees = NULL;
  recv_example_req.bundles = NULL;
  recv_example_req.tags = NULL;
  recv_example_req.addresses = NULL;

  flex_trits_from_trytes(bundle_hash_flex, NUM_TRITS_BUNDLE, bundle_hash,
                         NUM_TRITS_HASH, NUM_TRYTES_BUNDLE);
  hash243_queue_push(&recv_example_req.bundles, bundle_hash);
  // TODO - replace with iota_client_get_bundle when it's implemented
  retcode_t err = iota_client_find_transaction_objects(serv, &recv_example_req,
                                                       out_tx_objs);
  if (err != RC_OK) {
    fprintf(stderr, "iota_client_find_transaction_objects failed with %d\n",
            err);
    return out_tx_objs;
  } else {
    fprintf(stderr, "iota_client_find_transaction_objects succeeded\n");
  }

  if (utarray_len(out_tx_objs) > 0) {
    fprintf(stderr, "number of transactions for given bundle: %d\n",
            utarray_len(out_tx_objs));
  }

  return out_tx_objs;
}

// TODO Merge into cclient
static void receive_bundle(mam_api_t *const api, char const *const host,
                           uint16_t const port,
                           tryte_t const *const bundle_hash) {
  retcode_t err;
  iota_client_service_t serv;
  recv_example_init_client_service(&serv, host, port);
  iota_client_extended_init();

  transaction_array_t out_tx_objs = get_bundle_transactions(&serv, bundle_hash);

  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  get_first_bundle_from_transactions(out_tx_objs, bundle);

  tryte_t *payload_trytes = NULL;
  size_t payload_size = 0;

  mam_psk_t_set_add(&api->psks, &psk);
  err = mam_api_bundle_read_msg(api, bundle, &payload_trytes, &payload_size);

  if (err == RC_OK) {
    char *payload = calloc(payload_size * 2 + 1, sizeof(char));

    trytes_to_ascii(payload_trytes, payload_size, payload);
    fprintf(stderr, "Payload: %s\n", payload);
    free(payload);

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

int main(int ac, char **av) {
  mam_api_t api;
  int ret = EXIT_SUCCESS;

  if (ac != 4) {
    fprintf(stderr, "usage: recv <host> <port> <bundle>\n");
    return EXIT_FAILURE;
  }

  if (mam_api_init(&api, (tryte_t *)DUMMY_SEED) != RC_OK) {
    fprintf(stderr, "mam_api_init failed\n");
    return EXIT_FAILURE;
  }

  receive_bundle(&api, av[1], atoi(av[2]), av[3]);

  if (mam_api_destroy(&api) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
