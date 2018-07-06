// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERVICE_H_
#define CCLIENT_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "serialization/serializer.h"

typedef struct {
  char const host[255];
  size_t port;
  char const content_type[255];
  int version;
  int timeout;
  serializer_t serializer;
} iota_service_t;

typedef struct {
  size_t is_error;
  size_t duration;
} iota_api_result_t;

typedef struct {
  char* data;
  size_t length;
} iota_response_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERVICE_H_
