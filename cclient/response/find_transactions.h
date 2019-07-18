/*
 * Copyright (c) 2018 IOTA Stiftung
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
#ifndef CCLIENT_RESPONSE_FIND_TRANSACTIONS_H
#define CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of find transactions response.
 *
 */
typedef struct find_transactions_res {
  /**
   * The transaction hashes which are returned depend on your input.
   * - bundles : returns an array of transaction hashes that contain the given bundle hash.
   * - addresses : returns an array of transaction hashes that contain the given address in the address field.
   * - tags : returns an array of transaction hashes that contain the given value in the tag field.
   * - approvees : returns an array of transaction hashes that contain the given transactions in their branch
   * transaction or trunk transaction fields.
   *
   */
  hash243_queue_t hashes;
} find_transactions_res_t;

/**
 * @brief Allocates a find transactions response object.
 *
 * @return A pointer to find transactions response object.
 */
find_transactions_res_t* find_transactions_res_new();

/**
 * @brief Frees a find transactions response object.
 *
 * @param[in] res The response object.
 */
void find_transactions_res_free(find_transactions_res_t** res);

/**
 * @brief Adds a hash to the response.
 *
 * @param[in] res The response object.
 * @param[in] hash A hash.
 * @return #retcode_t
 */
static inline retcode_t find_transactions_res_hashes_add(find_transactions_res_t* const res,
                                                         flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&res->hashes, hash);
}

/**
 * @brief Gets a hash by index.
 *
 * @param[in] res The response object.
 * @param[in] index The index of hash list.
 * @return A pointer to the hash. NULL if the index is invalid.
 */
static inline flex_trit_t* find_transactions_res_hashes_get(find_transactions_res_t* const res, size_t index) {
  if (!res) {
    return NULL;
  }
  return hash243_queue_at(res->hashes, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_FIND_TRANSACTIONS_H

/** @} */