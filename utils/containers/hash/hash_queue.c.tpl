/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "utils/containers/hash/hash{SIZE}_queue.h"

bool hash{SIZE}_queue_empty(hash{SIZE}_queue_t const queue) { return (queue == NULL); }

retcode_t hash{SIZE}_queue_push(hash{SIZE}_queue_t *const queue,
                          flex_trit_t const *const hash) {
  hash{SIZE}_queue_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash{SIZE}_queue_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_{SIZE});
  CDL_APPEND(*queue, entry);
  return RC_OK;
}

void hash{SIZE}_queue_pop(hash{SIZE}_queue_t *const queue) {
  hash{SIZE}_queue_entry_t *tmp = NULL;

  tmp = *queue;
  if (tmp != NULL) {
    CDL_DELETE(*queue, *queue);
    free(tmp);
  }
}

flex_trit_t *hash{SIZE}_queue_peek(hash{SIZE}_queue_t const queue) {
  if (queue == NULL) {
    return NULL;
  }
  return (flex_trit_t *)(queue->hash);
}

void hash{SIZE}_queue_free(hash{SIZE}_queue_t *const queue) {
  hash{SIZE}_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
}

size_t hash{SIZE}_queue_count(hash{SIZE}_queue_t *const queue) {
  hash{SIZE}_queue_entry_t *iter = NULL;
  size_t count = 0;
  CDL_COUNT(*queue, iter, count);
  return count;
}

flex_trit_t *hash{SIZE}_queue_at(hash{SIZE}_queue_t *const queue, size_t index) {
  hash{SIZE}_queue_entry_t *iter = NULL;
  size_t count = 0;
  CDL_FOREACH(*queue, iter) {
    if (count == index) {
      return (flex_trit_t *)(iter->hash);
    }
    count++;
  }
  return NULL;
}
