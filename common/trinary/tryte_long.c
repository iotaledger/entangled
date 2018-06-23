/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled/
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/tryte_long.h"
#include <math.h>

#define TRYTE_SPACE 27
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

size_t min_trytes(int64_t value) {
    // Need minimum 1 tryte to represent any number
    size_t num = 1;
    value = value < 0 ? -value : value;
    // As long as value is > than (27^2)/2, need one more tryte
    while (value > pow(27, num) / 2) {
        num++;
    }
    return num;
}

int64_t trytes_to_long(const tryte_t *trytes, size_t const i) {
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

// Return the number of trytes needed to encode the value
size_t long_to_trytes(int64_t value, tryte_t *trytes) {
    // Calculate numer of iterations (number of trytes)
    size_t num_trytes = min_trytes(value);
    for (int i = num_trytes - 1; i >= 0; i--) {
        // Check if the current value to convert is negative
        uint8_t negative = value < 0;
        // Get its absolute value
        value = negative ? -value : value;
        // Precalculate the current power order (27^i)
        int64_t radix_pow = pow(TRYTE_SPACE, i);
        // Find out the highest order tryte for this value
        int64_t tryte = (value + radix_pow / 2) / radix_pow;
        // Store the remainder of value minus the value of the highest order tryte
        value-= radix_pow * tryte;
        // If the original value was negative, the tryte value should also be negative
        if (negative) {
            tryte = tryte ? 27 - tryte : tryte;
            value = -value;
        }
        trytes[i] = TRYTE_STRING[tryte];
    }
    return num_trytes;
}

