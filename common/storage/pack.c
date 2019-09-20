/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/storage/pack.h"
#include "common/trinary/flex_trit.h"

retcode_t hash_pack_resize(iota_stor_pack_t *pack, size_t resize_factor) {
  if (resize_factor < 1) {
    return RC_OK;
  }

  pack->insufficient_capacity = false;

  for (size_t i = 0; i < pack->capacity; ++i) {
    free(pack->models[i]);
  }
  pack->capacity *= resize_factor;
  pack->models = realloc(pack->models, sizeof(flex_trit_t *) * pack->capacity);
  if (pack->models == NULL) {
    return RC_OOM;
  }

  for (size_t i = 0; i < pack->capacity; ++i) {
    pack->models[i] = malloc(FLEX_TRIT_SIZE_243);
    if (pack->models[i] == NULL) {
      return RC_OOM;
    }
  }

  return RC_OK;
}

retcode_t hash_pack_init(iota_stor_pack_t *pack, size_t size) {
  pack->capacity = size;
  pack->num_loaded = 0;
  pack->insufficient_capacity = false;
  pack->models = malloc(sizeof(flex_trit_t *) * pack->capacity);
  if (pack->models == NULL) {
    return RC_OOM;
  }

  for (size_t i = 0; i < pack->capacity; ++i) {
    pack->models[i] = malloc(FLEX_TRIT_SIZE_243);
    if (pack->models[i] == NULL) {
      return RC_OOM;
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
  for (size_t i = 0; i < pack->capacity; ++i) {
    free(pack->models[i]);
  }
  free(pack->models);
  return RC_OK;
}
