/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MEMSET_SAFE_H__
#define __UTILS_MEMSET_SAFE_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t memset_safe(void *dest, size_t destsz, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_MEMSET_SAFE_H__
