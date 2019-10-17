/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup common_model
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __COMMON_MODEL_BUNDLE_H__
#define __COMMON_MODEL_BUNDLE_H__

#include "common/crypto/iss/normalize.h"
#include "common/crypto/kerl/kerl.h"
#include "common/errors.h"
#include "common/model/inputs.h"
#include "common/model/transaction.h"
#include "common/model/transfer.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IOTA_SUPPLY 2779530283277761LL

/**
 * @brief bundle validation status.
 *
 */
typedef enum bundle_status_e {
  BUNDLE_NOT_INITIALIZED = 0,
  BUNDLE_VALID,
  BUNDLE_EMPTY,
  BUNDLE_INCOMPLETE,
  BUNDLE_INVALID_TX,
  BUNDLE_INVALID_INPUT_ADDRESS,
  BUNDLE_INVALID_VALUE,
  BUNDLE_INVALID_HASH,
  BUNDLE_INVALID_SIGNATURE
} bundle_status_t;

typedef UT_array bundle_transactions_t;
typedef UT_array bundle_hashes_t;

/**
 * @brief Allocates a bundle object.
 *
 * @param[in] bundle A bundle object.
 */
void bundle_transactions_new(bundle_transactions_t **const bundle);

/**
 * @brief Frees a bundle object.
 *
 * @param[in] bundle A bundle object.
 */
void bundle_transactions_free(bundle_transactions_t **const bundle);

/**
 * @brief Adds a transaction to the bundle.
 *
 * @param[in] bundle The bundle object.
 * @param[in] transaction A transaction object.
 */
void bundle_transactions_add(bundle_transactions_t *const bundle, iota_transaction_t const *const transaction);

/**
 * @brief Gets a transaction from the bundle by index.
 *
 * @param[in] bundle The bundle object.
 * @param[in] index The index of a transaction.
 * @return #iota_transaction_t
 */
iota_transaction_t *bundle_at(bundle_transactions_t *const bundle, size_t index);

/**
 * @brief Gets the number of transactions in the bundle.
 *
 * @param[in] bundle The bundle object.
 * @return An number of transactions.
 */
static inline size_t bundle_transactions_size(bundle_transactions_t const *const bundle) {
  if (bundle == NULL) {
    return 0;
  }

  return utarray_len(bundle);
}

/**
 * @brief Clear the bundle_transactions_t object to empty.
 *
 * @param[in] bundle The pointer of bundle object.
 */
static inline void bundle_transactions_clear(bundle_transactions_t *bundle) { utarray_clear(bundle); }

/**
 * @brief Get bundle hash from a bundle transaction object
 *
 * @param[in] bundle The bundle transaction object
 * @return The bundle hash
 */
static inline flex_trit_t const *bundle_transactions_bundle_hash(bundle_transactions_t const *const bundle) {
  if (bundle) {
    if (utarray_len(bundle) > 0) {
      return transaction_bundle((iota_transaction_t *)(utarray_eltptr(bundle, 0)));
    }
  }
  return NULL;
}

/**
 * @brief Calculates bundle hash using Kerl.
 *
 * @param[in] bundle A bundle object.
 * @param[in] kerl A Kerl object.
 * @param[out] out The bundle hash.
 */
void bundle_calculate_hash(bundle_transactions_t *bundle, Kerl *const kerl, flex_trit_t *out);

/**
 * @brief Finalizes a bundle by calculating the bundle hash.
 *
 * @param[in] bundle A bundle object.
 * @param[in] kerl A Kerl object.
 */
void bundle_finalize(bundle_transactions_t *bundle, Kerl *const kerl);

/**
 * @brief Validates a bundle.
 *
 * @param[in] bundle A bundle object.
 * @param[out] status The status of the bundle.
 * @return #retcode_t
 */
retcode_t bundle_validate(bundle_transactions_t *const bundle, bundle_status_t *const status);

/**
 * @brief Re-calculates the current and last index of the bundle.
 *
 * @param[in] bundle A bundle object.
 */
void bundle_reset_indexes(bundle_transactions_t *const bundle);

/**
 * @brief Sets bundle message.
 *
 * @param[in] bundle A bundle object.
 * @param[in] messages A message for the bundle.
 */
void bundle_set_messages(bundle_transactions_t *bundle, signature_fragments_t *messages);

/**
 * @brief Adds signature to transactions in a bundle.
 *
 * @param[in] bundle A bundle object.
 * @param[in] seed The seed of inputs.
 * @param[in] inputs The inputs for the bundle.
 * @param[in] kerl A Kerl object.
 * @return #retcode_t
 */
retcode_t bundle_sign(bundle_transactions_t *const bundle, flex_trit_t const *const seed, inputs_t const *const inputs,
                      Kerl *const kerl);

#ifdef DEBUG
void bundle_dump(bundle_transactions_t *bundle);
#endif

/**
 * @brief The bundle iterator.
 *
 */
#define BUNDLE_FOREACH(txs, tx) \
  for (tx = (iota_transaction_t *)utarray_front(txs); tx != NULL; tx = (iota_transaction_t *)utarray_next(txs, tx))

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_BUNDLE_H__

/** @} */
