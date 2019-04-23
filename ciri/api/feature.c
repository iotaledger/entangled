/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "feature.h"

char* const feature_str[FEATURES_NUM] = {"snapshotPruning",  "dnsRefresher",      "testnet",
                                         "zeroMessageQueue", "tipSolidification", "RemotePOW"};

void features_set(uint8_t features, UT_array* feature_ut) {
  node_info_mask_feature_t elt = (1u << 0);

  for (int i = 0; i < FEATURES_NUM; i++) {
    if (features & elt) {
      utarray_push_back(feature_ut, &feature_str[i]);
    }

    elt <<= 1;
  }
}
