/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#define __STDC_WANT_LIB_EXT1__ 1
#include "memset_secure.h"
#include <string.h>

int memset_secure(void *dest, size_t destsz, int ch, size_t count) {
#if (__STDC_VERSION__ >= 201112L || __cplusplus >= 201103L) && defined (__STDC_LIB_EXT1__)
  // If this is C11, we can simply use memset_s
  return memset_s(dest, destsz, ch, count);
#elif defined(_WIN32)
  // https://msdn.microsoft.com/en-us/library/windows/desktop/aa366877
  SecureZeroMemory(dest, destsz);
  return 0;
#else
  // https://wiki.sei.cmu.edu/confluence/display/c/MSC06-C.+Beware+of+compiler+optimizations
  if (dest == NULL) return EINVAL;
  if (destsz > SIZE_MAX) return EINVAL;
  if (count > destsz) return EINVAL;

  volatile unsigned char *p = dest;
  while (destsz-- && count--) {
    *p++ = ch;
  }
  return 0;
#endif
}
