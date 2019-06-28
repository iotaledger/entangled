/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_TIPS_H__
#define __NODE_TIPS_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/handles/rw_lock.h"

// A fixed capacity FIFO-behaving tips cache
typedef struct tips_cache_s {
  hash243_set_t tips;
  rw_lock_handle_t tips_lock;
  hash243_set_t solid_tips;
  rw_lock_handle_t solid_tips_lock;
  size_t capacity;
} tips_cache_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a tips cache
 *
 * @param cache The cache
 * @param capacity The cache capacity
 *
 * @return a status code
 */
retcode_t tips_cache_init(tips_cache_t* const cache, size_t const capacity);

/**
 * Destroys a tips cache
 *
 * @param cache The cache
 *
 * @return a status code
 */
retcode_t tips_cache_destroy(tips_cache_t* const cache);

/**
 * Gets all tips from a tips cache
 *
 * @param cache The cache
 * @param tips A stack of tips to be filled
 *
 * @return a status code
 */
retcode_t tips_cache_get_tips(tips_cache_t* const cache, hash243_stack_t* const tips);

/**
 * Adds a tip to a tips cache
 *
 * @param cache The cache
 * @param tip The tip
 *
 * @return a status code
 */
retcode_t tips_cache_add(tips_cache_t* const cache, flex_trit_t const* const tip);

/**
 * Removes a tip from a tips cache
 *
 * @param cache The cache
 * @param tip The tip
 *
 * @return a status code
 */
retcode_t tips_cache_remove(tips_cache_t* const cache, flex_trit_t const* const tip);

/**
 * Sets a cached tip as solid
 *
 * @param cache The cache
 * @param tip The tip
 *
 * @return a status code
 */
retcode_t tips_cache_set_solid(tips_cache_t* const cache, flex_trit_t const* const tip);

/**
 * Gets the number of non solid tips in a tips cache
 *
 * @param cache The cache
 *
 * @return the number of non solid tips
 */
size_t tips_cache_non_solid_size(tips_cache_t* const cache);

/**
 * Gets the number of solid tips in a tips cache
 *
 * @param cache The cache
 *
 * @return the number of solid tips
 */
size_t tips_cache_solid_size(tips_cache_t* const cache);

/**
 * Gets the total number of tips in a tips cache
 *
 * @param cache The cache
 *
 * @return the total number of tips
 */
size_t tips_cache_size(tips_cache_t* const cache);

/**
 * Gets a random tip from a tip cache
 *
 * @param cache The cache
 * @param tip A tip to be filled
 *
 * @return a status code
 */
retcode_t tips_cache_random_tip(tips_cache_t* const cache, flex_trit_t* const tip);

/**
 * Gets a random solid tip from a tip cache
 *
 * @param cache The cache
 * @param tip A tip to be filled
 *
 * @return a status code
 */
retcode_t tips_cache_random_solid_tip(tips_cache_t* const cache, flex_trit_t* const tip);

#ifdef __cplusplus
}
#endif

#endif  // __NODE_TIPS_H__
