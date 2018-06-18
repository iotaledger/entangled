/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled/
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRYTE_LONG_H_
#define __COMMON_TRINARY_TRYTE_LONG_H_

#include "common/trinary/tryte.h"

size_t min_trytes(int64_t value);
int64_t trytes_to_long(const tryte_t *trytes, size_t const i);
size_t long_to_trytes(int64_t value, tryte_t *trytes);

#endif // __COMMON_TRINARY_TRYTE_LONG_H_
#ifdef __cplusplus
}
#endif

