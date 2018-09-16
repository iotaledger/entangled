/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/bundle.h"
#include "common/kerl/kerl.h"
#include "common/model/transfer.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_long.h"

#define TRYTE_HASH_LENGTH 81
#define TRIT_HASH_LENGTH 243
#define MAX_TRYTE_VALUE 13
#define MIN_TRYTE_VALUE -13
#define TRYTE_WIDTH 3
#define NUMBER_OF_SECURITY_LEVELS 3
#define NORMALIZED_FRAGMENT_LENGTH 27

UT_icd bundle_transactions_icd = {sizeof(struct _iota_transaction), 0, 0, 0};
UT_icd bundle_hashes_icd = {FLEX_TRIT_SIZE_243, 0, 0, 0};

void bundle_transactions_new(bundle_transactions_t **const bundle) {
  utarray_new(*bundle, &bundle_transactions_icd);
}

void bundle_transactions_free(bundle_transactions_t **const bundle) {
  utarray_clear(*bundle);
  utarray_free(*bundle);
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
       curr_tx = (iota_transaction_t *)utarray_next(bundle, curr_tx)) {
    memset(essence_trits, 0, NUM_TRITS_ESSENCE);

    flex_trits_to_trits(essence_trits, NUM_TRITS_ADDRESS, curr_tx->address,
                        NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
    long_to_trits(curr_tx->value, &essence_trits[NUM_TRITS_ADDRESS]);
    flex_trits_to_trits(&essence_trits[NUM_TRITS_ADDRESS + NUM_TRITS_VALUE],
                        NUM_TRITS_OBSOLETE_TAG, curr_tx->obsolete_tag,
                        NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
    long_to_trits(
        curr_tx->timestamp,
        &essence_trits[NUM_TRITS_ADDRESS + NUM_TRITS_VALUE + NUM_TRITS_TAG]);
    long_to_trits(curr_tx->current_index,
                  &essence_trits[NUM_TRITS_ADDRESS + NUM_TRITS_VALUE +
                                 NUM_TRITS_TAG + NUM_TRITS_TIMESTAMP]);
    long_to_trits(
        curr_tx->last_index,
        &essence_trits[NUM_TRITS_ADDRESS + NUM_TRITS_VALUE + NUM_TRITS_TAG +
                       NUM_TRITS_TIMESTAMP + NUM_TRITS_CURRENT_INDEX]);
    // Absorb essence in kerl
    kerl_absorb(&kerl, essence_trits, NUM_TRITS_ESSENCE);
  }

  // Squeeze kerl to get the bundle hash
  kerl_squeeze(&kerl, bundle_hash_trits, NUM_TRITS_HASH);
  flex_trits_from_trits(out, NUM_TRITS_HASH, bundle_hash_trits, NUM_TRITS_HASH,
                        NUM_TRITS_HASH);
}

void normalize_bundle(flex_trit_t const *const bundle_hash,
                      byte_t *const normalized_bundle_hash) {
  trit_t bundle_hash_trits[NUM_TRITS_HASH];
  flex_trits_to_trits(bundle_hash_trits, NUM_TRITS_HASH, bundle_hash,
                      NUM_TRITS_HASH, NUM_TRITS_HASH);

  for (int i = 0; i < NUMBER_OF_SECURITY_LEVELS; i++) {
    int sum = 0;
    for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
         j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
      normalized_bundle_hash[j] = (bundle_hash_trits[j * TRYTE_WIDTH] +
                                   bundle_hash_trits[j * TRYTE_WIDTH + 1] * 3 +
                                   bundle_hash_trits[j * TRYTE_WIDTH + 2] * 9);
      sum += normalized_bundle_hash[j];
    }
    if (sum > 0) {
      while (sum-- > 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle_hash[j] > MIN_TRYTE_VALUE) {
            normalized_bundle_hash[j]--;
            break;
          }
        }
      }
    } else {
      while (sum++ < 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle_hash[j] < MAX_TRYTE_VALUE) {
            normalized_bundle_hash[j]++;
            break;
          }
        }
      }
    }
  }
}

void flex_normalize_bundle(flex_trit_t const *const bundle_hash,
                           byte_t *const normalized_bundle_hash) {
  trit_t bundle_hash_trits[TRIT_HASH_LENGTH];

  flex_trits_to_trits(bundle_hash_trits, TRIT_HASH_LENGTH, bundle_hash,
                      TRIT_HASH_LENGTH, TRIT_HASH_LENGTH);
  normalize_bundle(bundle_hash_trits, normalized_bundle_hash);
}
