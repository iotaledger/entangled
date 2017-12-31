#ifndef __COMMON_CURL_P_PTRIT_H_
#define __COMMON_CURL_P_PTRIT_H_

#include "common/curl-p/const.h"
#include "common/trinary/ptrit_incr.h"

typedef struct {
  ptrit_t state[STATE_LENGTH];
  CurlType type;
} PCurl;

void init_ptrit_curl(PCurl* ctx);

void ptrit_transform(PCurl*);
void ptrit_curl_absorb(PCurl*, ptrit_t* const, size_t);
void ptrit_curl_squeeze(PCurl*, ptrit_t* const, size_t);
void ptrit_curl_reset(PCurl*);

#endif
