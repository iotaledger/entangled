#ifndef __CURL_POW_H
#define __CURL_POW_H

#include <stddef.h>
#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

trit_t* do_pow(Curl* const curl, trit_t const* const trits_in,
               size_t const trits_len, uint8_t const mwm);

#ifdef __cplusplus
}
#endif

#endif  //__CURL_POW_H
