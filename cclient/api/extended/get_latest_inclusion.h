/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_extended
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_GET_LATEST_INCLUSION_H
#define CCLIENT_API_GET_LATEST_INCLUSION_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets inclusion states.
 *
 * Fetches inclusion states of given transactions and a list of tips, by calling #iota_client_get_inclusion_states and
 * using <b>latestSolidSubtangleMilestone</b> as the tip.
 *
 * @param[in] serv client service
 * @param[in] transactions List of transaction hashes
 * @param[out] out_states List of inclusion states
 * @return #retcode_t
 */
retcode_t iota_client_get_latest_inclusion(iota_client_service_t const* const serv, hash243_queue_t const transactions,
                                           get_inclusion_states_res_t* out_states);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_LATEST_INCLUSION_H

/** @} */