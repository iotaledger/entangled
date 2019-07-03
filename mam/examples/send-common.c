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

retcode_t mam_example_announce_channel(mam_api_t* const api, tryte_t const* const channel_id,
                                       bundle_transactions_t* const bundle, trit_t* const msg_id,
                                       tryte_t* const new_channel_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  mam_psk_t_set_t psks = NULL;

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  ERR_BIND_RETURN(mam_api_channel_create(api, MAM_EXAMPLES_MSS_HEIGHT, new_channel_id), ret);

  if ((ret = mam_api_bundle_announce_channel(api, channel_id, new_channel_id, psks, NULL, bundle, msg_id)) != RC_OK) {
    return ret;
  }
  mam_psk_t_set_free(&psks);

  trits_to_trytes(msg_id, msg_id_trytes, MAM_MSG_ID_SIZE);
  fprintf(stderr, "Message ID: ");
  for (size_t i = 0; i < MAM_MSG_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", msg_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "CH1 ID: ");
  for (size_t i = 0; i < MAM_CHANNEL_ID_TRYTE_SIZE; i++) {
    fprintf(stderr, "%c", new_channel_id[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_announce_endpoint(mam_api_t* const api, tryte_t const* const channel_id,
                                        bundle_transactions_t* const bundle, trit_t* const msg_id,
                                        tryte_t* const new_endpoint_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  mam_psk_t_set_t psks = NULL;

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  ERR_BIND_RETURN(mam_api_endpoint_create(api, MAM_EXAMPLES_MSS_HEIGHT, channel_id, new_endpoint_id), ret);

  if ((ret = mam_api_bundle_announce_endpoint(api, channel_id, new_endpoint_id, psks, NULL, bundle, msg_id)) != RC_OK) {
    return ret;
  }
  mam_psk_t_set_free(&psks);

  trits_to_trytes(msg_id, msg_id_trytes, MAM_MSG_ID_SIZE);
  fprintf(stderr, "Message ID: ");
  for (size_t i = 0; i < MAM_MSG_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", msg_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "EP1 ID: ");
  for (size_t i = 0; i < MAM_ENDPOINT_ID_TRYTE_SIZE; i++) {
    fprintf(stderr, "%c", new_endpoint_id[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_write_header_on_channel(mam_api_t* const api, tryte_t const* const channel_id,
                                              bundle_transactions_t* const bundle, trit_t* const msg_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / 3];
  mam_psk_t_set_t psks = NULL;

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  if ((ret = mam_api_bundle_write_header_on_channel(api, channel_id, psks, NULL, bundle, msg_id)) != RC_OK) {
    return ret;
  }
  mam_psk_t_set_free(&psks);

  trits_to_trytes(msg_id, msg_id_trytes, MAM_MSG_ID_SIZE);
  fprintf(stderr, "Message ID: ");
  for (size_t i = 0; i < MAM_MSG_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", msg_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_write_header_on_endpoint(mam_api_t* const api, tryte_t const* const channel_id,
                                               tryte_t const* const endpoint_id, bundle_transactions_t* const bundle,
                                               trit_t* const msg_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / 3];
  mam_psk_t_set_t psks = NULL;

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  if ((ret = mam_api_bundle_write_header_on_endpoint(api, channel_id, endpoint_id, psks, NULL, bundle, msg_id)) !=
      RC_OK) {
    return ret;
  }

  mam_psk_t_set_free(&psks);

  trits_to_trytes(msg_id, msg_id_trytes, MAM_MSG_ID_SIZE);
  fprintf(stderr, "Message ID: ");
  for (size_t i = 0; i < MAM_MSG_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", msg_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_write_packet(mam_api_t* const api, bundle_transactions_t* const bundle, char const* const payload,
                                   trit_t const* const msg_id, bool is_last_packet) {
  retcode_t ret = RC_OK;
  tryte_t* payload_trytes = (tryte_t*)malloc(2 * strlen(payload) * sizeof(tryte_t));

  ascii_to_trytes(payload, payload_trytes);
  if ((ret = mam_api_bundle_write_packet(api, msg_id, payload_trytes, strlen(payload) * 2, 0, is_last_packet,
                                         bundle)) != RC_OK) {
    return ret;
  }
  free(payload_trytes);

  return ret;
}
