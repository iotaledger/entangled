/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "{PARENT_DIRECTORY}/{KEY_TYPE}_to_{VALUE_TYPE}_map.h"

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_init({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map, size_t const key_size,
                                               size_t const value_size) {
  map->key_size = key_size;
  map->value_size = value_size;
  map->map = NULL;

  return RC_OK;
}

size_t {KEY_TYPE}_to_{VALUE_TYPE}_map_size({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map) {
  return HASH_COUNT(map.map);
}

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_add({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                              {KEY_TYPE} const *const key, {VALUE_TYPE} const *const value) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *map_entry = NULL;
  map_entry = ({KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *)malloc(sizeof({KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t));

  if (map_entry == NULL) {
    return RC_OOM;
  }

  if ((map_entry->key = ({KEY_TYPE} *)malloc(map->key_size)) == NULL) {
    return RC_OOM;
  }

  if ((map_entry->value = ({VALUE_TYPE} *)malloc(map->value_size)) == NULL) {
    return RC_OOM;
  }

  memcpy(map_entry->key, key, map->key_size);
  memcpy(map_entry->value, value, map->value_size);
  HASH_ADD_KEYPTR(hh, map->map, map_entry->key, map->key_size, map_entry);

  return RC_OK;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_contains({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map,
                                              {KEY_TYPE} const *const key) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *entry = NULL;

  if (map.map == NULL) {
    return false;
  }

  HASH_FIND(hh, map.map, key, map.key_size, entry);

  return entry != NULL;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_find({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map, {KEY_TYPE} const *const key,
                                          {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t **res) {
  if (map.map == NULL) {
    return false;
  }

  HASH_FIND(hh, map.map, key, map.key_size, *res);

  return *res != NULL;
}

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_free({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *curr_entry = NULL;
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, map->map, curr_entry, tmp_entry) {
    free(curr_entry->key);
    free(curr_entry->value);
    HASH_DEL(map->map, curr_entry);
    free(curr_entry);
  }

  map->map = NULL;
  return RC_OK;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_cmp({KEY_TYPE}_to_{VALUE_TYPE}_map_t const lhs,
                                         {KEY_TYPE}_to_{VALUE_TYPE}_map_t const rhs) {
  if (HASH_COUNT(lhs.map) != HASH_COUNT(rhs.map)) {
    return false;
  }

  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *curr_entry = NULL;
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, lhs.map, curr_entry, tmp_entry) {
    if (!{KEY_TYPE}_to_{VALUE_TYPE}_map_contains(rhs, curr_entry->key)) {
      return false;
    }
  }
  return true;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_remove({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                            {KEY_TYPE} const *const key) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *entry = NULL;

  if (map == NULL || map->map == NULL) {
    return false;
  }

  HASH_FIND(hh, map->map, key, map->key_size, entry);

  {KEY_TYPE}_to_{VALUE_TYPE}_map_remove_entry(map, entry);

  return entry != NULL;
}

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_remove_entry({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                                       {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *const entry) {
  if (entry != NULL) {
    free(entry->key);
    free(entry->value);
    HASH_DEL(map->map, entry);
    free(entry);
  }

  return RC_OK;
}
