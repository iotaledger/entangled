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
#include "mam/v2/mam/mam_types.h"
#include "mam/v2/mam/message.h"
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

retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed);

retcode_t mam_api_destroy(mam_api_t *const api);

retcode_t mam_api_add_ntru_sk(mam_api_t *const api,
                              mam_ntru_sk_t const *const ntru_sk);

retcode_t mam_api_add_ntru_pk(mam_api_t *const api,
                              mam_ntru_pk_t const *const ntru_pk);

retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk);

retcode_t mam_api_bundle_write_header(
    mam_api_t *const api, mam_channel_t const *const ch,
    mam_endpoint_t const *const ep, mam_channel_t const *const ch1,
    mam_endpoint_t const *const ep1, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle);

retcode_t mam_api_bundle_write_packet(mam_api_t *const api,
                                      tryte_t const *const payload,
                                      mam_msg_checksum_t checksum,
                                      bundle_transactions_t *const bundle);

retcode_t mam_api_bundle_read(mam_api_t *const api,
                              mam_channel_t const *const cha,
                              bundle_transactions_t const *const bundle,
                              flex_trit_t *const payload);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_API_API_H__
