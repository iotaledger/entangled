/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup response
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H
#define CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H

#include "utils/containers/hash/hash243_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of get missing transactions
 *
 */
typedef struct get_missing_transactions_res_s {
  hash243_stack_t hashes; /*!< List of transaction hashes */
} get_missing_transactions_res_t;

/**
 * @brief Allocates a get missing transactions response.
 *
 * @return A pointer to the response.
 */
get_missing_transactions_res_t* get_missing_transactions_res_new();

/**
 * @brief Gets the size of the transaction list.
 *
 * @param[in] res The response.
 * @return The size of the transaction list.
 */
size_t get_missing_transactions_res_hash_num(get_missing_transactions_res_t* res);

/**
 * @brief Frees a get missing transactions response object.
 *
 * @param[in] res The response object.
 */
void get_missing_transactions_res_free(get_missing_transactions_res_t** res);

/**
 * @brief Adds a transaction hash to the list.
 *
 * @param[in] res The response object.
 * @param[in] hash A transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_missing_transactions_res_hashes_add(get_missing_transactions_res_t* const res,
                                                                flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_stack_push(&res->hashes, hash);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_MISSING_TRANSACTIONS_H

/** @} */