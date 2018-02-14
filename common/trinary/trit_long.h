#ifndef __COMMON_TRINARY_TRIT_LONG_H_
#define __COMMON_TRINARY_TRIT_LONG_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"

int64_t trits_to_long(trit_t *trit, size_t const i);
size_t min_trits(int64_t value);
size_t long_to_trits(int64_t value, trit_t *trits);

size_t nearest_greater_power_of_three(size_t value, size_t *power);
size_t encoded_length(int64_t value);
int encode_long(int64_t value, trit_t *out, size_t size);
int64_t get_encoded_long(trit_t *trits, size_t length, size_t *end);

#endif
