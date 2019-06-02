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
#ifndef CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H
#define CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H

#include "common/errors.h"
#include "utils/containers/hash/hash_array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of attach to tangle response.
 *
 */
typedef struct {
  /**
   * Transaction trytes that include a valid nonce field which you can input into #iota_client_broadcast_transactions
   * and #iota_client_store_transactions.
   *
   * The last 243 trytes of the return value consist of the following: <b>trunk transaction</b> + <b> branch
   * transaction</b> + <b>nonce</b>.
   */
  hash8019_array_p trytes;

} attach_to_tangle_res_t;

/**
 * @brief Allocates an attach to tangle response object.
 *
 * @return A pointer to the response object.
 */
attach_to_tangle_res_t* attach_to_tangle_res_new();

/**
 * @brief Adds a transaction trytes in a response object.
 *
 * @param[in] res The response object.
 * @param[in] trytes A transaction trytes.
 * @return #retcode_t
 */
retcode_t attach_to_tangle_res_trytes_add(attach_to_tangle_res_t* res, flex_trit_t const* const trytes);

/**
 * @brief Gets a transaction trytes by index.
 *
 * @param[in] res The response object.
 * @param[in] index The index of the list.
 * @return A pointer to a transaction trytes, NULL if not found.
 */
flex_trit_t* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res, int index);

/**
 * @brief Gets the size of the transaction list.
 *
 * @param[in] res The response object.
 * @return The size of the list.
 */
size_t attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res);

/**
 * @brief Frees an attach to tangle response object.
 *
 * @param[in] res the response object.
 */
void attach_to_tangle_res_free(attach_to_tangle_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H

/** @} */