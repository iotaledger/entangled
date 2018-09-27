/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MEMSET_SECURE_H
#define __UTILS_MEMSET_SECURE_H

#ifdef __cplusplus
extern "C" {
#endif

errno_t memset_secure(void *dest, rsize_t destsz, int ch, rsize_t count);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_MEMSET_SECURE_H
