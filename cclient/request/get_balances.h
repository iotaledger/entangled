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
#ifndef CCLIENT_REQUEST_GET_BALANCES_H
#define CCLIENT_REQUEST_GET_BALANCES_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get balances request
 *
 */
typedef struct {
  /**
   * The confirmation threshold between 0 and 100(inclusive).
   *  Should be set to 100 for getting balance by counting only confirmed
   * transactions.
   */
  uint8_t threshold;
  /**
   * List of addresses you want to get the confirmed balance for.
   */
  hash243_queue_t addresses;
  /**
   * List of hashes, if present calculate the balance of addresses
   * from the PoV of these transactions, can be used to chain bundles.
   */
  hash243_queue_t tips;
} get_balances_req_t;

/**
 * @brief Allocates a get balances request object.
 *
 * @return A pointer to the request.
 */
get_balances_req_t* get_balances_req_new();

/**
 * @brief Free a get balances request object.
 *
 * @param[in] req The request object.
 */
void get_balances_req_free(get_balances_req_t** req);

/**
 * @brief Add an address hash to a get balances request.
 *
 * @param[in] req The request object.
 * @param[in] hash An address hash.
 * @return #retcode_t
 */
static inline retcode_t get_balances_req_address_add(get_balances_req_t* const req, flex_trit_t const* const hash) {
  if (!req || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&req->addresses, hash);
}

/**
 * @brief Get an address by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the address list.
 * @return A pointer to an address hash.
 */
static inline flex_trit_t* get_balances_req_address_get(get_balances_req_t* const req, size_t index) {
  if (!req) {
    return NULL;
  }
  return hash243_queue_at(req->addresses, index);
}

/**
 * @brief Add a tip transaction to the request object.
 *
 * @param[in] req The request object.
 * @param[in] hash A tips transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_balances_req_tip_add(get_balances_req_t* const req, flex_trit_t const* const hash) {
  if (!req || !hash) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&req->tips, hash);
}

/**
 * @brief Get a tip transaction by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the tip transaction list.
 * @return A pointer to a tip transaction hash.
 */
static inline flex_trit_t* get_balances_req_tip_get(get_balances_req_t* const req, size_t index) {
  if (!req) {
    return NULL;
  }
  return hash243_queue_at(req->tips, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_BALANCES_H

/** @} */