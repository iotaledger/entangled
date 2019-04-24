/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/spent_addresses/spent_addresses_service.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/logger_helper.h"

#define SPENT_ADDRESSES_SERVICE_LOGGER_ID "spent_addresses_service"

static logger_id_t logger_id;

/*
 * Private functions
 */

#if defined(IOTA_MAINNET)
static retcode_t iota_spent_addresses_service_read_files(spent_addresses_service_t *const sas) {
  retcode_t ret = RC_OK;
  spent_addresses_provider_t sap;
  connection_config_t db_conf = {.db_path = sas->conf->spent_addresses_db_path};
  char *spent_addresses_files[] = {SPENT_ADDRESSES_FILES, NULL};

  if ((ret = iota_spent_addresses_provider_init(&sap, &db_conf)) != RC_OK) {
    log_error(logger_id, "Initializing spent addresses database connection failed\n");
    return ret;
  }

  for (size_t i = 0; spent_addresses_files[i]; i++) {
    if ((ret = iota_spent_addresses_provider_read_file(&sap, spent_addresses_files[i])) != RC_OK) {
      break;
    }
  }

  if ((ret = iota_spent_addresses_provider_destroy(&sap)) != RC_OK) {
    log_error(logger_id, "Destroying spent addresses database connection failed\n");
  }

  return ret;
}
#endif

static retcode_t iota_spent_addresses_service_was_tx_spent_from(tangle_t const *const tangle,
                                                                iota_transaction_t const *const tx, bool *const spent) {
  retcode_t ret = RC_OK;

  if (transaction_value(tx) < 0) {
    bundle_transactions_t *bundle = NULL;
    bundle_status_t status = BUNDLE_NOT_INITIALIZED;
    flex_trit_t tail[FLEX_TRIT_SIZE_243];

    // Transaction is confirmed
    if (transaction_snapshot_index(tx) != 0) {
      *spent = true;
      return RC_OK;
    }

    // Transaction is pending
    if ((ret = iota_tangle_find_tail(tangle, transaction_hash(tx), tail, spent)) != RC_OK) {
      return ret;
    }
    if (*spent) {
      bundle_transactions_new(&bundle);
      ret = iota_consensus_bundle_validator_validate(tangle, tail, bundle, &status);
      bundle_transactions_free(&bundle);
      // TODO: What if the bundle is invalid but signature is still OK ?
      *spent = (status == BUNDLE_VALID);
    }
    // TODO: What if incomplete bundle ?

  } else {
    *spent = false;
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  sas->conf = conf;
  logger_id = logger_helper_enable(SPENT_ADDRESSES_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

#if defined(IOTA_MAINNET)
  if ((ret = iota_spent_addresses_service_read_files(sas)) != RC_OK) {
    log_critical(logger_id, "Reading previous spent addresses files failed\n");
  }
#endif

  return ret;
}

retcode_t iota_spent_addresses_service_destroy(spent_addresses_service_t *const sas) {
  logger_helper_release(logger_id);

  return RC_OK;
}
