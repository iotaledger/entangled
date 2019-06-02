/*
 * Copyright (c) 2019 IOTA Stiftung
 * Copyright (c) 2019 Cybercrypt A/S
 * https://github.com/iotaledger/entangled
 * Based on c-mnd implementation: https://github.com/c-mnd/troika
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __COMMON_FTROIKA_FTROIKA_H__
#define __COMMON_FTROIKA_FTROIKA_H__

#include <stdio.h>

#include "common/crypto/ftroika/general.h"
#include "common/crypto/ftroika/t27.h"
#include "common/trinary/tryte.h"

void ftroika_sub_trytes(t27_t *state);
void ftroika_shift_rows(t27_t *state);
void ftroika_shift_lanes(t27_t *state);
void ftroika_add_column_parity(t27_t *state);
void ftroika_print_round_constants();
void ftroika_add_round_constant(t27_t *state, int round);
void ftroika_permutation(t27_t *state, size_t const num_rounds);
void ftroika_nullify_state(t27_t *state);
void ftroika_nullify_rate(t27_t *state);
void ftroika_nullify_capacity(t27_t *state);
void ftroika_trits_to_rate(t27_t *state, const trit_t *trits, int len);
void ftroika_rate_to_trits(const t27_t *state, trit_t *trits, int len);
int ftroika_trytes_to_state(t27_t *state, const tryte_t *trytes, int len);
int ftroika_compare_states(t27_t *state, t27_t *other);
void ftroika_increase_state(t27_t *state);
void ftroika_absorb(t27_t *state, unsigned int rate, const trit_t *message, unsigned long long message_length,
                    unsigned long long num_rounds);
void ftroika_squeeze(trit_t *hash, unsigned long long hash_length, unsigned int rate, t27_t *state,
                     unsigned long long num_rounds);
void ftroika(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen);
void ftroika_var_rounds(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen,
                        unsigned long long num_rounds);
void ftroika243_repeated(trit_t *out, const trit_t *in, int repeat);

#endif  // __COMMON_FTROIKA_FTROIKA_H__
