/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "{PARENT_DIRECTORY}/{TYPE}_set.h"

size_t {TYPE}_set_size({TYPE}_set_t const set) { return HASH_COUNT(set); }

retcode_t {TYPE}_set_add({TYPE}_set_t *const set, {TYPE} const *const value) {
  {TYPE}_set_entry_t *entry = NULL;

  if (!{TYPE}_set_contains(*set, value)) {
    if ((entry = ({TYPE}_set_entry_t *)malloc(sizeof({TYPE}_set_entry_t))) == NULL) {
      return RC_OOM;
    }
    memcpy(&entry->value, value, sizeof({TYPE}));
    HASH_ADD(hh, *set, value, sizeof({TYPE}), entry);
  }
  return RC_OK;
}

retcode_t {TYPE}_set_remove({TYPE}_set_t *const set, {TYPE} const *const value) {
  {TYPE}_set_entry_t *entry = NULL;

  if (set != NULL && *set != NULL && value != NULL) {
    HASH_FIND(hh, *set, value, sizeof({TYPE}), entry);
    return {TYPE}_set_remove_entry(set, entry);
  }
  return RC_OK;
}

retcode_t {TYPE}_set_remove_entry({TYPE}_set_t *const set, {TYPE}_set_entry_t *const entry) {
  if (set != NULL && *set != NULL && entry != NULL) {
    HASH_DEL(*set, entry);
    free(entry);
  }
  return RC_OK;
}

retcode_t {TYPE}_set_append({TYPE}_set_t const *const set1, {TYPE}_set_t *const set2) {
  retcode_t ret = RC_OK;
  {TYPE}_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set1, iter, tmp) {
    if ((ret = {TYPE}_set_add(set2, &iter->value)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}

bool {TYPE}_set_contains({TYPE}_set_t const set, {TYPE} const *const value) {
  {TYPE}_set_entry_t *entry = NULL;

  if (set == NULL) {
    return false;
  }

  HASH_FIND(hh, set, value, sizeof({TYPE}), entry);
  return entry != NULL;
}

bool {TYPE}_set_find({TYPE}_set_t const set, {TYPE} const *const value, {TYPE}_set_entry_t **entry) {
  if (set == NULL) {
    return false;
  }

  if (entry == NULL) {
    return RC_NULL_PARAM;
  }

  HASH_FIND(hh, set, value, sizeof({TYPE}), *entry);
  return *entry != NULL;
}

void {TYPE}_set_free({TYPE}_set_t *const set) {
  {TYPE}_set_entry_t *iter = NULL, *tmp = NULL;

  if (set == NULL || *set == NULL) {
    return;
  }

  HASH_ITER(hh, *set, iter, tmp) {
    HASH_DEL(*set, iter);
    free(iter);
  }
  *set = NULL;
}

retcode_t {TYPE}_set_for_each({TYPE}_set_t const set, {TYPE}_on_container_func func, void *const container) {
  retcode_t ret = RC_OK;
  {TYPE}_set_entry_t *curr_entry = NULL;
  {TYPE}_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, set, curr_entry, tmp_entry) {
    if ((ret = func(container, &curr_entry->value)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}

bool {TYPE}_set_cmp({TYPE}_set_t const lhs, {TYPE}_set_t const rhs) {
  if (HASH_COUNT(lhs) != HASH_COUNT(rhs)) {
    return false;
  }

  {TYPE}_set_entry_t *curr_entry = NULL;
  {TYPE}_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, lhs, curr_entry, tmp_entry) {
    if (!({TYPE}_set_contains(rhs, &curr_entry->value))) {
      return false;
    }
  }
  return true;
}
