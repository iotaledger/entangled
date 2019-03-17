/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_TRIT_BYTE_H__
#define __COMMON_TRINARY_TRIT_BYTE_H__

#include "common/defs.h"
#include "common/trinary/bytes.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Returns the number of bytes needed to pack num_trits trits
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of bytes needed
#define MIN_BYTES(num_trits) (((num_trits) + NUMBER_OF_TRITS_IN_A_BYTE - 1) / NUMBER_OF_TRITS_IN_A_BYTE)

/// Packs an array of trits into byte (max 5 packed trits)
/// @param[in] trits - An array of trits
/// @param[in] num_trits - the number of trits to convert
/// @return byte_t - the num_trits trits packed into a byte
byte_t trits_to_byte(trit_t const *const trits, size_t const num_trits);

/// Packs an array of trits into an array of bytes
/// @param[in] trits - An array of trits
/// @param[in] cum - the accumulator, maybe be not 0
/// @param[in] num_trits - the number of trits to convert
/// @return byte_t - the num_trits trits packed into a byte
void trits_to_bytes(trit_t const *const trits, byte_t *const bytes, size_t const num_trits);

/// Unpacks a byte into an array of trits
/// @param[in] byte - A byte of packed trits
/// @param[in] trits - An array of trits
/// @param[in] num_trits - the number of trits to unpack (max 5)
void byte_to_trits(byte_t const byte, trit_t *const trits, size_t const num_trits);

/// Unpacks an array of byte into an array of trits
/// @param[in] bytes - An array bytes (packed trits)
/// @param[in] n_bytes - the number of bytes in the array
/// @param[in] trits - An array of trits
/// @param[in] num_trits - the number of trits to unpack
void bytes_to_trits(byte_t const *const bytes, size_t const num_bytes, trit_t *const trits, size_t const num_trits);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_TRINARY_TRIT_BYTE_H__
