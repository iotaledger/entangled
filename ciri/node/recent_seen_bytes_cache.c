/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/recent_seen_bytes_cache.h"

retcode_t recent_seen_bytes_cache_init(recent_seen_bytes_cache_t *const cache, size_t const capacity) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  cache->capacity = capacity;
  cache->miss = 0;
  cache->hit = 0;
  rw_lock_handle_init(&cache->lock);

  return uint64_t_to_flex_trit_t_map_init(&cache->map, sizeof(uint64_t), FLEX_TRIT_SIZE_243);
}

retcode_t recent_seen_bytes_cache_destroy(recent_seen_bytes_cache_t *const cache) {
  if (cache == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_destroy(&cache->lock);

  return uint64_t_to_flex_trit_t_map_free(&cache->map);
}

retcode_t recent_seen_bytes_cache_get(recent_seen_bytes_cache_t *const cache, uint64_t const digest,
                                      flex_trit_t *const hash, bool *const found) {
  retcode_t ret = RC_OK;
  uint64_t_to_flex_trit_t_map_entry_t *entry = NULL;

  if (cache == NULL || hash == NULL || found == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->lock);

  if ((*found = uint64_t_to_flex_trit_t_map_find(cache->map, &digest, &entry))) {
    memcpy(hash, entry->value, FLEX_TRIT_SIZE_243);
    cache->hit++;
  } else {
    cache->miss++;
  }

  rw_lock_handle_unlock(&cache->lock);

  return ret;
}

retcode_t recent_seen_bytes_cache_put(recent_seen_bytes_cache_t *const cache, uint64_t const digest,
                                      flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  uint64_t_to_flex_trit_t_map_entry_t *entry = NULL;

  if (cache == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&cache->lock);

  if (!uint64_t_to_flex_trit_t_map_find(cache->map, &digest, &entry)) {
    if (uint64_t_to_flex_trit_t_map_size(cache->map) >= cache->capacity) {
      if ((ret = uint64_t_to_flex_trit_t_map_remove_entry(&cache->map, cache->map.map)) != RC_OK) {
        goto done;
      }
    }
    ret = uint64_t_to_flex_trit_t_map_add(&cache->map, &digest, hash);
  }

done:
  rw_lock_handle_unlock(&cache->lock);

  return ret;
}
