/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "common/trinary/flex_trit.h"

#define TRITS_IN  -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1, 0
#define NUM_TRITS 18

namespace {
TEST(FlexTritTest, testFlexTritArrayAt) {
using namespace testing;
using namespace iota;
using namespace trinary;

flex_trit_t trits[] = {TRITS_IN};
FlexTrit ft(trits, NUM_TRITS);
ASSERT_EQ(ft[0], -1);
ft[0] = 1;
ASSERT_EQ(ft[0], 1);
}
}