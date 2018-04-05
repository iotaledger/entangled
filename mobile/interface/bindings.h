#ifndef __MOBILE_INTERFACE_BINDINGS_H_
#define __MOBILE_INTERFACE_BINDINGS_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* do_pow(const char* trytes, int mwm);
char* generate_address(const char* seed, const size_t index,
                       const size_t security);
char* generate_signature(const char* seed, const size_t index,
                         const size_t security, const size_t bundleHash);

#ifdef __cplusplus
}
#endif

#endif
