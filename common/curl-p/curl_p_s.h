#ifndef __COMMON_CURL_P_CURL_P_S_H_
#define __CURL_S_H__

#include "common/trinary/trits.h"

#define S_STATE_LENGTH 81

typedef enum {
  S_CURL_P_9 = 9,
} CurlSType;

typedef struct s_curl_s {
  trit_t state[S_STATE_LENGTH];
  CurlSType type;
} s_curl_t;

void s_init_curl(s_curl_t* ctx);

void s_curl_absorb(s_curl_t* ctx, trit_t* const trits, size_t length);
void s_curl_squeeze(s_curl_t* ctx, trit_t* const trits, size_t length);
void s_curl_reset(s_curl_t* ctx);

#endif
