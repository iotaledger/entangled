/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_STDINT_H_
#define COMMON_STDINT_H_

#if defined(_WIN32) && !defined(MINGW)
#else
#include <unistd.h>
#endif
#include <stdint.h>
#endif  // COMMON_STDINT_H_
