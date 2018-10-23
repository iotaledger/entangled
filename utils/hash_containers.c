/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/hash_containers.h"

/*
 * Queue operations
 */

bool hash_queue_empty(hash_queue_t const queue) { return (queue == NULL); }

retcode_t hash_queue_push(hash_queue_t *const queue,
                          flex_trit_t const *const hash) {
  hash_dll_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash_dll_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(*queue, entry);
  return RC_OK;
}

void hash_queue_pop(hash_queue_t *const queue) {
  hash_dll_entry_t *tmp = NULL;

  tmp = *queue;
  CDL_DELETE(*queue, *queue);
  free(tmp);
}

flex_trit_t *hash_queue_peek(hash_queue_t const queue) {
  return (flex_trit_t *)(queue->hash);
}

void hash_queue_free(hash_queue_t *const queue) {
  hash_dll_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
}

size_t hash_queue_count(hash_queue_t *const queue) {
  hash_dll_entry_t *iter = NULL;
  size_t count = 0;
  CDL_COUNT(*queue, iter, count);
  return count;
}

flex_trit_t *hash_queue_at(hash_queue_t *const queue, size_t index) {
  hash_dll_entry_t *iter = NULL;
  size_t count = 0;
  CDL_FOREACH(*queue, iter) {
    if (count == index) {
      return (flex_trit_t *)(iter->hash);
    }
    count++;
  }
  return NULL;
}

/*
 * Stack operations
 */

bool hash_stack_empty(hash_stack_t const stack) { return (stack == NULL); }

retcode_t hash_stack_push(hash_stack_t *const stack,
                          flex_trit_t const *const hash) {
  hash_list_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash_list_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_243);
  LL_PREPEND(*stack, entry);
  return RC_OK;
}

void hash_stack_pop(hash_stack_t *const stack) {
  hash_list_entry_t *tmp = NULL;

  tmp = *stack;
  LL_DELETE(*stack, *stack);
  free(tmp);
}

flex_trit_t *hash_stack_peek(hash_stack_t const stack) {
  return (flex_trit_t *)(stack->hash);
}

void hash_stack_free(hash_stack_t *const stack) {
  hash_list_entry_t *iter = NULL, *tmp = NULL;

  LL_FOREACH_SAFE(*stack, iter, tmp) {
    LL_DELETE(*stack, iter);
    free(iter);
  }
}

size_t hash_stack_count(hash_stack_t const stack) {
  hash_stack_t curr = stack;
  size_t count = 0;
  while (curr != NULL) {
    ++count;
    curr = curr->next;
  }
  return count;
}
/*
 * Hash-int map operations
 */

retcode_t hash_int_map_add(hash_int_map_t *const map,
                           flex_trit_t const *const hash, int64_t const value) {
  hash_to_int_map_entry_t *map_entry = NULL;

  map_entry =
      (hash_to_int_map_entry_t *)malloc(sizeof(hash_to_int_map_entry_t));
  if (map_entry == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(map_entry->hash, hash, FLEX_TRIT_SIZE_243);
  map_entry->value = value;
  HASH_ADD(hh, *map, hash, FLEX_TRIT_SIZE_243, map_entry);
  return RC_OK;
}

bool hash_int_map_contains(hash_int_map_t const *const map,
                           flex_trit_t const *const hash) {
  hash_to_int_map_entry_t *entry = NULL;

  if (*map == NULL) {
    return false;
  }

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, entry);
  return entry != NULL;
}

void hash_int_map_free(hash_int_map_t *const map) {
  hash_to_int_map_entry_t *curr_entry = NULL;
  hash_to_int_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *map, curr_entry, tmp_entry) {
    HASH_DEL(*map, curr_entry);
    free(curr_entry);
  }
}

/*
 * Hash-indexed_hash_set map
 */

bool hash_to_indexed_hash_set_map_contains(
    hash_to_indexed_hash_set_map_t const *const map,
    flex_trit_t const *const hash) {
  hash_to_indexed_hash_set_entry_t *entry = NULL;

  if (!(*map)) {
    return false;
  }

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, entry);
  return entry != NULL;
}

retcode_t hash_to_indexed_hash_set_map_add_new_set(
    hash_to_indexed_hash_set_map_t *const map, flex_trit_t const *const hash,
    hash_to_indexed_hash_set_entry_t **const new_set_entry,
    size_t const index) {
  *new_set_entry = (hash_to_indexed_hash_set_entry_t *)malloc(
      sizeof(hash_to_indexed_hash_set_entry_t));
  if (*new_set_entry == NULL) {
    return RC_UTILS_OOM;
  }

  (*new_set_entry)->approvers = NULL;
  (*new_set_entry)->idx = index;
  memcpy((*new_set_entry)->hash, hash, FLEX_TRIT_SIZE_243);

  HASH_ADD(hh, *map, hash, FLEX_TRIT_SIZE_243, *new_set_entry);

  return RC_OK;
}

void hash_to_indexed_hash_set_map_free(
    hash_to_indexed_hash_set_map_t *const map) {
  hash_to_indexed_hash_set_entry_t *curr_entry = NULL;
  hash_to_indexed_hash_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *map, curr_entry, tmp_entry) {
    hash243_set_free(&((*map)->approvers));
    HASH_DEL(*map, curr_entry);
    free(curr_entry);
  }
}
