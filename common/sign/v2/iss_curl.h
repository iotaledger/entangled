#ifndef __ISS_CURL_H_
#define __ISS_CURL_H_

#include "common/curl-p/trit.h"

#define HASH_PREFIX curl
#define HASH_STATE Curl

#include "iss.h.inc"

#undef HASH_PREFIX
#undef HASH_STATE

#endif /* __ISS_CURL_H_ */
