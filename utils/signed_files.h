/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_SIGNED_FILES_H__
#define __UTILS_SIGNED_FILES_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

#ifdef __cplusplus
extern "C" {
#endif

extern retcode_t is_file_signature_valid(char const *const filename,
                                         char const *const signature_filename,
                                         tryte_t const *const public_key,
                                         size_t depth, size_t index,
                                         bool *const valid);

extern retcode_t validate_signature(char const *const signature_filename,
                                    tryte_t const *const public_key,
                                    size_t depth, size_t index,
                                    flex_trit_t *const digest,
                                    bool *const valid);

extern retcode_t digest_file(char const *const filename,
                             flex_trit_t *const digest);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_SIGNED_FILES_H__
