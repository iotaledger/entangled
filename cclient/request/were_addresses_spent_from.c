/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "were_addresses_spent_from.h"

were_addresses_spent_from_req_t* were_addresses_spent_from_req_new() {
  were_addresses_spent_from_req_t* addresses = NULL;
  utarray_new(addresses, &ut_str_icd);
  return addresses;
}

void were_addresses_spent_from_req_add(
    were_addresses_spent_from_req_t* addresses, const char* addr) {
  utarray_push_back(addresses, &addr);
}

void were_addresses_spent_from_req_free(were_addresses_spent_from_req_t* ut) {
  utarray_free(ut);
}