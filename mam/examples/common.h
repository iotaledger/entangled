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

#include "cclient/api/core/core_api.h"
#include "cclient/api/extended/extended_api.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/tryte_ascii.h"
#include "mam/api/api.h"

#define DUMMY_SEED                                                             \
  "DUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEEDDUMMYSEED99" \
  "9999999"
#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_MSS_DEPTH 1
#define MAM_FILE "/tmp/mam.bin"

#ifdef __cplusplus
extern "C" {
#endif

find_transactions_req_t recv_example_req;

extern mam_psk_t const psk;

retcode_t mam_example_channel_create(mam_api_t *const api, tryte_t *const channel_id);

retcode_t send_bundle(char const *const host, uint16_t const port, bundle_transactions_t *const bundle);
retcode_t receive_bundle(char const *const host, uint16_t const port, tryte_t const *const bundle_hash,
                         bundle_transactions_t *const bundle);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_EXAMPLES_COMMON_H__
