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

/**
 * @brief Upper bound of log(n) base 3
 *
 * @param[in] n positive number
 * @return [log_3(n)]
 */
size_t ptrit_log3(size_t n);

/**
 * @brief Init slices in `range` with `[value..value+PTRIT_SIZE)`
 *
 * Trits in `value` encode an integer number
 *
 * @param[in] n number of ptrits in `range`
 * @param[out] range pointer to ptrits
 * @param[in,out] value value to be assigned to slices in `range`, the output value encodes `value + PTRIT_SIZE`
 */
void ptrit_set_iota(size_t n, ptrit_t *range, trit_t *value);

/**
 * @brief Homogenous ptrits increment
 *
 * All slices in ptrits must be equal
 * Invariant:
 *   * for each ptrit `t` in [`t`, `t+n`) all non-`NaT` trits in `t` are equal;
 *   * all ptrits in [`t`, `t+n`) contain `NaT`s in the same position;
 *   * `NaT` trits are preserved after increment.
 *
 * @param[in] n number of ptrits in `t`
 * @param[in,out] t pointer to ptrits
 * @return 1 if overflow
 * @return 0 if no overflow
 */
int ptrit_hincr(size_t n, ptrit_t *t);

#endif
#ifdef __cplusplus
}
#endif
