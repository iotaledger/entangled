/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_BUNDLE_BUNDLE_H__
#define __MAM_V2_BUNDLE_BUNDLE_H__

#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/trinary/flex_trit.h"
#include "mam/v2/mam/mam.h"
#include "mam/v2/state/state.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t mam_bundle_write_msg(
    mam_state_t *const state, mam_msg_pubkey_t pubkey,
    mam_msg_keyload_t keyload, mam_msg_checksum_t checksum,
    mam_channel_t const *const cha, mam_endpoint_t const *const epa,
    mam_channel_t const *const ch1a, mam_endpoint_t const *const ep1a,
    flex_trit_t const *const payload, bundle_transactions_t *const bundle);

retcode_t mam_bundle_write_packet(
    mam_state_t *const state, mam_msg_pubkey_t pubkey,
    mam_msg_keyload_t keyload, mam_msg_checksum_t checksum,
    mam_channel_t const *const cha, mam_endpoint_t const *const epa,
    mam_channel_t const *const ch1a, mam_endpoint_t const *const ep1a,
    flex_trit_t const *const payload, bundle_transactions_t *const bundle);

retcode_t mam_bundle_read(mam_state_t *const state,
                          mam_channel_t const *const cha,
                          bundle_transactions_t const *const bundle,
                          flex_trit_t *const payload);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_BUNDLE_BUNDLE_H__
