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

typedef struct connection_config_t {
  char const* db_path;
} connection_config_t;

extern retcode_t connection_init(void** const connection,
                                 connection_config_t const* const config);
extern retcode_t connection_destroy(void* const connection);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_CONNECTION_H__
