/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "node/recent_seen_bytes_cache.h"

void test_recent_seen_bytes_cache() {
  recent_seen_bytes_cache_t cache;
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  flex_trit_t tx_trits[4][FLEX_TRIT_SIZE_8019];
  byte_t tx_bytes[4][PACKET_SIZE];
  iota_transaction_t *txs[4];
  uint64_t digest = 0;
  bool digest_found = false;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(tx_trits[i], NUM_TRITS_SERIALIZED_TRANSACTION, txs_trytes[i],
                           NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
    flex_trits_to_bytes(tx_bytes[i], NUM_TRITS_SERIALIZED_TRANSACTION, tx_trits[i], NUM_TRITS_SERIALIZED_TRANSACTION,
                        NUM_TRITS_SERIALIZED_TRANSACTION);
  }
  transactions_deserialize(txs_trytes, txs, 4, true);

  TEST_ASSERT(recent_seen_bytes_cache_init(&cache, 3) == RC_OK);

  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 0);

  // Checking that 4 trytes are not in the cache

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[0], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_FALSE(digest_found);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[1], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_FALSE(digest_found);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[2], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_FALSE(digest_found);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[3], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_FALSE(digest_found);

  // Adding trytes in the cache and checking they are really in

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[0], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_put(&cache, digest, transaction_hash(txs[0])) == RC_OK);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_TRUE(digest_found);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[0]), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 1);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[1], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_put(&cache, digest, transaction_hash(txs[1])) == RC_OK);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_TRUE(digest_found);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[1]), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 2);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[2], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_put(&cache, digest, transaction_hash(txs[2])) == RC_OK);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_TRUE(digest_found);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[2]), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 3);

  // Adding an already added element

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[2], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_put(&cache, digest, transaction_hash(txs[2])) == RC_OK);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_TRUE(digest_found);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[2]), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 3);

  // Adding an element in excess should drop latest element

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[3], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_put(&cache, digest, transaction_hash(txs[3])) == RC_OK);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_TRUE(digest_found);
  TEST_ASSERT_EQUAL_MEMORY(transaction_hash(txs[3]), hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 3);

  digest = 0;
  digest_found = false;
  TEST_ASSERT(recent_seen_bytes_cache_hash(tx_bytes[0], &digest) == RC_OK);
  TEST_ASSERT(digest != 0);
  TEST_ASSERT(recent_seen_bytes_cache_get(&cache, digest, hash, &digest_found) == RC_OK);
  TEST_ASSERT_FALSE(digest_found);
  TEST_ASSERT(recent_seen_bytes_cache_size(&cache) == 3);

  TEST_ASSERT(recent_seen_bytes_cache_destroy(&cache) == RC_OK);
  transactions_free(txs, 4);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_recent_seen_bytes_cache);

  return UNITY_END();
}
