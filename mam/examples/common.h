/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_EXAMPLES_COMMON_H__
#define __MAM_EXAMPLES_COMMON_H__

#include <stdio.h>

#include "cclient/iota_client_extended_api.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/tryte_ascii.h"
#include "mam/api/api.h"

#define DUMMY_SEED                                                             \
  "DUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEED99" \
  "9999999"
#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_MSS_DEPTH 1

find_transactions_req_t recv_example_req;

extern mam_psk_t const psk;

void send_bundle(char const *const host, uint16_t const port,
                 bundle_transactions_t *const bundle);
void receive_bundle(char const *const host, uint16_t const port,
                    tryte_t const *const bundle_hash,
                    bundle_transactions_t *const bundle);

#endif  // __MAM_EXAMPLES_COMMON_H__
