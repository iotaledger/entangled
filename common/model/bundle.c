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

/**
 * Validate signatures in a bundle,
 * it's a private method called by bundle_validator()
 *
 * @param {bundle_transactions_t} bundle - the bundle with transactions.
 * @param {trit_t} normalized_bundle - the bundle hash
 * @param {Kerl} address_kerl - the kerl instance for address calculation
 * @param {Kerl} sig_frag_kerl - the kerl instance for signature calculation
 * @param {bool} is_valid - the result of validation
 *
 * @return {retcode_t}
 */
static retcode_t validate_signatures(bundle_transactions_t const *const bundle,
                                     trit_t const *const normalized_bundle,
                                     Kerl *const address_kerl,
                                     Kerl *const sig_frag_kerl,
                                     bool *const is_valid) {
  iota_transaction_t *curr_tx = NULL, *curr_inp_tx = NULL;
  trit_t digested_sig_trits[NUM_TRITS_ADDRESS];
  trit_t digested_address[NUM_TRITS_ADDRESS];
  trit_t key[NUM_TRITS_SIGNATURE];
  flex_trit_t digest[FLEX_TRIT_SIZE_243];
  size_t offset = 0, next_offset = 0;

  for (curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
       curr_tx != NULL;) {
    if (transaction_value(curr_tx) >= 0) {
      curr_tx = (iota_transaction_t *)utarray_next(bundle, curr_tx);
      continue;
    }
    curr_inp_tx = curr_tx;
    offset = 0;
    next_offset = 0;
    init_kerl(address_kerl);
    do {
      init_kerl(sig_frag_kerl);
      next_offset = (offset + ISS_FRAGMENTS * RADIX - 1) % NUM_TRITS_HASH + 1;
      flex_trits_to_trits(key, NUM_TRITS_SIGNATURE,
                          transaction_signature(curr_inp_tx),
                          NUM_TRITS_SIGNATURE, NUM_TRITS_SIGNATURE);
      iss_kerl_sig_digest(digested_sig_trits,
                          &normalized_bundle[offset % NUM_TRITS_HASH], key,
                          NUM_TRITS_SIGNATURE, sig_frag_kerl);
      kerl_absorb(address_kerl, digested_sig_trits, NUM_TRITS_ADDRESS);
      curr_inp_tx = (iota_transaction_t *)utarray_next(bundle, curr_inp_tx);
      offset = next_offset;
    } while (curr_inp_tx != NULL &&
             memcmp(transaction_address(curr_inp_tx),
                    transaction_address(curr_tx), FLEX_TRIT_SIZE_243) == 0 &&
             transaction_value(curr_inp_tx) == 0);

    kerl_squeeze(address_kerl, digested_address, NUM_TRITS_ADDRESS);
    flex_trits_from_trits(digest, NUM_TRITS_HASH, digested_address,
                          NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);

    if (memcmp(digest, transaction_address(curr_tx), FLEX_TRIT_SIZE_243) != 0) {
      *is_valid = false;
      break;
    }
    curr_tx = curr_inp_tx;
    *is_valid = true;
  }

  return RC_OK;
}

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

void bundle_calculate_hash(bundle_transactions_t *bundle, Kerl *const kerl,
                           flex_trit_t *out) {
  iota_transaction_t *curr_tx = NULL;
  trit_t essence_trits[NUM_TRITS_ESSENCE];
  trit_t bundle_hash_trits[NUM_TRITS_HASH];
  init_kerl(kerl);

  BUNDLE_FOREACH(bundle, curr_tx) {
    absorb_essence(
        kerl, transaction_address(curr_tx), transaction_value(curr_tx),
        transaction_obsolete_tag(curr_tx), transaction_timestamp(curr_tx),
        transaction_current_index(curr_tx), transaction_last_index(curr_tx),
        essence_trits);
  }

  // Squeeze kerl to get the bundle hash
  kerl_squeeze(kerl, bundle_hash_trits, NUM_TRITS_HASH);
  flex_trits_from_trits(out, NUM_TRITS_HASH, bundle_hash_trits, NUM_TRITS_HASH,
                        NUM_TRITS_HASH);
}

void bundle_finalize(bundle_transactions_t *bundle, Kerl *const kerl) {
  iota_transaction_t *curr_tx = NULL;
  bool valid_bundle = false;
  iota_transaction_t *head_tx = NULL;
  byte_t normalized_hash[HASH_LENGTH_TRYTE];
  trit_t increased_tag_trits[NUM_TRITS_TAG];
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];

  head_tx = (iota_transaction_t *)utarray_front(bundle);
  flex_trits_to_trits(increased_tag_trits, NUM_TRITS_TAG,
                      transaction_obsolete_tag(head_tx), NUM_TRITS_TAG,
                      NUM_TRITS_TAG);
  while (!valid_bundle) {
  update_hash:
    // bundle hash
    bundle_calculate_hash(bundle, kerl, bundle_hash);
    // normalize
    normalize_flex_hash(bundle_hash, normalized_hash);
    // checking 'M'
    for (int i = 0; i < HASH_LENGTH_TRYTE; i++) {
      if (normalized_hash[i] == 13) {
        // Insecure bundle. Increment Tag and recompute bundle hash.
        add_assign(increased_tag_trits, NUM_TRITS_TAG, 1);
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

retcode_t bundle_validator(bundle_transactions_t *const bundle,
                           bundle_status_t *const status) {
  retcode_t res = RC_OK;
  iota_transaction_t *curr_tx = NULL;
  int64_t index = 0, last_index = 0;
  int64_t bundle_value = 0, tx_value = 0;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  bool valid_sig = false;
  Kerl shared_kerl1, shared_kerl2;
  flex_trit_t bundle_hash_calculated[FLEX_TRIT_SIZE_243];
  trit_t normalized_bundle[HASH_LENGTH_TRIT];

  if (bundle == NULL) {
    *status = BUNDLE_NOT_INITIALIZED;
    return RC_NULL_PARAM;
  }

  curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  last_index = transaction_last_index(curr_tx);

  if (utarray_len(bundle) != last_index + 1) {
    *status = BUNDLE_INCOMPLETE;
    return res;
  }

  memcpy(bundle_hash, transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243);

  BUNDLE_FOREACH(bundle, curr_tx) {
    tx_value = transaction_value(curr_tx);
    if (llabs(tx_value) > MAX_IOTA_SUPPLY) {
      *status = BUNDLE_INVALID_VALUE;
      break;
    }

    bundle_value += tx_value;
    if (llabs(bundle_value) > MAX_IOTA_SUPPLY) {
      *status = BUNDLE_INVALID_VALUE;
      break;
    }

    if (transaction_current_index(curr_tx) != index++ ||
        transaction_last_index(curr_tx) != last_index) {
      *status = BUNDLE_INVALID_TX;
      break;
    }

    if (transaction_value(curr_tx) != 0 &&
        flex_trits_at(transaction_address(curr_tx), NUM_TRITS_ADDRESS,
                      NUM_TRITS_ADDRESS - 1) != 0) {
      *status = BUNDLE_INVALID_INPUT_ADDRESS;
      break;
    }

    if (transaction_current_index(curr_tx) == last_index) {
      if (bundle_value != 0) {
        *status = BUNDLE_INVALID_VALUE;
        break;
      }

      bundle_calculate_hash(bundle, &shared_kerl1, bundle_hash_calculated);
      if (memcmp(bundle_hash, bundle_hash_calculated, FLEX_TRIT_SIZE_243) !=
          0) {
        *status = BUNDLE_INVALID_HASH;
        break;
      }

      normalize_flex_hash_to_trits(bundle_hash_calculated, normalized_bundle);

      res = validate_signatures(bundle, normalized_bundle, &shared_kerl1,
                                &shared_kerl2, &valid_sig);
      if (res != RC_OK || !valid_sig) {
        *status = BUNDLE_INVALID_SIGNATURE;
        break;
      }
    }
    *status = BUNDLE_VALID;
  }
  return RC_OK;
}

#ifdef DEBUG
void bundle_dump(bundle_transactions_t *bundle) {
  iota_transaction_t *curr_tx = NULL;
  BUNDLE_FOREACH(bundle, curr_tx) { transaction_obj_dump(curr_tx); }
}
#endif
