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

retcode_t mam_example_write_header(mam_api_t* const api,
                                   mam_channel_t* const channel,
                                   bundle_transactions_t* const bundle,
                                   trit_t* const msg_id) {
  retcode_t ret = RC_OK;
  tryte_t msg_id_trytes[MAM_MSG_ID_SIZE / 3];
  mam_psk_t_set_t psks = NULL;

  if ((ret = mam_psk_t_set_add(&psks, &psk)) != RC_OK) {
    return ret;
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
                                   mam_channel_t* const channel,
                                   bundle_transactions_t* const bundle,
                                   char const* const payload,
                                   trit_t const* const msg_id) {
  retcode_t ret = RC_OK;
  tryte_t* payload_trytes =
      (tryte_t*)malloc(2 * strlen(payload) * sizeof(tryte_t));

  ascii_to_trytes(payload, payload_trytes);
  if ((ret = mam_api_bundle_write_packet(api, channel, msg_id, payload_trytes,
                                         strlen(payload) * 2, 0, bundle)) !=
      RC_OK) {
    return ret;
  }
  free(payload_trytes);

  return ret;
}
