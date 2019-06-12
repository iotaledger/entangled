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
#ifndef CCLIENT_API_CHECK_CONSISTENCY_H
#define CCLIENT_API_CHECK_CONSISTENCY_H

#include "cclient/http/http.h"
#include "cclient/request/check_consistency.h"
#include "cclient/response/check_consistency.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check the consistency of the subtangle formed by the provided tails.
 *
 * An inconsistent transaction will not be accepted by the network, and a reattachment is required by
 * #iota_client_replay_bundle.
 *
 * @param[in] service client service
 * @param[in] req The tails describing the subtangle.
 * @param[out] res Consistency state of given transaction or co-consistency of given transactions.
 * @return #retcode_t
 */
retcode_t iota_client_check_consistency(iota_client_service_t const* const service,
                                        check_consistency_req_t const* const req, check_consistency_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_CHECK_CONSISTENCY_H

/** @} */