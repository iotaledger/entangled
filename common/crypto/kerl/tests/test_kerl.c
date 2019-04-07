/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include <unity/unity.h>

#include "common/crypto/kerl/kerl.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/trits.h"

#define TRIT_LENGTH 243
#define TRYTE_LENGTH 81
#define BYTE_LENGTH 48
#define INT_LENGTH 12

void test_one_absorb(void) {
  const char expected[TRYTE_LENGTH] =
      "EJEAOOZYSAWFPZQESYDHZCGYNSTWXUMVJOVDWUNZJXDGWCLUFGIMZRMGCAZGKNPLBRLGUNYW"
      "KLJTYEAQX";

  char trytes[TRYTE_LENGTH] =
      "EMIDYNHBWMBCXVDEFOFWINXTERALUKYYPPHKP9JJFGJEIUY9MUDVNFZHMMWZUYUSWAIOWEVT"
      "HNWMHANBH";
  trit_t trits[TRIT_LENGTH];
  Kerl kerl;

  trytes_to_trits((tryte_t*)trytes, trits, TRYTE_LENGTH);

  kerl_init(&kerl);

  kerl_absorb(&kerl, trits, TRIT_LENGTH);
  kerl_squeeze(&kerl, trits, TRIT_LENGTH);

  trits_to_trytes(trits, (tryte_t*)trytes, TRIT_LENGTH);

  TEST_ASSERT_EQUAL_MEMORY(expected, trytes, TRYTE_LENGTH);
}

void test_multi_squeeze_multi_absorb(void) {
  const char* expected =
      "LUCKQVACOGBFYSPPVSSOXJEKNSQQRQKPZC9NXFSMQNRQCGGUL9OHVVKBDSKEQEBKXRNUJSRX"
      "YVHJTXBPDWQGNSCDCBAIRHAQCOWZEBSNHIJIGPZQITIBJQ9LNTDIBTCQ9EUWKHFLGFUVGGUW"
      "JONK9GBCDUIMAYMMQX";

  char trytes[TRYTE_LENGTH * 2] =
      "G9JYBOMPUXHYHKSNRNMMSSZCSHOFYOYNZRSZMAAYWDYEIMVVOGKPJBVBM9TD"
      "PULSFUNMTVXRKFIDOHUXXVYDLFSZYZTWQYTE9SPYYWYTXJYQ9IFGYOLZXWZB"
      "KWZN9QOOTBQMWMUBLEWUEEASRHRTNIQWJQNDWRYLCA";
  trit_t trits[TRIT_LENGTH * 2];
  Kerl kerl;

  trytes_to_trits((tryte_t*)trytes, trits, TRYTE_LENGTH * 2);

  kerl_init(&kerl);

  kerl_absorb(&kerl, trits, TRIT_LENGTH * 2);
  kerl_squeeze(&kerl, trits, TRIT_LENGTH * 2);

  trits_to_trytes(trits, (tryte_t*)trytes, TRIT_LENGTH * 2);

  TEST_ASSERT_EQUAL_MEMORY(expected, trytes, TRYTE_LENGTH * 2);
}

void test_multi_squeeze(void) {
  const char* expected =
      "G9JYBOMPUXHYHKSNRNMMSSZCSHOFYOYNZRSZMAAYWDYEIMVVOGKPJBVBM9TDPULSFUNMTVXR"
      "KFIDOHUXXVYDLFSZYZTWQYTE9SPYYWYTXJYQ9IFGYOLZXWZBKWZN9QOOTBQMWMUBLEWUEEAS"
      "RHRTNIQWJQNDWRYLCA";

  char trytes[TRYTE_LENGTH * 2] =
      "9MIDYNHBWMBCXVDEFOFWINXTERALUKYYPPHKP9JJFGJEIUY9MUDVNFZHMMWZUYUSWAIOWEVT"
      "HNWMHANBH";

  trit_t trits[TRIT_LENGTH * 2];
  Kerl kerl;

  trytes_to_trits((tryte_t*)trytes, trits, TRYTE_LENGTH);

  kerl_init(&kerl);

  kerl_absorb(&kerl, trits, TRIT_LENGTH);
  kerl_squeeze(&kerl, trits, TRIT_LENGTH * 2);

  trits_to_trytes(trits, (tryte_t*)trytes, TRIT_LENGTH * 2);

  TEST_ASSERT_EQUAL_MEMORY(expected, trytes, TRYTE_LENGTH * 2);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_one_absorb);
  RUN_TEST(test_multi_squeeze_multi_absorb);
  RUN_TEST(test_multi_squeeze);

  return UNITY_END();
}

#undef TRYTE_LENGTH
#undef TRIT_LENGTH
#undef BYTE_LENGTH
#undef INT_LENGTH
