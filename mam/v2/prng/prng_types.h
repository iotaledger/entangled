/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_PRNG_PRNG_TYPES_H__
#define __MAM_V2_PRNG_PRNG_TYPES_H__

#include "mam/v2/defs.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"

// PRNG secret key size
#define MAM2_PRNG_KEY_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

typedef enum prng_destination_tryte_e {
  // PRNG AE destination tryte
  MAM2_PRNG_DST_SEC_KEY = 0,
  // PRNG WOTS destination tryte
  MAM2_PRNG_DST_WOTS_KEY = 1,
  // PRNG NTRU destination tryte
  MAM2_PRNG_DST_NTRU_KEY = 2
} prng_destination_tryte_t;

// PRNG layer interface
typedef struct prng_s {
  mam_sponge_t* sponge;
  trit_t* secret_key;
} mam_prng_t;

typedef struct mam_prng_t_set_entry_s mam_prng_t_set_entry_t;
typedef mam_prng_t_set_entry_t* mam_prng_t_set_t;

size_t mam_prngs_serialized_size(mam_prng_t_set_t const prng_set);
retcode_t mam_prngs_serialize(mam_prng_t_set_t const prng_set, trits_t trits);
retcode_t mam_prngs_deserialize(trits_t const trits,
                                mam_prng_t_set_t* const prng_set);

size_t mam_prng_serialized_size(mam_prng_t const* const prng);
retcode_t mam_prng_serialize(mam_prng_t const* const prng, trits_t trits);
retcode_t mam_prng_deserialize(trits_t const trits, mam_prng_t* const prng);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_PRNG_PRNG_TYPES_H__
