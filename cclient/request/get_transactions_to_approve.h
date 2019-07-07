/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup request
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_REQUEST_GET_TRANSACTIONS_TO_APPROVE_H

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get transaction to approve.
 *
 */
typedef struct get_transactions_to_approve_req_s {
  uint32_t depth; /*!< Number of bundles to go back to determine the transactions for approval. */
  /**
   * Transaction hash from which to start the weighted random walk. Use this parameter to make sure the returned tip
   * transaction hashes approve a given reference transaction.
   */
  flex_trit_t* reference;
} get_transactions_to_approve_req_t;

/**
 * @brief Allocates a get transactions to approve request object.
 *
 * @return A pointer to the request.
 */
get_transactions_to_approve_req_t* get_transactions_to_approve_req_new();

/**
 * @brief Free a get transactions to approve request object.
 *
 * @param[in] req The request object.
 */
void get_transactions_to_approve_req_free(get_transactions_to_approve_req_t** const req);

/**
 * @brief Set the depth of the request.
 *
 * @param[in] req The request object.
 * @param[in] depth The depth value for Random Walk start from.
 */
void get_transactions_to_approve_req_set_depth(get_transactions_to_approve_req_t* const req, uint32_t const depth);

/**
 * @brief Set a reference transaction hash for the Random Walk start from.
 *
 * @param[in] req The request object.
 * @param[in] reference A reference transaction hash.
 * @return RC_OK or RC_OOM.
 */
retcode_t get_transactions_to_approve_req_set_reference(get_transactions_to_approve_req_t* const req,
                                                        flex_trit_t const* const reference);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H

/** @} */