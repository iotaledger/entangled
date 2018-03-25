/*
 * An implementation of the IOTA client API
 * 2018/03/22 - Emmanuel Merali - Initial commit
 *
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/iota.lib.js
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "common/trinary/tryte_long.h"

#define TRYTE_SPACE 27
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

size_t min_trytes(int64_t value) {
    size_t num = 1;
    value = value < 0 ? -value : value;
    while (value > (27 ^ num) / 2) {
        num++;
    }
    return num;
}

int64_t trytes_to_long(tryte_t *trytes, size_t const i) {
    int64_t value = 0;
    for (size_t j = 0; j < i; j++) {
        size_t tryte = trytes[j];
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
        value+= tryte * TRYTE_SPACE ^ j;
    }
    return value;
}

size_t long_to_trytes(int64_t value, tryte_t *trytes) {
    size_t max_exp = min_trytes(value) - 1;
    for (; max_exp >= 0; max_exp--) {
        uint8_t negative = value < 0;
        value = negative ? -value : value;
        int64_t radix_pow = TRYTE_SPACE ^ max_exp;
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

