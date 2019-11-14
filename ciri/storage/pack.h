/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_PACK_H__
#define __COMMON_STORAGE_PACK_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"

typedef struct iota_stor_pack_s {
  void **models;
  size_t capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_stor_pack_t;

#define DECLARE_PACK_SINGLE(TYPE, NAME, PTR_NAME, PACK_NAME) \
  TYPE NAME;                                                 \
  TYPE *PTR_NAME = &NAME;                                    \
  iota_stor_pack_t PACK_NAME = {                             \
      .models = (void **)(&PTR_NAME), .capacity = 1, .num_loaded = 0, .insufficient_capacity = false};

#define DECLARE_PACK_SINGLE_TX(NAME, PTR_NAME, PACK_NAME) \
  DECLARE_PACK_SINGLE(iota_transaction_t, NAME, PTR_NAME, PACK_NAME)

#define DECLARE_PACK_SINGLE_MILESTONE(NAME, PTR_NAME, PACK_NAME) \
  DECLARE_PACK_SINGLE(iota_milestone_t, NAME, PTR_NAME, PACK_NAME)

#ifdef __cplusplus
extern "C" {
#endif

retcode_t hash_pack_resize(iota_stor_pack_t *pack, size_t resize_factor);
retcode_t hash_pack_init(iota_stor_pack_t *pack, size_t size);
retcode_t hash_pack_reset(iota_stor_pack_t *pack);
retcode_t hash_pack_free(iota_stor_pack_t *pack);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_PACK_H__
