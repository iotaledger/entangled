#ifndef _COMMON_HELPERS_POW_H
#define _COMMON_HELPERS_POW_H

#include <stdint.h>

#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_pow(char const* const trytes_in, uint8_t const mwm);

#ifdef __cplusplus
}
#endif

#endif  // _COMMON_HELPERS_POW_H
