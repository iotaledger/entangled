#ifndef __COMMON_TRIT_TRYTE_SSE42_H_
#define __COMMON_TRIT_TRYTE_SSE42_H_

#include <nmmintrin.h>

#include "common/defs.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/tryte.h"

#define BLOCK_8BIT(type) (sizeof(type) / sizeof(int8_t))
#define BYTE_OF_128BIT 16
#define COMMA0
#define COMMA1 ,
#define COMMA(x) COMMA##x
#define INDEX_3DIFF_0F 0x00, 0x03, 0x06, 0x09, 0x0C, 0x0F
#define INDEX_3DIFF_1D 0x01, 0x04, 0x07, 0x0A, 0x0D
#define INDEX_3DIFF_2E 0x02, 0x05, 0x08, 0x0B, 0x0E
#define REPEAT0(str)
#define REPEAT1(str) str
#define REPEAT2(str) REPEAT1(str), str
#define REPEAT3(str) REPEAT2(str), str
#define REPEAT4(str) REPEAT3(str), str
#define REPEAT5(str) REPEAT4(str), str
#define REPEAT6(str) REPEAT5(str), str
#define REPEAT7(str) REPEAT6(str), str
#define REPEAT8(str) REPEAT7(str), str
#define REPEAT9(str) REPEAT8(str), str
#define REPEAT10(str) REPEAT9(str), str
#define REPEAT11(str) REPEAT10(str), str
#define REPEAT(n, str) REPEAT##n(str)

static inline void trits_to_trytes_sse42(trit_t const *const trits, tryte_t *const trytes, size_t const length) {
  const int block_8bit = BLOCK_8BIT(__m128i);
  const int8_t set_msb = 0x80;
  const __m128i tryte_alphabet[2] = {
      _mm_setr_epi8(TRYTE_ALPHABET[14], TRYTE_ALPHABET[15], TRYTE_ALPHABET[16], TRYTE_ALPHABET[17], TRYTE_ALPHABET[18],
                    TRYTE_ALPHABET[19], TRYTE_ALPHABET[20], TRYTE_ALPHABET[21], TRYTE_ALPHABET[22], TRYTE_ALPHABET[23],
                    TRYTE_ALPHABET[24], TRYTE_ALPHABET[25], TRYTE_ALPHABET[26], TRYTE_ALPHABET[0], TRYTE_ALPHABET[1],
                    TRYTE_ALPHABET[2]),
      _mm_setr_epi8(TRYTE_ALPHABET[3], TRYTE_ALPHABET[4], TRYTE_ALPHABET[5], TRYTE_ALPHABET[6], TRYTE_ALPHABET[7],
                    TRYTE_ALPHABET[8], TRYTE_ALPHABET[9], TRYTE_ALPHABET[10], TRYTE_ALPHABET[11], TRYTE_ALPHABET[12],
                    TRYTE_ALPHABET[13], 0, 0, 0, 0, 0)};
  /* For shuffling the bytes of the input trits */
  const __m128i shuffle_low[3] = {
      _mm_setr_epi8(REPEAT(0, set_msb) COMMA(0) INDEX_3DIFF_0F COMMA(1) REPEAT(10, set_msb)),
      _mm_setr_epi8(REPEAT(6, set_msb) COMMA(1) INDEX_3DIFF_2E COMMA(1) REPEAT(5, set_msb)),
      _mm_setr_epi8(REPEAT(11, set_msb) COMMA(1) INDEX_3DIFF_1D COMMA(0) REPEAT(0, set_msb))};
  const __m128i shuffle_mid[3] = {
      _mm_setr_epi8(REPEAT(0, set_msb) COMMA(0) INDEX_3DIFF_1D COMMA(1) REPEAT(11, set_msb)),
      _mm_setr_epi8(REPEAT(5, set_msb) COMMA(1) INDEX_3DIFF_0F COMMA(1) REPEAT(5, set_msb)),
      _mm_setr_epi8(REPEAT(11, set_msb) COMMA(1) INDEX_3DIFF_2E COMMA(0) REPEAT(0, set_msb))};
  const __m128i shuffle_high[3] = {
      _mm_setr_epi8(REPEAT(0, set_msb) COMMA(0) INDEX_3DIFF_2E COMMA(1) REPEAT(11, set_msb)),
      _mm_setr_epi8(REPEAT(5, set_msb) COMMA(1) INDEX_3DIFF_1D COMMA(1) REPEAT(6, set_msb)),
      _mm_setr_epi8(REPEAT(10, set_msb) COMMA(1) INDEX_3DIFF_0F COMMA(0) REPEAT(0, set_msb))};

  /* Start converting */
  for (size_t i = 0; i < length / NUMBER_OF_TRITS_IN_A_TRYTE / block_8bit; i++) {
    /* Get trit data */
    __m128i data_first = _mm_loadu_si128((__m128i *)(trits) + i * 3);
    __m128i data_mid = _mm_loadu_si128((__m128i *)(trits) + i * 3 + 1);
    __m128i data_last = _mm_loadu_si128((__m128i *)(trits) + i * 3 + 2);
    /*
     * Each block represents a trit.
     *                                            shuffle
     *              ----------------        ------                   ------     ------     ------
     * data_first = | a1 | a2 | a3 | ...... | f1 |       low_trit  = | a1 | ... | f1 | ... | p1 |
     *              ----------------        ------                   ------     ------     ------
     *              ----------------        ------                   ------     ------     ------
     * data_mid   = | f2 | f3 | g1 | ...... | k2 |  =>   mid_trit  = | a2 | ... | f2 | ... | p2 |
     *              ----------------        ------                   ------     ------     ------
     *              ----------------        ------                   ------     ------     ------
     * data_last  = | k3 | l1 | l2 | ...... | p3 |       high_trit = | a3 | ... | f3 | ... | p3 |
     *              ----------------        ------                   ------     ------     ------
     */
    __m128i low_trit = _mm_or_si128(
        _mm_shuffle_epi8(data_first, shuffle_low[0]),
        _mm_or_si128(_mm_shuffle_epi8(data_mid, shuffle_low[1]), _mm_shuffle_epi8(data_last, shuffle_low[2])));
    __m128i mid_trit = _mm_or_si128(
        _mm_shuffle_epi8(data_first, shuffle_mid[0]),
        _mm_or_si128(_mm_shuffle_epi8(data_mid, shuffle_mid[1]), _mm_shuffle_epi8(data_last, shuffle_mid[2])));
    __m128i high_trit = _mm_or_si128(
        _mm_shuffle_epi8(data_first, shuffle_high[0]),
        _mm_or_si128(_mm_shuffle_epi8(data_mid, shuffle_high[1]), _mm_shuffle_epi8(data_last, shuffle_high[2])));
    /* low_result = (low_trit) */
    __m128i low_result = low_trit;
    /* mid_result = (mid_trit * 3) = (mid_trit + mid_trit + mid_trit) */
    __m128i mid_result = _mm_add_epi8(mid_trit, _mm_add_epi8(mid_trit, mid_trit));
    /* high_result = (high_trit * 9) = (high_trit + high_trit * 4 + high_trit * 4)*/
    __m128i high_trit_2 = _mm_add_epi8(high_trit, high_trit);
    __m128i high_trit_4 = _mm_add_epi8(high_trit_2, high_trit_2);
    __m128i high_result = _mm_add_epi8(high_trit, _mm_add_epi8(high_trit_4, high_trit_4));
    /* alphabet_offset = (low_result + mid_result + high_result) */
    __m128i alphabet_offset = _mm_add_epi8(low_result, _mm_add_epi8(mid_result, high_result));
    /* Add 0x0D (13) to eliminate negative value */
    alphabet_offset = _mm_add_epi8(alphabet_offset, _mm_set_epi32(REPEAT(4, 0x0D0D0D0D)));

    /* Assign tryte alphabet */
    /* If the offset is >= 16 (> 15), then the compared result byte = 0xFF,
     * else = 0x00 */
    __m128i cmp_result =
        _mm_cmpgt_epi8(alphabet_offset, _mm_set_epi8(15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15));
    /* Use the offset to get the correct tryte alphabet from tryte_alphabet[]
     */
    __m128i result_lt = _mm_shuffle_epi8(tryte_alphabet[0], alphabet_offset);
    __m128i result_ge = _mm_shuffle_epi8(
        tryte_alphabet[1],
        /* alphabet_offset - 16 */
        _mm_sub_epi8(alphabet_offset, _mm_set_epi8(16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16)));
    __m128i result = _mm_or_si128(_mm_andnot_si128(cmp_result, result_lt), _mm_and_si128(cmp_result, result_ge));
    /* Store the tryte result */
    _mm_storeu_si128((__m128i *)(trytes + i * block_8bit), result);
  }
  /* The rest of the trits */
  for (size_t i = (length / NUMBER_OF_TRITS_IN_A_TRYTE / block_8bit) * block_8bit;
       i * NUMBER_OF_TRITS_IN_A_TRYTE < length; i++) {
    int k = 0;
    int used_trit = i * NUMBER_OF_TRITS_IN_A_TRYTE;
    for (size_t l = length - used_trit < NUMBER_OF_TRITS_IN_A_TRYTE ? length - used_trit : NUMBER_OF_TRITS_IN_A_TRYTE;
         l-- > 0;) {
      k *= RADIX;
      k += trits[used_trit + l];
    }

    if (k < 0) {
      k += TRYTE_SPACE_SIZE;
    }
    trytes[i] = TRYTE_ALPHABET[k];
  }
}

static inline void trytes_to_trits_sse42(tryte_t const *const trytes, trit_t *const trits, size_t const length) {
  const int block_8bit = BLOCK_8BIT(__m128i);
  /* For setting the most significant bit of a byte */
  const int8_t set_msb = 0x80;
  /* The set and range for indicating the trits value (0, 1, -1)
   * of the corresponding trytes */
  /* '9', 'C', 'F', 'I', 'L', 'O', 'R', 'U', 'X' */
  const char set_low_trit_0[BYTE_OF_128BIT] = "9CFILORUX";
  /* 'A', 'D', 'G', 'J', 'M', 'P', 'S', 'V', 'Y' */
  const char set_low_trit_p1[BYTE_OF_128BIT] = "ADGJMPSVY";
  /* 'B', 'E', 'H', 'K', 'N', 'Q', 'T', 'W', 'Z' */
  const char set_low_trit_n1[BYTE_OF_128BIT] = "BEHKNQTWZ";
  /* '9', 'A', 'H', 'I', 'J', 'Q', 'R', 'S', 'Z' */
  const char range_mid_trit_0[BYTE_OF_128BIT] = "99AAHJQSZZ";
  /* 'B', 'C', 'D', 'K', 'L', 'M', 'T', 'U', 'V' */
  const char range_mid_trit_p1[BYTE_OF_128BIT] = "BDKMTV";
  /* 'E', 'F', 'G', 'N', 'O', 'P', 'W', 'X', 'Y' */
  const char range_mid_trit_n1[BYTE_OF_128BIT] = "EGNPWY";
  /* '9', 'A', 'B', 'C', 'D', 'W', 'X', 'Y', 'Z' */
  const char range_high_trit_0[BYTE_OF_128BIT] = "99ADWZ";
  /* 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M' */
  const char range_high_trit_p1[BYTE_OF_128BIT] = "EM";
  /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V' */
  const char range_high_trit_n1[BYTE_OF_128BIT] = "NV";
  /* Convert the char array to the 128-bit data */
  const __m128i pattern_low_trit_0 = _mm_loadu_si128((__m128i *)(set_low_trit_0));
  const __m128i pattern_low_trit_p1 = _mm_loadu_si128((__m128i *)(set_low_trit_p1));
  const __m128i pattern_low_trit_n1 = _mm_loadu_si128((__m128i *)(set_low_trit_n1));
  const __m128i pattern_mid_trit_0 = _mm_loadu_si128((__m128i *)(range_mid_trit_0));
  const __m128i pattern_mid_trit_p1 = _mm_loadu_si128((__m128i *)(range_mid_trit_p1));
  const __m128i pattern_mid_trit_n1 = _mm_loadu_si128((__m128i *)(range_mid_trit_n1));
  const __m128i pattern_high_trit_0 = _mm_loadu_si128((__m128i *)(range_high_trit_0));
  const __m128i pattern_high_trit_p1 = _mm_loadu_si128((__m128i *)(range_high_trit_p1));
  const __m128i pattern_high_trit_n1 = _mm_loadu_si128((__m128i *)(range_high_trit_n1));
  /* The 128-bit data with the repeated same bytes */
  const __m128i pos_one = _mm_set1_epi8(1);
  const __m128i neg_one = _mm_set1_epi8(-1);
  const __m128i zero = _mm_set1_epi8(0);
  /* For shuffling the bytes of the trits transformed from the input trytes */
  const __m128i shuffle_first[3] = {
      _mm_setr_epi8(0x00, REPEAT2(set_msb), 0x01, REPEAT2(set_msb), 0x02, REPEAT2(set_msb), 0x03, REPEAT2(set_msb),
                    0x04, REPEAT2(set_msb), 0x05),
      _mm_setr_epi8(REPEAT1(set_msb), 0x00, REPEAT2(set_msb), 0x01, REPEAT2(set_msb), 0x02, REPEAT2(set_msb), 0x03,
                    REPEAT2(set_msb), 0x04, REPEAT2(set_msb)),
      _mm_setr_epi8(REPEAT2(set_msb), 0x00, REPEAT2(set_msb), 0x01, REPEAT2(set_msb), 0x02, REPEAT2(set_msb), 0x03,
                    REPEAT2(set_msb), 0x04, REPEAT1(set_msb))};
  const __m128i shuffle_mid[3] = {_mm_setr_epi8(REPEAT2(set_msb), 0x06, REPEAT2(set_msb), 0x07, REPEAT2(set_msb), 0x08,
                                                REPEAT2(set_msb), 0x09, REPEAT2(set_msb), 0x0A, REPEAT1(set_msb)),
                                  _mm_setr_epi8(0x05, REPEAT2(set_msb), 0x06, REPEAT2(set_msb), 0x07, REPEAT2(set_msb),
                                                0x08, REPEAT2(set_msb), 0x09, REPEAT2(set_msb), 0x0A),
                                  _mm_setr_epi8(REPEAT1(set_msb), 0x05, REPEAT2(set_msb), 0x06, REPEAT2(set_msb), 0x07,
                                                REPEAT2(set_msb), 0x08, REPEAT2(set_msb), 0x09, REPEAT2(set_msb))};
  const __m128i shuffle_last[3] = {_mm_setr_epi8(REPEAT1(set_msb), 0x0B, REPEAT2(set_msb), 0x0C, REPEAT2(set_msb), 0x0D,
                                                 REPEAT2(set_msb), 0x0E, REPEAT2(set_msb), 0x0F, REPEAT2(set_msb)),
                                   _mm_setr_epi8(REPEAT2(set_msb), 0x0B, REPEAT2(set_msb), 0x0C, REPEAT2(set_msb), 0x0D,
                                                 REPEAT2(set_msb), 0x0E, REPEAT2(set_msb), 0x0F, REPEAT1(set_msb)),
                                   _mm_setr_epi8(0x0A, REPEAT2(set_msb), 0x0B, REPEAT2(set_msb), 0x0C, REPEAT2(set_msb),
                                                 0x0D, REPEAT2(set_msb), 0x0E, REPEAT2(set_msb), 0x0F)};

  /* Start converting */
  /* The for loop handles the group of the 128-bit characters without the
   * end-of-string */
  for (size_t i = 0; i < length / block_8bit; i++) {
    /* Get tryte data */
    __m128i data = _mm_loadu_si128((__m128i *)(trytes) + i);

    /* The masks for setting the corresponding trits */
    __m128i mask_low_trit_0 = _mm_cmpistrm(pattern_low_trit_0, data,
                                           /* Signed byte comparison */
                                           _SIDD_SBYTE_OPS |
                                               /* Compare with the character set */
                                               _SIDD_CMP_EQUAL_ANY |
                                               /* Expand the corrsponding bit result to byte unit */
                                               _SIDD_UNIT_MASK);
    __m128i mask_low_trit_p1 =
        _mm_cmpistrm(pattern_low_trit_p1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_UNIT_MASK);
    __m128i mask_low_trit_n1 =
        _mm_cmpistrm(pattern_low_trit_n1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_UNIT_MASK);
    __m128i mask_mid_trit_0 = _mm_cmpistrm(pattern_mid_trit_0, data,
                                           /* Signed byte comparison */
                                           _SIDD_SBYTE_OPS |
                                               /* Compare with the character range */
                                               _SIDD_CMP_RANGES |
                                               /* Expand the corrsponding bit result to byte unit */
                                               _SIDD_UNIT_MASK);
    __m128i mask_mid_trit_p1 =
        _mm_cmpistrm(pattern_mid_trit_p1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
    __m128i mask_mid_trit_n1 =
        _mm_cmpistrm(pattern_mid_trit_n1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
    __m128i mask_high_trit_0 =
        _mm_cmpistrm(pattern_high_trit_0, data, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
    __m128i mask_high_trit_p1 =
        _mm_cmpistrm(pattern_high_trit_p1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
    __m128i mask_high_trit_n1 =
        _mm_cmpistrm(pattern_high_trit_n1, data, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);

    /*
     * Each block represents a trit.
     *                                         shuffle
     *             ------     ------     ------                    ----------------        ------
     * low_trit  = | a1 | ... | f1 | ... | p1 |       data_first = | a1 | a2 | a3 | ...... | f1 |
     *             ------     ------     ------                    ----------------        ------
     *             ------     ------     ------                    ----------------        ------
     * mid_trit  = | a2 | ... | f2 | ... | p2 |   =>  data_mid   = | f2 | f3 | g1 | ...... | k2 |
     *             ------     ------     ------                    ----------------        ------
     *             ------     ------     ------                    ----------------        ------
     * high_trit = | a3 | ... | f3 | ... | p3 |       data_last  = | k3 | l1 | l2 | ...... | p3 |
     *             ------     ------     ------                    ----------------        ------
     */
    __m128i low_trit =
        _mm_or_si128(_mm_and_si128(mask_low_trit_0, zero),
                     _mm_or_si128(_mm_and_si128(mask_low_trit_p1, pos_one), _mm_and_si128(mask_low_trit_n1, neg_one)));
    __m128i mid_trit =
        _mm_or_si128(_mm_and_si128(mask_mid_trit_0, zero),
                     _mm_or_si128(_mm_and_si128(mask_mid_trit_p1, pos_one), _mm_and_si128(mask_mid_trit_n1, neg_one)));
    __m128i high_trit = _mm_or_si128(
        _mm_and_si128(mask_high_trit_0, zero),
        _mm_or_si128(_mm_and_si128(mask_high_trit_p1, pos_one), _mm_and_si128(mask_high_trit_n1, neg_one)));
    __m128i data_first, data_mid, data_last;
    data_first = _mm_or_si128(
        _mm_shuffle_epi8(low_trit, shuffle_first[0]),
        _mm_or_si128(_mm_shuffle_epi8(mid_trit, shuffle_first[1]), _mm_shuffle_epi8(high_trit, shuffle_first[2])));
    data_mid = _mm_or_si128(
        _mm_shuffle_epi8(low_trit, shuffle_mid[0]),
        _mm_or_si128(_mm_shuffle_epi8(mid_trit, shuffle_mid[1]), _mm_shuffle_epi8(high_trit, shuffle_mid[2])));
    data_last = _mm_or_si128(
        _mm_shuffle_epi8(low_trit, shuffle_last[0]),
        _mm_or_si128(_mm_shuffle_epi8(mid_trit, shuffle_last[1]), _mm_shuffle_epi8(high_trit, shuffle_last[2])));

    /* Store the 3 * 128-bit trits converted from trytes */
    _mm_storeu_si128((__m128i *)(trits + (3 * i) * block_8bit), data_first);
    _mm_storeu_si128((__m128i *)(trits + (3 * i + 1) * block_8bit), data_mid);
    _mm_storeu_si128((__m128i *)(trits + (3 * i + 2) * block_8bit), data_last);
  }
  /* The rest of the trytes */
  for (size_t i = (length / block_8bit) * block_8bit,
              j = (length / block_8bit) * block_8bit * NUMBER_OF_TRITS_IN_A_TRYTE;
       i < length; i++, j += RADIX) {
    memcpy(trits + j, TRYTES_TRITS_LUT[INDEX_OF_TRYTE(trytes[i])], NUMBER_OF_TRITS_IN_A_TRYTE);
  }
}

#endif  // __COMMON_TRIT_TRYTE_SSE42_H_
