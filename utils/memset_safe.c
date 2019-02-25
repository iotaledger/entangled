/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/memset_safe.h"

#ifndef __STDC_LIB_EXT1__

errno_t memset_safe(void *dest, size_t destsz, int ch, size_t count) {
  if (dest == NULL) {
    return EINVAL;
  } else if (destsz > SIZE_MAX || count > SIZE_MAX) {
    return E2BIG;
  } else if (count > destsz) {
    return EOVERFLOW;
  }

  volatile unsigned char *ptr = dest;
  while (destsz-- && count--) {
    *ptr++ = ch;
  }

  return 0;
}

#endif