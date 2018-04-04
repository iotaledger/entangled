#include <stdio.h>
#include <string.h>
#include <unity/unity.h>

#include "common/kerl/converter.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/trits.h"

#define TRIT_LENGTH 243
#define TRYTE_LENGTH 81
#define BYTE_LENGTH 48
#define INT_LENGTH 12

void test_identity(tryte_t const* const trytes) {
  trit_t trits_in[TRIT_LENGTH] = {0};
  trit_t trits_out[TRIT_LENGTH] = {0};
  uint8_t bytes[BYTE_LENGTH] = {0};

  trytes_to_trits((tryte_t*)trytes, trits_in, TRYTE_LENGTH);
  trits_in[TRIT_LENGTH - 1] = 0;

  trits_to_bytes(trits_in, bytes);
  bytes_to_trits(bytes, trits_out);

  TEST_ASSERT_EQUAL_MEMORY(trits_in, trits_out, TRIT_LENGTH * sizeof(trit_t));
}

void bt(uint8_t const* const bytes_in, tryte_t const* const expected) {
  uint8_t bytes[BYTE_LENGTH] = {0};
  trit_t trits[TRIT_LENGTH] = {0};
  tryte_t trytes[TRYTE_LENGTH] = {0};

  memcpy(bytes, bytes_in, BYTE_LENGTH);
  bytes_to_trits(bytes, trits);
  trits_to_trytes(trits, trytes, TRIT_LENGTH);

  TEST_ASSERT_EQUAL_MEMORY(trytes, expected, TRYTE_LENGTH * sizeof(tryte_t));
}

void test_trits_all_bytes(void) {
  size_t i = 0;
  uint8_t bytes[BYTE_LENGTH] = {0};
  trit_t trits[TRIT_LENGTH] = {0};
  trit_t norm_trits[TRIT_LENGTH] = {0};

  tryte_t trytes[TRYTE_LENGTH] = {0};

  for (i = 0; i < 256; i++) {
    memset(bytes, i, BYTE_LENGTH);
    bytes_to_trits(bytes, trits);
    trits_to_trytes(trits, trytes, TRIT_LENGTH);
    trits_to_bytes(trits, bytes);
    bytes_to_trits(bytes, norm_trits);
    trits_to_trytes(norm_trits, trytes, TRIT_LENGTH);

    TEST_ASSERT_EQUAL_MEMORY(trits, norm_trits, TRIT_LENGTH * sizeof(trit_t));
  }
}

void test_trits_bytes_trits(void) {
  test_identity(
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999");
  test_identity(
      "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
      "ZZZZZZZZZ");
  test_identity(
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA");
  test_identity(
      "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
      "MMMMMMMMM");
  test_identity(
      "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
      "NNNNNNNNN");
  test_identity(
      "SCYLJDWIM9LIXCSLETSHLQOOFDKYOVFZLAHQYCCNMYHRTNIKBZRIRACFYPOWYNSOWDNXFZUG"
      "9OEOZPOTD");
}

void test_bytes_trits(void) {
  uint8_t bytes[BYTE_LENGTH];

  memset(bytes, 0, BYTE_LENGTH);
  bt(bytes,
     "9999999999999999999999999999999999999999999999999999999999999999999999999"
     "99999999");

  memset(bytes, 32, BYTE_LENGTH);
  bt(bytes,
     "FKMPGCDVPEYWPGTBVRFDVVRURXFHVDPGHBTEWHEBDCKOL9AVTISEFCWMDHTUBWBOFPSQERRDQ"
     "9MFGFINB");

  memset(bytes, 127, BYTE_LENGTH);
  bt(bytes,
     "LWWOYBGUIARWDZWMLWORYDNDMTEXKSWLW9HPHYPZW9GABECSCPBFOTVTBRUUNVPBVXYNGAVMK"
     "ONVGABBX");

  memset(bytes, 128, BYTE_LENGTH);
  bt(bytes,
     "NDDLBYTFRZIDWADNODLIBWMWNGVCPHDOD9SKSBKAD9TZYVXHXKYULGEGYIFFMEKYECBMTZENP"
     "LMETZYYC");

  memset(bytes, 220, BYTE_LENGTH);
  bt(bytes,
     "KEBSORVSJYXMUTVPUAYYBY9LXXSTMHTDQXFNSAFUPMKSRJWNUPXKSQH9ABNIRHWYUWVNNKYRA"
     "XGGGCUIY");

  memset(bytes, 255, BYTE_LENGTH);
  bt(bytes,
     "Z999999999999999999999999999999999999999999999999999999999999999999999999"
     "99999999");
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trits_all_bytes);
  RUN_TEST(test_trits_bytes_trits);
  RUN_TEST(test_bytes_trits);

  return UNITY_END();
}

#undef TRYTE_LENGTH
#undef TRIT_LENGTH
#undef BYTE_LENGTH
#undef INT_LENGTH
