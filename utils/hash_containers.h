/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HASH_CONTAINERS_H__
#define __UTILS_HASH_CONTAINERS_H__

#include "uthash.h"
#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_list_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_list_entry_s *next;
} hash_list_entry_t;
typedef hash_list_entry_t *hash_list_t;

typedef struct hash_queue_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_queue_entry_s *next;
  struct hash_queue_entry_s *prev;
} hash_queue_entry_t;
typedef hash_queue_entry_t *hash_queue_t;

typedef struct hash_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  UT_hash_handle hh;
} hash_set_entry_t;
typedef hash_set_entry_t *hash_set_t;

retcode_t hash_queue_push(hash_queue_t *queue, flex_trit_t *hash);
retcode_t hash_queue_pop(hash_queue_t *queue);
flex_trit_t *hash_queue_peek(hash_queue_t queue);
retcode_t hash_queue_free(hash_queue_t *queue);

retcode_t hash_set_add(hash_set_t *set, flex_trit_t *hash);
retcode_t hash_set_append(hash_set_t *set, hash_set_t *clone);
bool hash_set_contains(hash_set_t *set, flex_trit_t *hash);
retcode_t hash_set_free(hash_set_t *set);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HASH_CONTAINERS_H__
