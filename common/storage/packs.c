/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/storage/packs.h"

#define STORAGE_PACKS_LOGGER_ID "storage_packs"

retcode_t hash_pack_resize(iota_hashes_pack *pack, size_t resize_factor) {
  if (resize_factor < 1) {
    return RC_OK;
  }

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    trit_array_free(pack->hashes[i]);
  }
  pack->hashes_capacity *= resize_factor;
  pack->hashes = (trit_array_p *)realloc(
      pack->hashes, sizeof(trit_array_p) * pack->hashes_capacity);
  if (pack->hashes == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in realloc\n");
    return RC_STORAGE_OOM;
  }

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    pack->hashes[i] = trit_array_new(NUM_TRITS_ADDRESS);
    if (pack->hashes[i] == NULL) {
      return RC_STORAGE_OOM;
    }
  }

  return RC_OK;
}

retcode_t hash_pack_init(iota_hashes_pack *pack, size_t size) {
  pack->hashes_capacity = size;
  pack->num_loaded = 0;
  pack->hashes =
      (trit_array_p *)malloc(sizeof(trit_array_p) * pack->hashes_capacity);
  if (pack->hashes == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in malloc\n");
    return RC_STORAGE_OOM;
  }

  for (int i = 0; i < pack->hashes_capacity; ++i) {
    pack->hashes[i] = trit_array_new(NUM_TRITS_ADDRESS);
    if (pack->hashes[i] == NULL) {
      return RC_STORAGE_OOM;
    }
  }

  return RC_OK;
}

retcode_t hash_pack_free(iota_hashes_pack *pack) {
  for (int i = 0; i < pack->hashes_capacity; ++i) {
    trit_array_free(pack->hashes[i]);
  }
  free(pack->hashes);
  return RC_OK;
}
