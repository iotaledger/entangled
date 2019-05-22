/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/iss/normalize.h"
#include "common/model/bundle.h"
#include "common/trinary/flex_trit.h"
#include "common/trinary/tryte_ascii.h"

static tryte_t *trytes =
    (tryte_t *)"AABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRS"
    "TUVWXYZ";

static byte_t bytes[] = {0,   1,   2,   3,  4,  5,   6,   7,   8,   9,  10, 11, 12, 13, -13, -12, -11, -10, -9, -8, -7,
                         -6,  -5,  -4,  -3, -2, -1,  0,   1,   2,   3,  4,  5,  6,  7,  8,   9,   10,  11,  12, 13, -13,
                         -12, -11, -10, -9, -8, -7,  -6,  -5,  -4,  -3, -2, -1, 0,  1,  2,   3,   4,   5,   6,  7,  8,
                         9,   10,  11,  12, 13, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4,  -3,  -2,  -1};

static char const *const long_message =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla condimentum diam nisi, aliquam tempor risus ornare "
    "a. Phasellus dignissim orci eget sagittis ultricies. Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    "Duis facilisis turpis eu sem egestas tempus. Pellentesque a efficitur dui. Nullam vulputate euismod enim, nec "
    "vehicula augue venenatis vel. Vivamus tempor mollis dui a sollicitudin. Duis egestas lorem id elit vulputate, sit "
    "amet commodo odio cursus. Donec ac dictum lacus, a varius turpis. Proin scelerisque magna vitae tempor facilisis. "
    "Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Etiam id dui "
    "ultricies ligula faucibus eleifend et sed dolor. Duis aliquet lectus justo, at sodales ex vehicula ac. Integer "
    "luctus nulla et est luctus, sit amet lobortis ante pharetra.Sed sit amet ex tristique, faucibus felis et, "
    "dignissim elit. Sed vitae mollis lacus. Proin nec rutrum arcu. Vestibulum nec velit ac risus rutrum aliquet vitae "
    "dignissim est. Praesent non ante vitae est ultricies tempus. Nullam erat tellus, eleifend ut rutrum eget, "
    "sagittis a tortor. Morbi iaculis blandit laoreet. Quisque non lectus id ex pharetra fringilla id vitae mi. Nunc "
    "pharetra sed sapien ac blandit. Sed vulputate risus eget magna vehicula, quis facilisis quam vestibulum.Donec "
    "odio risus, maximus non elit in, mollis pharetra mi. Quisque lobortis cursus pretium. Pellentesque ac bibendum "
    "sem. Integer non leo placerat, mollis ex sit amet, dapibus sed.";

static char const *const short_message = "Hello IOTA!!!";

void test_normalized_bundle(void) {
  size_t length = strlen((char *)trytes);
  byte_t normalized_bundle_bytes[length];
  flex_trit_t bundle_flex_trits[FLEX_TRIT_SIZE_243];

  flex_trits_from_trytes(bundle_flex_trits, HASH_LENGTH_TRIT, trytes, length, length);
  normalize_flex_hash(bundle_flex_trits, normalized_bundle_bytes);
  TEST_ASSERT_EQUAL_MEMORY(bytes, normalized_bundle_bytes, length);
}

static void message_to_bundle(bundle_transactions_t *bundle, char const *const str_msg) {
  // converting ascii string to trytes string
  size_t trytes_msg_len = strlen(str_msg) * 2;
  tryte_t trytes_msg[trytes_msg_len + 1];
  ascii_to_trytes(str_msg, trytes_msg);
  trytes_msg[trytes_msg_len] = '\0';  // null terminator is needed.

  // apply trytes string to bundle
  iota_transaction_t tx = {};
  tryte_t msg_buff[NUM_TRYTES_MESSAGE + 1] = {};
  size_t msg_chunks = floor((double)trytes_msg_len / NUM_TRYTES_MESSAGE) + 1;
  signature_fragments_t *sign_fragments = signature_fragments_new();
  size_t last_chunk = 0;
  transaction_reset(&tx);

  for (size_t i = 0; i < (msg_chunks * NUM_TRYTES_MESSAGE); i += NUM_TRYTES_MESSAGE) {
    if (i + NUM_TRYTES_MESSAGE > trytes_msg_len) {
      last_chunk = trytes_msg_len - ((msg_chunks - 1) * NUM_TRYTES_MESSAGE);
      strncpy((char *)msg_buff, (char *)trytes_msg + i, last_chunk);
      msg_buff[last_chunk] = '\0';
    } else {
      strncpy((char *)msg_buff, (char *)trytes_msg + i, NUM_TRYTES_MESSAGE);
      msg_buff[NUM_TRYTES_MESSAGE] = '\0';
    }

    signature_fragments_add(sign_fragments, msg_buff);
    bundle_transactions_add(bundle, &tx);
  }

  bundle_set_messages(bundle, sign_fragments);
  signature_fragments_free(sign_fragments);
}

static void test_bundle_message(char const *const message) {
  // applying message to a bundle
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  message_to_bundle(bundle, message);

  // extracting bundle message
  iota_transaction_t *curr_tx = NULL;
  size_t trytes_buff_size = NUM_TRYTES_MESSAGE * bundle_transactions_size(bundle);
  tryte_t trytes_buff[trytes_buff_size + trytes_buff_size % 2];
  size_t index = 0;
  BUNDLE_FOREACH(bundle, curr_tx) {
    flex_trits_to_trytes(trytes_buff + (index * NUM_TRYTES_MESSAGE), NUM_TRYTES_MESSAGE, transaction_message(curr_tx),
                         NUM_TRITS_MESSAGE, NUM_TRITS_MESSAGE);
    index++;
  }

  // trytes to string
  char str_buff[trytes_buff_size / 2 + trytes_buff_size % 2];
  trytes_to_ascii(trytes_buff, trytes_buff_size, str_buff);

  TEST_ASSERT_EQUAL_MEMORY(message, str_buff, strlen((char *)message));
  bundle_transactions_free(&bundle);
}

void test_bundle_transactions_message_short(void) { test_bundle_message(short_message); }

void test_bundle_transactions_message_long(void) { test_bundle_message(long_message); }

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_normalized_bundle);
  RUN_TEST(test_bundle_transactions_message_long);
  RUN_TEST(test_bundle_transactions_message_short);

  return UNITY_END();
}
