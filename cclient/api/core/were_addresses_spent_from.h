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
#ifndef CCLIENT_API_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_API_WERE_ADDRESSES_SPENT_FROM_H

#include "cclient/http/http.h"
#include "cclient/request/were_addresses_spent_from.h"
#include "cclient/response/were_addresses_spent_from.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if a list of addresses was ever spent from, in the current epoch, or in previous epochs.
 *
 * @param[in] service client service
 * @param[in] req A list of address hash that you want to query for the status.
 * @param[in,out] res A list of address state.
 * @return #retcode_t
 */
retcode_t iota_client_were_addresses_spent_from(iota_client_service_t const* const service,
                                                were_addresses_spent_from_req_t const* const req,
                                                were_addresses_spent_from_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_WERE_ADDRESSES_SPENT_FROM_H

/** @} */