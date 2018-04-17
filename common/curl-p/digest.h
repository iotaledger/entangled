#ifndef __DIGEST_H
#define __DIGEST_H

#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

void curl_digest(const trit_t* const trits, size_t len, trit_t* out, Curl* curl);

#ifdef __cplusplus
}
#endif

#endif //__DIGEST_H
