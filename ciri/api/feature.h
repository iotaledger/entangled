/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_FEATURE_H__
#define __CIRI_API_FEATURE_H__

#include <stdint.h>
#include <stdlib.h>

#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NODE_FEATURES_NUM 6

typedef enum node_feature_e {
  NODE_FEATURE_SNAPSHOT_PRUNING = (1u << 0),
  NODE_FEATURE_DNS_REFRESHER = (1u << 1),
  NODE_FEATURE_TESTNET = (1u << 2),
  NODE_FEATURE_ZERO_MESSAGE_QUEUE = (1u << 3),
  NODE_FEATURE_TIP_SOLIDIFICATION = (1u << 4),
  NODE_FEATURE_REMOTE_POW = (1u << 5),
  NODE_FEATURE_ALL = NODE_FEATURE_SNAPSHOT_PRUNING | NODE_FEATURE_DNS_REFRESHER | NODE_FEATURE_TESTNET |
                     NODE_FEATURE_ZERO_MESSAGE_QUEUE | NODE_FEATURE_TIP_SOLIDIFICATION | NODE_FEATURE_REMOTE_POW
} node_feature_t;

static inline void node_feature_set(uint8_t* const features, node_feature_t const feature) { *features |= feature; }

static inline void node_feature_unset(uint8_t* const features, node_feature_t const feature) { *features &= !feature; }

static inline void node_features_clear(uint8_t* const features) { *features = 0; }

void node_features_set(uint8_t const features, UT_array* const feature_array);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_API_FEATURE_H__
