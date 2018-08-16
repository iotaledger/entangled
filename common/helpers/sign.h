#ifndef __COMMON_HELPERS_SIGN_H_
#define __COMMON_HELPERS_SIGN_H_

#include <stddef.h>

#include "utils/export.h"

#ifdef __cplusplus
extern "C" {
#endif

IOTA_EXPORT char* iota_sign_address_gen(char const* const seed,
                                        size_t const index,
                                        size_t const security);
IOTA_EXPORT char* iota_sign_signature_gen(char const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          char const* const bundleHash);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_HELPERS_SIGN_H_
