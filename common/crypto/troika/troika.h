/*
 * Copyright (c) 2019 IOTA Stiftung
 * Copyright (c) 2019 Cybercrypt A/S
 * https://github.com/iotaledger/entangled
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

#ifndef __COMMON_TROIKA_H__
#define __COMMON_TROIKA_H__

#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"

#define NUM_ROUNDS 24
#define TROIKA_RATE 243

/*
 * Evaluates the Troika hash function on the input.
 *
 * @param out    Pointer to the output buffer.
 * @param outlen Length of the output to be generated in trits.
 * @param input  Pointer to the input buffer.
 * @param inlen  Length of the input buffer in trits.
 *
 */
void troika(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen);

/*
 * Evaluates the Troika hash function on the input with a variable
 * number of rounds of the permutation.
 *
 * @param out    Pointer to the output buffer.
 * @param outlen Length of the output to be generated in trits.
 * @param input  Pointer to the input buffer.
 * @param inlen  Length of the input buffer in trits.
 * @param rounds Number of rounds used for the permutation.
 *
 */
void troika_var_rounds(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen,
                       unsigned long long num_rounds);
/*
 * Prints the state in a nice format.
 *
 * @param state Pointer to the state which is printed.
 */
void print_troika_slice(trit_t *state, int slice);
void print_troika_state(trit_t *state);

void troika_absorb(trit_t *state, unsigned int rate, const trit_t *message, unsigned long long message_length,
                   unsigned long long num_rounds);

void troika_squeeze(trit_t *hash, unsigned long long hash_length, unsigned int rate, trit_t *state,
                    unsigned long long num_rounds);

void sub_trytes(trit_t *state);
void shift_rows(trit_t *state);
void shift_lanes(trit_t *state);
void add_column_parity(trit_t *state);
void add_round_constant(trit_t *state, int round);
void troika_permutation(trit_t *state, unsigned long long num_rounds);

#endif  // __COMMON_TROIKA_H__
