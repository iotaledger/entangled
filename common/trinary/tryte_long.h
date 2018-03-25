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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRYTE_LONG_H_
#define __COMMON_TRINARY_TRYTE_LONG_H_

#include "common/trinary/tryte.h"

size_t min_trytes(int64_t value);
int64_t trytes_to_long(tryte_t *trytes, size_t const i);
size_t long_to_trytes(int64_t value, tryte_t *trytes);

#endif // __COMMON_TRINARY_TRYTE_LONG_H_
#ifdef __cplusplus
}
#endif

