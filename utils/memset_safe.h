/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MEMSET_SAFE_H__
#define __UTILS_MEMSET_SAFE_H__

#define __STDC_WANT_LIB_EXT1__ 1
#include <errno.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __STDC_LIB_EXT1__

typedef int errno_t;
errno_t memset_safe(void *dest, size_t destsz, int ch, size_t count);

#else

static inline errno_t memset_safe(void *dest, size_t destsz, int ch,
                                  size_t count) {
  return memset_s(dest, destsz, ch, count);
}

#endif

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_MEMSET_SAFE_H__
