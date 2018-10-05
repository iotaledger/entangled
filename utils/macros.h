/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_MACROS_H__
#define __UTILS_MACROS_H__

#define MAX(a, b)       \
  ({                    \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b;  \
  })

#define MIN(a, b)       \
  ({                    \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a < _b ? _a : _b;  \
  })

#endif  // __UTILS_MACROS_H__
