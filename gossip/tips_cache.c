/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/tips_cache.h"

/*
 * Private functions
 */

static retcode_t tips_cache_fifo_add(hash243_set_t* const set,
                                     size_t const capacity,
                                     flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (set == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  if (hash243_set_size(set) >= capacity) {
    if ((ret = hash243_set_remove_entry(set, *set)) != RC_OK) {
      return ret;
    }
  }

  return hash243_set_add(set, tip);
}

/*
 * Public functions
 */

retcode_t tips_cache_init(tips_cache_t* const cache, size_t const capacity) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  cache->tips = NULL;
  cache->solid_tips = NULL;
  cache->capacity = capacity;
  rw_lock_handle_init(&cache->lock);

  return RC_OK;
}

retcode_t tips_cache_destroy(tips_cache_t* const cache) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  hash243_set_free(&cache->tips);
  hash243_set_free(&cache->solid_tips);
  rw_lock_handle_destroy(&cache->lock);

  return RC_OK;
}

retcode_t tips_cache_add(tips_cache_t* const cache,
                         flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->lock);
  ret = tips_cache_fifo_add(&cache->tips, cache->capacity, tip);
  rw_lock_handle_unlock(&cache->lock);

  return ret;
}

retcode_t tips_cache_remove(tips_cache_t* const cache,
                            flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->lock);
  ret = hash243_set_remove(&cache->tips, tip);
  rw_lock_handle_unlock(&cache->lock);

  return ret;
}

retcode_t tips_cache_set_solid(tips_cache_t* const cache,
                               flex_trit_t const* const tip) {
  retcode_t ret = RC_OK;

  if (cache == NULL || tip == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->lock);
  if (hash243_set_contains(&cache->tips, tip)) {
    if ((ret = hash243_set_remove(&cache->tips, tip)) != RC_OK) {
      goto done;
    }
    if ((ret = tips_cache_fifo_add(&cache->solid_tips, cache->capacity, tip)) !=
        RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&cache->lock);
  return ret;
}

size_t tips_cache_non_solid_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->lock);
  size = hash243_set_size(&cache->tips);
  rw_lock_handle_unlock(&cache->lock);

  return size;
}

size_t tips_cache_solid_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->lock);
  size = hash243_set_size(&cache->solid_tips);
  rw_lock_handle_unlock(&cache->lock);

  return size;
}

size_t tips_cache_size(tips_cache_t* const cache) {
  size_t size = 0;

  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&cache->lock);
  size = hash243_set_size(&cache->tips) + hash243_set_size(&cache->solid_tips);
  rw_lock_handle_unlock(&cache->lock);

  return size;
}
