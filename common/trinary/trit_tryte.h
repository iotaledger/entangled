#ifndef __COMMON_TRINARY_TRIT_TRYTE_H_
#define __COMMON_TRINARY_TRIT_TRYTE_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

#define RADIX 3

void trits_to_trytes(trit_t*, tryte_t*, size_t);
void trytes_to_trits(tryte_t*, trit_t*, size_t);

#endif
