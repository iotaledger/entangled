#ifndef __COMMON_CURL_P_BCT_H
#define __COMMON_CURL_P_BCT_H

#include "common/curl-p/const.h"
#include "common/trinary/bct.h"

#define S_STATE_LENGTH 183

typedef struct {
  bct_t state[S_STATE_LENGTH];
  CurlType type;
} BCurl;

void init_s_curl(BCurl* ctx);

void s_transform(BCurl*);
void s_curl_absorb(BCurl*, bct_t* const, size_t, size_t);
void s_curl_squeeze(BCurl*, bct_t* const, size_t, size_t);
void s_curl_reset(BCurl*);

#endif /* __COMMON_CURL_P_BCT_H */
