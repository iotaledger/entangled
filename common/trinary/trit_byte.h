/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled/
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRIT_BYTE_H__
#define __COMMON_TRINARY_TRIT_BYTE_H__

#include "common/trinary/bytes.h"
#include "common/trinary/trits.h"

size_t min_bytes(size_t);
byte_t trits_to_byte(trit_t const *const trits, byte_t const cum,
                     size_t const i);
void trits_to_bytes(trit_t *, byte_t *, size_t);
void byte_to_trits(byte_t byte, trit_t *const trit, size_t const i);
void bytes_to_trits(byte_t const *const byte, size_t const n_bytes,
                    trit_t *const trit, size_t const n_trits);

#endif
#ifdef __cplusplus
}
#endif
