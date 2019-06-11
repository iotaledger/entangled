/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TEST_ADDR1 =
    (tryte_t *)"XUERGHWTYRTFUYKFKXURKHMFEVLOIFTTCNTXOGLDPCZ9CJLKHROOPGNAQYFJEPGK9OKUQROUECBAVNXRX";

static tryte_t const *const TEST_ADDR2 =
    (tryte_t *)"QMOR9KHSMPUZZNTTYCY9OAOHAZL9VSFNER9KYEFRVG9ATJGOHXFKMPZQBZTUSK99BVILMEAPUZRLZAQMD";

void example_were_addresses_spent_from(iota_client_service_t *s) {
  printf("[%s]\n", __FUNCTION__);
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  retcode_t ret_code = RC_OK;

  were_addresses_spent_from_req_t *addr_spent_req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_res_t *addr_spent_res = were_addresses_spent_from_res_new();
  if (!addr_spent_req || !addr_spent_res) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TEST_ADDR1, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret_code = were_addresses_spent_from_req_add(addr_spent_req, trits_243)) != RC_OK) {
    printf("Error: adding address_1 failed.\n");
    goto done;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TEST_ADDR2, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret_code = were_addresses_spent_from_req_add(addr_spent_req, trits_243)) != RC_OK) {
    printf("Error: adding address_2 failed.\n");
    goto done;
  }

  if ((ret_code = iota_client_were_addresses_spent_from(s, addr_spent_req, addr_spent_res)) == RC_OK) {
    for (size_t i = 0; i < were_addresses_spent_from_res_states_count(addr_spent_res); i++) {
      printf("%s: ", were_addresses_spent_from_res_states_at(addr_spent_res, i) ? "true" : "false");
      flex_trit_print(were_addresses_spent_from_req_get(addr_spent_req, i), NUM_TRITS_ADDRESS);
      printf("\n");
    }
  } else {
    printf("Error: %s\n", error_2_string(ret_code));
  }

done:
  were_addresses_spent_from_req_free(&addr_spent_req);
  were_addresses_spent_from_res_free(&addr_spent_res);
}
