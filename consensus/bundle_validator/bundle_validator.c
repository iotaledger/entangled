/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/bundle_validator/bundle_validator.h"
#include "common/sign/normalize.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_long.h"
#include "consensus/conf.h"
#include "utils/logger_helper.h"

#define BUNDLE_VALIDATOR_LOGGER_ID "consensus_bundle_validator"

/*
 * Private functions
 */

static retcode_t load_bundle_transactions(tangle_t const* const tangle,
                                          trit_array_p const tail_hash,
                                          bundle_transactions_t* const bundle) {
  retcode_t res = RC_OK;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  size_t last_index = 0, curr_index = 0;
  trit_array_t curr_tx_trunk = {.trits = NULL,
                                .num_trits = NUM_TRITS_HASH,
                                .num_bytes = FLEX_TRIT_SIZE_243,
                                .dynamic = 0};
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, tail_hash,
                                     &pack);
  if (res != RC_OK || pack.num_loaded == 0) {
    log_error(BUNDLE_VALIDATOR_LOGGER_ID,
              "No transactions were loaded for the provided tail hash\n");
    return res;
  }

  last_index = curr_tx->last_index;
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);

  while (curr_index <= last_index &&
         memcmp(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243) == 0) {
    bundle_transactions_add(bundle, curr_tx);
    curr_tx_trunk.trits = curr_tx->trunk;

    hash_pack_reset(&pack);
    res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH,
                                       &curr_tx_trunk, &pack);
    if (res != RC_OK || pack.num_loaded == 0) {
      log_error(BUNDLE_VALIDATOR_LOGGER_ID,
                "Failed in %s, could not load next transaction\n",
                __FUNCTION__);
      return res;
    }
  }

  return RC_OK;
}

static retcode_t validate_signature(bundle_transactions_t const* const bundle,
                                    trit_t const* const normalized_bundle,
                                    bool* const is_valid) {
  iota_transaction_t curr_tx = NULL, curr_inp_tx = NULL;
  Kerl address_kerl, sig_frag_kerl;
  trit_t digested_sig_trits[NUM_TRITS_ADDRESS];
  trit_t digested_address[NUM_TRITS_ADDRESS];
  trit_t key[NUM_TRITS_SIGNATURE];
  flex_trit_t digest[FLEX_TRIT_SIZE_243];
  size_t offset = 0, next_offset = 0;

  init_kerl(&address_kerl);
  init_kerl(&sig_frag_kerl);

  *is_valid = true;

  for (curr_tx = (iota_transaction_t)utarray_eltptr(bundle, 0);
       curr_tx != NULL;) {
    if (curr_tx->value >= 0) {
      curr_tx = (iota_transaction_t)utarray_next(bundle, curr_tx);
      continue;
    }
    curr_inp_tx = curr_tx;
    offset = 0;
    next_offset = 0;
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
    } while (curr_inp_tx != NULL &&
             memcmp(curr_inp_tx->address, curr_tx->address,
                    FLEX_TRIT_SIZE_243) == 0 &&
             curr_inp_tx->value == 0);

    kerl_squeeze(&address_kerl, digested_address, NUM_TRITS_ADDRESS);
    flex_trits_from_trits(digest, NUM_TRITS_HASH, digested_address,
                          NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);

    if (memcmp(digest, curr_tx->address, FLEX_TRIT_SIZE_243) != 0) {
      *is_valid = false;
      break;
    }
    curr_tx = curr_inp_tx;
  }

  return RC_OK;
}

/*
 * Public functions
 */

retcode_t iota_consensus_bundle_validator_init() {
  logger_helper_init(BUNDLE_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_destroy() {
  logger_helper_destroy(BUNDLE_VALIDATOR_LOGGER_ID);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_validate(
    tangle_t const* const tangle, trit_array_p const tail_hash,
    bundle_transactions_t* const bundle, bool* const is_valid) {
  retcode_t res = RC_OK;
  iota_transaction_t curr_tx = NULL;
  size_t index = 0, last_index = 0;
  int64_t bundle_value = 0;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];

  *is_valid = true;

  if (bundle == NULL) {
    log_error(BUNDLE_VALIDATOR_LOGGER_ID, "Bundle is not initialized\n");
    *is_valid = false;
    return RC_CONSENSUS_NULL_BUNDLE_PTR;
  }

  res = load_bundle_transactions(tangle, tail_hash, bundle);
  if (res != RC_OK || utarray_len(bundle) == 0) {
    log_error(BUNDLE_VALIDATOR_LOGGER_ID,
              "Loading transactions for tail failed\n");
    *is_valid = false;
    return res;
  }

  curr_tx = (iota_transaction_t)utarray_eltptr(bundle, 0);
  last_index = curr_tx->last_index;
  memcpy(bundle_hash, curr_tx->bundle, FLEX_TRIT_SIZE_243);

  for (; curr_tx != NULL;
       curr_tx = (iota_transaction_t)utarray_next(bundle, curr_tx)) {
    bundle_value += curr_tx->value;

    if (curr_tx->current_index != index++ ||
        curr_tx->last_index != last_index ||
        llabs(bundle_value) > IOTA_SUPPLY) {
      log_error(BUNDLE_VALIDATOR_LOGGER_ID, "Invalid transaction in bundle\n");
      *is_valid = false;
      break;
    }

    if (curr_tx->value != 0 &&
        flex_trits_at(curr_tx->address, NUM_TRITS_ADDRESS,
                      NUM_TRITS_ADDRESS - 1) != 0) {
      log_error(BUNDLE_VALIDATOR_LOGGER_ID, "Invalid input address\n");
      *is_valid = false;
      break;
    }

    if (curr_tx->current_index == last_index) {
      flex_trit_t bundle_hash_calculated[FLEX_TRIT_SIZE_243];
      trit_t normalized_bundle_trits[NUM_TRITS_HASH];
      byte_t normalized_bundle_bytes[NUM_TRYTES_HASH];

      if (bundle_value != 0) {
        log_error(BUNDLE_VALIDATOR_LOGGER_ID, "Bundle value is not zero\n");
        *is_valid = false;
        break;
      }

      calculate_bundle_hash(bundle, bundle_hash_calculated);
      if (memcmp(bundle_hash, bundle_hash_calculated, FLEX_TRIT_SIZE_243) !=
          0) {
        log_error(BUNDLE_VALIDATOR_LOGGER_ID,
                  "Bundle hash provided differs from calculated\n");
        *is_valid = false;
        break;
      }

      normalize_hash(bundle_hash_calculated, normalized_bundle_bytes);
      for (int c = 0; c < NUM_TRYTES_HASH; ++c) {
        long_to_trits(normalized_bundle_bytes[c],
                      &normalized_bundle_trits[c * RADIX]);
      }

      res = validate_signature(bundle, normalized_bundle_trits, is_valid);
      if (res != RC_OK || !(*is_valid)) {
        log_error(BUNDLE_VALIDATOR_LOGGER_ID, "Invalid signature\n");
        *is_valid = false;
        break;
      }
    }
  }

  return RC_OK;
}
