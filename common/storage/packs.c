/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/packs.h"

#define STORAGE_PACKS_LOGGER_ID "storage_packs"

retcode_t hash_pack_resize(iota_hashes_pack *pack, size_t resize_factor) {
  if (resize_factor < 1) {
    return RC_OK;
  }

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    trit_array_free(pack->hashes[i]);
  }
  pack->hashes_capacity *= 2;
  pack->hashes = (iota_transaction_t *)realloc(
      pack->hashes, sizeof(iota_transaction_t) * pack->hashes_capacity);
  if (pack->hashes == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in realloc");
    return RC_STORAGE_FAILED_IN_REALLOC;
  }

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    pack->hashes[i] = trit_array_new(NUM_TRITS_ADDRESS);
  }

  return RC_OK;
}

retcode_t hash_pack_init(iota_hashes_pack *pack, size_t size) {
  iota_transaction_t *txs = NULL;
  pack->hashes_capacity = size;
  pack->num_loaded = 0;
  txs = (iota_transaction_t *)malloc(sizeof(iota_transaction_t) *
                                     pack->hashes_capacity);
  if (txs == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in malloc");
    return RC_STORAGE_FAILED_IN_MALLOC;
  }
  pack->hashes = txs;

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    pack->hashes[i] = trit_array_new(NUM_TRITS_ADDRESS);
  }

  return RC_OK;
}

retcode_t hash_pack_free(iota_hashes_pack *pack) {
  for (int i = 0; i < pack->hashes_capacity; ++i) {
    trit_array_free(pack->hashes[i]);
  }
  free(pack->hashes);
}