/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_SERVICE_H_
#define CCLIENT_SERVICE_H_

#include <stdlib.h>

#include "cclient/serialization/serializer.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTP request information
 *
 */
typedef struct {
  char const* host;         /**< Host name */
  char const* path;         /**< The path of HTTP/HTTPS request */
  char const* content_type; /**< Content type of request */
  char const* accept;       /**< Accept content type of response */
  char const* ca_pem;       /**< String of root ca */
  uint16_t port;            /**< Port number of the host*/
  int api_version;          /**< Number of IOTA API version */
} http_info_t;

/**
 * @brief client service
 *
 */
typedef struct {
  http_info_t http;                  /**< The http request information */
  serializer_t serializer;           /**< The client serializer */
  serializer_type_t serializer_type; /** The type of serialization */
} iota_client_service_t;

/**
 * @brief init CClient service
 *
 * @param serv service object
 * @return error code
 */
retcode_t iota_client_service_init(iota_client_service_t* serv);

/**
 * @brief destory and clean CClient service
 *
 * @param serv service object
 */
void iota_client_service_destroy(iota_client_service_t* serv);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERVICE_H_

/** @} */