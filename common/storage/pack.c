/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/storage/pack.h"
#include "common/trinary/trit_array.h"
#include "utils/logger_helper.h"

#define STORAGE_PACKS_LOGGER_ID "storage_packs"

retcode_t hash_pack_resize(iota_stor_pack_t *pack, size_t resize_factor) {
  if (resize_factor < 1) {
    return RC_OK;
  }

  pack->insufficient_capacity = false;

  for (int i = 0; i < pack->capacity; ++i) {
    trit_array_free(pack->models[i]);
  }
  pack->capacity *= resize_factor;
  pack->models = realloc(pack->models, sizeof(trit_array_p) * pack->capacity);
  if (pack->models == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in realloc\n");
    return RC_STORAGE_OOM;
  }

  for (int i = 0; i < pack->capacity; ++i) {
    pack->models[i] = trit_array_new(HASH_LENGTH_TRIT);
    if (pack->models[i] == NULL) {
      return RC_STORAGE_OOM;
    }
  }

  return RC_OK;
}

retcode_t hash_pack_init(iota_stor_pack_t *pack, size_t size) {
  pack->capacity = size;
  pack->num_loaded = 0;
  pack->insufficient_capacity = false;
  pack->models = malloc(sizeof(trit_array_p) * pack->capacity);
  if (pack->models == NULL) {
    log_error(STORAGE_PACKS_LOGGER_ID, "Failed in malloc\n");
    return RC_STORAGE_OOM;
  }

  for (int i = 0; i < pack->capacity; ++i) {
    pack->models[i] = trit_array_new(HASH_LENGTH_TRIT);
    if (pack->models[i] == NULL) {
      return RC_STORAGE_OOM;
    }
  }

  return RC_OK;
}

retcode_t hash_pack_reset(iota_stor_pack_t *pack) {
  pack->num_loaded = 0;
  pack->insufficient_capacity = false;
  return RC_OK;
}

retcode_t hash_pack_free(iota_stor_pack_t *pack) {
  for (int i = 0; i < pack->capacity; ++i) {
    trit_array_free(pack->models[i]);
  }
  free(pack->models);
  return RC_OK;
}
