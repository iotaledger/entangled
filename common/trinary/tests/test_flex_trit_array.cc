/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <gtest/gtest.h>
#include <chrono>
#include "common/trinary/flex_trit_array.h"

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
const int8_t TRITS_IN[] = {-1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1, 0};
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
const uint8_t TRITS_IN[] = {0x48, 0x4a, 0x53, 0x42, 0x44, 0x42};
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
const uint8_t TRITS_IN[] = {0x53, 0x14, 0x1f, 0xC5, 0x01};
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
const uint8_t TRITS_IN[] = {0x23, 0x98, 0x25, 0x02};
#endif
#define TRYTES 'H', 'J', 'S', 'B', 'D', 'B'
#define TRITS_OUT -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1, 0
#define NUM_TRITS 18

namespace {
TEST(FlexTritTest, testFlexTritArrayAt) {
  using namespace testing;
  using namespace iota;
  using namespace trinary;

  std::vector<flex_trit_t> trits(TRITS_IN, TRITS_IN + sizeof(TRITS_IN));
  FlexTritArray ft(trits, NUM_TRITS);
  ASSERT_EQ(ft[0], -1);
  ASSERT_EQ(ft[1], 0);
  ASSERT_EQ(ft[2], 1);
  ft[0] = 1;
  ASSERT_EQ(ft[0], 1);
  ft[1] = -1;
  ASSERT_EQ(ft[1], -1);
  ft[2] = 0;
  ASSERT_EQ(ft[2], 0);
}

TEST(FlexTritTest, testFlexTritArraySlice) {
  using namespace testing;
  using namespace iota;
  using namespace trinary;

  std::vector<flex_trit_t> trits(TRITS_IN, TRITS_IN + sizeof(TRITS_IN));
  FlexTritArray ft(trits, NUM_TRITS);
  FlexTritArray st = ft.slice(4, 7);
  ASSERT_EQ(st.size(), 7);
  ASSERT_EQ(st[0], 0);
  ASSERT_EQ(st[6], 1);
}

TEST(FlexTritTest, testFlexTritArrayInsert) {
  using namespace testing;
  using namespace iota;
  using namespace trinary;

  std::vector<flex_trit_t> trits(TRITS_IN, TRITS_IN + sizeof(TRITS_IN));
  FlexTritArray ft(trits, NUM_TRITS);
  FlexTritArray st = ft.slice(8, 5);
  ASSERT_EQ(ft[4], 0);
  ASSERT_EQ(ft[8], -1);
  ft.insert(st, 4);
  ASSERT_EQ(ft[4], -1);
  ASSERT_EQ(ft[8], 1);
}

TEST(FlexTritTest, testFlexTritArrayTrits) {
  using namespace testing;
  using namespace iota;
  using namespace trinary;

  std::vector<flex_trit_t> trits(TRITS_IN, TRITS_IN + sizeof(TRITS_IN));
  FlexTritArray ft(trits, NUM_TRITS);
  std::vector<trit_t> expected = {TRITS_OUT};
  ASSERT_EQ(ft.trits(), expected);
}

TEST(FlexTritTest, testFlexTritArrayTrytes) {
  using namespace testing;
  using namespace iota;
  using namespace trinary;

  std::vector<flex_trit_t> trits(TRITS_IN, TRITS_IN + sizeof(TRITS_IN));
  FlexTritArray ft(trits, NUM_TRITS);
  std::vector<tryte_t> expected = {TRYTES};
  ASSERT_EQ(ft.trytes(), expected);
}

}  // namespace
