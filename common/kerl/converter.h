/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_KERL_CONVERTER_H_
#define __COMMON_KERL_CONVERTER_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"

void convert_trits_to_bytes(trit_t const *const trits, uint8_t *const bytes);

// This method consumes the input bytes.
void convert_bytes_to_trits(uint8_t *const bytes, trit_t *const trits);

#endif /* __COMMON_KERL_CONVERTER_H_ */
#ifdef __cplusplus
}
#endif
