/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_BCT_H__
#define __COMMON_TRINARY_BCT_H__

#include <stdint.h>

#include "common/trinary/trits.h"

typedef uint8_t bct_t;

#ifdef __cplusplus
extern "C" {
#endif

void copy_bct(bct_t *const d, size_t const o, bct_t const *const s,
              size_t const i, size_t const l);
void copy_trits_to_bct(bct_t *const s, size_t const i, trit_t const *const t,
                       size_t const l);
void copy_bct_to_trits(trit_t *const t, bct_t const *const s, size_t const i,
                       size_t const l);
void write_trit(bct_t *const data, int const t, trit_t const v);
trit_t get_trit(bct_t const *const data, int const t);

#ifdef __cplusplus
}
#endif

#endif
