/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_HELPERS_SIGN_H__
#define __COMMON_HELPERS_SIGN_H__

#include <stddef.h>

#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/flex_trit.h"
#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_sign_address_gen(char const* const seed,
                                        size_t const index,
                                        size_t const security);
IOTA_EXPORT flex_trit_t* iota_flex_sign_address_gen(
    flex_trit_t const* const seed, size_t const index, size_t const security);

IOTA_EXPORT char* iota_sign_signature_gen(char const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          char const* const bundle_hash);
IOTA_EXPORT flex_trit_t* iota_flex_sign_signature_gen(
    flex_trit_t const* const seed, size_t const index, size_t const security,
    flex_trit_t const* const bundle_hash);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_SIGN_H__
