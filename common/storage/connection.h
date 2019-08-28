/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_CONNECTION_H__
#define __COMMON_STORAGE_CONNECTION_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum storage_connection_type_e {
  STORAGE_CONNECTION_TANGLE,
  STORAGE_CONNECTION_SPENT_ADDRESSES,
} storage_connection_type_t;

typedef struct storage_connection_s {
  void* actual;
  storage_connection_type_t type;
} storage_connection_t;

typedef struct storage_connection_config_s {
  char const* db_path;
} storage_connection_config_t;

extern retcode_t storage_connection_init(storage_connection_t* const connection,
                                         storage_connection_config_t const* const config,
                                         storage_connection_type_t const type);
extern retcode_t storage_connection_destroy(storage_connection_t* const connection);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_CONNECTION_H__
