/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/bundle.h"
#include "common/crypto/iss/v1/iss_kerl.h"
#include "common/helpers/sign.h"
#include "common/trinary/trit_long.h"
#include "common/trinary/tryte_long.h"

static UT_icd bundle_transactions_icd = {sizeof(iota_transaction_t), 0, 0, 0};

static void absorb_essence(Kerl *const kerl, flex_trit_t const *address, int64_t value, flex_trit_t const *obsolete_tag,
                           uint64_t timestamp, int64_t current_index, int64_t last_index, trit_t *const essence_trits) {
  memset(essence_trits, 0, NUM_TRITS_ESSENCE);

  trit_t *curr_pos = essence_trits;

  flex_trits_to_trits(curr_pos, NUM_TRITS_ADDRESS, address, NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  curr_pos = &curr_pos[NUM_TRITS_ADDRESS];
  long_to_trits(value, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_VALUE];
  flex_trits_to_trits(curr_pos, NUM_TRITS_OBSOLETE_TAG, obsolete_tag, NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
  curr_pos = &curr_pos[NUM_TRITS_OBSOLETE_TAG];
  long_to_trits(timestamp, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_TIMESTAMP];
  long_to_trits(current_index, curr_pos);
  curr_pos = &curr_pos[NUM_TRITS_CURRENT_INDEX];
  long_to_trits(last_index, curr_pos);
  // Absorb essence in kerl
  kerl_absorb(kerl, essence_trits, NUM_TRITS_ESSENCE);
}

/**
 * Validate signatures in a bundle,
 * it's a private method called by bundle_validate()
 *
 * @param {bundle_transactions_t} bundle - the bundle with transactions.
 * @param {trit_t} normalized_bundle - the bundle hash
 * @param {Kerl} address_kerl - the kerl instance for address calculation
 * @param {Kerl} sig_frag_kerl - the kerl instance for signature calculation
 * @param {bool} is_valid - the result of validation
 *
 * @return {retcode_t}
 */
static retcode_t validate_signatures(bundle_transactions_t const *const bundle, trit_t const *const normalized_bundle,
                                     Kerl *const address_kerl, Kerl *const sig_frag_kerl, bool *const is_valid) {
  iota_transaction_t *curr_tx = NULL, *curr_inp_tx = NULL;
  trit_t digested_sig_trits[NUM_TRITS_ADDRESS];
  trit_t digested_address[NUM_TRITS_ADDRESS];
  trit_t key[NUM_TRITS_SIGNATURE];
  flex_trit_t digest[FLEX_TRIT_SIZE_243];
  size_t offset = 0, next_offset = 0;

  *is_valid = true;

  for (curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0); curr_tx != NULL;) {
    if (transaction_value(curr_tx) >= 0) {
      curr_tx = (iota_transaction_t *)utarray_next(bundle, curr_tx);
      continue;
    }
    curr_inp_tx = curr_tx;
    offset = 0;
    next_offset = 0;
    kerl_init(address_kerl);
    do {
      kerl_init(sig_frag_kerl);
      next_offset = (offset + ISS_FRAGMENTS * RADIX - 1) % NUM_TRITS_HASH + 1;
      flex_trits_to_trits(key, NUM_TRITS_SIGNATURE, transaction_signature(curr_inp_tx), NUM_TRITS_SIGNATURE,
                          NUM_TRITS_SIGNATURE);
      iss_kerl_sig_digest(digested_sig_trits, &normalized_bundle[offset % NUM_TRITS_HASH], key, NUM_TRITS_SIGNATURE,
                          sig_frag_kerl);
      kerl_absorb(address_kerl, digested_sig_trits, NUM_TRITS_ADDRESS);
      curr_inp_tx = (iota_transaction_t *)utarray_next(bundle, curr_inp_tx);
      offset = next_offset;
    } while (curr_inp_tx != NULL &&
             memcmp(transaction_address(curr_inp_tx), transaction_address(curr_tx), FLEX_TRIT_SIZE_243) == 0 &&
             transaction_value(curr_inp_tx) == 0);

    kerl_squeeze(address_kerl, digested_address, NUM_TRITS_ADDRESS);
    flex_trits_from_trits(digest, NUM_TRITS_HASH, digested_address, NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);

    if (memcmp(digest, transaction_address(curr_tx), FLEX_TRIT_SIZE_243) != 0) {
      *is_valid = false;
      break;
    }
    curr_tx = curr_inp_tx;
  }

  return RC_OK;
}

void bundle_transactions_new(bundle_transactions_t **const bundle) { utarray_new(*bundle, &bundle_transactions_icd); }

void bundle_transactions_free(bundle_transactions_t **const bundle) {
  if (bundle && *bundle) {
    utarray_free(*bundle);
  }
  *bundle = NULL;
}

void bundle_transactions_add(bundle_transactions_t *const bundle, iota_transaction_t const *const transaction) {
  if (transaction) {
    utarray_push_back(bundle, transaction);
  }
}

void bundle_calculate_hash(bundle_transactions_t *bundle, Kerl *const kerl, flex_trit_t *out) {
  iota_transaction_t *curr_tx = NULL;
  trit_t essence_trits[NUM_TRITS_ESSENCE];
  trit_t bundle_hash_trits[NUM_TRITS_HASH];

  kerl_init(kerl);
  BUNDLE_FOREACH(bundle, curr_tx) {
    absorb_essence(kerl, transaction_address(curr_tx), transaction_value(curr_tx), transaction_obsolete_tag(curr_tx),
                   transaction_timestamp(curr_tx), transaction_current_index(curr_tx), transaction_last_index(curr_tx),
                   essence_trits);
  }

  // Squeeze kerl to get the bundle hash
  kerl_squeeze(kerl, bundle_hash_trits, NUM_TRITS_HASH);
  flex_trits_from_trits(out, NUM_TRITS_HASH, bundle_hash_trits, NUM_TRITS_HASH, NUM_TRITS_HASH);
}

iota_transaction_t *bundle_at(bundle_transactions_t *const bundle, size_t index) {
  if (index < utarray_len(bundle)) {
    return (iota_transaction_t *)(utarray_eltptr(bundle, index));
  }
  return NULL;
}

void bundle_finalize(bundle_transactions_t *bundle, Kerl *const kerl) {
  iota_transaction_t *curr_tx = NULL;
  bool valid_bundle = false;
  iota_transaction_t *head_tx = NULL;
  byte_t normalized_hash[HASH_LENGTH_TRYTE];
  trit_t increased_tag_trits[NUM_TRITS_TAG];
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];

  head_tx = (iota_transaction_t *)utarray_front(bundle);
  flex_trits_to_trits(increased_tag_trits, NUM_TRITS_TAG, transaction_obsolete_tag(head_tx), NUM_TRITS_TAG,
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
        flex_trits_from_trits(transaction_obsolete_tag(head_tx), NUM_TRITS_TAG, increased_tag_trits, NUM_TRITS_TAG,
                              NUM_TRITS_TAG);
        goto update_hash;
      }
    }
    valid_bundle = true;
    // update bundle_hash
    BUNDLE_FOREACH(bundle, curr_tx) { transaction_set_bundle(curr_tx, bundle_hash); }
  }
}

retcode_t bundle_validate(bundle_transactions_t *const bundle, bundle_status_t *const status) {
  retcode_t res = RC_OK;
  iota_transaction_t *curr_tx = NULL;
  uint64_t current_index = 0, last_index = 0;
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

    if (transaction_current_index(curr_tx) != current_index++ || transaction_last_index(curr_tx) != last_index) {
      *status = BUNDLE_INVALID_TX;
      break;
    }

    if (transaction_value(curr_tx) != 0 &&
        flex_trits_at(transaction_address(curr_tx), NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS - 1) != 0) {
      *status = BUNDLE_INVALID_INPUT_ADDRESS;
      break;
    }

    if (transaction_current_index(curr_tx) == last_index) {
      if (bundle_value != 0) {
        *status = BUNDLE_INVALID_VALUE;
        break;
      }

      bundle_calculate_hash(bundle, &shared_kerl1, bundle_hash_calculated);
      if (memcmp(bundle_hash, bundle_hash_calculated, FLEX_TRIT_SIZE_243) != 0) {
        *status = BUNDLE_INVALID_HASH;
        break;
      }

      normalize_flex_hash_to_trits(bundle_hash_calculated, normalized_bundle);

      res = validate_signatures(bundle, normalized_bundle, &shared_kerl1, &shared_kerl2, &valid_sig);
      if (res != RC_OK || !valid_sig) {
        *status = BUNDLE_INVALID_SIGNATURE;
        break;
      }
    }
    *status = BUNDLE_VALID;
  }
  return RC_OK;
}

void bundle_reset_indexes(bundle_transactions_t *const bundle) {
  size_t last_index = 0;
  size_t current_index = 0;
  iota_transaction_t *current_tx = NULL;

  if (bundle == NULL) {
    return;
  }

  last_index = bundle_transactions_size(bundle) - 1;

  BUNDLE_FOREACH(bundle, current_tx) {
    transaction_set_last_index(current_tx, last_index);
    transaction_set_current_index(current_tx, current_index);
    current_index++;
  }
}

void bundle_set_messages(bundle_transactions_t *bundle, signature_fragments_t *messages) {
  iota_transaction_t *tx = NULL;
  size_t index = 0;
  tryte_t **msg = NULL;
  size_t msg_len = 0;

  BUNDLE_FOREACH(bundle, tx) {
    msg = signature_fragments_at(messages, index);
    if (msg == NULL) {
      break;
    }
    msg_len = strlen((char *)*msg);
    // trytes to flex_trits
    flex_trits_from_trytes(tx->data.signature_or_message, NUM_TRITS_SIGNATURE, *msg, NUM_TRYTES_SIGNATURE, msg_len);
    tx->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
    index++;
  }
}

retcode_t bundle_sign(bundle_transactions_t *const bundle, flex_trit_t const *const seed, inputs_t const *const inputs,
                      Kerl *const kerl) {
  iota_transaction_t *tx = NULL;
  input_t *input = NULL;
  size_t curr_index = 0;
  flex_trit_t *signed_signature = NULL;

  bundle_reset_indexes(bundle);
  bundle_finalize(bundle, kerl);

  // find the inputs and get the corresponding private key and calculate the signature fragment
  BUNDLE_FOREACH(bundle, tx) {
    if (transaction_value(tx) < 0) {  // input transactions
      INPUTS_FOREACH(inputs->input_array, input) {
        if (memcmp(input->address, transaction_address(tx), FLEX_TRIT_SIZE_243) == 0) {
          if (curr_index > transaction_current_index(tx)) {
            continue;
          }
          signed_signature =
              iota_sign_signature_gen_flex_trits(seed, input->key_index, input->security, transaction_bundle(tx));
          if (signed_signature) {
            // for each security level add signature
            for (int i = 0; i < input->security; i++) {
              tx = bundle_at(bundle, curr_index + i);
              memcpy(tx->data.signature_or_message, signed_signature + (i * NUM_FLEX_TRITS_SIGNATURE),
                     NUM_FLEX_TRITS_MESSAGE);
              tx->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
            }
            curr_index += input->security;
            free(signed_signature);
            signed_signature = NULL;
          } else {
            return RC_COMMON_BUNDLE_SIGN;
          }
        }
      }
    } else {
      curr_index++;
    }
  }
  bundle_reset_indexes(bundle);
  return RC_OK;
}

#ifdef DEBUG
void bundle_dump(bundle_transactions_t *bundle) {
  iota_transaction_t *curr_tx = NULL;
  BUNDLE_FOREACH(bundle, curr_tx) { transaction_obj_dump(curr_tx); }
}
#endif
