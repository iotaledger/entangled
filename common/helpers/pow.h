/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_HELPERS_POW_H__
#define __COMMON_HELPERS_POW_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "common/crypto/curl-p/trit.h"
#include "common/model/bundle.h"
#include "common/trinary/flex_trit.h"
#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

trit_t *do_pow(Curl *const curl, trit_t const *const trits_in, size_t const trits_len, uint8_t const mwm);

IOTA_EXPORT char *iota_pow_trytes(char const *const trytes_in, uint8_t const mwm);

IOTA_EXPORT flex_trit_t *iota_pow_flex(flex_trit_t const *const flex_trits_in, size_t num_trits, uint8_t const mwm);

IOTA_EXPORT retcode_t iota_pow_bundle(bundle_transactions_t *const bundle, flex_trit_t const *const trunk,
                                      flex_trit_t const *const branch, uint8_t const mwm);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_HELPERS_POW_H__
