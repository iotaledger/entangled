#ifndef __CURL_POW_H
#define __CURL_POW_H


#include <stddef.h>
#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif
char* do_pow(Curl* curl, const char* trits, size_t trits_len, uint8_t mwm);

#ifdef __cplusplus
}
#endif


#endif //__CURL_POW_H
