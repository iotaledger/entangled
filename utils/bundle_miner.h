/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_BUNDLE_MINER_H__
#define __UTILS_BUNDLE_MINER_H__

#include "common/errors.h"
#include "common/trinary/bytes.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes the probability of an attacker finding a bundle hash resulting in fund loss
 *
 * @param[in] normalized_hash The normalized bundle hash
 * @param[in] security        Desired security (1,2,3)
 *
 * @return The probability
 */
double bundle_miner_probability_of_losing(byte_t const *const normalized_hash, uint8_t const security);

/**
 * @brief Create a new normalized bundle hash made of the maximum bytes of two compared normalized bundle hashes
 *
 * @param[in]   lhs     The first normalized bundle hash
 * @param[in]   rhs     The second normalized bundle hash
 * @param[out]  max     The resulting normalized bundle hash
 * @param[in]   length  The length to compare
 */
void bundle_miner_normalized_bundle_max(byte_t const *const lhs, byte_t const *const rhs, byte_t *const max,
                                        size_t const length);

/**
 * @brief Mines a bundle hash that minimizes the risks of a brute force signature forging attack
 *
 * @param[in]   bundle_normalized_max The maximum already published bundle hash
 * @param[in]   security              The desired security
 * @param[in]   essence               The bundle essence
 * @param[in]   essence_length        The length of the bundle essence
 * @param[in]   count                 The desired number of iterations
 * @param[in]   nprocs                The desired number of threads to run the miner on
 * @param[in]   mining_threshold      If the found index has expected number of adversarial bundles
 *                                    needed in order to exploit published private key fragments that is
 *                                    greater than mining_threshold, then mining should be stopped
 * @param[out]  index                 The resulting index
 *
 * @return an error code
 */
retcode_t bundle_miner_mine(byte_t const *const bundle_normalized_max, uint8_t const security,
                            trit_t const *const essence, size_t const essence_length, uint32_t const count,
                            uint8_t const nprocs, uint64_t mining_threshold, uint64_t *const index);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_BUNDLE_MINER_H__
