/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MEMSET_SECURE_H
#define __UTILS_MEMSET_SECURE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memset_secure(void *dest, size_t destsz, int ch, size_t count);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_MEMSET_SECURE_H
