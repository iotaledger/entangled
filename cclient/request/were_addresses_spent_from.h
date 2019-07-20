/*
 * Copyright (c) 2019 IOTA Stiftung
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
#ifndef CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the were addresses spent from request.
 *
 */
typedef struct were_addresses_spent_from_req_s {
  /* List of address hashes for which request should get */
  hash243_queue_t addresses;
} were_addresses_spent_from_req_t;

/**
 * @brief Allocates a were addresses spent from request object.
 *
 * @return A pointer to the request object.
 */
were_addresses_spent_from_req_t* were_addresses_spent_from_req_new();

/**
 * @brief Free a were addresses spent from request object.
 *
 * @param[in] req The request object.
 */
void were_addresses_spent_from_req_free(were_addresses_spent_from_req_t** const req);

/**
 * @brief Adding an address hash to the request.
 *
 * @param[in] req The request object.
 * @param[in] address An address hash.
 * @return #retcode_t
 */
static inline retcode_t were_addresses_spent_from_req_add(were_addresses_spent_from_req_t* const req,
                                                          flex_trit_t const* const address) {
  if (!req || !address) {
    return RC_NULL_PARAM;
  }
  return hash243_queue_push(&req->addresses, address);
}

/**
 * @brief Get an address hash by index.
 *
 * @param[in] req The request object.
 * @param[in] index An index of addresses list.
 * @return A pointer to an address hash.
 */
static inline flex_trit_t* were_addresses_spent_from_req_get(were_addresses_spent_from_req_t* const req, size_t index) {
  if (!req) {
    return NULL;
  }
  return hash243_queue_at(req->addresses, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_WERE_ADDRESSES_SPENT_FROM_H

/** @} */