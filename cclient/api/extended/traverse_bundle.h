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
#ifndef CCLIENT_API_TRAVERSE_BUNDLE_H
#define CCLIENT_API_TRAVERSE_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetches the bundle of a given the <b>tail</b> transaction hash, by traversing through <i>trunk</i>
 * transaction. It does not validate the bundle.
 *
 * @param[in] serv Client service
 * @param[in] tail_hash Tail transaction hash
 * @param[out] bundle Bundle as list of transaction objects.
 * @param[out] trytes
 * @return #retcode_t
 */
retcode_t traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                          bundle_transactions_t* const bundle, hash8019_array_p trytes);

/**
 * @brief Wrapper function: Runs #traverse_bundle
 *
 * @param[in] serv Client service
 * @param[in] tail_hash Tail transaction hash.
 * @param[out] bundle Bundle as list of transaction objects.
 * @return #retcode_t
 */
retcode_t iota_client_traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                      bundle_transactions_t* const bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_TRAVERSE_BUNDLE_H

/** @} */