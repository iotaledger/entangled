#ifndef __COMMON_HELPERS_DIGEST_H
#define __COMMON_HELPERS_DIGEST_H

#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_digest(char const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_DIGEST_H
