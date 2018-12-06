/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_ENUMS_H__
#define __COMMON_STORAGE_ENUMS_H__

typedef enum load_model_e {
  MODEL_MILESTONE,
  MODEL_TRANSACTION_ALL,
  MODEL_TRANSACTION_HASH,
  MODEL_TRANSACTION_META_ALL,
  MODEL_TRANSACTION_SNAPSHOT_INDEX,
  MODEL_TRANSACTION_SOLID,
} load_model_e;

#endif  // __COMMON_STORAGE_ENUMS_H__
