/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_CONNECTION_H__
#define __COMMON_STORAGE_CONNECTION_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"

#include <logger.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct connection_t {
  void* db;
} connection_t;

typedef struct connection_config_t {
  const char* db_path;
  bool index_address;
  bool index_approvee;
  bool index_bundle;
  bool index_tag;
} connection_config_t;

extern retcode_t init_connection(connection_t* const,
                                 const connection_config_t* const config);
extern retcode_t destroy_connection(connection_t* const);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_CONNECTION_H__
