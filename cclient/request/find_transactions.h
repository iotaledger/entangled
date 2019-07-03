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
#ifndef CCLIENT_REQUEST_FIND_TRANSACTIONS_H
#define CCLIENT_REQUEST_FIND_TRANSACTIONS_H

#include "common/errors.h"
#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/hash81_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the find transactions request
 *
 */
typedef struct find_transactions_req_s {
  /**
   * List of bundle hashes.
   * Transactions belonging to bundles will be returned
   */
  hash243_queue_t bundles;

  /**
   * List of addresses.
   * Transactions with any of these addresses as input/output will be returned
   */
  hash243_queue_t addresses;

  /**
   * List of transaction tags.
   * Transactions with any of these tags will be returned
   */
  hash81_queue_t tags;

  /**
   * List of approvees of a transaction.
   * Transactions which directly approve any of approvees will be returned
   */
  hash243_queue_t approvees;
} find_transactions_req_t;

/**
 * @brief Allocates a find transactions request object.
 *
 * @return A pointer to a request object.
 */
find_transactions_req_t* find_transactions_req_new();

/**
 * @brief Free a find transactions request object.
 *
 * @param[in] req The request object.
 */
void find_transactions_req_free(find_transactions_req_t** req);

/**
 * @brief Add a bundle hash to request object.
 *
 * @param[in] req The request object.
 * @param[in] hash A bundle hash.
 * @return #retcode_t
 */
static inline retcode_t find_transactions_req_bundle_add(find_transactions_req_t* const req,
                                                         flex_trit_t const* const hash) {
  return hash243_queue_push(&req->bundles, hash);
}

/**
 * @brief Get a bundle hash by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the bundle list.
 * @return A pointer to a bundle hash.
 */
static inline flex_trit_t* find_transactions_req_bundle_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(req->bundles, index);
}

/**
 * @brief Add an address hash to request object.
 *
 * @param[in] req The request object.
 * @param[in] hash An address hash.
 * @return #retcode_t
 */
static inline retcode_t find_transactions_req_address_add(find_transactions_req_t* const req,
                                                          flex_trit_t const* const hash) {
  return hash243_queue_push(&req->addresses, hash);
}

/**
 * @brief Get an address by index.
 *
 * @param[in] req The request object
 * @param[in] index The index of the address list.
 * @return A pointer to an address hash.
 */
static inline flex_trit_t* find_transactions_req_address_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(req->addresses, index);
}

/**
 * @brief Add a tag to request object.
 *
 * @param[in] req The request object.
 * @param[in] hash A tag hash.
 * @return #retcode_t
 */
static inline retcode_t find_transactions_req_tag_add(find_transactions_req_t* const req,
                                                      flex_trit_t const* const hash) {
  return hash81_queue_push(&req->tags, hash);
}

/**
 * @brief Get a tag by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the tag list.
 * @return A pointer to a tag.
 */
static inline flex_trit_t* find_transactions_req_tag_get(find_transactions_req_t* const req, size_t index) {
  return hash81_queue_at(req->tags, index);
}

/**
 * @brief Add an approvee to request object.
 *
 * @param[in] req The request object.
 * @param[in] hash An approvee hash.
 * @return #retcode_t
 */
static inline retcode_t find_transactions_req_approvee_add(find_transactions_req_t* const req,
                                                           flex_trit_t const* const hash) {
  return hash243_queue_push(&req->approvees, hash);
}

/**
 * @brief Get an approvee by index.
 *
 * @param[in] req The request object.
 * @param[in] index The index of the approvee list.
 * @return A pointer to an approvee hash.
 */
static inline flex_trit_t* find_transactions_req_approvee_get(find_transactions_req_t* const req, size_t index) {
  return hash243_queue_at(req->approvees, index);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H

/** @} */