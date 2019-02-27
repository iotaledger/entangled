/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "common/trinary/tryte_ascii.h"

static char const ascii[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris ac leo a "
    "massa porta tempus scelerisque in leo. Duis ligula dolor, condimentum vel "
    "ipsum et, accumsan viverra lacus.";

static tryte_t trytes[] =
    "VBCDFDTCADEAXCDDGDIDADEASCCD9DCDFDEAGDXCHDEAPCADTCHDQAEARCCDBDGDTCRCHDTCHD"
    "IDFDEAPCSCXCDDXCGDRCXCBDVCEATC9DXCHDSAEAWBPCIDFDXCGDEAPCRCEA9DTCCDEAPCEAAD"
    "PCGDGDPCEADDCDFDHDPCEAHDTCADDDIDGDEAGDRCTC9DTCFDXCGDEDIDTCEAXCBDEA9DTCCDSA"
    "EANBIDXCGDEA9DXCVCID9DPCEASCCD9DCDFDQAEARCCDBDSCXCADTCBDHDIDADEAJDTC9DEAXC"
    "DDGDIDADEATCHDQAEAPCRCRCIDADGDPCBDEAJDXCJDTCFDFDPCEA9DPCRCIDGDSA";

void test_ascii_to_trytes(void) {
  size_t size = strlen(ascii) * 2;
  tryte_t buffer[size];

  ascii_to_trytes(ascii, buffer);
  TEST_ASSERT_EQUAL_MEMORY(trytes, buffer, size);
}

void test_trytes_to_ascii(void) {
  size_t size = strlen((char *)trytes) / 2;
  char buffer[size];

  trytes_to_ascii(trytes, strlen((char *)trytes), buffer);
  TEST_ASSERT_EQUAL_MEMORY(ascii, buffer, size);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ascii_to_trytes);
  RUN_TEST(test_trytes_to_ascii);

  return UNITY_END();
}
