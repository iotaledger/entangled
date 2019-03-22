/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>
#include <unity/unity.h>

#include "common/crypto/kerl/converter.h"
#include "common/defs.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/trits.h"

void test_identity(tryte_t const* const trytes) {
  trit_t trits_in[HASH_LENGTH_TRIT] = {0};
  trit_t trits_out[HASH_LENGTH_TRIT] = {0};
  uint8_t bytes[HASH_LENGTH_BYTE] = {0};

  trytes_to_trits((tryte_t*)trytes, trits_in, HASH_LENGTH_TRYTE);
  trits_in[HASH_LENGTH_TRIT - 1] = 0;

  convert_trits_to_bytes(trits_in, bytes);
  convert_bytes_to_trits(bytes, trits_out);

  TEST_ASSERT_EQUAL_MEMORY(trits_in, trits_out, HASH_LENGTH_TRIT * sizeof(trit_t));
}

void bt(uint8_t const* const bytes_in, tryte_t const* const expected) {
  uint8_t bytes[HASH_LENGTH_BYTE] = {0};
  trit_t trits[HASH_LENGTH_TRIT] = {0};
  tryte_t trytes[HASH_LENGTH_TRYTE] = {0};

  memcpy(bytes, bytes_in, HASH_LENGTH_BYTE);
  convert_bytes_to_trits(bytes, trits);
  trits_to_trytes(trits, trytes, HASH_LENGTH_TRIT);

  TEST_ASSERT_EQUAL_MEMORY(trytes, expected, HASH_LENGTH_TRYTE * sizeof(tryte_t));
}

void test_trits_all_bytes(void) {
  size_t i = 0;
  uint8_t bytes[HASH_LENGTH_BYTE] = {0};
  trit_t trits[HASH_LENGTH_TRIT] = {0};
  trit_t norm_trits[HASH_LENGTH_TRIT] = {0};

  tryte_t trytes[HASH_LENGTH_TRYTE] = {0};

  for (i = 0; i < 256; i++) {
    memset(bytes, i, HASH_LENGTH_BYTE);
    convert_bytes_to_trits(bytes, trits);
    trits_to_trytes(trits, trytes, HASH_LENGTH_TRIT);
    convert_trits_to_bytes(trits, bytes);
    convert_bytes_to_trits(bytes, norm_trits);
    trits_to_trytes(norm_trits, trytes, HASH_LENGTH_TRIT);

    TEST_ASSERT_EQUAL_MEMORY(trits, norm_trits, HASH_LENGTH_TRIT * sizeof(trit_t));
  }
}

void test_trits_bytes_trits(void) {
  test_identity((
      tryte_t*)"999999999999999999999999999999999999999999999999999999999999999"
               "999999999"
               "999999999");
  test_identity((
      tryte_t*)"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
               "ZZZZZZZZZ"
               "ZZZZZZZZZ");
  test_identity((
      tryte_t*)"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
               "AAAAAAAAA"
               "AAAAAAAAA");
  test_identity((
      tryte_t*)"MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
               "MMMMMMMMM"
               "MMMMMMMMM");
  test_identity((
      tryte_t*)"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
               "NNNNNNNNN"
               "NNNNNNNNN");
  test_identity((
      tryte_t*)"SCYLJDWIM9LIXCSLETSHLQOOFDKYOVFZLAHQYCCNMYHRTNIKBZRIRACFYPOWYNS"
               "OWDNXFZUG"
               "9OEOZPOTD");
}

void test_bytes_trits(void) {
  uint8_t bytes[HASH_LENGTH_BYTE];

  memset(bytes, 0, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "9999999999999999999999999999999999999999999999999999999999999999999999999"
     "99999999");

  memset(bytes, 32, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "FKMPGCDVPEYWPGTBVRFDVVRURXFHVDPGHBTEWHEBDCKOL9AVTISEFCWMDHTUBWBOFPSQERRDQ"
     "9MFGFINB");

  memset(bytes, 127, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "LWWOYBGUIARWDZWMLWORYDNDMTEXKSWLW9HPHYPZW9GABECSCPBFOTVTBRUUNVPBVXYNGAVMK"
     "ONVGABBX");

  memset(bytes, 128, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "NDDLBYTFRZIDWADNODLIBWMWNGVCPHDOD9SKSBKAD9TZYVXHXKYULGEGYIFFMEKYECBMTZENP"
     "LMETZYYC");

  memset(bytes, 220, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "KEBSORVSJYXMUTVPUAYYBY9LXXSTMHTDQXFNSAFUPMKSRJWNUPXKSQH9ABNIRHWYUWVNNKYRA"
     "XGGGCUIY");

  memset(bytes, 255, HASH_LENGTH_BYTE);
  bt(bytes,
     (tryte_t*) "Z999999999999999999999999999999999999999999999999999999999999999999999999"
     "99999999");
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trits_all_bytes);
  RUN_TEST(test_trits_bytes_trits);
  RUN_TEST(test_bytes_trits);

  return UNITY_END();
}
