/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRIT_TRYTE_H_
#define __COMMON_TRINARY_TRIT_TRYTE_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

size_t num_trytes_for_trits(size_t num_trits);
trit_t get_trit_at(tryte_t *const trytes, size_t const length, size_t index);
uint8_t set_trit_at(tryte_t *const trytes, size_t const length, size_t index,
                    trit_t trit);
void trits_to_trytes(trit_t const *const trits, tryte_t *const trytes,
                     size_t const length);
void trytes_to_trits(tryte_t const *const tryte, trit_t *const trits,
                     size_t const length);

#endif
#ifdef __cplusplus
}
#endif
