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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "common/crypto/ftroika/ftroika.h"
#include "common/crypto/ftroika/general.h"
#include "t27.h"

static const t27_t fround_constants[NUM_ROUNDS][COLUMNS] = {{{119734530, 1610953},
                                                             {5749794, 34095441},
                                                             {8585540, 55080601},
                                                             {37884008, 77409799},
                                                             {54010117, 84576},
                                                             {1516630, 113295913},
                                                             {67149892, 28728632},
                                                             {18946819, 46269656},
                                                             {71707578, 53494784}},
                                                            {{16777439, 103161856},
                                                             {106015553, 10769436},
                                                             {21266449, 11549090},
                                                             {25182214, 106707976},
                                                             {3511622, 21651481},
                                                             {99250704, 98573},
                                                             {86049024, 8946816},
                                                             {115430790, 18522649},
                                                             {34802142, 90448384}},
                                                            {{52954114, 4518880},
                                                             {42049594, 69225857},
                                                             {64652, 119014242},
                                                             {2361764, 79725587},
                                                             {11788385, 71306002},
                                                             {104925460, 18936387},
                                                             {126091277, 7368848},
                                                             {50448421, 76157720},
                                                             {8389632, 69472985}},
                                                            {{5267465, 119801412},
                                                             {219376, 96215813},
                                                             {69452824, 31209699},
                                                             {2458688, 26900536},
                                                             {9216196, 23217449},
                                                             {9479304, 84560389},
                                                             {14721540, 118622586},
                                                             {18134123, 33751056},
                                                             {17839280, 8454144}},
                                                            {{50535754, 83100304},
                                                             {77465099, 56709376},
                                                             {3229283, 54835588},
                                                             {111780009, 4473088},
                                                             {78153311, 1384832},
                                                             {2200712, 85617187},
                                                             {3410924, 71341072},
                                                             {75661345, 34434134},
                                                             {56763059, 69011456}},
                                                            {{111543554, 1650793},
                                                             {16908812, 37251073},
                                                             {104910882, 1517085},
                                                             {26041368, 103842404},
                                                             {48022528, 2229055},
                                                             {54104125, 71320960},
                                                             {35722818, 93087928},
                                                             {84559900, 3190850},
                                                             {27582482, 37816716}},
                                                            {{68786250, 54928432},
                                                             {3686682, 63278693},
                                                             {70045, 100557312},
                                                             {38150276, 94408058},
                                                             {46798629, 2394242},
                                                             {1202190, 8988112},
                                                             {34308201, 94384916},
                                                             {17518227, 3145772},
                                                             {973329, 136}},
                                                            {{56633740, 8765490},
                                                             {68419770, 749061},
                                                             {100942913, 23267584},
                                                             {79923980, 51667986},
                                                             {41853745, 25172098},
                                                             {39327896, 75776000},
                                                             {44671808, 68175902},
                                                             {2245138, 13929772},
                                                             {33650945, 79037966}},
                                                            {{270473, 90363412},
                                                             {72887432, 25346582},
                                                             {100829319, 16593224},
                                                             {40087630, 68684337},
                                                             {6369457, 110496512},
                                                             {4784407, 25472000},
                                                             {33891012, 79219770},
                                                             {53838530, 8936492},
                                                             {68643936, 525057}},
                                                            {{102302534, 16841864},
                                                             {50364433, 75530210},
                                                             {84025378, 41014464},
                                                             {25225495, 102827176},
                                                             {4194888, 1050917},
                                                             {84026756, 39440496},
                                                             {2102125, 76284930},
                                                             {219, 101056512},
                                                             {100738441, 5820436}},
                                                            {{10228162, 67365944},
                                                             {5235808, 8393488},
                                                             {51989651, 2228780},
                                                             {16847505, 76433508},
                                                             {67651608, 33591874},
                                                             {69017778, 35784448},
                                                             {33587208, 76568885},
                                                             {117440518, 4257472},
                                                             {96273297, 154690}},
                                                            {{124317824, 1508111},
                                                             {34873472, 98616918},
                                                             {111182400, 1330494},
                                                             {69374511, 54871056},
                                                             {27626369, 38929480},
                                                             {37879972, 26052698},
                                                             {71587392, 44040194},
                                                             {14000288, 2101064},
                                                             {35672064, 97980170}},
                                                            {{81296, 47317509},
                                                             {38469910, 25472072},
                                                             {29738560, 36700214},
                                                             {3267745, 117973262},
                                                             {97993472, 528537},
                                                             {84567940, 13731898},
                                                             {77335148, 21041296},
                                                             {51463726, 6724033},
                                                             {1116193, 23601996}},
                                                            {{80396928, 18153737},
                                                             {117581700, 10059826},
                                                             {21505356, 101124275},
                                                             {23679023, 42993616},
                                                             {103681057, 4268108},
                                                             {72885098, 18914433},
                                                             {97846858, 2627621},
                                                             {8422144, 104538235},
                                                             {83948099, 40176916}},
                                                            {{6928902, 67256433},
                                                             {67184746, 41588096},
                                                             {69355878, 38529},
                                                             {41473220, 67313922},
                                                             {50647688, 4336995},
                                                             {92288643, 248148},
                                                             {12134919, 37884008},
                                                             {35146408, 2601044},
                                                             {9423489, 17835048}},
                                                            {{68231686, 6477024},
                                                             {57284529, 8398852},
                                                             {69316740, 34996770},
                                                             {33696260, 24642995},
                                                             {46333986, 85212492},
                                                             {54665779, 12422144},
                                                             {47791116, 311458},
                                                             {44671584, 72368411},
                                                             {2773762, 29428924}},
                                                            {{70976736, 62972703},
                                                             {123864709, 10004498},
                                                             {4202128, 11157861},
                                                             {4859922, 61129797},
                                                             {43331726, 69782577},
                                                             {83935362, 49559848},
                                                             {18875398, 1355904},
                                                             {34433676, 78808178},
                                                             {106038508, 27888147}},
                                                            {{4627260, 33570944},
                                                             {4538630, 121155721},
                                                             {9245346, 117613917},
                                                             {33571009, 29853970},
                                                             {2392559, 43652096},
                                                             {93343744, 37793194},
                                                             {17309712, 36148998},
                                                             {3276900, 118312456},
                                                             {101315856, 5638796}},
                                                            {{1587272, 132514822},
                                                             {4229205, 77297034},
                                                             {5767570, 84216428},
                                                             {110247047, 21528952},
                                                             {125878920, 7743841},
                                                             {42033186, 73801480},
                                                             {8388866, 2699881},
                                                             {127159080, 2240724},
                                                             {17324188, 112468544}},
                                                            {{34341913, 91345154},
                                                             {105251840, 2623560},
                                                             {4798982, 10634481},
                                                             {39389184, 84174433},
                                                             {88113152, 8667000},
                                                             {34284722, 329},
                                                             {39360568, 67200132},
                                                             {6844996, 58720546},
                                                             {104799233, 29368426}},
                                                            {{51429916, 80362691},
                                                             {88855204, 262411},
                                                             {8655522, 71558228},
                                                             {17838342, 11076784},
                                                             {92751916, 1577424},
                                                             {33559104, 8931338},
                                                             {1055746, 99418513},
                                                             {85018341, 39885072},
                                                             {63800, 120587968}},
                                                            {{5517104, 84070467},
                                                             {118067364, 5522242},
                                                             {39922643, 68435980},
                                                             {73796250, 35144996},
                                                             {2528811, 37838868},
                                                             {37880008, 17144593},
                                                             {21317458, 112754688},
                                                             {113268098, 20677181},
                                                             {2597136, 47730886}},
                                                            {{68438280, 50397942},
                                                             {59853500, 68030786},
                                                             {1475096, 41965991},
                                                             {85852370, 37775145},
                                                             {1071361, 44113962},
                                                             {68040205, 62931234},
                                                             {5847109, 78005290},
                                                             {34465024, 12720668},
                                                             {71860611, 44513824}},
                                                            {{109057155, 3197812},
                                                             {2396909, 16843778},
                                                             {67383952, 31605828},
                                                             {70387369, 37875732},
                                                             {119275955, 28228},
                                                             {34079753, 73679286},
                                                             {50603056, 71422530},
                                                             {10385546, 86017108},
                                                             {227426, 12060561}}};

#define ZERO (_0(a) & _0(b) & _0(c))  // 0
#define A (_1(a) & _0(b) & _0(c))     // 1
#define B (_2(a) & _0(b) & _0(c))     // 2
#define C (_0(a) & _1(b) & _0(c))     // 3
#define D (_1(a) & _1(b) & _0(c))     // 4
#define E (_2(a) & _1(b) & _0(c))     // 5
#define F (_0(a) & _2(b) & _0(c))     // 6
#define G (_1(a) & _2(b) & _0(c))     // 7
#define H (_2(a) & _2(b) & _0(c))     // 8
#define I (_0(a) & _0(b) & _1(c))     // 9
#define J (_1(a) & _0(b) & _1(c))     // 10
#define K (_2(a) & _0(b) & _1(c))     // 11
#define L (_0(a) & _1(b) & _1(c))     // 12
#define M (_1(a) & _1(b) & _1(c))     // 13
#define N (_2(a) & _1(b) & _1(c))     // 14
#define O (_0(a) & _2(b) & _1(c))     // 15
#define P (_1(a) & _2(b) & _1(c))     // 16
#define Q (_2(a) & _2(b) & _1(c))     // 17
#define R (_0(a) & _0(b) & _2(c))     // 18
#define S (_1(a) & _0(b) & _2(c))     // 19
#define T (_2(a) & _0(b) & _2(c))     // 20
#define U (_0(a) & _1(b) & _2(c))     // 21
#define V (_1(a) & _1(b) & _2(c))     // 22
#define W (_2(a) & _1(b) & _2(c))     // 23
#define X (_0(a) & _2(b) & _2(c))     // 24
#define Y (_1(a) & _2(b) & _2(c))     // 25
#define Z (_2(a) & _2(b) & _2(c))     // 26

// static const int shift_rows_param[3] = {0, 1, 2};

static const int shift_lanes_param[27] = {19, 13, 21, 10, 24, 15, 2,  9,  3, 14, 0,  6,  5, 1,
                                          25, 22, 23, 20, 7,  17, 26, 12, 8, 18, 16, 11, 4};

static uint8_t ft_get(const t27_t *state, const int rowcol, const int slice) {
  uint8_t val;
  val = t27_get(&state[rowcol], slice);
  return val;
}
static void ft_set(t27_t *state, const int rowcol, const int slice, const uint8_t val) {
  t27_set(&state[rowcol], slice, val);
}
void fPrintTroikaSlice(t27_t *state, int slice) {
  fprintf(stderr, "#### Slice %i ####\n", slice);
  for (int row = 0; row < ROWS; ++row) {
    for (int column = 0; column < COLUMNS; ++column) {
      fprintf(stderr, "%i ", t27_get(&state[COLUMNS * row + column], slice));
    }
    fprintf(stderr, "\n");
  } /*
   fprintf(stderr, "------------------\n");
   for (int i = 0; i < COLUMNS; i++) {
       fprintf(stderr, "%i ", (state[slice*SLICES + 0*COLUMNS + i] +
   state[slice*SLICES + 1*COLUMNS + i] + state[slice*SLICES + 2*COLUMNS + i]) %
   3);
   }*/
  fprintf(stderr, "\n");
}

void fPrintTroikaState(t27_t *state) {
  // fprintf(stderr, "Troika State:\n");
  for (int slice = 0; slice < SLICES; ++slice) {
    fPrintTroikaSlice(state, slice);
  }
}

void ftroika_sub_tryte(t27_t *a, t27_t *b, t27_t *c) {
  t27_t x[3];
  x[0].p = A | E | F | J | M | P | S | U | Z;
  x[0].n = B | C | G | K | N | Q | T | V | X;
  x[1].p = C | F | M | Q | R | S | T | W | Y;
  x[1].n = ZERO | A | B | D | H | N | P | U | X;
  x[2].p = B | D | E | L | P | Q | S | V | W;
  x[2].n = A | G | H | M | N | O | T | Y | Z;
  *a = t27_clean(x[0]);
  *b = t27_clean(x[1]);
  *c = t27_clean(x[2]);
}

void ftroika_sub_trytes(t27_t *state) {
  for (size_t col = 0; col < COLUMNS; ++col) {
    ftroika_sub_tryte(&state[ROWS * col + 2], &state[ROWS * col + 1], &state[ROWS * col]);
  }
}
void ftroika_shift_rows(t27_t *state) {
  t27_t new_state[SLICESIZE];
  const int shifts[27] = {0,  1, 2,  3,  4,  5,  6,  7,  8,  12, 13, 14, 15, 16,
                          17, 9, 10, 11, 24, 25, 26, 18, 19, 20, 21, 22, 23};
  for (size_t i = 9; i < SLICESIZE; ++i) {
    new_state[shifts[i]] = state[i];
  }
  memcpy(state + 9, new_state + 9, (SLICESIZE - 9) * sizeof(t27_t));
}

void ftroika_shift_lanes(t27_t *state) {
  int rowcol;
  t27_t new_state[SLICESIZE];
  for (rowcol = 0; rowcol < SLICESIZE; ++rowcol) {
    int shift = shift_lanes_param[rowcol];
    // T27 orig = state[rowcol];
    new_state[rowcol] = t27_roll(state[rowcol], shift);
    // T27 test = t27_roll(new_state[COLUMNS*row + col], 27 - shift);
  }
  memcpy(state, new_state, SLICESIZE * sizeof(t27_t));
}

void ftroika_add_column_parity(t27_t *state) {
  int row, col, idx;
  t27_t col_sum, sum_to_add;
  t27_t parity[COLUMNS];
  // First compute parity for each column
  for (col = 0; col < COLUMNS; ++col) {
    col_sum = t27_init(0, 0);
    for (row = 0; row < ROWS; ++row) {
      col_sum = t27_sum(&col_sum, &state[COLUMNS * row + col]);
    }
    parity[col] = col_sum;
  }
  // Add parity
  for (row = 0; row < ROWS; ++row) {
    for (col = 0; col < COLUMNS; ++col) {
      idx = COLUMNS * row + col;
      t27_t t1 = parity[(col == 0) ? (COLUMNS - 1) : (col - 1)],
            t2 = t27_roll(parity[(col == COLUMNS - 1) ? (0) : (col + 1)], SLICES - 1);
      sum_to_add = t27_sum(&t1, &t2);
      state[idx] = t27_sum(&state[idx], &sum_to_add);
    }
  }
}

void ftroika_add_round_constant(t27_t *state, int round) {
  int col;
  for (col = 0; col < COLUMNS; ++col) {
    state[col] = t27_sum(&state[col], &fround_constants[round][col]);
  }
}

void ftroika_permutation(t27_t *state, size_t const num_rounds) {
  assert(num_rounds <= NUM_ROUNDS);
  // PrintTroikaState(state);
  for (size_t round = 0; round < num_rounds; round++) {
    ftroika_sub_trytes(state);
    ftroika_shift_rows(state);
    ftroika_shift_lanes(state);
    ftroika_add_column_parity(state);
    ftroika_add_round_constant(state, round);
  }
  // PrintTroikaState(state);
}
void ftroika_nullify_state(t27_t *state) {
  const uint32_t mask = 0;
  for (int i = 0; i < SLICESIZE; i++) {
    state[i].p &= mask;
    state[i].n &= mask;
  }
}
void ftroika_nullify_rate(t27_t *state) {
  const uint32_t mask = 0x07fffe00;
  for (int i = 0; i < SLICESIZE; i++) {
    state[i].p &= mask;
    state[i].n &= mask;
  }
}
void ftroika_nullify_capacity(t27_t *state) {
  const uint32_t mask = 0x000001ff;
  for (int i = 0; i < SLICESIZE; i++) {
    state[i].p &= mask;
    state[i].n &= mask;
  }
}
int ftroika_compare_states(t27_t *state, t27_t *other) {
  for (int i = 0; i < SLICES; i++) {
    if (!t27_eq(state[i], other[i])) {
      return 0;
    }
  }
  return 1;
}
void ftroika_trits_to_rate(t27_t *state, const trit_t *trits, int len) {
  int rowcol = 0, slice = 0;
  for (int i = 0; i < len; i++) {
    if (rowcol == SLICESIZE) {
      rowcol = 0;
      slice++;
    }
    ft_set(state, rowcol, slice, trits[i]);
    rowcol++;
  }
}
void ftroika_rate_to_trits(const t27_t *state, trit_t *trits, int len) {
  int rowcol = 0, slice = 0;
  for (int i = 0; i < len; i++) {
    if (rowcol == SLICESIZE) {
      rowcol = 0;
      slice++;
    }
    trits[i] = ft_get(state, rowcol, slice);
    rowcol++;
  }
}
// this is still buggy
int ftroika_trytes_to_state(t27_t *state, const tryte_t *trytes,
                            int len) {  // len in trits
  t27_t t[3];
  t[0] = t27_init(0x0003fe00, 0x07fc0000);
  t[1] = t27_init(0x00e07038, 0x070381c0);
  t[2] = t27_init(0x02492492, 0x04924924);
  uint32_t mask = 1;
  char tryte = trytes[0];
  for (int i = 0; i < len; i++) {
    if (i % 3 == 0) tryte = trytes[i / 3];
    if (tryte != '0') {
      state[i % 27].p |= (1 << (tryte - 64) & t[i % 3].p) ? mask : 0;
      state[i % 27].n |= (1 << (tryte - 64) & t[i % 3].n) ? mask : 0;
    }
    if (i % 27 == 26) mask = mask << 1;
  }
  return 1;
}
void ftroika_increase_state(t27_t *state) {
  int rowcol = 0, slice = 0;
  uint8_t val;
  while (1) {
    val = ft_get(state, rowcol, slice);
    if (val < 2) {
      ft_set(state, rowcol, slice, val + 1);
      break;
    } else {
      ft_set(state, rowcol, slice, 0);
      rowcol++;
      if (rowcol == SLICESIZE) {
        rowcol = 0;
        slice++;
      }
    }
  }
}
void ftroika_absorb(t27_t *state, unsigned int rate, const trit_t *message, unsigned long long message_length,
                    unsigned long long num_rounds) {
  unsigned long long trit_idx;
  int rowcol, slice;
  while (message_length >= rate) {
    // Copy message block over the state
    rowcol = 0;
    slice = 0;
    for (trit_idx = 0; trit_idx < rate; ++trit_idx) {
      if (rowcol == SLICESIZE) {
        rowcol = 0;
        slice++;
      }
      ft_set(state, rowcol, slice, message[trit_idx]);
      rowcol++;
    }
    ftroika_permutation(state, num_rounds);
    message_length -= rate;
    message += rate;
  }
  // Pad last block
  uint8_t last_block[rate];
  memset(last_block, 0, rate);
  // Copy over last incomplete message block
  for (trit_idx = 0; trit_idx < message_length; ++trit_idx) {
    last_block[trit_idx] = message[trit_idx];
  }
  // Apply padding
  last_block[trit_idx] = PADDING;

  // Insert last message block
  rowcol = 0;
  slice = 0;
  for (trit_idx = 0; trit_idx < rate; ++trit_idx) {
    if (rowcol == SLICESIZE) {
      rowcol = 0;
      slice++;
    }
    ft_set(state, rowcol, slice, last_block[trit_idx]);
    rowcol++;
  }
}

void ftroika_squeeze(trit_t *hash, unsigned long long hash_length, unsigned int rate, t27_t *state,
                     unsigned long long num_rounds) {
  unsigned long long trit_idx;
  int rowcol, slice;
  while (hash_length >= rate) {
    ftroika_permutation(state, num_rounds);
    // Extract rate output
    rowcol = 0;
    slice = 0;
    for (trit_idx = 0; trit_idx < rate; ++trit_idx) {
      if (rowcol == SLICESIZE) {
        rowcol = 0;
        slice++;
      }
      hash[trit_idx] = ft_get(state, rowcol, slice);
      rowcol++;
    }
    hash += rate;
    hash_length -= rate;
  }
  // Check if there is a last incomplete block
  if (hash_length > 0) {
    ftroika_permutation(state, num_rounds);
    rowcol = 0;
    slice = 0;
    for (trit_idx = 0; trit_idx < hash_length; ++trit_idx) {
      if (rowcol == SLICESIZE) {
        rowcol = 0;
        slice++;
      }
      hash[trit_idx] = ft_get(state, rowcol, slice);
      rowcol++;
    }
  }
}

void ftroika(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen) {
  ftroika_var_rounds(out, outlen, in, inlen, NUM_ROUNDS);
}

void ftroika_var_rounds(trit_t *out, unsigned long long outlen, const trit_t *in, unsigned long long inlen,
                        unsigned long long num_rounds) {
  t27_t state[SLICESIZE];
  memset(state, 0, SLICESIZE * sizeof(t27_t));
  ftroika_absorb(state, TROIKA_RATE, in, inlen, num_rounds);
  ftroika_squeeze(out, outlen, TROIKA_RATE, state, num_rounds);
}

// one typical usage of Troika will be with 243 trits input and output
// and rehashing of the last output
// this function does it
void ftroika243_repeated(trit_t *out, const trit_t *in, int repeat) {
  t27_t state[SLICESIZE];
  ftroika_nullify_state(state);
  ftroika_trits_to_rate(state, in, 243);
  for (int i = 0; i < repeat; i++) {
    ftroika_nullify_capacity(state);
    ftroika_permutation(state, 24);
    ftroika_nullify_rate(state);
    ft_set(state, 0, 0, PADDING);
    ftroika_permutation(state, 24);
  }
  ftroika_rate_to_trits(state, out, 243);
}
