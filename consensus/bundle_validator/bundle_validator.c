/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/bundle_validator/bundle_validator.h"
#include "common/helpers/sign.h"
#include "common/kerl/kerl.h"
#include "common/trinary/trit_long.h"
#include "consensus/defs.h"
#include "math.h"
#include "utils/logger_helper.h"

#define BUNDLE_VALIDATOR_ID "bundle_validator_id"

static retcode_t load_bundle_transactions(const tangle_t* const tangle,
                                          trit_array_p tail_hash,
                                          bundle_transactions_t* bundle);

static retcode_t validate_signature(bundle_transactions_t* bundle,
                                    trit_t* normalized_bundle, bool* is_valid);

retcode_t bundle_validate(const tangle_t* const tangle, trit_array_p tail_hash,
                          bundle_transactions_t* bundle, bool* is_valid) {
  retcode_t res = RC_OK;
  iota_transaction_t curr_tx;
  iota_transaction_t curr_inp_tx;

  if (bundle == NULL) {
    log_error(BUNDLE_VALIDATOR_ID, "Failed in memory allocation\n");
    return RC_CONSENSUS_OOM;
  }

  res = load_bundle_transactions(tangle, tail_hash, bundle);

  if (res || (utarray_len(bundle) == 0)) {
    log_error(BUNDLE_VALIDATOR_ID, "Failed loading transactions for tail\n");
    return res;
  }

  size_t i = 0;
  curr_tx = (iota_transaction_t)utarray_eltptr(bundle, 0);
  size_t last_index = curr_tx->last_index;
  int64_t bundle_value = 0;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);

  for (; curr_tx != NULL;
       curr_tx = (iota_transaction_t)utarray_next(bundle, curr_tx)) {
    bundle_value += curr_tx->value;
    if (curr_tx->current_index != i++ || curr_tx->last_index != last_index ||
        llabs(bundle_value) > IOTA_SUPPLY) {
      log_error(BUNDLE_VALIDATOR_ID, "Invalid transaction in bundle\n");
      *is_valid = false;
      break;
    }

    if (curr_tx->value != 0 &&
        flex_trits_at(curr_tx->address, NUM_TRITS_ADDRESS,
                      NUM_TRITS_HASH - 1) != 0) {
      log_error(BUNDLE_VALIDATOR_ID, "Invalid input address\n");
      *is_valid = false;
      break;
    }

    if (curr_tx->current_index == last_index && bundle_value != 0) {
      log_error(BUNDLE_VALIDATOR_ID, "Bundle value is not zero\n");
      *is_valid = false;
      break;
    }

    if (curr_tx->current_index == last_index) {
      flex_trit_t bundle_flex_trits[FLEX_TRIT_SIZE_243];
      trit_t bundle_trits[NUM_TRITS_HASH];
      trit_t normalized_bundle_trits[NUM_TRITS_HASH];
      byte_t normalized_bundle_bytes[NUM_TRYTES_HASH];

      *is_valid = true;
      calculate_bundle_hash(bundle, bundle_trits);
      flex_trits_from_trits(bundle_flex_trits, NUM_TRITS_HASH, bundle_trits,
                            NUM_TRITS_HASH, NUM_TRITS_HASH);
      if (memcmp(bundle_hash, bundle_flex_trits, FLEX_TRIT_SIZE_243) != 0) {
        log_error(BUNDLE_VALIDATOR_ID,
                  "Bundle hash provided differs from calculated \n");
        *is_valid = false;
        break;
      }

      normalize_bundle(bundle_trits, normalized_bundle_bytes);
      for (int c = 0; c < NUM_TRYTES_HASH; ++c) {
        long_to_trits(normalized_bundle_bytes[c],
                      &normalized_bundle_trits[c * RADIX]);
      }

      res = validate_signature(bundle, normalized_bundle_trits, is_valid);
      if (res || !(*is_valid)) {
        log_error(BUNDLE_VALIDATOR_ID, "Invalid signature.\n");
        break;
      }
    }
  }

  return RC_OK;
}

retcode_t load_bundle_transactions(const tangle_t* const tangle,
                                   trit_array_p tail_hash,
                                   bundle_transactions_t* bundle) {
  retcode_t res = RC_OK;
  iota_stor_pack_t pack;
  pack.capacity = 1;
  pack.num_loaded = 0;
  pack.insufficient_capacity = false;
  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx = &curr_tx_s;
  pack.models = (void**)(&curr_tx);
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  flex_trit_t curr_tx_trunk_trits[FLEX_TRIT_SIZE_243];

  res = iota_tangle_transaction_load(tangle, TRANSACTION_COL_HASH, tail_hash,
                                     &pack);

  if (res != RC_OK || pack.num_loaded == 0) {
    log_error(BUNDLE_VALIDATOR_ID,
              "No transactions were loaded for the provided tail hash\n");
    return res;
  }

  size_t last_index = curr_tx->last_index;
  size_t curr_index = 0;
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);
  trit_array_t curr_tx_trunk;
  curr_tx_trunk.dynamic = 0;
  curr_tx_trunk.trits = curr_tx_trunk_trits;
  while (curr_index < last_index &&
         memcmp(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243) == 0) {
    bundle_transactions_add(bundle, curr_tx);
    pack.num_loaded = 0;
    pack.insufficient_capacity = false;
    trit_array_set_trits(&curr_tx_trunk, curr_tx->trunk, NUM_TRITS_HASH);
    res = iota_tangle_transaction_load(tangle, TRANSACTION_COL_HASH,
                                       &curr_tx_trunk, &pack);

    if (res != RC_OK || pack.num_loaded == 0) {
      log_error(BUNDLE_VALIDATOR_ID,
                "No transactions were loaded for the provided tail hash\n");
      return res;
    }
  }

  return RC_OK;
}

retcode_t validate_signature(bundle_transactions_t* bundle,
                             trit_t* normalized_bundle, bool* is_valid) {
  retcode_t res = RC_OK;
  iota_transaction_t curr_tx;
  iota_transaction_t curr_inp_tx;
  Kerl address_kerl;
  Kerl sig_frag_kerl;
  trit_t digested_sig_trits[NUM_TRITS_ADDRESS];
  trit_t digested_address[NUM_TRITS_ADDRESS];
  trit_t tx_address_trits[NUM_TRITS_ADDRESS];
  trit_t key[NUM_TRITS_SIGNATURE];
  size_t i = 0;

  init_kerl(&address_kerl);
  init_kerl(&sig_frag_kerl);

  for (curr_tx = (iota_transaction_t)utarray_eltptr(bundle, 0);
       curr_tx != NULL;) {
    if (curr_tx->value >= 0) {
      curr_tx = (iota_transaction_t)utarray_next(bundle, curr_tx);
      continue;
    }
    curr_inp_tx = curr_tx;
    size_t offset = 0, next_offset = 0;
    kerl_reset(&address_kerl);
    do {
      kerl_reset(&sig_frag_kerl);
      next_offset = (offset + ISS_FRAGMENTS * RADIX - 1) % NUM_TRITS_HASH + 1;
      flex_trits_to_trits(key, NUM_TRITS_SIGNATURE,
                          curr_inp_tx->signature_or_message,
                          NUM_TRITS_SIGNATURE, NUM_TRITS_SIGNATURE);

      iss_kerl_sig_digest(digested_sig_trits,
                          &normalized_bundle[offset % NUM_TRITS_HASH], key,
                          NUM_TRITS_SIGNATURE, &sig_frag_kerl);
      kerl_absorb(&address_kerl, digested_sig_trits, NUM_TRITS_ADDRESS);
      curr_inp_tx = (iota_transaction_t)utarray_next(bundle, curr_inp_tx);
      offset = next_offset;
    } while (curr_inp_tx != NULL, memcmp(curr_inp_tx->address, curr_tx->address,
                                         FLEX_TRIT_SIZE_243) == 0 &&
                                      curr_inp_tx->value == 0);
    kerl_squeeze(&address_kerl, digested_address, NUM_TRITS_ADDRESS);

    flex_trit_t digest[FLEX_TRIT_SIZE_243];

    flex_trits_from_trits(digest, NUM_TRITS_HASH, digested_address,
                          NUM_TRITS_HASH, NUM_TRITS_HASH);
    flex_trits_to_trits(tx_address_trits, NUM_TRITS_ADDRESS, curr_tx->address,
                        NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
    curr_tx = curr_inp_tx;
    if (memcmp(tx_address_trits, digested_address, NUM_TRITS_ADDRESS) != 0) {
      *is_valid = false;
      break;
    }
  }

  return RC_OK;
}
