/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/memset_safe.h"
#include <stdint.h>

#ifndef __STDC_LIB_EXT1__

retcode_t memset_safe(void *dest, size_t destsz, int ch, size_t count) {
  if (dest == NULL) {
    return RC_OOM;
  } else if (destsz > SIZE_MAX) {
    return RC_OOM;
  } else if (count > destsz) {
    return RC_OOM;
  }

  volatile unsigned char *ptr = dest;
  while (destsz-- && count--) {
    *ptr++ = ch;
  }

  return RC_OK;
}

#endif