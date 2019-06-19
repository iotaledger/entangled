/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_HTTP_HTTP_H__
#define __CIRI_API_HTTP_HTTP_H__

#include <stdbool.h>
#include <stdint.h>

#include "utarray.h"

#include "cclient/serialization/serializer.h"
#include "ciri/api/api.h"
#include "common/errors.h"
#include "common/storage/connection.h"
#include "consensus/consensus.h"
#include "node/components/broadcaster.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_api_http_s {
  bool running;
  iota_api_t *api;
  serializer_t serializer;
  void *state;
  UT_array *tangle_db_connections;
  UT_array *spent_addresses_db_connections;

} iota_api_http_t;

/**
 * Initializes an HTTP API
 *
 * @param http_api The HTTP API
 * @param api An API instance
 *
 * @return a status code
 */
retcode_t iota_api_http_init(iota_api_http_t *const http, iota_api_t *const api);

/**
 * Starts an HTTP API
 *
 * @param api The HTTP API
 *
 * @return a status code
 */
retcode_t iota_api_http_start(iota_api_http_t *const api);

/**
 * Stops an HTTP API
 *
 * @param api The HTTP API
 *
 * @return a status code
 */
retcode_t iota_api_http_stop(iota_api_http_t *const api);

/**
 * Destroys an HTTP API
 *
 * @param api The HTTP API
 *
 * @return a status code
 */
retcode_t iota_api_http_destroy(iota_api_http_t *const api);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_API_HTTP_HTTP_H__
