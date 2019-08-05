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
#include "mam/api/api.h"
#include "mam/ntru/ntru.h"

#define SENDERSEED "SENDERSEED9SENDERSEED9SENDERSEED9SENDERSEED9SENDERSEED9SENDERSEED9SENDERSEED99999"
#define RECEIVERSEED "RECEIVERSEED9RECEIVERSEED9RECEIVERSEED9RECEIVERSEED9RECEIVERSEED9RECEIVERSEED9999"
#define KEYSEED "AAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCCAAABBBCCC"
#define NONCE "ABCDEFGHIJKLMNOPQRSTUVWXYZABCD"
int main() {
  //Generating a keypair for the receiver api
  mam_prng_t prng;
  mam_ntru_sk_t ntru;
  MAM_TRITS_DEF(nonce, 3 * 10);
  MAM_TRITS_DEF(key, MAM_PRNG_SECRET_KEY_SIZE);
  nonce = MAM_TRITS_INIT(nonce, 3 * 10);
  key = MAM_TRITS_INIT(key, MAM_PRNG_SECRET_KEY_SIZE);
  trits_from_str(key,KEYSEED);
  trits_from_str(nonce,NONCE);
  mam_prng_init(&prng, key);
  ntru_sk_reset(&ntru);
  ntru_sk_gen(&ntru, &prng, nonce);
  //Setting up two APIs
  mam_api_t sender_api;
  tryte_t *sender_seed = (tryte_t *)SENDERSEED;
  mam_api_init(&sender_api, sender_seed);
  mam_api_t receiver_api;
  tryte_t *receiver_seed = (tryte_t *)RECEIVERSEED;
  mam_api_init(&receiver_api, receiver_seed);
  //Setting the generated keypair to the receiver API
  mam_api_add_ntru_sk(&receiver_api, &ntru);
  //Adding the receiver APIs public key to the sender API public key list
  mam_api_add_ntru_pk(&sender_api, &ntru.public_key);
  //Preparing channel and endpoint with sender API with randomly chosen height
  tryte_t channel_id[MAM_CHANNEL_ID_TRYTE_SIZE];
  mam_api_channel_create(&sender_api, 2, channel_id);
  tryte_t endpoint_id[MAM_ENDPOINT_ID_TRYTE_SIZE];
  mam_api_endpoint_create(&sender_api, 2, channel_id, endpoint_id);
  //prepare bundle for sender API to write to
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  //prepare and write header and packet
  trit_t message_id[MAM_MSG_ID_SIZE];
  mam_api_bundle_write_header_on_endpoint(&sender_api, channel_id, endpoint_id, NULL, NULL, bundle, message_id);
  mam_api_bundle_write_packet(&sender_api, message_id, (tryte_t *)"PAYLOADTEST", 11, MAM_MSG_CHECKSUM_SIG, true, bundle);
  //Set created channel and endpoint as trusted in receiver API
  mam_api_add_trusted_channel_pk(&receiver_api, channel_id);
  mam_api_add_trusted_endpoint_pk(&receiver_api, endpoint_id);
  //read the bundle with the receiver API and print the decrypted payload
  tryte_t *payload = NULL;
  size_t payload_size = 0;
  bool is_last_packet = false;
  mam_api_bundle_read(&receiver_api, bundle, &payload, &payload_size, &is_last_packet);
  fprintf(stderr, "Payload: ");
  for (size_t i = 0; i < payload_size; i++) {
    fprintf(stderr, "%c", payload[i]);
  }
  fprintf(stderr, "\n");
  //free ressources
  free(payload);
  bundle_transactions_free(&bundle);
  mam_api_destroy(&sender_api);
  mam_api_destroy(&receiver_api);
  //finished
  return 0;
}
