/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */


#include <assert.h>
#include "utils/containers/{KEY_TYPE}_to_{VALUE_TYPE}_map.h"

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_add({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                            {KEY_TYPE} const *const key,
                                            {VALUE_TYPE} const value) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *map_entry = NULL;
  map_entry = ({KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *)malloc(
      sizeof({KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t));

  if (map_entry == NULL) {
    return RC_UTILS_OOM;
  }

  memcpy(&map_entry->key,key, sizeof({KEY_TYPE}));
  memcpy(&map_entry->value,&value, sizeof({VALUE_TYPE}));
  HASH_ADD(hh, *map, key, sizeof({KEY_TYPE}), map_entry);
  return RC_OK;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_contains(
    {KEY_TYPE}_to_{VALUE_TYPE}_map_t const *const map,
    {KEY_TYPE} const *const key) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *entry = NULL;

  if (*map == NULL) {
    return false;
  }

  HASH_FIND(hh, *map, key, sizeof({KEY_TYPE}), entry);
  return entry != NULL;
}

{VALUE_TYPE} {KEY_TYPE}_to_{VALUE_TYPE}_map_at(
{KEY_TYPE}_to_{VALUE_TYPE}_map_t const *const map,
{KEY_TYPE} const *const key){
{KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *entry = NULL;

assert(*map);
HASH_FIND(hh, *map, key, sizeof({KEY_TYPE}), entry);
assert(entry);
return entry->value;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_set(
{KEY_TYPE}_to_{VALUE_TYPE}_map_t const *const map,
{KEY_TYPE} const *const key,
{VALUE_TYPE} const * const value) {
{KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t* res;
if (map == NULL || *map == NULL) {
return false;
}

HASH_FIND(hh, *map, key, sizeof({KEY_TYPE}), res);
memcpy(res->value,value,sizeof({VALUE_TYPE}));
return res != NULL;
}

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_find(
    {KEY_TYPE}_to_{VALUE_TYPE}_map_t const *const map,
    {KEY_TYPE} const *const key,
    {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t **const res) {
  if (map == NULL || *map == NULL) {
    return false;
  }
  if (res == NULL) {
    return false;
  }

  HASH_FIND(hh, *map, key, sizeof({KEY_TYPE}), *res);
  return *res != NULL;
}

void {KEY_TYPE}_to_{VALUE_TYPE}_map_set_all({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map, {VALUE_TYPE} const * const value) {
{KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *curr_entry = NULL;
{KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *tmp_entry = NULL;

HASH_ITER(hh, *map, curr_entry, tmp_entry) {
memcpy(&curr_entry->value,value,sizeof({VALUE_TYPE}));
}
}


void {KEY_TYPE}_to_{VALUE_TYPE}_map_free({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map) {
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *curr_entry = NULL;
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *map, curr_entry, tmp_entry) {
    HASH_DEL(*map, curr_entry);
    free(curr_entry);
  }
  *map = NULL;
}


