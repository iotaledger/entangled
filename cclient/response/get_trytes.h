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
#ifndef CCLIENT_RESPONSE_GET_TRYTES_H
#define CCLIENT_RESPONSE_GET_TRYTES_H

#include "common/errors.h"
#include "utils/containers/hash/hash8019_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get trytes response.
 *
 */
typedef struct get_trytes_res_s {
  /*
   * The raw transaction data (trytes) of a specific transaction. These trytes can then be easily converted into the
   * actual transaction object.
   */
  hash8019_queue_t trytes;
} get_trytes_res_t;

/**
 * @brief Allocates a get trytes response.
 *
 * @return A pointer to the response object.
 */
get_trytes_res_t* get_trytes_res_new();

/**
 * @brief Frees a get trytes response.
 *
 * @param[in] res The response object.
 */
void get_trytes_res_free(get_trytes_res_t** const res);

/**
 * @brief Adds a raw transaction to the response.
 *
 * @param[in] res The response object.
 * @param[in] hash A raw transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_trytes_res_trytes_add(get_trytes_res_t* const res, flex_trit_t const* const hash) {
  return hash8019_queue_push(&res->trytes, hash);
}

/**
 * @brief Gets a raw transaction by index.
 *
 * @param[in] res The response object.
 * @param[in] index An index of the transaction list.
 * @return A pointer to a raw transaction hash.
 */
static inline flex_trit_t* get_trytes_res_trytes_get(get_trytes_res_t* const res, size_t index) {
  return hash8019_queue_at(res->trytes, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRYTES_H

/** @} */