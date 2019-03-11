/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_TEST_UTILS_TEST_UTILS_H__
#define __MAM_TEST_UTILS_TEST_UTILS_H__

#include "mam/prng/prng.h"

#ifdef __cplusplus
extern "C" {
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

#endif  // __MAM_TEST_UTILS_TEST_UTILS_H__
