#ifndef ENTANGLED_POW_H
#define ENTANGLED_POW_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

char* iota_pow(char const* const trytes, uint8_t const mwm);

#ifdef __cplusplus
}
#endif

#endif  // ENTANGLED_POW_H
