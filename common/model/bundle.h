/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_BUNDLE_H__
#define __COMMON_MODEL_BUNDLE_H__

#include "common/kerl/kerl.h"
#include "common/model/transaction.h"
#include "common/sign/normalize.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UT_array bundle_transactions_t;
typedef UT_array bundle_hashes_t;

void bundle_transactions_new(bundle_transactions_t **const bundle);
void bundle_transactions_free(bundle_transactions_t **const bundle);
void bundle_transactions_add(bundle_transactions_t *const bundle,
                             iota_transaction_t const *const transaction);

static inline size_t bundle_transactions_size(
    bundle_transactions_t *const bundle) {
  if (bundle == NULL) {
    return 0;
  }

  return utarray_len(bundle);
}

void bundle_calculate_hash(bundle_transactions_t *bundle, Kerl *const kerl,
                           flex_trit_t *out);
void bundle_finalize(bundle_transactions_t *bundle, Kerl *const kerl);
#ifdef DEBUG
void bundle_dump(bundle_transactions_t *bundle);
#endif

#define BUNDLE_FOREACH(txs, tx)                                   \
  for (tx = (iota_transaction_t *)utarray_front(txs); tx != NULL; \
       tx = (iota_transaction_t *)utarray_next(txs, tx))

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_BUNDLE_H__
