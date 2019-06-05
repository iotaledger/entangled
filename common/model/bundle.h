/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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

void bundle_transactions_new(bundle_transactions_t **const bundle);
void bundle_transactions_free(bundle_transactions_t **const bundle);
void bundle_transactions_add(bundle_transactions_t *const bundle, iota_transaction_t const *const transaction);
iota_transaction_t *bundle_at(bundle_transactions_t *const bundle, size_t index);

static inline size_t bundle_transactions_size(bundle_transactions_t const *const bundle) {
  if (bundle == NULL) {
    return 0;
  }

  return utarray_len(bundle);
}

/**
 * @brief Get bundle hash from a bundle transaction object
 *
 * @param bundle The bundle transaction object
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

void bundle_calculate_hash(bundle_transactions_t *bundle, Kerl *const kerl, flex_trit_t *out);
void bundle_finalize(bundle_transactions_t *bundle, Kerl *const kerl);
retcode_t bundle_validate(bundle_transactions_t *const bundle, bundle_status_t *const status);

void bundle_reset_indexes(bundle_transactions_t *const bundle);

void bundle_set_messages(bundle_transactions_t *bundle, signature_fragments_t *messages);
retcode_t bundle_sign(bundle_transactions_t *const bundle, flex_trit_t const *const seed, inputs_t const *const inputs,
                      Kerl *const kerl);

#ifdef DEBUG
void bundle_dump(bundle_transactions_t *bundle);
#endif

#define BUNDLE_FOREACH(txs, tx) \
  for (tx = (iota_transaction_t *)utarray_front(txs); tx != NULL; tx = (iota_transaction_t *)utarray_next(txs, tx))

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_BUNDLE_H__
