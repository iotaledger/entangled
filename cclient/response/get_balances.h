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
#ifndef CCLIENT_RESPONSE_GET_BALANCES_H
#define CCLIENT_RESPONSE_GET_BALANCES_H

#include "common/errors.h"
#include "utarray.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get balances response.
 *
 */
typedef struct {
  /**
   * The index of the milestone that confirmed the most recent balance.
   */
  uint64_t milestone_index;
  /**
   * Array of balances in the same order as the <b>addresses</b> parameters were passed to the endpoint.
   */
  UT_array* balances;
  /**
   * The referencing tips. If no <b>tips</b> parameter was passed to the endpoint, this field contains the hash of the
   * latest milestone that confirmed the balance
   */
  hash243_queue_t references;
} get_balances_res_t;

/**
 * @brief Allocates a get balances response object.
 *
 * @return A pointer to the response object.
 */
get_balances_res_t* get_balances_res_new();

/**
 * @brief Frees a get balances response object.
 *
 * @param[in] res The response object.
 */
void get_balances_res_free(get_balances_res_t** res);

/**
 * @brief Adds a reference hash to the response object.
 *
 * @param[in] res The response object.
 * @param[in] hash A reference hash.
 * @return #retcode_t
 */
static inline retcode_t get_balances_res_reference_add(get_balances_res_t* const res, flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&res->references, hash);
}

/**
 * @brief Gets a reference hash by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of reference list.
 * @return A pointer to a reference hash. NULL if index is invalid.
 */
static inline flex_trit_t* get_balances_res_reference_get(get_balances_res_t* const res, size_t index) {
  if (!res) {
    return NULL;
  }
  return hash243_queue_at(res->references, index);
}

/**
 * @brief Gets size of the balances element in the response.
 *
 * @param[in] res The response.
 * @return The size of the balances.
 */
size_t get_balances_res_balances_num(get_balances_res_t const* const res);

/**
 * @brief Gets a balance value by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of the balance list.
 * @return The balance to the corresponding index.
 */
uint64_t get_balances_res_balances_at(get_balances_res_t const* const res, size_t const index);

/**
 * @brief Adds a balance to the response object.
 *
 * @param[in] res The response object.
 * @param[in] value A balance value.
 * @return #retcode_t
 */
retcode_t get_balances_res_balances_add(get_balances_res_t* const res, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_BALANCES_H

/** @} */
