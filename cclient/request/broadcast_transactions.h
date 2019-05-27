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
#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#include "common/errors.h"
#include "utils/containers/hash/hash_array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the broadcast transactions request.
 *
 */
typedef struct broadcast_transactions_req_s {
  hash8019_array_p trytes; /*!< List of valid transaction trytes to be broadcasted */
} broadcast_transactions_req_t;

/**
 * @brief Allocates broadcast transactions request
 *
 * @return A pointer to broadcast transactions request object.
 */
broadcast_transactions_req_t* broadcast_transactions_req_new();

/**
 * @brief Free the request object
 *
 * @param[in] req The request object
 */
void broadcast_transactions_req_free(broadcast_transactions_req_t** const req);

/**
 * @brief Add a transaction trytes to the request object.
 *
 * @param[in] req The request object.
 * @param[in] raw_trytes A transaction trytes.
 * @return #retcode_t
 */
retcode_t broadcast_transactions_req_trytes_add(broadcast_transactions_req_t* req, flex_trit_t const* const raw_trytes);

// get trytes from array.
/**
 * @brief Get a transaction trytes by index.
 *
 * @param[in] req The request object
 * @param[in] index The index of list.
 * @return A pointer to transaction trytes.
 */
flex_trit_t* broadcat_transactions_req_trytes_get(broadcast_transactions_req_t* req, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

/** @} */