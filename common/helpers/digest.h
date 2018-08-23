#ifndef __COMMON_HELPERS_DIGEST_H
#define __COMMON_HELPERS_DIGEST_H

#include "common/trinary/trit_array.h"
#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_digest(char const* const trytes);

IOTA_EXPORT flex_trit_t* iota_flex_digest(flex_trit_t const* const flex_trits,
                                          size_t num_trits);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_DIGEST_H
