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

retcode_t mam_example_announce_new_channel(mam_api_t* const api,
                                           mam_channel_t* const channel,
                                           bundle_transactions_t* const bundle,
                                           trit_t* const msg_id,
                                           mam_channel_t** const new_channel) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  tryte_t new_ch_id_trytes[MAM_CHANNEL_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  mam_psk_t_set_t psks = NULL;

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  ERR_BIND_RETURN(mam_api_create_channel(api, TEST_MSS_DEPTH, new_ch_id_trytes),
                  ret);

  *new_channel = mam_api_get_channel(api, new_ch_id_trytes);

  if (*new_channel == NULL) {
    return RC_MAM_API_FAILED_CREATE_CHANNEL;
  }

  if ((ret = mam_api_bundle_write_header_announce_new_channel(
           api, channel, *new_channel, psks, NULL, 0, bundle, msg_id)) !=
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

  fprintf(stderr, "CH1 ID: ");
  for (size_t i = 0; i < MAM_CHANNEL_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", new_ch_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_announce_new_endpoint(
    mam_api_t* const api, mam_channel_t* const channel,
    bundle_transactions_t* const bundle, trit_t* const msg_id,
    mam_endpoint_t** const new_endpoint) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  tryte_t ch_id_trytes[MAM_CHANNEL_ID_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  mam_psk_t_set_t psks = NULL;
  tryte_t ep_id_trytes[MAM_ENDPOINT_ID_SIZE];

  if (!mam_psk_t_set_contains(psks, &psk)) {
    if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
      return ret;
    }
  }

  trits_to_trytes(trits_begin(mam_channel_id(channel)), ch_id_trytes,
                  MAM_CHANNEL_ID_SIZE);
  ERR_BIND_RETURN(
      mam_api_create_endpoint(api, TEST_MSS_DEPTH, ch_id_trytes, ep_id_trytes),
      ret);

  *new_endpoint = mam_api_get_endpoint(api, ch_id_trytes, ep_id_trytes);

  if (*new_endpoint == NULL) {
    return RC_MAM_API_FAILED_CREATE_ENDPOINT;
  }

  if ((ret = mam_api_bundle_write_header_announce_new_endpoint(
           api, channel, *new_endpoint, psks, NULL, 0, bundle, msg_id)) !=
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

  fprintf(stderr, "EP1 ID: ");
  for (size_t i = 0; i < MAM_ENDPOINT_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", ep_id_trytes[i]);
  }
  fprintf(stderr, "\n");

  return ret;
}

retcode_t mam_example_write_header(mam_api_t* const api,
                                   mam_channel_t* const channel,
                                   bundle_transactions_t* const bundle,
                                   trit_t* const msg_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / 3];
  mam_psk_t_set_t psks = NULL;
  mam_channel_t* ch_new = NULL;

  if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
    return ret;
  }
  if (mam_mss_num_remaining_sks(&channel->mss) == 0) {
    // TODO
    // - remove old ch/ep
    // - create new ch/ep
    // - add ch/ep via `mam_api_add_channel/mam_api_add_endpoint`

    return RC_OK;
  }

  if ((ret = mam_api_bundle_write_header(api, channel, NULL, NULL, NULL, psks,
                                         NULL, 0, bundle, msg_id)) != RC_OK) {
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

retcode_t mam_example_write_packet(mam_api_t* const api,
                                   bundle_transactions_t* const bundle,
                                   char const* const payload,
                                   trit_t const* const msg_id,
                                   bool is_last_packet) {
  retcode_t ret = RC_OK;
  tryte_t* payload_trytes =
      (tryte_t*)malloc(2 * strlen(payload) * sizeof(tryte_t));

  ascii_to_trytes(payload, payload_trytes);
  if ((ret = mam_api_bundle_write_packet(api, msg_id, payload_trytes,
                                         strlen(payload) * 2, 0, is_last_packet,
                                         bundle)) != RC_OK) {
    return ret;
  }
  free(payload_trytes);

  return ret;
}
