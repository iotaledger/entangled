#ifndef __COMMON_CHECKSUM_DEFAULT_H_
#define __COMMON_CHECKSUM_DEFAULT_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* iota_checksum(const char* input, const size_t inputLength,
                    const size_t checksumLength);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_CHECKSUM_DEFAULT_H_
