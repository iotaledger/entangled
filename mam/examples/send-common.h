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

#define MAM_EXAMPLES_MSS_HEIGHT 3

/**
 * Creates and announce a channel (Header only)
 *
 * @param api - The API [in,out]
 * @param channel_id - A known channel ID [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id
 * @param new_channel_id - The new channel ID
 *
 * @return return code
 */
retcode_t mam_example_announce_channel(mam_api_t* const api, tryte_t const* const channel_id,
                                       bundle_transactions_t* const bundle, trit_t* const msg_id,
                                       tryte_t* const new_channel_id);

/**
 * Creates and announce a endpoint (Header only)
 *
 * @param api - The API [in,out]
 * @param channel_id - A known channel ID [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id
 * @param new_endpoint - The new endpoint
 *
 * @return return code
 */
retcode_t mam_example_announce_endpoint(mam_api_t* const api, tryte_t const* const channel_id,
                                        bundle_transactions_t* const bundle, trit_t* const msg_id,
                                        tryte_t* const new_endpoint_id);

/**
 * Writes a header only bundle on a channel
 *
 * @param api - The API [in,out]
 * @param channel_id - A known channel ID [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id
 *
 * @return return code
 */
retcode_t mam_example_write_header_on_channel(mam_api_t* const api, tryte_t const* const channel_id,
                                              bundle_transactions_t* const bundle, trit_t* const msg_id);

/**
 * Writes a header only bundle on an endpoint
 *
 * @param api - The API [in,out]
 * @param channel_id - A known channel ID [in]
 * @param endpoint_id - A known endpoint ID [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id
 *
 * @return return code
 */
retcode_t mam_example_write_header_on_endpoint(mam_api_t* const api, tryte_t const* const channel_id,
                                               tryte_t const* const endpoint_id, bundle_transactions_t* const bundle,
                                               trit_t* const msg_id);

/**
 * Writes a packet on a bundle
 *
 * @param api - The API [in,out]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param payload - The payload to write [in]
 * @param msg_id - The msg_id [in]
 * @param is_last_packet - True if this is the last packet to be written
 *
 * @return return code
 */
retcode_t mam_example_write_packet(mam_api_t* const api, bundle_transactions_t* const bundle, char const* const payload,
                                   trit_t const* const msg_id, bool is_last_packet);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_EXAMPLES_SEND_COMMON_H__
