/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/api/api.h"
#include "mam/v2/mam/mam_psk_t_set.h"
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/prng/prng.h"

retcode_t mam_api_init(mam_api_t *const api, trits_t const prng_secret_key) {
  retcode_t ret = RC_OK;

  api->version = MAM_API_VERSION;
  if ((ret = mam_prng_init(&api->prng, prng_secret_key)) != RC_OK) {
    return ret;
  }
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;

  return ret;
}

retcode_t mam_api_destroy(mam_api_t *const api) {
  retcode_t ret = RC_OK;

  if ((ret = mam_prng_destroy(&api->prng)) != RC_OK) {
    return ret;
  }
  mam_ntru_sk_t_set_free(&api->ntru_sks);
  mam_ntru_pk_t_set_free(&api->ntru_pks);
  mam_psk_t_set_free(&api->psks);

  return RC_OK;
}

retcode_t mam_state_add_ntru_sk(mam_state_t* const state,
                                mam_ntru_sk_t const* const ntru_sk) {
  return mam_ntru_sk_t_set_add(&state->ntru_sks, ntru_sk);
}

retcode_t mam_state_add_ntru_pk(mam_state_t* const state,
                                mam_ntru_pk_t const* const ntru_pk) {
  return mam_ntru_pk_t_set_add(&state->ntru_pks, ntru_pk);
}

retcode_t mam_state_add_psk(mam_state_t* const state,
                            mam_psk_t const* const psk) {
  return mam_psk_t_set_add(&state->psks, psk);
}

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
