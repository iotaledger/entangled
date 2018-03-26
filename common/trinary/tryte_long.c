/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/tryte_long.h"
#include <math.h>

#define TRYTE_SPACE 27
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

size_t min_trytes(int64_t value) {
    size_t num = 1;
    value = value < 0 ? -value : value;
    while (value > pow(27, num) / 2) {
        num++;
    }
    return num;
}

int64_t trytes_to_long(tryte_t *trytes, size_t const i) {
    int64_t value = 0;
    for (size_t j = 0; j < i; j++) {
        tryte_t tryte = trytes[j];
        // Trivial case, tryte value is 0
        if (tryte == '9') {
            continue;
        }
        // Get the tryte index - it's also the tryte value
        tryte = tryte - '@';
        // For indexes > 13 the value is negative
        if (tryte > 13) {
            tryte = tryte - 27;
        }
        // Accumulate the tryte values
        value+= tryte * pow(TRYTE_SPACE, j);
    }
    return value;
}

size_t long_to_trytes(int64_t value, tryte_t *trytes) {
    size_t max_exp = min_trytes(value) - 1;
    for (; max_exp < -1; max_exp--) {
        uint8_t negative = value < 0;
        value = negative ? -value : value;
        int64_t radix_pow = pow(TRYTE_SPACE, max_exp);
        int64_t tryte = (value + radix_pow / 2) / radix_pow;
        int64_t tryte_index = tryte;
        value-= radix_pow * tryte;
        if (negative) {
            tryte_index = 27 - tryte_index;
            value = -value;
        }
        trytes[max_exp] = TRYTE_STRING[tryte_index];
    }
    return max_exp;
}

