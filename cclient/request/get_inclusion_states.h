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
#ifndef CCLIENT_REQUEST_GET_INCLUSION_STATES_H
#define CCLIENT_REQUEST_GET_INCLUSION_STATES_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get inclusion states request.
 *
 */
typedef struct {
  /**
   * List of transactions you want to get the inclusion state for.
   */
  hash243_queue_t transactions;
  /**
   * List of tips (including milestones) you want to search for the inclusion
   * state.
   */
  hash243_queue_t tips;

} get_inclusion_states_req_t;

/**
 * @brief Allocates a get inclusion states request object.
 *
 * @return A pointer to the request.
 */
get_inclusion_states_req_t* get_inclusion_states_req_new();

/**
 * @brief Free a get inclusion states request object.
 *
 * @param[in] req The request object.
 */
void get_inclusion_states_req_free(get_inclusion_states_req_t** req);

/**
 * @brief Add a transaction hash to the request object.
 *
 * @param[in] req The request object.
 * @param[in] hash A transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_inclusion_states_req_hash_add(get_inclusion_states_req_t* const req,
                                                          flex_trit_t const* const hash) {
  if (!req || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&req->transactions, hash);
}

/**
 * @brief Get a transation hash by index.
 *
 * @param[in] req The request object.
 * @param[in] index An index of the transaction hash list.
 * @return A pointer to a transaction hash.
 */
static inline flex_trit_t* get_inclusion_states_req_hash_get(get_inclusion_states_req_t* const req, size_t index) {
  if (!req) {
    return NULL;
  }
  return hash243_queue_at(req->transactions, index);
}

/**
 * @brief Add a tip transaction to the request object.
 *
 * @param[in] req The request object.
 * @param[in] hash A tip transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_inclusion_states_req_tip_add(get_inclusion_states_req_t* const req,
                                                         flex_trit_t const* const hash) {
  if (!req || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&req->tips, hash);
}

/**
 * @brief Get a tip hash by index.
 *
 * @param[in] req The request object.
 * @param[in] index An index of the tip transaction hash list.
 * @return A pointer to a tip transaction hash.
 */
static inline flex_trit_t* get_inclusion_states_req_tip_get(get_inclusion_states_req_t* const req, size_t index) {
  if (!req) {
    return NULL;
  }
  return hash243_queue_at(req->tips, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_INCLUSION_STATES_H

/** @} */