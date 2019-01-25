/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_FORCED_INLINE_H__
#define __UTILS_FORCED_INLINE_H__

#if defined(_MSC_VER)
#define FORCED_INLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define FORCED_INLINE __attribute__((always_inline)) inline
#else
#define FORCED_INLINE inline
#endif

#endif  // __UTILS_FORCED_INLINE_H__
