/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <gtest/gtest.h>
#include <chrono>

#include "common/model/tests/defs.h"
#include "common/model/tryte_transaction.h"

namespace {
TEST(TransactionTest, testTransaction) {
  using namespace testing;
  using namespace iota;
  using namespace model;

  std::string trytes(reinterpret_cast<char const*>(TRYTES));

  TryteTransaction transaction = TryteTransaction(trytes);
  std::vector<flex_trit_t> value = transaction.serialize();
  std::vector<flex_trit_t> ref(TRITS, TRITS + sizeof(TRITS));
  ASSERT_EQ(ref, value);
}
}  // namespace
