/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/defs.h"
#include "common/trinary/trit_tryte.h"
#include "mam/troika/troika.h"

#define EXPECTED_OUTPUT                                                        \
  "IONONZOXVGTNGVEOCXPUIJU9LTJPI9LSJJTPABX9QQVPGMGZSVGOMTOGXJXRGMBODKKKBCNCXQ" \
  "UXWMCIIGAYEIOYWGHRLPFCRB9AJZZOQOUBUVWKW9I9CDHCEYPQZNTE9TZAKFRZLSLANRUPNVLO" \
  "LTIWHCKBAWSGTPMFCPW9IWV9HEJGQQUXBJTPXDFLKYIWSCHQIVPUBLDBARZLQNMNQASAPST9LP" \
  "SGDCIUGWJCIQQKJCLBGFU"

#define TEST_TROIKA_STATE_SIZE 729

static void troika_test(void) {
  trit_t state[TEST_TROIKA_STATE_SIZE];
  tryte_t output_trytes[TEST_TROIKA_STATE_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE];
  memset(state, 0, TEST_TROIKA_STATE_SIZE * sizeof(trit_t));
  mam_ftroika_transform(state, TEST_TROIKA_STATE_SIZE);
  trits_to_trytes(state, output_trytes, TEST_TROIKA_STATE_SIZE);
  TEST_ASSERT_EQUAL_MEMORY(EXPECTED_OUTPUT, output_trytes, TEST_TROIKA_STATE_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(troika_test);

  return UNITY_END();
}
