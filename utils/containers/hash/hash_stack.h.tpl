/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_HASH_HASH{SIZE}_STACK_H__
#define __UTILS_CONTAINERS_HASH_HASH{SIZE}_STACK_H__

#include "utstack.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#define hash{SIZE}_stack_empty(stack) STACK_EMPTY(stack)
#define HASH_STACK_FOREACH(stack, iter) for (iter = stack; iter != NULL; iter = iter->next)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash{SIZE}_stack_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_{SIZE}];
  struct hash{SIZE}_stack_entry_s *next;
}
hash{SIZE}_stack_entry_t;

typedef hash{SIZE}_stack_entry_t *hash{SIZE}_stack_t;

retcode_t hash{SIZE}_stack_push(hash{SIZE}_stack_t *const stack, flex_trit_t const *const hash);
void hash{SIZE}_stack_pop(hash{SIZE}_stack_t *const stack);
flex_trit_t *hash{SIZE}_stack_peek(hash{SIZE}_stack_t const stack);
void hash{SIZE}_stack_free(hash{SIZE}_stack_t *const stack);
size_t hash{SIZE}_stack_count(hash{SIZE}_stack_t const stack);
flex_trit_t *hash{SIZE}_stack_at(hash{SIZE}_stack_t const stack, size_t const index);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH{SIZE}_STACK_H__
