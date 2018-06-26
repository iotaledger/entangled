#ifndef __COMMON_CURL_P_HASHCASH_H_
#define __COMMON_CURL_P_HASHCASH_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/trit.h"

typedef enum {
  TAIL,
  BODY,
  HEAD,
} SearchType;

PearlDiverStatus hashcash(Curl *const ctx, SearchType const type,
                          unsigned short const offset, unsigned short const end,
                          unsigned short const min_weight);
#endif
