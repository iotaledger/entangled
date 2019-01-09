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

static UT_icd bundle_transactions_icd = {sizeof(iota_transaction_t), 0, 0, 0};
static const trit_t one[NUM_TRITS_TAG] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void bundle_transactions_new(bundle_transactions_t **const bundle) {
  utarray_new(*bundle, &bundle_transactions_icd);
}

void bundle_transactions_free(bundle_transactions_t **const bundle) {
  if (bundle && *bundle) {
    utarray_clear(*bundle);
    utarray_free(*bundle);
  }
  *bundle = NULL;
}

void bundle_transactions_add(bundle_transactions_t *const bundle,
                             iota_transaction_t const *const transaction) {
  utarray_push_back(bundle, transaction);
}

void calculate_bundle_hash(bundle_transactions_t *bundle, flex_trit_t *out) {
  iota_transaction_t *curr_tx = NULL;
  trit_t essence_trits[NUM_TRITS_ESSENCE];
  Kerl kerl = {};
  init_kerl(&kerl);

  trit_t bundle_hash_trits[NUM_TRITS_HASH];

  BUNDLE_FOREACH(bundle, curr_tx) {
    absorb_essence(
        &kerl, transaction_address(curr_tx), transaction_value(curr_tx),
        transaction_obsolete_tag(curr_tx), transaction_timestamp(curr_tx),
        transaction_current_index(curr_tx), transaction_last_index(curr_tx),
        essence_trits);
  }

  // Squeeze kerl to get the bundle hash
  kerl_squeeze(&kerl, bundle_hash_trits, NUM_TRITS_HASH);
  flex_trits_from_trits(out, NUM_TRITS_HASH, bundle_hash_trits, NUM_TRITS_HASH,
                        NUM_TRITS_HASH);
}

void finalize_bundle(bundle_transactions_t *bundle) {
  iota_transaction_t *curr_tx = NULL;
  bool valid_bundle = false;
  iota_transaction_t *head_tx = NULL;
  byte_t normalized_hash[HASH_LENGTH_TRYTE];
  trit_t increased_tag_trits[NUM_TRITS_TAG];
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];

  Kerl kerl = {};
  init_kerl(&kerl);

  while (!valid_bundle) {
  update_hash:
    // bundle hash
    calculate_bundle_hash(bundle, bundle_hash);
    // normalize
    normalize_hash(bundle_hash, normalized_hash);
    kerl_reset(&kerl);
    // checking 'M'
    for (int i = 0; i < HASH_LENGTH_TRYTE; i++) {
      if (normalized_hash[i] == 13) {
        // Insecure bundle. Increment Tag and recompute bundle hash.
        head_tx = (iota_transaction_t *)utarray_front(bundle);
        flex_trits_to_trits(increased_tag_trits, NUM_TRITS_TAG,
                            transaction_obsolete_tag(head_tx), NUM_TRITS_TAG,
                            NUM_TRITS_TAG);
        add_trits(one, increased_tag_trits, NUM_TRITS_TAG);
        flex_trits_from_trits(transaction_obsolete_tag(head_tx), NUM_TRITS_TAG,
                              increased_tag_trits, NUM_TRITS_TAG,
                              NUM_TRITS_TAG);
        goto update_hash;
      }
    }
    valid_bundle = true;
    // update bundle_hash
    BUNDLE_FOREACH(bundle, curr_tx) {
      transaction_set_bundle(curr_tx, bundle_hash);
    }
  }
}

// for debugging
void dump_bundle(bundle_transactions_t *bundle) {
#ifdef DEBUG
  iota_transaction_t *curr_tx = NULL;
  BUNDLE_FOREACH(bundle, curr_tx) { transaction_obj_dump(curr_tx); }
#endif
}
