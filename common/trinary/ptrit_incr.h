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

size_t ptrit_log3(size_t n);
void ptrit_set_iota(size_t n, ptrit_t *range, trit_t *value);
int ptrit_hincr(size_t n, ptrit_t *t);

#endif
#ifdef __cplusplus
}
#endif
