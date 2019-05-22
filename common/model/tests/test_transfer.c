/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/model/tests/defs.h"
#include "common/model/transfer.h"
#include "common/trinary/tryte_ascii.h"
#include "utils/time.h"

static char const msg[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris ac leo a "
    "massa porta tempus scelerisque in leo. Duis ligula dolor, condimentum vel "
    "ipsum et, accumsan viverra lacus.";
static tryte_t trytes[] =
    "VBCDFDTCADEAXCDDGDIDADEASCCD9DCDFDEAGDXCHDEAPCADTCHDQAEARCCDBDGDTCRCHDTCHD"
    "IDFDEAPCSCXCDDXCGDRCXCBDVCEATC9DXCHDSAEAWBPCIDFDXCGDEAPCRCEA9DTCCDEAPCEAAD"
    "PCGDGDPCEADDCDFDHDPCEAHDTCADDDIDGDEAGDRCTC9DTCFDXCGDEDIDTCEAXCBDEA9DTCCDSA"
    "EANBIDXCGDEA9DXCVCID9DPCEASCCD9DCDFDQAEARCCDBDSCXCADTCBDHDIDADEAJDTC9DEAXC"
    "DDGDIDADEATCHDQAEAPCRCRCIDADGDPCBDEAJDXCJDTCFDFDPCEA9DPCRCIDGDSA";

void test_fragments() {
  signature_fragments_t* fragments = signature_fragments_new();
  tryte_t** fg = NULL;
  signature_fragments_add(fragments, trytes);
  signature_fragments_add(fragments, TEST_SEED);
  signature_fragments_add(fragments, TEST_TAG);

  fg = signature_fragments_at(fragments, 0);
  TEST_ASSERT_EQUAL_MEMORY(*fg, trytes, strlen((char*)trytes));

  fg = signature_fragments_at(fragments, 1);
  TEST_ASSERT_EQUAL_MEMORY(*fg, TEST_SEED, strlen((char*)TEST_SEED));

  fg = signature_fragments_at(fragments, 2);
  TEST_ASSERT_EQUAL_MEMORY(*fg, TEST_TAG, strlen((char*)TEST_TAG));

  fg = signature_fragments_at(fragments, 3);
  TEST_ASSERT_NULL(fg);

  signature_fragments_free(fragments);
}

void test_transfer() {
  transfer_t tf = {};
  size_t size = strlen(msg);
  char buffer[size];
  size_t trytes_len = strlen((char*)trytes);

  // test message from trytes
  TEST_ASSERT_NULL(tf.message);
  transfer_message_set_trytes(&tf, trytes, trytes_len);
  TEST_ASSERT_EQUAL_MEMORY(transfer_message_get(&tf), trytes, trytes_len);

  trytes_to_ascii(trytes, strlen((char*)trytes), buffer);
  TEST_ASSERT_EQUAL_MEMORY(msg, buffer, size);

  transfer_message_free(&tf);
  tf.message = NULL;

  // test message from string
  TEST_ASSERT_NULL(tf.message);
  transfer_message_set_string(&tf, msg);
  TEST_ASSERT_EQUAL_MEMORY(transfer_message_get(&tf), trytes, trytes_len);
  transfer_message_free(&tf);
  tf.message = NULL;

  // test timestamp
  TEST_ASSERT_EQUAL_UINT64(0, tf.timestamp);
  uint64_t now = current_timestamp_ms();
  tf.timestamp = now;
  TEST_ASSERT_EQUAL_UINT64(now, tf.timestamp);

  // test value
  TEST_ASSERT_EQUAL_INT64(0, tf.value);
  tf.value = 123456789123456789;
  TEST_ASSERT_EQUAL_INT64(123456789123456789, tf.value);

  // test address
  flex_trit_t address[FLEX_TRIT_SIZE_243] = {};
  TEST_ASSERT_EQUAL_MEMORY(address, tf.address, FLEX_TRIT_SIZE_243);
  flex_trits_from_trytes(address, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  TEST_ASSERT_EQUAL_MEMORY(address, tf.address, FLEX_TRIT_SIZE_243);

  // test tag
  flex_trit_t tag[FLEX_TRIT_SIZE_81] = {};
  TEST_ASSERT_EQUAL_MEMORY(tag, tf.tag, FLEX_TRIT_SIZE_81);
  flex_trits_from_trytes(tag, NUM_TRITS_TAG, TEST_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, TEST_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  TEST_ASSERT_EQUAL_MEMORY(tag, tf.tag, FLEX_TRIT_SIZE_81);
}

void test_transfer_array() {
  transfer_array_t* tf_array = transfer_array_new();
  transfer_t tf = {};
  transfer_t* elm = NULL;
  transfer_array_add(tf_array, &tf);
  transfer_array_add(tf_array, &tf);
  transfer_array_add(tf_array, &tf);
  transfer_array_add(tf_array, &tf);
  TEST_ASSERT_EQUAL_UINT(4, transfer_array_count(tf_array));

  TRANSFER_FOREACH(tf_array, elm) {
    TEST_ASSERT_NULL(elm->message);
    TEST_ASSERT_EQUAL_UINT64(0, elm->timestamp);
    TEST_ASSERT_EQUAL_INT64(0, elm->value);
  }

  elm = transfer_array_at(tf_array, 3);
  TEST_ASSERT_NOT_NULL(elm);
  elm->value = -1235566778;

  elm = transfer_array_at(tf_array, 5);
  TEST_ASSERT_NULL(elm);

  elm = transfer_array_at(tf_array, 3);
  TEST_ASSERT_EQUAL_INT64(-1235566778, elm->value);

  size_t trytes_len = strlen((char*)trytes);
  transfer_message_set_trytes(elm, trytes, trytes_len);

  // trytes to ascii
  size_t size = strlen((char*)trytes) / 2;
  char buffer[size];
  trytes_to_ascii(trytes, trytes_len, buffer);
  TEST_ASSERT_EQUAL_MEMORY(msg, buffer, size);
  transfer_message_free(elm);

  transfer_array_free(tf_array);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_fragments);
  RUN_TEST(test_transfer);
  RUN_TEST(test_transfer_array);

  return UNITY_END();
}
