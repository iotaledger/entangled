/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/memset_safe.h"

#define __STDC_WANT_LIB_EXT1__ 1
#include <errno.h>
#include <stdint.h>
#include <string.h>

int memset_safe(void *dest, size_t destsz, int ch, size_t count) {
#ifdef __STDC_LIB_EXT1__
  return memset_s(dest, destsz, ch, count);
#else
  if (dest == NULL) {
    return EINVAL;
  } else if (destsz > SIZE_MAX || count > SIZE_MAX) {
    return E2BIG;
  } else if (count > destsz) {
    return EOVERFLOW;
  }

  volatile unsigned char *ptr = (volatile unsigned char *)dest;
  while (destsz-- && count--) {
    *ptr++ = ch;
  }

  return 0;
#endif
}
