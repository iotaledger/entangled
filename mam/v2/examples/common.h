/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "common/trinary/trit_tryte.h"

#define DUMMY_SEED                                                             \
  "DUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEED99" \
  "9999999"
#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_MSS_DEPTH 1

find_transactions_req_t recv_example_req;

mam_psk_t const psk = {
    .id = {1,  0,  -1, -1, 0,  -1, -1, 0,  0,  1,  -1, 0,  1,  0,  0,  1,  1,
           1,  -1, 1,  1,  0,  1,  1,  0,  0,  -1, 1,  -1, -1, -1, -1, -1, -1,
           -1, 1,  -1, -1, 0,  -1, -1, 1,  0,  -1, -1, -1, 1,  1,  1,  0,  0,
           -1, 1,  -1, -1, -1, 0,  -1, 1,  -1, -1, -1, 1,  1,  -1, 1,  0,  0,
           1,  1,  1,  -1, -1, 0,  0,  -1, -1, 1,  0,  -1, 1},
    .key = {-1, 1,  -1, -1, 1,  -1, -1, 0,  0,  0,  -1, -1, 1,  1,  1,  -1, -1,
            -1, 0,  0,  0,  0,  -1, -1, 1,  1,  1,  0,  -1, -1, -1, 0,  0,  0,
            -1, -1, 1,  -1, 0,  0,  1,  0,  0,  -1, 1,  1,  0,  -1, 0,  0,  1,
            -1, 1,  0,  1,  0,  0,  -1, 1,  1,  -1, 1,  0,  -1, 0,  -1, 1,  -1,
            -1, -1, 0,  -1, -1, 0,  -1, -1, 0,  0,  -1, -1, 1,  -1, 0,  0,  -1,
            -1, -1, -1, 0,  -1, -1, -1, 1,  -1, -1, 1,  1,  1,  1,  1,  0,  1,
            0,  1,  -1, 0,  0,  1,  0,  1,  0,  0,  1,  0,  -1, 0,  1,  1,  0,
            0,  -1, -1, 1,  1,  0,  0,  1,  -1, 1,  1,  1,  0,  1,  1,  1,  0,
            0,  -1, -1, -1, -1, 1,  1,  1,  0,  0,  -1, 0,  1,  -1, 1,  1,  1,
            0,  0,  1,  -1, -1, 0,  -1, 1,  -1, 1,  0,  0,  1,  -1, 0,  1,  -1,
            0,  0,  1,  1,  1,  1,  1,  0,  0,  1,  -1, 1,  -1, 1,  0,  1,  1,
            1,  -1, 0,  0,  -1, 1,  1,  0,  -1, -1, 0,  0,  -1, 1,  0,  1,  -1,
            0,  0,  -1, 1,  -1, 1,  1,  1,  -1, 0,  1,  1,  0,  0,  -1, -1, -1,
            0,  0,  1,  0,  1,  0,  -1, 1,  -1, 0,  1,  0,  -1, 1,  1,  -1, -1,
            0,  0,  -1, 0,  -1}};
