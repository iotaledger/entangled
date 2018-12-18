/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_MODEL_H__
#define __CONSENSUS_MODEL_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tips_pair {
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
} tips_pair_t;

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_MODEL_H__
