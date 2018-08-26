/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_KERL_BIGINT_H_
#define __COMMON_KERL_BIGINT_H_

#include "common/stdint.h"

void bigint_not(uint32_t* const base, size_t const len);
size_t bigint_add_small(uint32_t* const base, uint32_t const other);
void bigint_add(uint32_t* const base, uint32_t const* const rh,
                size_t const len);
void bigint_sub(uint32_t* const base, uint32_t const* const rh,
                size_t const len);
int8_t bigint_cmp(uint32_t const* const lh, uint32_t const* const rh,
                  size_t const len);

#endif /* __COMMON_KERL_BIGINT_H_ */
#ifdef __cplusplus
}
#endif
