/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_GET_INCLUSION_STATES_H
#define CCLIENT_API_GET_INCLUSION_STATES_H

#include "cclient/http/http.h"
#include "cclient/request/get_inclusion_states.h"
#include "cclient/response/get_inclusion_states.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetches inclusion states of given list of transactions.
 *
 * <p>
 * Get the inclusion states of a set of transactions. This is for determining if a transaction was accepted and
 * confirmed by the network or not. You can search for multiple tips (and thus, milestones) to get past inclusion states
 * of transactions.
 * </p>
 * <p>
 * This API call returns a list of boolean values in the same order as the submitted transactions. Boolean values will
 * be <b>true</b> for confirmed transactions, otherwise <b>false</b>.
 * </p>
 *
 * @param[in] service client service
 * @param[in] req Transactions and Tips that you want to search for the inclusion state.
 * @param[out] res A list of inclusion state.
 * @return #retcode_t
 */
retcode_t iota_client_get_inclusion_states(iota_client_service_t const* const service,
                                           get_inclusion_states_req_t const* const req,
                                           get_inclusion_states_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_INCLUSION_STATES_H

/** @} */