#ifndef __COMMON_TRINARY_TRIT_LONG_H__
#define __COMMON_TRINARY_TRIT_LONG_H__

#include "common/trinary/trits.h"

int64_t trits_to_long(trit_t const *const trit, size_t const i);
size_t min_trits(int64_t value);
size_t long_to_trits(int64_t value, trit_t *trits);

size_t nearest_greater_multiple_of_three(size_t value);

size_t encoded_length(int64_t const value);
int encode_long(int64_t const value, trit_t *const trits, size_t const size);
int64_t decode_long(trit_t const *const trits, size_t const length,
                    size_t *const end);

#endif
