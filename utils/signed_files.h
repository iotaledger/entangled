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

#ifdef __cplusplus
extern "C" {
#endif

retcode_t iota_file_signature_validate(char const *const filename, char const *const signature_filename,
                                       flex_trit_t const *const public_key, uint32_t depth, size_t index,
                                       bool *const valid);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_SIGNED_FILES_H__
