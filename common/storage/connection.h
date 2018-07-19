/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_CONNECTION_H__
#define __COMMON_STORAGE_SQL_CONNECTION_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

struct connection_t {
  void* db;
};

struct connection_config_t {
  const char* dbPath;
  bool indexAddress;
  bool indexApprovee;
  bool indexBundle;
  bool indexTag;
};

extern retcode_t init_connection(connection_t* const,
                                 const connection_config_t* const config);
extern retcode_t destroy_connection(connection_t* const);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_CONNECTION_H__
