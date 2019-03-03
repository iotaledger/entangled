/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_TEST_UTILS_TEST_UTILS_H__
#define __MAM_V2_TEST_UTILS_TEST_UTILS_H__

#include <stdlib.h>

#include "mam/mss/mss.h"
#include "mam/ntru/ntru.h"
#include "mam/ntru/poly.h"
#include "mam/prng/prng.h"
#include "mam/sponge/sponge.h"
#include "mam/sponge/spongos.h"
#include "mam/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MAM_MSS_TEST_MAX_D)
#define MAM_MSS_TEST_MAX_D 3
#endif

/**
 * PRNG output generation
 *
 * @param prng A PRNG interface
 * @param destination A destination tryte
 * @param nonce The nonce as a string
 * @param output Pseudorandom output trits
 */
void prng_gen_str(mam_prng_t *prng, tryte_t destination, char const *nonce,
                  trits_t output);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_TEST_UTILS_TEST_UTILS_H__
