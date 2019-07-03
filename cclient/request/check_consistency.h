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
#ifndef CCLIENT_REQUEST_CHECK_CONSISTENCY_H
#define CCLIENT_REQUEST_CHECK_CONSISTENCY_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the check consistency request.
 *
 */
typedef struct check_consistency_req_s {
  /**
   * Tail transaction hash (hash of transaction with <b>currentIndex=0</b>), or list of tail transaction hashes.
   */
  hash243_queue_t tails;
} check_consistency_req_t;

/**
 * @brief Allocates a check consistency request object.
 *
 * @return A pointer of the request object.
 */
check_consistency_req_t* check_consistency_req_new();

/**
 * @brief Free the request object.
 *
 * @param[in] req the request object.
 */
void check_consistency_req_free(check_consistency_req_t** req);

/**
 * @brief Add a tail transaction to the request.
 *
 * @param[in] req The request object.
 * @param[in] hash The hash of a tail transaction.
 * @return #retcode_t
 */
static inline retcode_t check_consistency_req_tails_add(check_consistency_req_t* const req,
                                                        flex_trit_t const* const hash) {
  return hash243_queue_push(&req->tails, hash);
}

/**
 * @brief Get the size of the tail transactions.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the tail transaction list.
 * @return A pointer to a tail transaction hash.
 */
static inline flex_trit_t* check_consistency_req_tails_get(check_consistency_req_t* const req, size_t index) {
  return hash243_queue_at(req->tails, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_CHECK_CONSISTENCY_H

/** @} */