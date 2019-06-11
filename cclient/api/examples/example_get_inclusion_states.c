/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TEST_HASH =
    (tryte_t *)"RVORZ9SIIP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT9XNMVKWYGVA";

static tryte_t const *const TEST_TIP =
    (tryte_t *)"99999999IP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT9XNMVKWYGVA";

void example_get_inclusion_states(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  retcode_t ret_code = RC_OK;

  get_inclusion_states_req_t *get_inclusion_req = get_inclusion_states_req_new();
  get_inclusion_states_res_t *get_inclusion_res = get_inclusion_states_res_new();
  if (!get_inclusion_req || !get_inclusion_res) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TEST_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret_code = get_inclusion_states_req_hash_add(get_inclusion_req, trits_243)) != RC_OK) {
    printf("Error: adding hash failed.\n");
    goto done;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TEST_TIP, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret_code = get_inclusion_states_req_tip_add(get_inclusion_req, trits_243)) != RC_OK) {
    printf("Error: adding tip failed.\n");
    goto done;
  }

  if ((ret_code = iota_client_get_inclusion_states(s, get_inclusion_req, get_inclusion_res)) == RC_OK) {
    for (size_t i = 0; i < get_inclusion_states_res_states_count(get_inclusion_res); i++) {
      printf("[%ld]:%s\n", i, get_inclusion_states_res_states_at(get_inclusion_res, i) ? "true" : "false");
    }
  } else {
    printf("Error: %s\n", error_2_string(ret_code));
  }

done:
  get_inclusion_states_req_free(&get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
}