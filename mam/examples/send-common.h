/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_EXAMPLES_SEND_COMMON_H__
#define __MAM_EXAMPLES_SEND_COMMON_H__

#include "mam/examples/common.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t mam_example_write_header(mam_api_t* const api,
                                   mam_channel_t* const channel,
                                   bundle_transactions_t* const bundle,
                                   trit_t* const msg_id);

retcode_t mam_example_write_packet(mam_api_t* const api,
                                   mam_channel_t* const channel,
                                   bundle_transactions_t* const bundle,
                                   char const* const payload,
                                   trit_t const* const msg_id,
                                   bool is_last_packet);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_EXAMPLES_SEND_COMMON_H__
