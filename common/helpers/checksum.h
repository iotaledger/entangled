#ifndef __COMMON_HELPERS_CHECKSUM_H_
#define __COMMON_HELPERS_CHECKSUM_H_

#include <stddef.h>

#include "common/trinary/flex_trit.h"
#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_checksum(const char* input, const size_t inputLength,
                                const size_t checksumLength);

IOTA_EXPORT flex_trit_t* iota_flex_checksum(const flex_trit_t* flex_trits,
                                            const size_t num_trits,
                                            const size_t checksumLength);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_CHECKSUM_H_
