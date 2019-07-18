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
#ifndef CCLIENT_REQUEST_ATTACH_TO_TANGLE_H
#define CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

#include "common/errors.h"
#include "common/model/transaction.h"
#include "utils/containers/hash/hash_array.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATTACH_TO_TANGLE_MAIN_MWM 14
#define ATTACH_TO_TANGLE_TEST_MWM 9

/**
 * @brief The data structure of the attach to tangle request.
 *
 */
typedef struct {
  /**
   * List of trytes (raw transaction data) to attach to the tangle.
   */
  hash8019_array_p trytes;
  /**
   * Min Weight Magnitude,Proof of Work intensity. The value for mainnet is 14 and for others is 9.
   */
  uint8_t mwm;
  /**
   * Trunk transaction to approve
   */
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
  /**
   * branch transaction to approve
   */
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
} attach_to_tangle_req_t;

/**
 * @brief Allocates an attach to tangle request.
 *
 * @return A pointer to the request object.
 */
attach_to_tangle_req_t* attach_to_tangle_req_new();

/**
 * @brief Free a request object.
 *
 * @param[in] req The request object to be freed.
 */
void attach_to_tangle_req_free(attach_to_tangle_req_t** req);

/**
 * @brief Set trunk, branch, and mwm to the request.
 *
 * @param[in] req The request object.
 * @param[in] trunk Trunk transaction hash.
 * @param[in] branch Branch transaction hash.
 * @param[in] mwm Minimum Weight Magnitude for Proof-of-Work.
 */
void attach_to_tangle_req_init(attach_to_tangle_req_t* req, flex_trit_t const* const trunk,
                               flex_trit_t const* const branch, uint8_t mwm);
/**
 * @brief Set transaction trytes to the request object.
 *
 * @param[in] req The request object.
 * @param[in] raw_trytes A transaction trytes.
 * @return #retcode_t
 */
retcode_t attach_to_tangle_req_trytes_add(attach_to_tangle_req_t* req, flex_trit_t const* const raw_trytes);

/**
 * @brief Get transaction trytes by index.
 *
 * @param[in] req The attach to tangle request object.
 * @param[in] index The index of transaction trytes list.
 * @return Null on failed, otherwise return a pointer of flex_trit_t.
 */
flex_trit_t* attach_to_tangle_req_trytes_get(attach_to_tangle_req_t* req, size_t index);

/**
 * @brief Get trunk hash from the request object.
 *
 * @param[in] req The attach to tangle request object.
 * @return The pointer of trunk transaction hash.
 */
static inline flex_trit_t* attach_to_tangle_req_trunk(attach_to_tangle_req_t* const req) {
  if (!req) return NULL;
  return req->trunk;
}

/**
 * @brief Get branch hash from the request object.
 *
 * @param[in] req The attach to tangle request object.
 * @return The pointer of branch transaction hash.
 */
static inline flex_trit_t* attach_to_tangle_req_branch(attach_to_tangle_req_t* const req) {
  if (!req) return NULL;
  return req->branch;
}

/**
 * @brief Get MWM from the request object.
 *
 * @param[in] req The attach to tangle request object.
 * @return The value of MWM.
 */
static inline uint8_t attach_to_tangle_req_mwm(attach_to_tangle_req_t* const req) {
  if (!req) return 0;
  return req->mwm;
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ATTACH_TO_TANGLE_H

/** @} */