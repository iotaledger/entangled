/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/bundle_validator/bundle_validator.h"
#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss_kerl.h"
#include "common/trinary/trit_long.h"
#include "consensus/conf.h"
#include "utils/logger_helper.h"

#define BUNDLE_VALIDATOR_LOGGER_ID "bundle_validator"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t load_bundle_transactions(tangle_t const* const tangle, flex_trit_t* const tail_hash,
                                          bundle_transactions_t* const bundle) {
  retcode_t res = RC_OK;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  size_t last_index = 0, curr_index = 0;
  flex_trit_t* curr_tx_trunk = NULL;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, tail_hash, &pack);
  if (res != RC_OK || pack.num_loaded == 0) {
    log_error(logger_id, "No transactions were loaded for the provided tail hash\n");
    return res;
  }

  last_index = transaction_last_index(curr_tx);
  memcpy(bundle_hash, transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243);

  while (pack.num_loaded != 0 && curr_index <= last_index &&
         memcmp(bundle_hash, transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243) == 0) {
    bundle_transactions_add(bundle, curr_tx);
    curr_tx_trunk = transaction_trunk(curr_tx);

    hash_pack_reset(&pack);
    if ((res = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, curr_tx_trunk, &pack)) != RC_OK) {
      return res;
    }
    curr_index++;
  }

  return res;
}

/*
 * Public functions
 */

retcode_t iota_consensus_bundle_validator_init() {
  logger_id = logger_helper_enable(BUNDLE_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_destroy() {
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_validate(tangle_t const* const tangle, flex_trit_t* const tail_hash,
                                                   bundle_transactions_t* const bundle, bundle_status_t* const status) {
  retcode_t res = RC_OK;

  if (bundle == NULL) {
    log_error(logger_id, "Bundle is not initialized\n");
    *status = BUNDLE_NOT_INITIALIZED;
    return RC_CONSENSUS_NULL_BUNDLE_PTR;
  }

  res = load_bundle_transactions(tangle, tail_hash, bundle);
  if (res != RC_OK || utarray_len(bundle) == 0) {
    log_error(logger_id, "Loading transactions for tail failed\n");
    *status = BUNDLE_TAIL_NOT_FOUND;
    return res;
  }
  return bundle_validator(bundle, status);
}
