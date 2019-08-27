/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_PTRIT_INCREMENT_H_
#define __COMMON_TRINARY_PTRIT_INCREMENT_H_

#include "common/trinary/ptrit.h"

#if (64 == PTRIT_SIZE)
#define PTRIT_SIZE_LOG3 4
#elif (128 == PTRIT_SIZE)
#define PTRIT_SIZE_LOG3 5
#elif (256 == PTRIT_SIZE)
#define PTRIT_SIZE_LOG3 6
#elif (512 == PTRIT_SIZE)
#define PTRIT_SIZE_LOG3 6
#else
#error Invalid PTRIT_SIZE.
#endif  // PTRIT_SIZE

void ptrits_set_iota(ptrit_t range[PTRIT_SIZE_LOG3]);
int ptrit_hincr(ptrit_t *const begin, ptrit_t *const end);

void ptrit_increment(ptrit_t *const trits, size_t const begin, size_t const end);

#endif
#ifdef __cplusplus
}
#endif
