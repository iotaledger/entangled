#include <cstdint>
#include <iostream>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mobile/interface/bindings.h"

namespace {

TEST(KerlTest, testAddressGeneration) {
  using namespace testing;

  const std::string SEED =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQ"
      "RSTUVWXYZ9";

  const std::string EX_ADD_0_1 =
      "CFOYUCXLHLSUBAEYOTAWUNRPJFA9TSJNLBFLMZQASPTVCMTFBOQQRGGQ9MRZCJWYGBORJZQW"
      "VSBLVKBVW";
  const std::string EX_ADD_2_2 =
      "TZZUOMKXUUIIAJEQJJRIJQKWQVYK9YPFNWWUUHDQWVVWIBWHC9J9FJJEFZKOZZAIMUDDGHOI"
      "WTIWCOHGD";
  const std::string EX_ADD_2_3 =
      "MLAUELWJHZ9QBPCIYLOXAWCVSZTK9XUEIWQSRLFDWEORDOLVOMOF9RUMFXSAMYWCGXDAVXZM"
      "RWQOJH9RY";

  const char* out_1 = generate_address(SEED.c_str(), 0, 1);
  const char* out_2 = generate_address(SEED.c_str(), 2, 2);
  const char* out_3 = generate_address(SEED.c_str(), 2, 3);

  EXPECT_EQ(out_1, EX_ADD_0_1);
  EXPECT_EQ(out_2, EX_ADD_2_2);
  EXPECT_EQ(out_3, EX_ADD_2_3);
}

}  // namespace
