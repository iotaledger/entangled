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
#include "common/trinary/flex_trit.h"

namespace {
TEST(TransactionTest, testTransaction) {
  using namespace testing;
  using namespace iota;
  using namespace model;

  std::string trytes(TRYTES, sizeof(TRYTES));
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t*)TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  TryteTransaction transaction = TryteTransaction(trytes);
  std::vector<flex_trit_t> value = transaction.serialize();
  std::vector<flex_trit_t> ref(trits, trits + sizeof(trits));
  ASSERT_EQ(ref, value);
}
}  // namespace
