/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/bundle_validator/bundle_validator.h"
#include "ciri/consensus/conf.h"
#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss_kerl.h"
#include "common/trinary/trit_long.h"
#include "utils/logger_helper.h"

#define BUNDLE_VALIDATOR_LOGGER_ID "bundle_validator"

static logger_id_t logger_id;

retcode_t iota_consensus_bundle_validator_init() {
  logger_id = logger_helper_enable(BUNDLE_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_destroy() {
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_consensus_bundle_validator_validate(tangle_t const* const tangle, flex_trit_t const* const tail_hash,
                                                   bundle_transactions_t* const bundle, bundle_status_t* const status) {
  retcode_t res = RC_OK;
  *status = BUNDLE_NOT_INITIALIZED;

  if (tangle == NULL || tail_hash == NULL || bundle == NULL || status == NULL) {
    return RC_NULL_PARAM;
  }

  if ((res = iota_tangle_bundle_load(tangle, tail_hash, bundle)) != RC_OK) {
    log_error(logger_id, "Loading bundle transactions failed\n");
    return res;
  }

  if (utarray_len(bundle) == 0) {
    *status = BUNDLE_EMPTY;
    return RC_OK;
  }

  {
    DECLARE_PACK_SINGLE_TX(tx, txp, pack);

    if ((res = iota_tangle_transaction_load_partial(tangle, tail_hash, &pack, PARTIAL_TX_MODEL_METADATA)) != RC_OK) {
      log_error(logger_id, "Loading transaction metadata failed\n");
      return res;
    }
    if ((*status = transaction_validity(txp)) != BUNDLE_NOT_INITIALIZED) {
      return RC_OK;
    }
  }

  if ((res = bundle_validate(bundle, status)) != RC_OK) {
    log_error(logger_id, "Bundle validation failed\n");
    return res;
  }

  if (*status != BUNDLE_INCOMPLETE) {
    if ((res = iota_tangle_bundle_update_validity(tangle, bundle, *status))) {
      log_error(logger_id, "Updating bundle validaty failed\n");
      return res;
    }
  }

  return res;
}
