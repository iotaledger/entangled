/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CRYPTO_ISS_V1_ISS_H__
#define __COMMON_CRYPTO_ISS_V1_ISS_H__

#include "common/crypto/sponge/sponge.h"

#ifdef __cplusplus
extern "C" {
#endif

int iss_subseed(sponge_t *const sponge, trit_t const *const seed, trit_t *const out, int64_t const index);

int iss_key(sponge_t *const sponge, trit_t const *const subseed, trit_t *key, size_t const key_length);

int iss_key_digest(sponge_t *const sponge, trit_t *key, trit_t *digest, size_t const key_length);

int iss_address(sponge_t *const sponge, trit_t const *const digest, trit_t *const address, size_t const digest_length);

int iss_signature(sponge_t *const sponge, trit_t *sig, trit_t const *const hash, trit_t const *const key,
                  size_t key_len);

int iss_sig_digest(sponge_t *const sponge, trit_t *const dig, trit_t const *const hash, trit_t *sig,
                   size_t const sig_len);

int iss_merkle_root(sponge_t *const sponge, trit_t *const hash, trit_t const *const siblings,
                    size_t const siblings_number, size_t const leaf_index);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CRYPTO_ISS_V1_ISS_H__
