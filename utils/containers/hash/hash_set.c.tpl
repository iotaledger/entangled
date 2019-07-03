/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash{SIZE}_set.h"
#include "utils/handles/rand.h"

uint32_t hash{SIZE}_set_size(hash{SIZE}_set_t const set) { return HASH_COUNT(set); }

retcode_t hash{SIZE}_set_add(hash{SIZE}_set_t *const set, flex_trit_t const *const hash) {
  hash{SIZE}_set_entry_t *entry = NULL;

  if (!hash{SIZE}_set_contains(*set, hash)) {
    if ((entry = (hash{SIZE}_set_entry_t *)malloc(sizeof(hash{SIZE}_set_entry_t))) == NULL) {
      return RC_OOM;
    }
    memcpy(entry->hash, hash, FLEX_TRIT_SIZE_{SIZE});
    HASH_ADD(hh, *set, hash, FLEX_TRIT_SIZE_{SIZE}, entry);
  }
  return RC_OK;
}

retcode_t hash{SIZE}_set_remove(hash{SIZE}_set_t *const set, flex_trit_t const *const hash) {
  hash{SIZE}_set_entry_t *entry = NULL;

  if (set != NULL && *set != NULL && hash != NULL) {
    HASH_FIND(hh, *set, hash, FLEX_TRIT_SIZE_{SIZE}, entry);
    return hash{SIZE}_set_remove_entry(set, entry);
  }
  return RC_OK;
}

retcode_t hash{SIZE}_set_remove_entry(hash{SIZE}_set_t *const set, hash{SIZE}_set_entry_t *const entry) {
  if (set != NULL && *set != NULL && entry != NULL) {
    HASH_DEL(*set, entry);
    free(entry);
  }
  return RC_OK;
}

retcode_t hash{SIZE}_set_append(hash{SIZE}_set_t const *const set1, hash{SIZE}_set_t *const set2) {
  retcode_t ret = RC_OK;
  hash{SIZE}_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set1, iter, tmp) {
    if ((ret = hash{SIZE}_set_add(set2, iter->hash)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}

bool hash{SIZE}_set_contains(hash{SIZE}_set_t const set, flex_trit_t const *const hash) {
  hash{SIZE}_set_entry_t *entry = NULL;

  if (set == NULL) {
    return false;
  }

  HASH_FIND(hh, set, hash, FLEX_TRIT_SIZE_{SIZE}, entry);
  return entry != NULL;
}

bool hash{SIZE}_set_find(hash{SIZE}_set_t const set, flex_trit_t const *const hash, hash{SIZE}_set_entry_t **entry) {
  if (set == NULL) {
    return false;
  }

  if (entry == NULL) {
    return RC_NULL_PARAM;
  }

  HASH_FIND(hh, set, hash, FLEX_TRIT_SIZE_{SIZE}, *entry);
  return *entry != NULL;
}

void hash{SIZE}_set_free(hash{SIZE}_set_t *const set) {
  hash{SIZE}_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set, iter, tmp) {
    HASH_DEL(*set, iter);
    free(iter);
  }
  *set = NULL;
}

retcode_t hash{SIZE}_set_for_each(hash{SIZE}_set_t const set, hash{SIZE}_on_container_func func,
                                   void *const container) {
  retcode_t ret = RC_OK;
  hash{SIZE}_set_entry_t *curr_entry = NULL;
  hash{SIZE}_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, set, curr_entry, tmp_entry) {
    if ((ret = func(container, curr_entry->hash)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}

retcode_t hash{SIZE}_set_random_hash(hash{SIZE}_set_t const *const set, flex_trit_t *const hash) {
  hash{SIZE}_set_entry_t *iter = NULL;
  hash{SIZE}_set_entry_t *tmp = NULL;

  if (set == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  int index = rand_handle_rand_interval(0, hash{SIZE}_set_size(*set));
  HASH_ITER(hh, *set, iter, tmp) {
    if (index-- == 0) {
      memcpy(hash, iter->hash, FLEX_TRIT_SIZE_{SIZE});
      break;
    }
  }

  return RC_OK;
}
