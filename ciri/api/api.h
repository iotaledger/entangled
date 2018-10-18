/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_API_H__
#define __CIRI_API_API_H__

#include <stdbool.h>
#include <stdint.h>

#include "cclient/serialization/serializer.h"
#include "common/errors.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_api_s {
  thread_handle_t thread;
  bool running;
  uint16_t port;
  serializer_t serializer;
  serializer_type_t serializer_type;
} iota_api_t;

/**
 * Initializes an API
 *
 * @param api The API
 * @param port The API port
 * @param serializer_type A serializer type
 *
 * @return a status code
 */
retcode_t iota_api_init(iota_api_t *const api, uint16_t port,
                        serializer_type_t serializer_type);

/**
 * Starts an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_start(iota_api_t *const api);

/**
 * Stops an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_stop(iota_api_t *const api);

/**
 * Destroys an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_destroy(iota_api_t *const api);

#ifdef __cplusplus
}
#endif

#endif
