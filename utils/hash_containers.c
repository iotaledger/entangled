/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/hash_containers.h"

retcode_t hash_queue_push(hash_queue_t *queue, flex_trit_t *hash) {
  hash_queue_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash_queue_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(*queue, entry);
  return RC_OK;
}

retcode_t hash_queue_pop(hash_queue_t *queue) {
  hash_queue_entry_t *tmp = NULL;

  tmp = *queue;
  CDL_DELETE(*queue, *queue);
  free(tmp);
  return RC_OK;
}

flex_trit_t *hash_queue_peek(hash_queue_t queue) {
  return (flex_trit_t *)(queue->hash);
}

retcode_t hash_queue_free(hash_queue_t *queue) {
  hash_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
  return RC_OK;
}

retcode_t hash_set_add(hash_set_t *set, flex_trit_t *hash) {
  hash_set_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash_set_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_243);
  HASH_ADD(hh, *set, hash, FLEX_TRIT_SIZE_243, entry);
  return RC_OK;
}

retcode_t hash_set_append(hash_set_t *set1, hash_set_t *set2) {
  retcode_t ret = RC_OK;
  hash_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set1, iter, tmp) {
    if (!hash_set_contains(set2, iter->hash)) {
      if ((ret = hash_set_add(set2, iter->hash)) != RC_OK) {
        return ret;
      }
    }
  }
  return ret;
}

bool hash_set_contains(hash_set_t *set, flex_trit_t *hash) {
  hash_set_entry_t *entry = NULL;

  HASH_FIND(hh, *set, hash, FLEX_TRIT_SIZE_243, entry);
  return entry != NULL;
}

retcode_t hash_set_free(hash_set_t *set) {
  hash_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set, iter, tmp) {
    HASH_DEL(*set, iter);
    free(iter);
  }
  return RC_OK;
}
