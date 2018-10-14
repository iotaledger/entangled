/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_CONNECTION_H__
#define __COMMON_STORAGE_CONNECTION_H__

#include <stdbool.h>

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct connection_t {
  void* db;
} connection_t;

typedef struct connection_config_t {
  const char* db_path;
} connection_config_t;

extern retcode_t init_connection(const connection_t* const,
                                 const connection_config_t* const config);
extern retcode_t destroy_connection(const connection_t* const);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_CONNECTION_H__
