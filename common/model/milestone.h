/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_MILESTONE_H__
#define __COMMON_MODEL_MILESTONE_H__

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_milestone_s {
  uint64_t index;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} iota_milestone_t;

static inline void milestone_reset(iota_milestone_t *const milestone) {
  milestone->index = 0;
  memset(milestone->hash, FLEX_TRIT_NULL_VALUE, sizeof(milestone->hash));
}

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_MILESTONE_H__
