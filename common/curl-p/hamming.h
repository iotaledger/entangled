#ifndef __COMMON_CURL_P_HAMMING_H_
#define __COMMON_CURL_P_HAMMING_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/trit.h"

PearlDiverStatus hamming(Curl *const, unsigned short const offset,
                         unsigned short const end,
                         unsigned short const security);

#endif
