/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_API_API_H__
#define __MAM_V2_API_API_H__

#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/trinary/flex_trit.h"
#include "mam/v2/mam/mam.h"
#include "mam/v2/mam/mam_types.h"
#include "mam/v2/ntru/ntru_types.h"
#include "mam/v2/prng/prng_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_api_s {
  mam_prng_t prng;
  mam_ntru_sk_t_set_t ntru_sks;
  mam_ntru_pk_t_set_t ntru_pks;
  mam_psk_t_set_t psks;
} mam_api_t;

retcode_t mam_api_init(mam_api_t *const api, trits_t const prng_secret_key);

retcode_t mam_api_destroy(mam_api_t *const api);

retcode_t mam_api_add_ntru_sk(mam_api_t *const api,
                              mam_ntru_sk_t const *const ntru_sk);

retcode_t mam_api_add_ntru_pk(mam_api_t *const api,
                              mam_ntru_pk_t const *const ntru_pk);

retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk);

retcode_t mam_api_bundle_write_msg(
    mam_api_t *const api, mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t const *const cha,
    mam_endpoint_t const *const epa, mam_channel_t const *const ch1a,
    mam_endpoint_t const *const ep1a, flex_trit_t const *const payload,
    bundle_transactions_t *const bundle);

retcode_t mam_api_bundle_write_packet(
    mam_api_t *const api, mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t const *const cha,
    mam_endpoint_t const *const epa, mam_channel_t const *const ch1a,
    mam_endpoint_t const *const ep1a, flex_trit_t const *const payload,
    bundle_transactions_t *const bundle);

retcode_t mam_api_bundle_read(mam_api_t *const api,
                              mam_channel_t const *const cha,
                              bundle_transactions_t const *const bundle,
                              flex_trit_t *const payload);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_API_API_H__
