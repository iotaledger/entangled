/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_KERL_CONVERTER_H_
#define __COMMON_KERL_CONVERTER_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"
#include "utils/memset_safe.h"

#ifdef __cplusplus
extern "C" {
#endif

void convert_trits_to_bytes(trit_t const *const trits, uint8_t *const bytes);

// This method consumes the input bytes.
void convert_bytes_to_trits(uint8_t *const bytes, trit_t *const trits);

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_KERL_CONVERTER_H_ */
