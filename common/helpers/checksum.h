/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_HELPERS_CHECKSUM_H_
#define __COMMON_HELPERS_CHECKSUM_H_

#include <stddef.h>

#include "common/trinary/flex_trit.h"
#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_checksum(const char* input, const size_t input_length,
                                const size_t checksum_length);

IOTA_EXPORT flex_trit_t* iota_flex_checksum(const flex_trit_t* flex_trits,
                                            const size_t num_trits,
                                            const size_t checksum_length);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_CHECKSUM_H_
