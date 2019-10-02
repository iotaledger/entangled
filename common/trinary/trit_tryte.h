/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_TRIT_TRYTE_H_
#define __COMMON_TRINARY_TRIT_TRYTE_H_

#include "common/defs.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

#ifdef __cplusplus
extern "C" {
#endif

static const trit_t TRYTES_TRITS_LUT[TRYTE_SPACE_SIZE][NUMBER_OF_TRITS_IN_A_TRYTE] = {
    {0, 0, 0},  {1, 0, 0},  {-1, 1, 0},  {0, 1, 0},  {1, 1, 0},  {-1, -1, 1},  {0, -1, 1},  {1, -1, 1},  {-1, 0, 1},
    {0, 0, 1},  {1, 0, 1},  {-1, 1, 1},  {0, 1, 1},  {1, 1, 1},  {-1, -1, -1}, {0, -1, -1}, {1, -1, -1}, {-1, 0, -1},
    {0, 0, -1}, {1, 0, -1}, {-1, 1, -1}, {0, 1, -1}, {1, 1, -1}, {-1, -1, 0},  {0, -1, 0},  {1, -1, 0},  {-1, 0, 0}};

static inline size_t num_trytes_for_trits(size_t num_trits) {
  return (num_trits + NUMBER_OF_TRITS_IN_A_TRYTE - 1) / NUMBER_OF_TRITS_IN_A_TRYTE;
}

trit_t get_trit_at(tryte_t const *const trytes, size_t const length, size_t const index);
uint8_t set_trit_at(tryte_t *const trytes, size_t const length, size_t const index, trit_t const trit);
void trits_to_trytes(trit_t const *const trits, tryte_t *const trytes, size_t const length);
void trytes_to_trits(tryte_t const *const trytes, trit_t *const trits, size_t const length);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_TRINARY_TRIT_TRYTE_H_
