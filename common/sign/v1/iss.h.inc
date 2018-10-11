/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/trinary/add.h"

#define CAT(A, ...) _CAT(A, __VA_ARGS__)
#define _CAT(A, ...) A##__VA_ARGS__

#define _ISS_PREFIX(A) CAT(CAT(CAT(iss_, HASH_PREFIX), _), A)

#ifdef __cplusplus
extern "C" {
#endif

int _ISS_PREFIX(subseed)(trit_t const *const seed, trit_t *const out,
                         int64_t const index, HASH_STATE *const state);

int _ISS_PREFIX(key)(trit_t const *const subseed, trit_t *key,
                     size_t const key_length, HASH_STATE *const state);

int _ISS_PREFIX(key_digest)(trit_t *key, trit_t *digest,
                            size_t const key_length, HASH_STATE *const state);

int _ISS_PREFIX(address)(trit_t const *const digest, trit_t *const address,
                         size_t const digest_length, HASH_STATE *const state);

int _ISS_PREFIX(signature)(trit_t *sig, trit_t const *const hash,
                           trit_t const *const key, size_t key_len,
                           HASH_STATE *const state);

int _ISS_PREFIX(sig_digest)(trit_t *const dig, trit_t const *const hash,
                            trit_t *sig, size_t const sig_len,
                            HASH_STATE *const state);

#ifdef __cplusplus
}
#endif

#undef _ISS_PREFIX
#undef CAT
#undef _CAT
