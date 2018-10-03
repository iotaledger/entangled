/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_TRAVERSAL_H__
#define __UTILS_TRAVERSAL_H__

#include "uthash.h"
#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_queue_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_queue_s *next;
  struct hash_queue_s *prev;
} hash_queue_t;

typedef struct hash_set_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  UT_hash_handle hh;
} hash_set_t;

extern retcode_t hash_queue_push(hash_queue_t **queue, flex_trit_t *hash);
extern flex_trit_t *hash_queue_peek(hash_queue_t *queue);
extern retcode_t hash_queue_free(hash_queue_t **queue);

extern retcode_t hash_set_add(hash_set_t **set, flex_trit_t *hash);
extern bool hash_set_contains(hash_set_t **set, flex_trit_t *hash);
extern retcode_t hash_set_free(hash_set_t **set);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_TRAVERSAL_H__
