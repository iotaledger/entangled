/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#define RADIX 3

// Trytes related definitions

#define TRYTE_ALPHABET "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define TRYTE_VALUE_MAX 13
#define TRYTE_VALUE_MIN -13
#define TRYTE_WIDTH 3

// Conversion definitions

#define NUMBER_OF_TRITS_IN_A_TRYTE 3
#define NUMBER_OF_TRITS_IN_A_BYTE 5

// Hash length definitions

#define HASH_LENGTH_BYTE 48
#define HASH_LENGTH_TRYTE 81
#define HASH_LENGTH_TRIT 243

// IOTA Signing Scheme definitions

#define SECURITY_LEVEL_MAX 3
#define NORMALIZED_FRAGMENT_LENGTH 27
#define ISS_FRAGMENTS 27
#define ISS_KEY_LENGTH (HASH_LENGTH_TRIT * ISS_FRAGMENTS)
#define ISS_CHUNK_LENGTH HASH_LENGTH_TRIT / TRYTE_WIDTH

#endif  // __COMMON_DEFS_H__
