/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_FEATURE_H__
#define __GOSSIP_FEATURE_H__

#include <stdint.h>
#include <stdlib.h>

#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FEATURES_NUM 6

typedef enum node_info_mask_feature_e {
  MASK_FEATURE_SNAPSHOT_PRUNING = (1u << 0),
  MASK_FEATURE_DNS_REFRESHER = (1u << 1),
  MASK_FEATURE_TESTNET = (1u << 2),
  MASK_FEATURE_ZERO_MESSAGE_QUEUE = (1u << 3),
  MASK_FEATURE_TIP_SOLIDIFICATION = (1u << 4),
  MASK_FEATURE_REMOTE_POW = (1u << 5),
  MASK_FEATURE_ALL = MASK_FEATURE_SNAPSHOT_PRUNING | MASK_FEATURE_DNS_REFRESHER | MASK_FEATURE_TESTNET |
                     MASK_FEATURE_ZERO_MESSAGE_QUEUE | MASK_FEATURE_TIP_SOLIDIFICATION | MASK_FEATURE_REMOTE_POW
} node_info_mask_feature_t;

static inline void get_node_info_feature_set(uint8_t* node_features, node_info_mask_feature_t input_feature) {
  *node_features |= input_feature;
}

static inline void get_node_info_feature_clear(uint8_t* node_features, node_info_mask_feature_t input_feature) {
  *node_features &= (!input_feature);
}

static inline void get_node_info_feature_clear_all(uint8_t* node_features) { *node_features = 0; }

void features_set(uint8_t features, UT_array* feature_ut);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_FEATURE_H__
