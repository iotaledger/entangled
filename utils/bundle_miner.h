/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_BUNDLE_MINER_H__
#define __UTILS_BUNDLE_MINER_H__

#include "common/trinary/bytes.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

double bundle_miner_probability_of_losing(byte_t const *const nb, uint64_t const n);
double bundle_miner_security_level(double const probability, double const radix);
void bundle_miner_min_normalized_bundle(byte_t const *const a, byte_t const *const b, byte_t *const out,
                                        size_t const length);
uint32_t bundle_miner_mine(byte_t const *const min, size_t const number_of_fragments, trit_t const *const essence,
                           size_t const essence_length, uint32_t const offset, uint32_t const count);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_BUNDLE_MINER_H__
