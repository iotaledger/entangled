/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_{KEY_TYPE}_{VALUE_TYPE}_MAP_H__
#define __UTILS_CONTAINERS_{KEY_TYPE}_{VALUE_TYPE}_MAP_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/trinary/trits.h"
#include "uthash.h"
{ADDITIONAL_INCLUDE_PATH}

  /*
   * This Generic map allows mapping any key type to any value type
   * assuming that key can be trivially copied, to allow for
   * user-defined types, user should add dependency in "map_generator.bzl"
   * and include the required files in this header file
   */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_s {
  {KEY_TYPE} * key;
  {VALUE_TYPE} * value;
  UT_hash_handle hh;
} {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t;

typedef struct {KEY_TYPE}_to_{VALUE_TYPE}_map_s {
  size_t key_size;
  size_t value_size;
  {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *map;
} {KEY_TYPE}_to_{VALUE_TYPE}_map_t;

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_init({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map, size_t const key_size,
                                               size_t const value_size);

size_t {KEY_TYPE}_to_{VALUE_TYPE}_map_size({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map);

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_add({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                              {KEY_TYPE} const *const key, {VALUE_TYPE} const *const value);

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_contains({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map,
                                              {KEY_TYPE} const *const key);

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_find({KEY_TYPE}_to_{VALUE_TYPE}_map_t const map, {KEY_TYPE} const *const key,
                                          {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t **res);

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_free({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map);

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_cmp({KEY_TYPE}_to_{VALUE_TYPE}_map_t const lhs,
                                         {KEY_TYPE}_to_{VALUE_TYPE}_map_t const rhs);

bool {KEY_TYPE}_to_{VALUE_TYPE}_map_remove({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                            {KEY_TYPE} const *const key);

retcode_t {KEY_TYPE}_to_{VALUE_TYPE}_map_remove_entry({KEY_TYPE}_to_{VALUE_TYPE}_map_t *const map,
                                                       {KEY_TYPE}_to_{VALUE_TYPE}_map_entry_t *const entry);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_{KEY_TYPE}_{VALUE_TYPE}_MAP_H__
