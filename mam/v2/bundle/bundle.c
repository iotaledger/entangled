/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/bundle/bundle.h"

retcode_t mam_bundle_write_msg(
    mam_api_t *const api, mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t const *const cha,
    mam_endpoint_t const *const epa, mam_channel_t const *const ch1a,
    mam_endpoint_t const *const ep1a, flex_trit_t const *const payload,
    bundle_transactions_t *const bundle) {
  return RC_MAM2_NOT_IMPLEMENTED;
}

retcode_t mam_bundle_write_packet(
    mam_api_t *const api, mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t const *const cha,
    mam_endpoint_t const *const epa, mam_channel_t const *const ch1a,
    mam_endpoint_t const *const ep1a, flex_trit_t const *const payload,
    bundle_transactions_t *const bundle) {
  return RC_MAM2_NOT_IMPLEMENTED;
}

retcode_t mam_bundle_read(mam_api_t *const api, mam_channel_t const *const cha,
                          bundle_transactions_t const *const bundle,
                          flex_trit_t *const payload) {
  return RC_MAM2_NOT_IMPLEMENTED;
}
