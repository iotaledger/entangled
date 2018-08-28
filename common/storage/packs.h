/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_PACKS_H__
#define __COMMON_STORAGE_PACKS_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_array.h"
#include "utils/logger_helper.h"

// Forward declaration
typedef struct iota_milestone_s iota_milestone_t;

#ifdef __cplusplus
extern "C" {
#endif

// TODO: code duplication

typedef struct iota_transactions_pack {
  iota_transaction_t *txs;
  size_t txs_capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_transactions_pack;

typedef struct iota_milestones_pack {
  iota_milestone_t **milestones;
  size_t txs_capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_milestones_pack;

typedef struct iota_hashes_pack {
  trit_array_p *hashes;
  size_t hashes_capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_hashes_pack;

extern retcode_t hash_pack_resize(iota_hashes_pack *pack, size_t resize_factor);
extern retcode_t hash_pack_init(iota_hashes_pack *pack, size_t size);
extern retcode_t hash_pack_free(iota_hashes_pack *pack);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_PACKS_H__
