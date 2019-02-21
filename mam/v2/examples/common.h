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

static tryte_t SENDER_SEED[81] =
    "SENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSENDERSEEDSEND"
    "ERSEED9";
#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_MSS_DEPTH 1
#define HOST "173.249.44.234"
#define PORT 14265

static tryte_t BUNDLE_HASH[NUM_TRYTES_BUNDLE] =
    "YKJJHKXLXWHIBNSHZTLEXOOHJXHKWIGGSIGDVKFSCKQZOISJXTN9JPCZGAH9KWJXIKKESSQICC"
    "DEWKZD9";

static tryte_t BUNDLE_HASH_PSK_EXAMPLE[NUM_TRYTES_BUNDLE] =
    "B9GDZPWNYVWCZOSXLUAHGIWHG9KONHBCKTANCXUCDDKSKOOELUJNSDXDPWBGBFCFKGIQGKWKO9"
    "AHSKNVD";
find_transactions_req_t recv_example_req;

const mam_psk_t psk = {
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