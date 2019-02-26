/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_TROIKA_H
#define COMMON_TROIKA_H

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
void Troika(trit_t *out, unsigned long long outlen, const trit_t *in,
            unsigned long long inlen);

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
void TroikaVarRounds(trit_t *out, unsigned long long outlen, const trit_t *in,
                     unsigned long long inlen, unsigned long long num_rounds);
/*
 * Prints the state in a nice format.
 *
 * @param state Pointer to the state which is printed.
 */
void PrintTroikaSlice(trit_t *state, int slice);
void PrintTroikaState(trit_t *state);

void TroikaAbsorb(trit_t *state, unsigned int rate, const trit_t *message,
                  unsigned long long message_length,
                  unsigned long long num_rounds);

void TroikaSqueeze(trit_t *hash, unsigned long long hash_length,
                   unsigned int rate, trit_t *state,
                   unsigned long long num_rounds);

void Subtryte_ts(trit_t *state);
void ShiftRows(trit_t *state);
void ShiftLanes(trit_t *state);
void AddColumnParity(trit_t *state);
void AddRoundConstant(trit_t *state, int round);
void TroikaPermutation(trit_t *state, unsigned long long num_rounds);

#endif  // COMMON_TROIKA_H
