/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_TRYTE_ASCII_H_
#define __COMMON_TRINARY_TRYTE_ASCII_H_

#include <stdbool.h>

#include "common/trinary/tryte.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ascii_to_trytes(char const *const input, tryte_t *const output);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_TRINARY_TRYTE_ASCII_H_
