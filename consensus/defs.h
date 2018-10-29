/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_DEFS_H__
#define __CONSENSUS_DEFS_H__

#include "common/trinary/flex_trit.h"

#define IOTA_SUPPLY 2779530283277761LL

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_consensus_defs_s {
  flex_trit_t genesis_hash[FLEX_TRIT_SIZE_243];
} iota_consensus_defs_t;

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_DEFS_H__
