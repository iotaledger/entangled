#ifndef __COMMON_TRINARY_TRIT_LONG_H_
#define __COMMON_TRINARY_TRIT_LONG_H_

#include "common/stdint.h"
#include "common/trinary/trits.h"

long long trits_to_long(trit_t *trit, size_t const i);
size_t min_trits(long long value);
size_t long_to_trits(long long value, trit_t *trits);

size_t nearest_greater_power_of_three(size_t value, size_t *power);
size_t encoded_length(long long value);
int encode_long(long long value, trit_t *out, size_t size);
long long get_encoded_long(trit_t *trits, size_t length, size_t *end);

#endif
