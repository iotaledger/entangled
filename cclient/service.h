// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_SERVICE_H
#define CCLIENT_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef struct {
  const char* const host;
  size_t port;
  int tieout;
} iota_service_t;

typedef struct {
  size_t is_error;
  int duration;
} iota_api_result_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERVICE_H