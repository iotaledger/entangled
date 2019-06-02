/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MACROS_H__
#define __UTILS_MACROS_H__

#define UNUSED(...) (void)(__VA_ARGS__)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// max uint64 18446744073709551616
#define MAX_CHARS_UINT64 20

#endif  // __UTILS_MACROS_H__
