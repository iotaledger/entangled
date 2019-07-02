/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_SET_{TYPE}_SET_H__
#define __UTILS_CONTAINERS_SET_{TYPE}_SET_H__

#include "uthash.h"

#include <inttypes.h>
#include <stdbool.h>
#include "common/errors.h"
{ADDITIONAL_INCLUDE_PATH}

#define SET_ITER(set, entry, tmp) HASH_ITER(hh, set, entry, tmp)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {TYPE}_set_entry_s {
  {TYPE} value;
  UT_hash_handle hh;
}
{TYPE}_set_entry_t;

typedef {TYPE}_set_entry_t *{TYPE}_set_t;

typedef retcode_t (*{TYPE}_on_container_func)(void *container, {TYPE} * type);

size_t {TYPE}_set_size({TYPE}_set_t const set);
retcode_t {TYPE}_set_add({TYPE}_set_t *const set, {TYPE} const *const value);
retcode_t {TYPE}_set_remove({TYPE}_set_t *const set, {TYPE} const *const value);
retcode_t {TYPE}_set_remove_entry({TYPE}_set_t *const set, {TYPE}_set_entry_t *const entry);
retcode_t {TYPE}_set_append({TYPE}_set_t const *const set1, {TYPE}_set_t *const set2);
bool {TYPE}_set_contains({TYPE}_set_t const set, {TYPE} const *const value);
bool {TYPE}_set_find({TYPE}_set_t const set, {TYPE} const *const, {TYPE}_set_entry_t **entry);
void {TYPE}_set_free({TYPE}_set_t *const set);
retcode_t {TYPE}_set_for_each({TYPE}_set_t const set, {TYPE}_on_container_func func, void *const container);

bool {TYPE}_set_cmp({TYPE}_set_t const lhs, {TYPE}_set_t const rhs);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_SET_{TYPE}_SET_H__
