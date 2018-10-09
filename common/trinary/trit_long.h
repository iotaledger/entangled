/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRIT_LONG_H__
#define __COMMON_TRINARY_TRIT_LONG_H__

#include "common/trinary/trits.h"

/// Returns the number of trits needed to encode the value
/// @return size_t - the number of trit needed to encode the value
size_t min_trits(int64_t const value);

/// Convert an array of trits to an integer value
/// @param[in] trits - an array of trits
/// @param[in] num_trits - the number of trits in the array
/// @return int64_t - the value encoded in the array of trits
int64_t trits_to_long(trit_t const *const trits, size_t const num_trits);
/// Convert an integer value to an array of trits
/// @param[in] value - an integer value
/// @param[in] trits - an array of trits
/// @return size_t - the number of trits needed to encode the value
size_t long_to_trits(int64_t const value, trit_t *const trits);

/// Returns the nearest multiple of 3 of the value, rounded up
/// @param[in] value - an integer value
/// @return size_t - the nearest multiple of 3 of the value
size_t nearest_greater_multiple_of_three(size_t const value);

/// Returns the number of trits needed for encoding of a value
/// @return size_t - the number of trit needed for encoding
size_t encoded_length(int64_t const value);

/// Encode a value (for MAM)
/// @param[in] value - an integer value
/// @param[in] trits - an array of trits
/// @param[in] num_trits - the number of trits in the array
/// @return int - return 0 on succes, -1 on failure
int encode_long(int64_t const value, trit_t *const trits,
                size_t const num_trits);
/// Decode a value (for MAM)
/// @param[in] trits - an array of trits
/// @param[in] num_trits - the number of trits in the array
/// @param[in/out] size - the size of the encoded data
/// @return int64_t - the value decoded from the array of trits
int64_t decode_long(trit_t const *const trits, size_t const num_trits,
                    size_t *const size);

#endif
#ifdef __cplusplus
}
#endif
