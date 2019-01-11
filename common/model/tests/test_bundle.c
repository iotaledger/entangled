/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/model/bundle.h"
#include "common/sign/normalize.h"

static tryte_t *trytes =
    (tryte_t *)"AABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRS"
    "TUVWXYZ";

static byte_t bytes[] = {
    0,   1,   2,   3,   4,  5,  6,  7,  8,  9,  10, 11, 12,  13,
    -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,  0,
    1,   2,   3,   4,   5,  6,  7,  8,  9,  10, 11, 12, 13,  -13,
    -12, -11, -10, -9,  -8, -7, -6, -5, -4, -3, -2, -1, 0,   1,
    2,   3,   4,   5,   6,  7,  8,  9,  10, 11, 12, 13, -13, -12,
    -11, -10, -9,  -8,  -7, -6, -5, -4, -3, -2, -1};

void test_normalized_bundle(void) {
  size_t length = strlen((char *)trytes);
  tryte_t normalized_bundle_bytes[length];
  flex_trit_t bundle_flex_trits[FLEX_TRIT_SIZE_243];

  flex_trits_from_trytes(bundle_flex_trits, HASH_LENGTH_TRIT, trytes, length,
                         length);
  normalize_hash(bundle_flex_trits, normalized_bundle_bytes);
  TEST_ASSERT_EQUAL_MEMORY(bytes, normalized_bundle_bytes, length);
}

void test_bundle_finalize(void) {
  bundle_transactions_t *bundle = NULL;
  flex_trit_t addr_sender[FLEX_TRIT_SIZE_243] = {};
  flex_trit_t addr_receiver[FLEX_TRIT_SIZE_243] = {};
  flex_trit_t tag[FLEX_TRIT_SIZE_81] = {};
  flex_trit_t obsolete_tag[FLEX_TRIT_SIZE_81] = {};
  iota_transaction_t tx;
  iota_transaction_t *bundle_tx;
  Kerl kerl = {};

  transaction_reset(&tx);
  flex_trits_from_trytes(addr_receiver, NUM_TRITS_ADDRESS,
    (const tryte_t *)"LQZYGHAQLJLENO9IBSFOFIYHIBKOHEWVAEHKYOED9WBERCCLGGLOJVIZSIUUXGJ9WONIGBXKTVAWUXNHW",
    NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  flex_trits_from_trytes(addr_sender, NUM_TRITS_ADDRESS,
    (const tryte_t *)"SEPKTEEOIIVHFJNQRHX9VXKQDYTXXRTF9TYXBIBXMYEXFDUKOTLNGCMRIWXMNT9ZBJUCXFWTNHGPAZESX",
    NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  flex_trits_from_trytes(tag, NUM_TRITS_TAG,
                         (const tryte_t *)"IOTA99999999999999999999999",
                         NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  flex_trits_from_trytes(obsolete_tag, NUM_TRITS_TAG,
                         (const tryte_t *)"9PTA99999999999999999999999",
                         NUM_TRYTES_TAG, NUM_TRYTES_TAG);

  bundle_transactions_new(&bundle);
  // receiver tx
  transaction_set_address(&tx, addr_receiver);
  transaction_set_tag(&tx, tag);
  transaction_set_obsolete_tag(&tx, tag);
  transaction_set_timestamp(&tx, 1546760794);
  transaction_set_value(&tx, 1000);
  transaction_set_current_index(&tx, 0);
  transaction_set_last_index(&tx, 2);
  tx.loaded_columns_mask |= MASK_ALL_COLUMNS;
  bundle_transactions_add(bundle, &tx);

  // sender tx
  transaction_reset(&tx);
  transaction_set_address(&tx, addr_sender);
  transaction_set_tag(&tx, tag);
  transaction_set_obsolete_tag(&tx, tag);
  transaction_set_timestamp(&tx, 1546760811);
  transaction_set_value(&tx, -1000);
  transaction_set_current_index(&tx, 1);
  transaction_set_last_index(&tx, 2);
  tx.loaded_columns_mask |= MASK_ALL_COLUMNS;
  bundle_transactions_add(bundle, &tx);
  // security level 2
  transaction_reset(&tx);
  transaction_set_address(&tx, addr_sender);
  transaction_set_tag(&tx, tag);
  transaction_set_obsolete_tag(&tx, tag);
  transaction_set_timestamp(&tx, 1546760811);
  transaction_set_value(&tx, 0);
  transaction_set_current_index(&tx, 2);
  transaction_set_last_index(&tx, 2);
  tx.loaded_columns_mask |= MASK_ALL_COLUMNS;
  bundle_transactions_add(bundle, &tx);

  bundle_finalize(bundle, &kerl);
  bundle_dump(bundle);
  bundle_tx = (iota_transaction_t *)utarray_front(bundle);

  TEST_ASSERT_EQUAL_MEMORY(transaction_obsolete_tag(bundle_tx), obsolete_tag,
                           FLEX_TRIT_SIZE_81);

  bundle_transactions_free(&bundle);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_normalized_bundle);
  RUN_TEST(test_bundle_finalize);

  return UNITY_END();
}
