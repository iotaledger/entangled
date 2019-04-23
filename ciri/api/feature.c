/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "feature.h"

char* const features_name[NODE_FEATURES_NUM] = {"snapshotPruning",  "dnsRefresher",      "testnet",
                                                "zeroMessageQueue", "tipSolidification", "RemotePOW"};

void node_features_set(uint8_t const features, UT_array* const feature_array) {
  node_feature_t elt = (1u << 0);

  for (int i = 0; i < NODE_FEATURES_NUM; i++) {
    if (features & elt) {
      utarray_push_back(feature_array, &features_name[i]);
    }

    elt <<= 1;
  }
}
