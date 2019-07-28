/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup ciri_api
 *
 * @{
 *
 * @file
 * @brief The IOTA API implementation
 *
 * https://docs.iota.org/docs/iri/0.1/references/api-reference
 *
 */
#ifndef __CIRI_API_API_H__
#define __CIRI_API_API_H__

#include "cclient/request/requests.h"
#include "cclient/response/responses.h"
#include "ciri/api/conf.h"
#include "ciri/core.h"
#include "common/errors.h"

#define API_ERROR_UNSYNCED_NODE "This operation cannot be executed: the node is unsynced"
#define API_ERROR_INVALID_URI_SCHEME "Invalid URI scheme"
#define API_ERROR_TAILS_BUNDLE_INVALID "Tails are not consistent (bundle is invalid)"
#define API_ERROR_TAILS_NOT_CONSISTENT \
  "Tails are not consistent (would lead to inconsistent ledger state or below max depth)"
#define API_ERROR_TAILS_NOT_SOLID "Tails are not solid (missing a referenced tx)"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_api_s {
  iota_api_conf_t conf;  //!< The API configuration
  core_t *core;          //!< Reference to a cIRI core
  uint8_t features;      //!< Enabled features of the cIRI instance
} iota_api_t;

/**
 * @brief Initializes an API
 *
 * @param[in] api   The API
 * @param[in] core  The core
 *
 * @return a status code
 */
retcode_t iota_api_init(iota_api_t *const api, core_t *const core);

/**
 * @brief Destroys an API
 *
 * @param[in] api The API
 *
 * @return a status code
 */
retcode_t iota_api_destroy(iota_api_t *const api);

/**
 * @brief Temporarily adds a list of neighbors to the node.
 *
 * Added neighbors will be removed after restarting cIRI.
 * Add the neighbors to the config file or supply them in the -n command line option if you want them to be permanent.
 *
 * The URI (Unique Resource Identification) for adding neighbors is: tcp://ip:port
 *
 * @param[in]   api   The API
 * @param[in]   req   The request
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_add_neighbors(iota_api_t const *const api, add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res, error_res_t **const error);

/**
 * @brief Prepares the specified transactions trytes for attachment to the Tangle by doing Proof of Work.
 *
 * You need to supply branchTransaction as well as trunkTransaction, they are the tips which you're going to validate
 * and reference with this transaction and are both obtainable by the getTransactionsToApprove API call.
 * The returned value is a different set of trytes which you can input into broadcastTransactions and
 * storeTransactions.
 * The last 243 trytes of the returned trytes consist of: trunkTransaction + branchTransaction + nonce.
 * These are valid trytes which are then accepted by the network.
 *
 * @param[in]   api   The API
 * @param[in]   req   The request
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_attach_to_tangle(iota_api_t const *const api, attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res, error_res_t **const error);

/**
 * @brief Broadcasts a list of transactions to all the node neighbors.
 *
 * The trytes to be used for this call should be valid and attached transaction trytes.
 * These trytes are returned by attachToTangle, or by doing proof of work somewhere else.
 *
 * @param[in]   api   The API
 * @param[in]   req   The request
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_broadcast_transactions(iota_api_t const *const api, broadcast_transactions_req_t const *const req,
                                          error_res_t **const error);

/**
 * @brief Checks the consistency of transactions.
 *
 * Error is returned if:
 * - Transaction does not exist
 * - Transaction is not a tail
 * False is returned if:
 * - Tails are not solid
 * - Tails bundles are not valid
 * - Tails would lead to inconsistent ledger
 * True is returned otherwise
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_check_consistency(iota_api_t const *const api, tangle_t *const tangle,
                                     check_consistency_req_t const *const req, check_consistency_res_t *const res,
                                     error_res_t **const error);

/**
 * @brief Finds the transactions which match the specified input and return.
 *
 * All input values are lists, for which a list of return values (transaction hashes), in the same order, is returned
 * for all individual elements.
 * The input fields can either be bundles, addresses, tags or approvees.
 * Using multiple of these input fields returns the intersection of the values.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_find_transactions(iota_api_t const *const api, tangle_t *const tangle,
                                     find_transactions_req_t const *const req, find_transactions_res_t *const res,
                                     error_res_t **const error);

/**
 * @brief Calculates the confirmed balances of addresses, as viewed by the specified tips.
 *
 * In case tips are not supplied, the balances are based on the latest confirmed milestone.
 * In addition to the balances, it also returns the referencing tips (or milestone), as well as the index with which the
 * confirmed balances were determined.
 * The balances are returned as a list in the same order as the addresses were provided as input.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_balances(iota_api_t const *const api, tangle_t *const tangle,
                                get_balances_req_t const *const req, get_balances_res_t *const res,
                                error_res_t **const error);

/**
 * @brief Gets the inclusion states of a set of transactions.
 *
 * This is for determining if a transaction was accepted and confirmed by the network or not.
 * You can search for multiple tips (and thus, milestones) to get past inclusion states of transactions.
 * This API call simply returns a list of boolean values in the same order as the submitted transactions.
 * Boolean values will be true or false whether a transaction is confirmed or not.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_inclusion_states(iota_api_t const *const api, tangle_t *const tangle,
                                        get_inclusion_states_req_t const *const req,
                                        get_inclusion_states_res_t *const res, error_res_t **const error);

/**
 * @brief Returns hashes of transactions currently missing from the node.
 *
 * @param[in]   api   The API
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_missing_transactions(iota_api_t const *const api, get_missing_transactions_res_t *const res,
                                            error_res_t **const error);

/**
 * @brief Returns the set of neighbors the node is connected with, as well as their activity statistics or counters.
 *
 * These numbers are reset after restarting cIRI.
 *
 * @param[in]   api   The API
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_neighbors(iota_api_t const *const api, get_neighbors_res_t *const res,
                                 error_res_t **const error);

/**
 * @brief Returns information about the node.
 *
 * @param[in]   api   The API
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_node_info(iota_api_t const *const api, get_node_info_res_t *const res,
                                 error_res_t **const error);

/**
 * @brief Returns tips currently known by the node.
 *
 * @param[in]   api   The API
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_tips(iota_api_t const *const api, get_tips_res_t *const res, error_res_t **const error);

/**
 * @brief Tips selection which returns trunkTransaction and branchTransaction.
 *
 * The input value depth determines how many milestones to go back to for finding the transactions to approve. The
 * higher the depth value, the more work has to do done confirming more transactions. If the depth is too
 * large (usually above 15, it depends on the node's configuration) an error will be returned.
 * The reference is an optional hash of a transaction to approve. If it can't be found at the specified depth
 * then an error will be returned.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_transactions_to_approve(iota_api_t const *const api, tangle_t *const tangle,
                                               get_transactions_to_approve_req_t const *const req,
                                               get_transactions_to_approve_res_t *const res, error_res_t **const error);

/**
 * @brief Returns the raw transaction data (trytes) of a specific transaction.
 *
 * These trytes can then be easily converted into the actual transaction object.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_get_trytes(iota_api_t const *const api, tangle_t *const tangle, get_trytes_req_t const *const req,
                              get_trytes_res_t *const res, error_res_t **const error);

/**
 * @brief Interrupts and completely aborts the attachToTangle process.
 *
 * @param[in]   api   The API
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_interrupt_attaching_to_tangle(iota_api_t const *const api, error_res_t **const error);

/**
 * @brief Temporarily removes a list of neighbors from the node.
 *
 * Removed neighbors will be added again after relaunching cIRI.
 * Remove the neighbors from the config file or make sure they aren't supplied in the -n command line option if you
 * don't want to keep them anymore.
 *
 * The URI (Unique Resource Identification) for removing neighbors is: tcp://ip:port
 *
 * @param[in]   api   The API
 * @param[in]   req   The request
 * @param[out]  res   The response
 * @param[out]  error An error response
 *
 * @return a status code
 */
retcode_t iota_api_remove_neighbors(iota_api_t const *const api, remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res, error_res_t **const error);

/**
 * @brief Stores transactions into the local storage.
 *
 * The trytes to be used for this call should be valid and attached transaction trytes.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in]     req     The request
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_store_transactions(iota_api_t const *const api, tangle_t *const tangle,
                                      store_transactions_req_t const *const req, error_res_t **const error);

/**
 * @brief Checks if a list of addresses was ever spent from, in the current epoch, or in previous epochs.
 *
 * If an address has a pending transaction, it is also marked as spent.
 *
 * @param[in]     api     The API
 * @param[in,out] tangle  A tangle connection
 * @param[in,out] sap     A spent addresses provider connection
 * @param[in]     req     The request
 * @param[out]    res     The response
 * @param[out]    error   An error response
 *
 * @return a status code
 */
retcode_t iota_api_were_addresses_spent_from(iota_api_t const *const api, tangle_t *const tangle,
                                             spent_addresses_provider_t *const sap,
                                             were_addresses_spent_from_req_t const *const req,
                                             were_addresses_spent_from_res_t *const res, error_res_t **const error);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
