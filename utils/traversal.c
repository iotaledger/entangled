/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/traversal.h"

extern retcode_t hash_queue_add(hash_queue_t **queue, flex_trit_t *hash) {
  hash_queue_t *elem = NULL;

  if ((elem = malloc(sizeof(hash_queue_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(elem->hash, hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(*queue, elem);
  return RC_OK;
}

extern retcode_t hash_set_add(hash_set_t **set, flex_trit_t *hash) {
  hash_set_t *elem = NULL;

  if ((elem = malloc(sizeof(hash_set_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(elem->hash, hash, FLEX_TRIT_SIZE_243);
  HASH_ADD(hh, *set, hash, FLEX_TRIT_SIZE_243, elem);
  return RC_OK;
}
