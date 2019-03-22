/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "common/trinary/add.h"
#include "common/trinary/trit_tryte.h"
#include "mam/prototype/mask.h"

void test_mask(void) {
  char* const payload =
      "AAMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESS"
      "AGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFOR"
      "YOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AM"
      "ESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9AMESSAGE"
      "FORYOU9AMESSAGEFORYOU9AMESSAGEFORYOU9MESSAGEFORYOU9";
  char* const auth_id = "MYMERKLEROOTHASH";
  size_t const payload_size = strlen(payload);
  size_t const auth_id_size = strlen(auth_id);
  trit_t payload_trits[3 * payload_size];
  trit_t cipher_trits[3 * payload_size];
  trit_t auth_id_trits[3 * auth_id_size];
  trytes_to_trits((tryte_t*)payload, payload_trits, payload_size);
  trytes_to_trits((tryte_t*)auth_id, auth_id_trits, auth_id_size);
  size_t index = 5;
  Curl c;
  c.type = CURL_P_27;

  add_assign(auth_id_trits, 3 * auth_id_size, index);

  curl_init(&c);
  curl_absorb(&c, auth_id_trits, 3 * auth_id_size);
  mask(cipher_trits, payload_trits, 3 * payload_size, &c);

  curl_reset(&c);
  curl_absorb(&c, auth_id_trits, 3 * auth_id_size);
  unmask(cipher_trits, cipher_trits, 3 * payload_size, &c);

  TEST_ASSERT_EQUAL_MEMORY(payload_trits, cipher_trits, 3 * payload_size);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_mask);

  return UNITY_END();
}
