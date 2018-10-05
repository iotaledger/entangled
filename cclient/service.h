/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERVICE_H_
#define CCLIENT_SERVICE_H_

#include <stdlib.h>

#include "common/errors.h"
#include "serialization/serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum serializer_type_t {
  SR_JSON = 0,
  SR_UNIMPLEMENTED,
};
typedef enum serializer_type_t serializer_type_t;

typedef struct {
  const char* host;
  size_t port;
  int api_version;  // IOTA API version number.
} http_info_t;

typedef struct {
  http_info_t http;
  serializer_t serializer;
  serializer_type_t serializer_type;
} iota_client_service_t;

retcode_t iota_client_service_init(iota_client_service_t* serv);
void iota_client_service_destroy(iota_client_service_t* serv);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERVICE_H_
