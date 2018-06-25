#include <gtest/gtest.h>
#include <cstdint>

#include "common/helpers/checksum.h"

namespace {

TEST(ChecksumTest, testChecksumEquility) {
  const std::string ADDRESS =
      "UYEEERFQYTPFAHIPXDQAQYWYMSMCLMGBTYAXLWFRFFWPYFOICOVLK9A9VYNCKK9TQUNBTARC"
      "EQXJHD9VY";

  const std::string CHECKSUM =
      "CFOHVYSWYAKMFOPCSUXAYJQFXIZCAFLJSLZQNNJMYCBEERFGZ9C9GYDZYDADZIHJMUWGIYRP"
      "XOEDEOMRC";

  char* out = iota_checksum(ADDRESS.c_str(), ADDRESS.size(), 9);
  EXPECT_EQ(out, CHECKSUM.substr(CHECKSUM.size() - 9, 9));
  free(out);

  out = iota_checksum(ADDRESS.c_str(), ADDRESS.size(), 0);
  EXPECT_EQ(out, nullptr);
  free(out);

  out = iota_checksum(ADDRESS.c_str(), ADDRESS.size(), 3);
  EXPECT_EQ(out, CHECKSUM.substr(CHECKSUM.size() - 3, 3));
  free(out);

  out = iota_checksum(ADDRESS.c_str(), ADDRESS.size(), 81);
  EXPECT_EQ(out, CHECKSUM);
  free(out);
}

}  // namespace
