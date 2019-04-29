/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_RECENT_SEEN_BYTES_CACHE_H__
#define __GOSSIP_RECENT_SEEN_BYTES_CACHE_H__

#include "xxhash.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "gossip/iota_packet.h"
#include "gossip/uint64_t_to_flex_trit_t_map.h"
#include "utils/handles/rw_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

// A fixed capacity FIFO-behaving recent seen bytes cache
typedef struct recent_seen_bytes_cache_s {
  uint64_t_to_flex_trit_t_map_t map;
  size_t capacity;
  double drop_rate;
  uint64_t miss;
  uint64_t hit;
  rw_lock_handle_t lock;
} recent_seen_bytes_cache_t;

retcode_t recent_seen_bytes_cache_init(recent_seen_bytes_cache_t *const cache, size_t const capacity,
                                       double const drop_rate);
retcode_t recent_seen_bytes_cache_destroy(recent_seen_bytes_cache_t *const cache);
retcode_t recent_seen_bytes_cache_get(recent_seen_bytes_cache_t *const cache, uint64_t const digest,
                                      flex_trit_t *const hash, bool *const found);
retcode_t recent_seen_bytes_cache_put(recent_seen_bytes_cache_t *const cache, uint64_t const digest,
                                      flex_trit_t const *const hash);

static inline retcode_t recent_seen_bytes_cache_hash(byte_t const *const bytes, uint64_t *const digest) {
  if (bytes == NULL || digest == NULL) {
    return RC_NULL_PARAM;
  }

  *digest = XXH64(bytes, PACKET_TX_SIZE, 0);

  return RC_OK;
}

static inline size_t recent_seen_bytes_cache_size(recent_seen_bytes_cache_t *const cache) {
  size_t size = 0;

  rw_lock_handle_rdlock(&cache->lock);
  size = uint64_t_to_flex_trit_t_map_size(&cache->map);
  rw_lock_handle_unlock(&cache->lock);

  return size;
}

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_RECENT_SEEN_BYTES_CACHE_H__
