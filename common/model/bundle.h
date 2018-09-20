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
                             iota_transaction_t transaction);

void calculate_bundle_hash(bundle_transactions_t *bundle, flex_trit_t *out);

#ifdef __cplusplus
}
#endif

#endif  //  __COMMON_MODEL_BUNDLE_H__
