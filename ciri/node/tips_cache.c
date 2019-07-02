/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/tips_cache.h"

/*
 * Private functions
 */

static retcode_t tips_cache_fifo_add(hash243_set_t* const set, size_t const capacity, flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (set == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  if (hash243_set_size(*set) >= capacity) {
    if ((ret = hash243_set_remove_entry(set, *set)) != RC_OK) {
      return ret;
    }
  }

  return hash243_set_add(set, tip);
}

static retcode_t tips_cache_random_tip_from_set(hash243_set_t* const set, flex_trit_t* const tip) {
  if (set == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  if (hash243_set_size(*set) == 0) {
    memset(tip, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    return RC_OK;
  }

  return hash243_set_random_hash(set, tip);
}

/*
 * Public functions
 */

retcode_t tips_cache_init(tips_cache_t* const cache, size_t const capacity) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  cache->tips = NULL;
  rw_lock_handle_init(&cache->tips_lock);
  cache->solid_tips = NULL;
  rw_lock_handle_init(&cache->solid_tips_lock);
  cache->capacity = capacity;

  return RC_OK;
}

retcode_t tips_cache_destroy(tips_cache_t* const cache) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  hash243_set_free(&cache->tips);
  rw_lock_handle_destroy(&cache->tips_lock);
  hash243_set_free(&cache->solid_tips);
  rw_lock_handle_destroy(&cache->solid_tips_lock);

  return RC_OK;
}

retcode_t tips_cache_get_tips(tips_cache_t* const cache, hash243_stack_t* const tips) {
  retcode_t ret = RC_OK;
  hash243_set_entry_t* iter = NULL;
  hash243_set_entry_t* tmp = NULL;

  if (cache == NULL || tips == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->tips_lock);
  HASH_SET_ITER(cache->tips, iter, tmp) {
    if ((ret = hash243_stack_push(tips, iter->hash)) != RC_OK) {
      break;
    }
  }
  rw_lock_handle_unlock(&cache->tips_lock);

  if (ret != RC_OK) {
    return ret;
  }

  rw_lock_handle_rdlock(&cache->solid_tips_lock);
  HASH_SET_ITER(cache->solid_tips, iter, tmp) {
    if ((ret = hash243_stack_push(tips, iter->hash)) != RC_OK) {
      break;
    }
  }
  rw_lock_handle_unlock(&cache->solid_tips_lock);

  return ret;
}

retcode_t tips_cache_add(tips_cache_t* const cache, flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->tips_lock);
  ret = tips_cache_fifo_add(&cache->tips, cache->capacity, tip);
  rw_lock_handle_unlock(&cache->tips_lock);

  return ret;
}

retcode_t tips_cache_remove(tips_cache_t* const cache, flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->tips_lock);
  ret = hash243_set_remove(&cache->tips, tip);
  rw_lock_handle_unlock(&cache->tips_lock);

  if (ret != RC_OK) {
    return ret;
  }

  rw_lock_handle_wrlock(&cache->solid_tips_lock);
  ret = hash243_set_remove(&cache->solid_tips, tip);
  rw_lock_handle_unlock(&cache->solid_tips_lock);

  return ret;
}

retcode_t tips_cache_set_solid(tips_cache_t* const cache, flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;
  bool solidify = false;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->tips_lock);
  if ((solidify = hash243_set_contains(cache->tips, tip))) {
    ret = hash243_set_remove(&cache->tips, tip);
  }
  rw_lock_handle_unlock(&cache->tips_lock);

  if (ret != RC_OK) {
    return ret;
  }

  if (solidify) {
    rw_lock_handle_wrlock(&cache->solid_tips_lock);
    ret = tips_cache_fifo_add(&cache->solid_tips, cache->capacity, tip);
    rw_lock_handle_unlock(&cache->solid_tips_lock);
  }

  return ret;
}

size_t tips_cache_non_solid_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->tips_lock);
  size = hash243_set_size(cache->tips);
  rw_lock_handle_unlock(&cache->tips_lock);

  return size;
}

size_t tips_cache_solid_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->solid_tips_lock);
  size = hash243_set_size(cache->solid_tips);
  rw_lock_handle_unlock(&cache->solid_tips_lock);

  return size;
}

size_t tips_cache_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->tips_lock);
  size = hash243_set_size(cache->tips);
  rw_lock_handle_unlock(&cache->tips_lock);

  rw_lock_handle_rdlock(&cache->solid_tips_lock);
  size += hash243_set_size(cache->solid_tips);
  rw_lock_handle_unlock(&cache->solid_tips_lock);

  return size;
}

retcode_t tips_cache_random_tip(tips_cache_t* const cache, flex_trit_t* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->tips_lock);
  ret = tips_cache_random_tip_from_set(&cache->tips, tip);
  rw_lock_handle_unlock(&cache->tips_lock);

  return ret;
}

retcode_t tips_cache_random_solid_tip(tips_cache_t* const cache, flex_trit_t* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->solid_tips_lock);
  ret = tips_cache_random_tip_from_set(&cache->solid_tips, tip);
  rw_lock_handle_unlock(&cache->solid_tips_lock);

  return ret;
}
