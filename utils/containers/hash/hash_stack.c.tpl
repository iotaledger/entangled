/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "utils/containers/hash/hash{SIZE}_stack.h"

retcode_t hash{SIZE}_stack_push(hash{SIZE}_stack_t *const stack, flex_trit_t const *const hash) {
  hash{SIZE}_stack_entry_t *entry = NULL;

  if ((entry = (hash{SIZE}_stack_entry_t *)malloc(sizeof(hash{SIZE}_stack_entry_t))) == NULL) {
    return RC_OOM;
  }

  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_{SIZE});
  STACK_PUSH(*stack, entry);

  return RC_OK;
}

void hash{SIZE}_stack_pop(hash{SIZE}_stack_t *const stack) {
  hash{SIZE}_stack_entry_t *tmp = NULL;

  tmp = *stack;
  STACK_POP(*stack, *stack);
  free(tmp);
}

flex_trit_t *hash{SIZE}_stack_peek(hash{SIZE}_stack_t const stack) { return (flex_trit_t *)(STACK_TOP(stack)->hash); }

void hash{SIZE}_stack_free(hash{SIZE}_stack_t *const stack) {
  hash{SIZE}_stack_entry_t *iter = NULL;

  while (!STACK_EMPTY(*stack)) {
    STACK_POP(*stack, iter);
    free(iter);
  }
}

size_t hash{SIZE}_stack_count(hash{SIZE}_stack_t const stack) {
  hash{SIZE}_stack_entry_t *tmp = NULL;
  size_t count = 0;

  STACK_COUNT(stack, tmp, count);

  return count;
}

flex_trit_t *hash{SIZE}_stack_at(hash{SIZE}_stack_t const stack, size_t const index) {
  hash{SIZE}_stack_entry_t *iter = NULL;
  size_t count = 0;

  HASH_STACK_FOREACH(stack, iter) {
    if (count == index) {
      return (flex_trit_t *)(iter->hash);
    }
    count++;
  }
  return NULL;
}
