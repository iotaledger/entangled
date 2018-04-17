#ifndef __COMMON_SIGN_DEFAULT_H_
#define __COMMON_SIGN_DEFAULT_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* iota_sign_address_gen(const char* seed, const size_t index,
                       const size_t security);
char* iota_sign_signature_gen(const char* seed, const size_t index,
                         const size_t security, const char* bundleHash);

#ifdef __cplusplus
}
#endif

#endif //__COMMON_SIGN_DEFAULT_H_
