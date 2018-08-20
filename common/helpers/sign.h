#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_SIGN_DEFAULT_H_
#define __COMMON_SIGN_DEFAULT_H_

#include <stddef.h>
#include "common/trinary/trit_array.h"

IOTA_EXPORT char* iota_sign_address_gen(char const* const seed,
                                        size_t const index,
                                        size_t const security);
IOTA_EXPORT char* iota_sign_signature_gen(char const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          char const* const bundleHash);

flex_trit_t* iota_flex_sign_address_gen(flex_trit_t const* const seed,
                                        size_t const index,
                                        size_t const security);
flex_trit_t* iota_flex_sign_signature_gen(flex_trit_t const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          flex_trit_t const* const bundleHash);

#endif  //__COMMON_SIGN_DEFAULT_H_

#ifdef __cplusplus
}
#endif
