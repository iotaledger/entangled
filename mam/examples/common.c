/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/examples/common.h"
#include "mam/mam/mam_channel_t_set.h"

mam_psk_t const psk = {
    .id = {1,  0,  -1, -1, 0,  -1, -1, 0,  0,  1,  -1, 0,  1,  0,  0,  1,  1,
           1,  -1, 1,  1,  0,  1,  1,  0,  0,  -1, 1,  -1, -1, -1, -1, -1, -1,
           -1, 1,  -1, -1, 0,  -1, -1, 1,  0,  -1, -1, -1, 1,  1,  1,  0,  0,
           -1, 1,  -1, -1, -1, 0,  -1, 1,  -1, -1, -1, 1,  1,  -1, 1,  0,  0,
           1,  1,  1,  -1, -1, 0,  0,  -1, -1, 1,  0,  -1, 1},
    .key = {-1, 1,  -1, -1, 1,  -1, -1, 0,  0,  0,  -1, -1, 1,  1,  1,  -1, -1,
            -1, 0,  0,  0,  0,  -1, -1, 1,  1,  1,  0,  -1, -1, -1, 0,  0,  0,
            -1, -1, 1,  -1, 0,  0,  1,  0,  0,  -1, 1,  1,  0,  -1, 0,  0,  1,
            -1, 1,  0,  1,  0,  0,  -1, 1,  1,  -1, 1,  0,  -1, 0,  -1, 1,  -1,
            -1, -1, 0,  -1, -1, 0,  -1, -1, 0,  0,  -1, -1, 1,  -1, 0,  0,  -1,
            -1, -1, -1, 0,  -1, -1, -1, 1,  -1, -1, 1,  1,  1,  1,  1,  0,  1,
            0,  1,  -1, 0,  0,  1,  0,  1,  0,  0,  1,  0,  -1, 0,  1,  1,  0,
            0,  -1, -1, 1,  1,  0,  0,  1,  -1, 1,  1,  1,  0,  1,  1,  1,  0,
            0,  -1, -1, -1, -1, 1,  1,  1,  0,  0,  -1, 0,  1,  -1, 1,  1,  1,
            0,  0,  1,  -1, -1, 0,  -1, 1,  -1, 1,  0,  0,  1,  -1, 0,  1,  -1,
            0,  0,  1,  1,  1,  1,  1,  0,  0,  1,  -1, 1,  -1, 1,  0,  1,  1,
            1,  -1, 0,  0,  -1, 1,  1,  0,  -1, -1, 0,  0,  -1, 1,  0,  1,  -1,
            0,  0,  -1, 1,  -1, 1,  1,  1,  -1, 0,  1,  1,  0,  0,  -1, -1, -1,
            0,  0,  1,  0,  1,  0,  -1, 1,  -1, 0,  1,  0,  -1, 1,  1,  -1, -1,
            0,  0,  -1, 0,  -1}};

retcode_t mam_example_create_channel(mam_api_t *const api,
                                     mam_channel_t **const channel) {
  tryte_t address[NUM_TRYTES_ADDRESS];

  if (mam_channel_t_set_size(api->channels) == 0) {
    trits_t channel_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));

    trits_from_str(channel_name, TEST_CHANNEL_NAME);
    if ((*channel = malloc(sizeof(mam_channel_t))) == NULL) {
      fprintf(stderr, "malloc failed\n");
      return EXIT_FAILURE;
    }
    mam_channel_create(&api->prng, TEST_MSS_DEPTH, channel_name, *channel);

    mam_api_add_channel(api, *channel);
  }
  *channel = &api->channels->value;

  trits_to_trytes(mam_channel_id(*channel).p, address, NUM_TRITS_ADDRESS);
  fprintf(stderr, "Address: ");
  for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
    fprintf(stderr, "%c", address[i]);
  }
  fprintf(stderr, "\n");

  return RC_OK;
}

// TODO Merge into cclient
retcode_t send_bundle(char const *const host, uint16_t const port,
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

  fprintf(stderr, "Bundle: ");
  for (size_t i = 0; i < FLEX_TRIT_SIZE_243; i++) {
    fprintf(stderr, "%c",
            ((iota_transaction_t *)utarray_front(bundle))->essence.bundle[i]);
  }
  fprintf(stderr, "\n");

  return RC_OK;
}

static int idx_sort(void const *lhs, void const *rhs) {
  iota_transaction_t *_lhs = (iota_transaction_t *)lhs;
  iota_transaction_t *_rhs = (iota_transaction_t *)rhs;

  return (transaction_current_index(_lhs) < transaction_current_index(_rhs))
             ? -1
             : (transaction_current_index(_lhs) >
                transaction_current_index(_rhs));
}

static void get_first_bundle_from_transactions(
    transaction_array_t const transactions,
    bundle_transactions_t *const bundle) {
  iota_transaction_t *curr_tx;

  utarray_sort(transactions, idx_sort);

  TX_OBJS_FOREACH(transactions, curr_tx) {
    bundle_transactions_add(bundle, curr_tx);
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
  }

  return out_tx_objs;
}

// TODO Merge into cclient
retcode_t receive_bundle(char const *const host, uint16_t const port,
                         tryte_t const *const bundle_hash,
                         bundle_transactions_t *const bundle) {
  iota_client_service_t serv;
  recv_example_init_client_service(&serv, host, port);
  iota_client_extended_init();

  transaction_array_t out_tx_objs = get_bundle_transactions(&serv, bundle_hash);

  get_first_bundle_from_transactions(out_tx_objs, bundle);

  transaction_array_free(out_tx_objs);

  iota_client_extended_destroy();
  iota_client_core_destroy(&serv);

  hash243_queue_free(&recv_example_req.bundles);
  recv_example_req.bundles = NULL;
  return RC_OK;
}
