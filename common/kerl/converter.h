#ifndef __COMMON_KERL_CONVERTER_H_
#define __COMMON_KERL_CONVERTER_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

void trits_to_bytes(trit_t const *const trits, uint8_t *const bytes);

// This method consumes the input bytes.
void bytes_to_trits(uint8_t *const bytes, trit_t *const trits);

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_KERL_CONVERTER_H_ */
