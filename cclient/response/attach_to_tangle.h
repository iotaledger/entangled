// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H
#define CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  flex_trit_p trytes;
  size_t size;
} attach_to_tangle_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H
