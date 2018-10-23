/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "utils/containers/hash/hash{SIZE}_stack.h"

bool hash{SIZE}_stack_empty(hash{SIZE}_stack_t const stack) { return (stack == NULL); }

retcode_t hash{SIZE}_stack_push(hash{SIZE}_stack_t *const stack,
                          flex_trit_t const *const hash) {
  hash{SIZE}_stack_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash{SIZE}_stack_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_{SIZE});
  LL_PREPEND(*stack, entry);
  return RC_OK;
}

void hash{SIZE}_stack_pop(hash{SIZE}_stack_t *const stack) {
  hash{SIZE}_stack_entry_t *tmp = NULL;

  tmp = *stack;
  LL_DELETE(*stack, *stack);
  free(tmp);
}

flex_trit_t *hash{SIZE}_stack_peek(hash{SIZE}_stack_t const stack) {
  return (flex_trit_t *)(stack->hash);
}

void hash{SIZE}_stack_free(hash{SIZE}_stack_t *const stack) {
  hash{SIZE}_stack_entry_t *iter = NULL, *tmp = NULL;

  LL_FOREACH_SAFE(*stack, iter, tmp) {
    LL_DELETE(*stack, iter);
    free(iter);
  }
}

size_t hash{SIZE}_stack_count(hash{SIZE}_stack_t const stack) {
  hash{SIZE}_stack_t curr = stack;
  size_t count = 0;
  while (curr != NULL) {
    ++count;
    curr = curr->next;
  }
  return count;
}
