/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/bundle.h"
#include "common/model/transfer.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_long.h"

static UT_icd bundle_transactions_icd = {sizeof(struct _iota_transaction), 0, 0,
                                         0};

void bundle_transactions_new(bundle_transactions_t **const bundle) {
  utarray_new(*bundle, &bundle_transactions_icd);
}

void bundle_transactions_free(bundle_transactions_t **const bundle) {
  if (bundle && *bundle) {
    utarray_clear(*bundle);
    utarray_free(*bundle);
  }
}

void bundle_transactions_add(bundle_transactions_t *const bundle,
                             iota_transaction_t transaction) {
  utarray_push_back(bundle, transaction);
}

void calculate_bundle_hash(bundle_transactions_t *bundle, flex_trit_t *out) {
  iota_transaction_t curr_tx = NULL;
  trit_t essence_trits[NUM_TRITS_ESSENCE];
  Kerl kerl = {};
  init_kerl(&kerl);

  trit_t bundle_hash_trits[NUM_TRITS_HASH];

  for (curr_tx = (iota_transaction_t)utarray_front(bundle); curr_tx != NULL;
       curr_tx = (iota_transaction_t)utarray_next(bundle, curr_tx)) {
    absorb_essence(&kerl, curr_tx->address, curr_tx->value,
                   curr_tx->obsolete_tag, curr_tx->timestamp,
                   curr_tx->current_index, curr_tx->last_index, essence_trits);
  }

  // Squeeze kerl to get the bundle hash
  kerl_squeeze(&kerl, bundle_hash_trits, NUM_TRITS_HASH);
  flex_trits_from_trits(out, NUM_TRITS_HASH, bundle_hash_trits, NUM_TRITS_HASH,
                        NUM_TRITS_HASH);
}
