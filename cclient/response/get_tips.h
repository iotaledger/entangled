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
#ifndef CCLIENT_RESPONSE_GET_TIPS_H
#define CCLIENT_RESPONSE_GET_TIPS_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get tips response.
 *
 */
typedef struct get_tips_res_s {
  hash243_stack_t hashes; /*!< Current tip transaction hashes */
} get_tips_res_t;

/**
 * @brief Allocates a get tips response.
 *
 * @return A pointer to the response object.
 */
get_tips_res_t* get_tips_res_new();

/**
 * @brief Gets the number of tip transactions.
 *
 * @param[in] res The response object.
 * @return Number of tip transactions.
 */
size_t get_tips_res_hash_num(get_tips_res_t* res);

/**
 * @brief Frees a get tips response.
 *
 * @param[in] res The response object.
 */
void get_tips_res_free(get_tips_res_t** res);

/**
 * @brief Adds a tip transaction hash to the response.
 *
 * @param[in] res The response object.
 * @param[in] hash A tip transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_tips_res_hashes_push(get_tips_res_t* res, flex_trit_t const* const hash) {
  if (!res || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_stack_push(&res->hashes, hash);
}

/**
 * @brief Removes a tip transaction from the response.
 *
 * @param[in] res The response object.
 * @param[in] buf A tip transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_tips_res_hashes_pop(get_tips_res_t* res, flex_trit_t* const buf) {
  if (!res || !buf) {
    return RC_NULL_PARAM;
  }
  memcpy(buf, hash243_stack_peek(res->hashes), FLEX_TRIT_SIZE_243);
  hash243_stack_pop(&res->hashes);
  return RC_OK;
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H

/** @} */
