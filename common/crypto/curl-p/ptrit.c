/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <string.h>

#include "common/crypto/curl-p/ptrit.h"
#include "utils/forced_inline.h"

static FORCED_INLINE void pcurl_s2(ptrit_t const *a, ptrit_t const *b, ptrit_t *c) {
#if defined(PCURL_S2_CIRCUIT4)
#if defined(PTRIT_CVT_ORN)
  // (Xor AH (Orn BL AL),Xor AL (Orn BH (Xor AH (Orn BL AL))))
  c->low = XORORN(a->high, b->low, a->low);
  c->high = XORORN(a->low, b->high, c->low);
#elif defined(PTRIT_CVT_ANDN)
  // (Xor AH (Andn BL AL),Xor AL (And BH (Xor AH (Andn BL AL))))
  c->low = XORANDN(a->high, b->low, a->low);
  c->high = XORAND(a->low, b->high, c->low);
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT

#elif defined(PCURL_S2_CIRCUIT5)
#if defined(PTRIT_CVT_ANDN)
  ptrit_s d = AND(a->low, XOR(b->low, a->high));
  c->low = NOT(d);
  c->high = OR(XOR(a->low, b->high), d);
#else
#error Invalid PTRIT_CVT: with PCURL_S2_CIRCUIT5 only PTRIT_CVT_ANDN can be used.
#endif  // PTRIT_CVT
#else
#error Invalid PCURL_S2_CIRCUIT.
#endif  // PCURL_S2_CIRCUIT
}

#if defined(PCURL_S2_ARGS_LUT)
#define CURL_SBOX_INDEX_TABLE                                                                                          \
  0, 364, 728, 363, 727, 362, 726, 361, 725, 360, 724, 359, 723, 358, 722, 357, 721, 356, 720, 355, 719, 354, 718,     \
      353, 717, 352, 716, 351, 715, 350, 714, 349, 713, 348, 712, 347, 711, 346, 710, 345, 709, 344, 708, 343, 707,    \
      342, 706, 341, 705, 340, 704, 339, 703, 338, 702, 337, 701, 336, 700, 335, 699, 334, 698, 333, 697, 332, 696,    \
      331, 695, 330, 694, 329, 693, 328, 692, 327, 691, 326, 690, 325, 689, 324, 688, 323, 687, 322, 686, 321, 685,    \
      320, 684, 319, 683, 318, 682, 317, 681, 316, 680, 315, 679, 314, 678, 313, 677, 312, 676, 311, 675, 310, 674,    \
      309, 673, 308, 672, 307, 671, 306, 670, 305, 669, 304, 668, 303, 667, 302, 666, 301, 665, 300, 664, 299, 663,    \
      298, 662, 297, 661, 296, 660, 295, 659, 294, 658, 293, 657, 292, 656, 291, 655, 290, 654, 289, 653, 288, 652,    \
      287, 651, 286, 650, 285, 649, 284, 648, 283, 647, 282, 646, 281, 645, 280, 644, 279, 643, 278, 642, 277, 641,    \
      276, 640, 275, 639, 274, 638, 273, 637, 272, 636, 271, 635, 270, 634, 269, 633, 268, 632, 267, 631, 266, 630,    \
      265, 629, 264, 628, 263, 627, 262, 626, 261, 625, 260, 624, 259, 623, 258, 622, 257, 621, 256, 620, 255, 619,    \
      254, 618, 253, 617, 252, 616, 251, 615, 250, 614, 249, 613, 248, 612, 247, 611, 246, 610, 245, 609, 244, 608,    \
      243, 607, 242, 606, 241, 605, 240, 604, 239, 603, 238, 602, 237, 601, 236, 600, 235, 599, 234, 598, 233, 597,    \
      232, 596, 231, 595, 230, 594, 229, 593, 228, 592, 227, 591, 226, 590, 225, 589, 224, 588, 223, 587, 222, 586,    \
      221, 585, 220, 584, 219, 583, 218, 582, 217, 581, 216, 580, 215, 579, 214, 578, 213, 577, 212, 576, 211, 575,    \
      210, 574, 209, 573, 208, 572, 207, 571, 206, 570, 205, 569, 204, 568, 203, 567, 202, 566, 201, 565, 200, 564,    \
      199, 563, 198, 562, 197, 561, 196, 560, 195, 559, 194, 558, 193, 557, 192, 556, 191, 555, 190, 554, 189, 553,    \
      188, 552, 187, 551, 186, 550, 185, 549, 184, 548, 183, 547, 182, 546, 181, 545, 180, 544, 179, 543, 178, 542,    \
      177, 541, 176, 540, 175, 539, 174, 538, 173, 537, 172, 536, 171, 535, 170, 534, 169, 533, 168, 532, 167, 531,    \
      166, 530, 165, 529, 164, 528, 163, 527, 162, 526, 161, 525, 160, 524, 159, 523, 158, 522, 157, 521, 156, 520,    \
      155, 519, 154, 518, 153, 517, 152, 516, 151, 515, 150, 514, 149, 513, 148, 512, 147, 511, 146, 510, 145, 509,    \
      144, 508, 143, 507, 142, 506, 141, 505, 140, 504, 139, 503, 138, 502, 137, 501, 136, 500, 135, 499, 134, 498,    \
      133, 497, 132, 496, 131, 495, 130, 494, 129, 493, 128, 492, 127, 491, 126, 490, 125, 489, 124, 488, 123, 487,    \
      122, 486, 121, 485, 120, 484, 119, 483, 118, 482, 117, 481, 116, 480, 115, 479, 114, 478, 113, 477, 112, 476,    \
      111, 475, 110, 474, 109, 473, 108, 472, 107, 471, 106, 470, 105, 469, 104, 468, 103, 467, 102, 466, 101, 465,    \
      100, 464, 99, 463, 98, 462, 97, 461, 96, 460, 95, 459, 94, 458, 93, 457, 92, 456, 91, 455, 90, 454, 89, 453, 88, \
      452, 87, 451, 86, 450, 85, 449, 84, 448, 83, 447, 82, 446, 81, 445, 80, 444, 79, 443, 78, 442, 77, 441, 76, 440, \
      75, 439, 74, 438, 73, 437, 72, 436, 71, 435, 70, 434, 69, 433, 68, 432, 67, 431, 66, 430, 65, 429, 64, 428, 63,  \
      427, 62, 426, 61, 425, 60, 424, 59, 423, 58, 422, 57, 421, 56, 420, 55, 419, 54, 418, 53, 417, 52, 416, 51, 415, \
      50, 414, 49, 413, 48, 412, 47, 411, 46, 410, 45, 409, 44, 408, 43, 407, 42, 406, 41, 405, 40, 404, 39, 403, 38,  \
      402, 37, 401, 36, 400, 35, 399, 34, 398, 33, 397, 32, 396, 31, 395, 30, 394, 29, 393, 28, 392, 27, 391, 26, 390, \
      25, 389, 24, 388, 23, 387, 22, 386, 21, 385, 20, 384, 19, 383, 18, 382, 17, 381, 16, 380, 15, 379, 14, 378, 13,  \
      377, 12, 376, 11, 375, 10, 374, 9, 373, 8, 372, 7, 371, 6, 370, 5, 369, 4, 368, 3, 367, 2, 366, 1, 365, 0

static size_t const curl_index[CURL_STATE_SIZE + 1] = {CURL_SBOX_INDEX_TABLE};
// 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
#endif  // PCURL_S2_ARGS_LUT

#if defined(PCURL_STATE_DOUBLE)
static FORCED_INLINE void pcurl_sbox(ptrit_t *c, ptrit_t const *s) {
#if defined(PCURL_S2_ARGS_LUT)
  size_t i;

#if defined(PCURL_SBOX_UNWIND_2)
  pcurl_s2(s + curl_index[0], s + curl_index[1], c);
  for (i = 1; i < CURL_STATE_SIZE;) {
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  pcurl_s2(s + curl_index[0], s + curl_index[1], c);
  for (i = 1; i < CURL_STATE_SIZE;) {
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
  }
#elif defined(PCURL_SBOX_UNWIND_8)
  pcurl_s2(s + curl_index[0], s + curl_index[1], c);
  for (i = 1; i < CURL_STATE_SIZE;) {
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
    ++i;
  }
#else
  for (i = 0; i < CURL_STATE_SIZE; ++i) {
    pcurl_s2(s + curl_index[i], s + curl_index[i + 1], c + i);
  }
#endif  // PCURL_SBOX_UNWIND

#elif defined(PCURL_S2_ARGS_PTR)
  size_t i;

  // 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
  ptrit_t const *x = s + 0, *y = s + 364;
  pcurl_s2(x, y, c++);
  x = s + 728;

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 2; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#elif defined(PCURL_SBOX_UNWIND_8)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif  // PCURL_SBOX_UNWIND

#else
#error Invalid PCURL_S2_ARGS.
#endif  // PCURL_S2_ARGS
}

#elif defined(PCURL_STATE_SHORT)
#if defined(PCURL_S2_ARGS_PTR)
// 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
// a : [  0..  364]-- => --[0,728..365]++ ->   xxxxxxxxxxxx   -> ++[0    ..364]
// b : [365..728,0]-- ->   xxxxxxxxxxxx   => --[364  ..  0]++ => ++[365..728,0]
// c : xxxxxxxxxxxx   => ++[0    ..364]-- => --[0,728..365]++ ->   xxxxxxxxxxxx
// c : xxxxxxxxxxxx   => --[364  ..  0]++ => ++[365..728,0]-- ->   xxxxxxxxxxxx
static FORCED_INLINE void pcurl_sbox_0(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  a = a + 364;
  b = b + 364;
  c = c + 0;
  ptrit_t *aa = a;
  ptrit_t *c0 = c;

  pcurl_s2(b--, a, c++);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *aa = *c0;
}
static FORCED_INLINE void pcurl_sbox_1(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  c = c + 364;
  a = a + 0;
  b = b + 364;
  ptrit_t *cc = c;
  ptrit_t *b0 = b;

  pcurl_s2(a++, c, b--);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *cc = *b0;
}
static FORCED_INLINE void pcurl_sbox_2(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  b = b + 0;
  c = c + 0;
  a = a + 0;
  ptrit_t *bb = b;
  ptrit_t *a0 = a;

  pcurl_s2(c++, b, a++);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *bb = *a0;
}
#else
#error Invalid PCURL_S2_ARGS.
#endif  // PCURL_S2_ARGS
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE

void pcurl_init(pcurl_t *ctx, size_t round_count) {
  pcurl_reset(ctx);
  ctx->round_count = round_count;
}
void pcurl_absorb(pcurl_t *ctx, ptrit_t const *ptrits, size_t length) {
  size_t n = length / CURL_RATE;
  for (; n--;) {
    memcpy(ctx->state, ptrits, CURL_RATE * sizeof(ptrit_t));
    ptrits += CURL_RATE;
    // no padding!
    pcurl_transform(ctx);
  }
  length %= CURL_RATE;

  if (0 < length) {
    memcpy(ctx->state, ptrits, length * sizeof(ptrit_t));
    ptrits += length;
    // no padding!
    pcurl_transform(ctx);
    length = 0;
  }
}
void pcurl_squeeze(pcurl_t *ctx, ptrit_t *ptrits, size_t length) {
  size_t n = length / CURL_RATE;
  for (; n--;) {
    memcpy(ptrits, ctx->state, CURL_RATE * sizeof(ptrit_t));
    ptrits += CURL_RATE;
    // no padding!
    pcurl_transform(ctx);
  }
  length %= CURL_RATE;

  if (0 < length) {
    memcpy(ptrits, ctx->state, length * sizeof(ptrit_t));
    ptrits += length;
    // no padding!
    pcurl_transform(ctx);
    length = 0;
  }
}
void pcurl_get_hash(pcurl_t *ctx, ptrit_t *hash) {
  memcpy(hash, ctx->state, CURL_RATE * sizeof(ptrit_t));
  pcurl_reset(ctx);
}
void pcurl_hash_data(pcurl_t *ctx, ptrit_t const *data, size_t size, ptrit_t *hash) {
  pcurl_reset(ctx);
  pcurl_absorb(ctx, data, size);
  pcurl_get_hash(ctx, hash);
}

#if defined(PCURL_DEBUG)
#include <stdio.h>
void ptrit_print(ptrit_t const *p) { printf("%d:%d ", *(int const *)(&p->low) & 1, *(int const *)(&p->high) & 1); }
void ptrits_print(size_t n, ptrit_t const *p) {
  for (; n--;) ptrit_print(p++);
  printf("\n");
}
void ptrits_print2(size_t n, ptrit_t const *p) {
  ptrits_print((n + 1) / 2, p);
  ptrits_print(n - (n + 1) / 2, p + (n + 1) / 2);
}
void ptrits_rprint(size_t n, ptrit_t const *p) {
  for (p += n; n--;) ptrit_print(--p);
  printf("\n");
}
#endif

#if defined(PCURL_STATE_SHORT)
void pcurl_transform(pcurl_t *ctx) {
  size_t round;

  ptrit_t *a = ctx->state;
  ptrit_t *b = a + (CURL_STATE_SIZE + 1) / 2;
  ptrit_t *c = b + (CURL_STATE_SIZE + 1) / 2;
  b[364] = a[0];
#if defined(PCURL_DEBUG)
  ptrits_print((CURL_STATE_SIZE + 1) / 2, a);
  ptrits_print((CURL_STATE_SIZE + 1) / 2, b);
#endif
  for (round = 0; round < ctx->round_count / 3; ++round) {
    pcurl_sbox_0(a, b, c);
#if defined(PCURL_DEBUG)
    printf("---\n");
    ptrits_print((CURL_STATE_SIZE + 1) / 2, c);
    ptrits_rprint((CURL_STATE_SIZE + 1) / 2, a);
#endif
    pcurl_sbox_1(a, b, c);
#if defined(PCURL_DEBUG)
    printf("---\n");
    ptrits_rprint((CURL_STATE_SIZE + 1) / 2, b);
    ptrits_rprint((CURL_STATE_SIZE + 1) / 2, c);
#endif
    pcurl_sbox_2(a, b, c);
#if defined(PCURL_DEBUG)
    printf("---\n");
    ptrits_print((CURL_STATE_SIZE + 1) / 2, a);
    ptrits_print((CURL_STATE_SIZE + 1) / 2, b);
#endif
  }
}
#elif defined(PCURL_STATE_DOUBLE)
void pcurl_transform(pcurl_t *ctx) {
  size_t round;

  ptrit_t *a = ctx->state, *t;
  ptrit_t *c = a + CURL_STATE_SIZE;
#if defined(PCURL_DEBUG)
  ptrits_print2(CURL_STATE_SIZE, a);
#endif
  for (round = 0; round < ctx->round_count; ++round) {
    pcurl_sbox(c, a);
    t = a;
    a = c;
    c = t;
#if defined(PCURL_DEBUG)
    printf("---\n");
    ptrits_print2(CURL_STATE_SIZE, a);
#endif
  }
  if (1 & ctx->round_count) memcpy(c, a, sizeof(ptrit_t) * CURL_STATE_SIZE);
}
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE

void pcurl_reset(pcurl_t *ctx) {
  // TODO: memset_safe
#if defined(PTRIT_CVT_ANDN)
  // 0 -> (1,1)
  memset(ctx->state, -1, sizeof(ctx->state));

#elif defined(PTRIT_CVT_ORN)
  // 0 -> (0,1)
  size_t i;
#if defined(PCURL_STATE_SHORT)
  for (i = 0; i < 3 * (CURL_STATE_SIZE + 1) / 2; ++i)
#elif defined(PCURL_STATE_DOUBLE)
  for (i = 0; i < 2 * CURL_STATE_SIZE; ++i)
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE
  {
    memset(&ctx->state[i].low, 0, sizeof(ptrit_s));
    memset(&ctx->state[i].high, -1, sizeof(ptrit_s));
  }
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
}

void ptrit_curl_init(PCurl *const ctx, CurlType type) { pcurl_init(ctx, (size_t)type); }

void ptrit_curl_absorb(PCurl *const ctx, ptrit_t const *const trits, size_t length) {
  pcurl_absorb(ctx, trits, length);
}

void ptrit_curl_squeeze(PCurl *const ctx, ptrit_t *const trits, size_t length) { pcurl_squeeze(ctx, trits, length); }

void ptrit_transform(PCurl *const ctx) { pcurl_transform(ctx); }

void ptrit_curl_reset(PCurl *const ctx) { pcurl_reset(ctx); }
