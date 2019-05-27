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
#ifndef CCLIENT_REQUEST_STORE_TRANSACTIONS_H
#define CCLIENT_REQUEST_STORE_TRANSACTIONS_H

#include "common/errors.h"
#include "utils/containers/hash/hash_array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the store transactions request.
 *
 */
typedef struct store_transactions_req_s {
  hash8019_array_p trytes; /*!< List of transaction tytes to be stored */
} store_transactions_req_t;

/**
 * @brief Allocates a store transactions request.
 *
 * @return A pointer to the request object.
 */
store_transactions_req_t* store_transactions_req_new();

/**
 * @brief Free a store transactions request object.
 *
 * @param[in] req The request object.
 */
void store_transactions_req_free(store_transactions_req_t** const req);

/**
 * @brief Add a transaction trytes to the request.
 *
 * @param[in] req The request object.
 * @param[in] raw_trytes A transaction trytes.
 * @return #retcode_t
 */
retcode_t store_transactions_req_trytes_add(store_transactions_req_t* req, flex_trit_t const* const raw_trytes);

/**
 * @brief Get a transaction trytes by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of transaction list.
 * @return A pointer to the transaction trytes.
 */
flex_trit_t* store_transactions_req_trytes_get(store_transactions_req_t* req, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_STORE_TRANSACTIONS_H

/** @} */