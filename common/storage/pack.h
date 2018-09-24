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

#ifdef __cplusplus
extern "C" {
#endif

extern retcode_t hash_pack_resize(iota_stor_pack_t *pack, size_t resize_factor);

extern retcode_t hash_pack_init(iota_stor_pack_t *pack, size_t size);

extern retcode_t hash_pack_reset(iota_stor_pack_t *pack);

extern retcode_t hash_pack_free(iota_stor_pack_t *pack);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_PACK_H__
