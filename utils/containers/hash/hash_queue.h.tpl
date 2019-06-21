/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_HASH_HASH{SIZE}_QUEUE_H__
#define __UTILS_CONTAINERS_HASH_HASH{SIZE}_QUEUE_H__

#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash{SIZE}_queue_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_{SIZE}];
  struct hash{SIZE}_queue_entry_s *next;
  struct hash{SIZE}_queue_entry_s *prev;
} hash{SIZE}_queue_entry_t;

typedef hash{SIZE}_queue_entry_t *hash{SIZE}_queue_t;

bool hash{SIZE}_queue_empty(hash{SIZE}_queue_t const queue);
retcode_t hash{SIZE}_queue_push(hash{SIZE}_queue_t *const queue, flex_trit_t const *const hash);
hash{SIZE}_queue_entry_t *hash{SIZE}_queue_pop(hash{SIZE}_queue_t *const queue);
flex_trit_t *hash{SIZE}_queue_peek(hash{SIZE}_queue_t const queue);
void hash{SIZE}_queue_free(hash{SIZE}_queue_t *const queue);
size_t hash{SIZE}_queue_count(hash{SIZE}_queue_t const queue);
flex_trit_t *hash{SIZE}_queue_at(hash{SIZE}_queue_t const queue, size_t index);
retcode_t hash{SIZE}_queue_copy(hash{SIZE}_queue_t *dest, hash{SIZE}_queue_t const src, size_t size);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH{SIZE}_QUEUE_H__
