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
#ifndef CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get transactions to approve response.
 *
 */
typedef struct get_transactions_to_approve_res_s {
  flex_trit_t branch[FLEX_TRIT_SIZE_243]; /*!< The valid branch transaction hash */
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];  /*!< The valid trunk transaction hash */
} get_transactions_to_approve_res_t;

/**
 * @brief Allocates a get transactions to approve response.
 *
 * @return The response object.
 */
get_transactions_to_approve_res_t* get_transactions_to_approve_res_new();

/**
 * @brief Frees a get transactions to approve response.
 *
 * @param[in] res The response object.
 */
void get_transactions_to_approve_res_free(get_transactions_to_approve_res_t** const res);

/**
 * @brief Sets the branch transaction to the response.
 *
 * @param[in] res The response object.
 * @param[in] branch A valid branch transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_transactions_to_approve_res_set_branch(get_transactions_to_approve_res_t* const res,
                                                                   flex_trit_t const* const branch) {
  if (!res || !branch) {
    return RC_NULL_PARAM;
  }
  memcpy(res->branch, branch, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

/**
 * @brief Gets the branch transaction from the response.
 *
 * @param[in] res The response object.
 * @return A pointer to the branch transaction hash.
 */
static inline flex_trit_t const* get_transactions_to_approve_res_branch(
    get_transactions_to_approve_res_t const* const res) {
  if (!res) {
    return NULL;
  }
  return res->branch;
}

/**
 * @brief Sets the trunk transaction to the response.
 *
 * @param[in] res The response object.
 * @param[in] trunk A valid trunk transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_transactions_to_approve_res_set_trunk(get_transactions_to_approve_res_t* const res,
                                                                  flex_trit_t const* const trunk) {
  if (!res || !trunk) {
    return RC_NULL_PARAM;
  }
  memcpy(res->trunk, trunk, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

/**
 * @brief Gets the trunk transaction from the response.
 *
 * @param[in] res The response object.
 * @return A pointer to the trunk transaction hash.
 */
static inline flex_trit_t const* get_transactions_to_approve_res_trunk(
    get_transactions_to_approve_res_t const* const res) {
  if (!res) {
    return NULL;
  }
  return res->trunk;
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

/** @} */
