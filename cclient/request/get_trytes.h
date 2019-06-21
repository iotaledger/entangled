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
#ifndef CCLIENT_REQUEST_GET_TRYTES_H
#define CCLIENT_REQUEST_GET_TRYTES_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get trytes request.
 *
 */
typedef struct get_trytes_req_s {
  /* List of transaction hashes for which request should get */
  hash243_queue_t hashes;
} get_trytes_req_t;

/**
 * @brief Allocates a get trytes request object.
 *
 * @return A pointer to the request object.
 */
get_trytes_req_t* get_trytes_req_new();

/**
 * @brief Free a get trytes request object.
 *
 * @param[in] req The request object.
 */
void get_trytes_req_free(get_trytes_req_t** const req);

/**
 * @brief Add a transaction hash to the request.
 *
 * @param[in] req The request object.
 * @param[in] hash A transaction hash.
 * @return #retcode_t
 */
static inline retcode_t get_trytes_req_hash_add(get_trytes_req_t* const req, flex_trit_t const* const hash) {
  return hash243_queue_push(&req->hashes, hash);
}

/**
 * @brief Get a transaction hash by index.
 *
 * @param[in] req The request object.
 * @param[in] index An index of transaction list.
 * @return A pointer to a transaction hash.
 */
static inline flex_trit_t* get_trytes_req_hash_get(get_trytes_req_t* const req, size_t index) {
  return hash243_queue_at(req->hashes, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_GET_TRYTES_H

/** @} */