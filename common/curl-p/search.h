#ifndef __COMMON_CURL_P_SEARCH_H_
#define __COMMON_CURL_P_SEARCH_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/ptrit.h"
#include "common/curl-p/trit.h"

PearlDiverStatus pd_search(Curl *const ctx, unsigned short const offset,
                           unsigned short const end,
                           short (*test)(PCurl *const, unsigned short const),
                           unsigned short const param);

#endif
