#ifndef __COMMON_SIGN_DEFAULT_H_
#define __COMMON_SIGN_DEFAULT_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* iota_sign_address_gen(char const* const seed, size_t const index,
                            size_t const security);
char* iota_sign_signature_gen(char const* const seed, size_t const index,
                              size_t const security,
                              char const* const bundleHash);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_SIGN_DEFAULT_H_
