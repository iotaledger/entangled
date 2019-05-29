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

#define TRITMAX (729)

#include <stdbool.h>

#include <unity/unity.h>

#include "common/crypto/ftroika/ftroika.h"
#include "common/crypto/troika/troika.h"

bool states_eq(trit_t *state, t27_t *fstate) {
  const int slices = 27, colums = 9, rows = 3, slicesize = rows * colums;
  for (int slice = 0; slice < slices; ++slice) {
    for (int row = 0; row < rows; ++row) {
      for (int colum = 0; colum < colums; ++colum) {
        trit_t t = state[slicesize * slice + colums * row + colum];
        trit_t ft = t27_get(&fstate[colums * row + colum], slice);
        if (t != ft) {
          return false;
        }
      }
    }
  }
  return true;
}

void test_absorb() {
  int times = 729, rounds = 24, i;
  trit_t state[SLICES * SLICESIZE], in[729];
  memset(state, 0, SLICES * SLICESIZE * sizeof(trit_t));
  memset(in, 0, 729 * sizeof(trit_t));
  t27_t fstate[SLICES];
  trit_t fin[729];
  memset(fstate, 0, SLICESIZE * sizeof(t27_t));
  memset(fin, 0, 729 * sizeof(trit_t));
  for (i = 1; i < times; i++) {
    troika_absorb(state, 243, in, i, rounds);
    ftroika_absorb(fstate, 243, fin, i, rounds);
    TEST_ASSERT(states_eq(state, fstate));
    memcpy(in, state, 729);
    memcpy(fin, in, 729);
  }
}
void test_permutations() {
  int times = 1000, rounds = 24, i;
  trit_t state[SLICES * SLICESIZE];
  memset(state, 0, SLICES * SLICESIZE * sizeof(trit_t));
  t27_t fstate[SLICES];
  memset(fstate, 0, SLICESIZE * sizeof(t27_t));
  for (i = 0; i < times; i++) {
    troika_permutation(state, rounds);
    ftroika_permutation(fstate, rounds);
    TEST_ASSERT(states_eq(state, fstate));
  }
}

void test_repeated() {
  size_t times = 1000;
  trit_t in[243], out[243];
  memset(in, 0, 243 * sizeof(trit_t));
  memset(out, 0, 243 * sizeof(trit_t));
  trit_t fin[243], fout[243];
  memset(fin, 0, 243 * sizeof(trit_t));
  memset(fout, 0, 243 * sizeof(trit_t));
  for (size_t i = 0; i < times; i++) {
    troika(out, 243, in, 243);
    memcpy(in, out, 243 * sizeof(trit_t));
  }
  ftroika243_repeated(fout, fin, times);
  TEST_ASSERT_EQUAL_INT(memcmp(fout, out, 243 * sizeof(trit_t)), 0);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_absorb);
  RUN_TEST(test_permutations);
  RUN_TEST(test_repeated);

  return UNITY_END();
}
