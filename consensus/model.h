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
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tips_pair {
  trit_array_p trunk;
  trit_array_p branch;
} tips_pair;

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_MODEL_H__
