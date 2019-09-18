/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/spent_addresses/spent_addresses_service.h"
#include "ciri/consensus/bundle_validator/bundle_validator.h"
#include "ciri/consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define SPENT_ADDRESSES_SERVICE_LOGGER_ID "spent_addresses_service"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t iota_spent_addresses_service_read_files(spent_addresses_service_t *const sas) {
  retcode_t ret = RC_OK;
  char *file = NULL;
  char *cpy = NULL;
  char *ptr = NULL;
  spent_addresses_provider_t sap;
  storage_connection_config_t db_conf = {.db_path = sas->conf->spent_addresses_db_path};

  if (sas->conf->spent_addresses_files == NULL) {
    return RC_OK;
  }

  if ((ret = iota_spent_addresses_provider_init(&sap, &db_conf)) != RC_OK) {
    log_error(logger_id, "Initializing spent addresses database connection failed\n");
    return ret;
  }

  if ((ptr = cpy = strdup(sas->conf->spent_addresses_files)) == NULL) {
    ret = RC_OOM;
    goto done;
  }

  while ((file = strsep(&cpy, " ")) != NULL) {
    if ((ret = iota_spent_addresses_provider_import(&sap, file)) != RC_OK) {
      log_warning(logger_id, "Reading spent addresses file \"%s\" failed\n", file);
    }
  }

done:
  if ((ret = iota_spent_addresses_provider_destroy(&sap)) != RC_OK) {
    log_error(logger_id, "Destroying spent addresses database connection failed\n");
  }

  free(ptr);

  return ret;
}

retcode_t iota_spent_addresses_service_was_tx_spent_from(tangle_t const *const tangle,
                                                         iota_transaction_t const *const tx,
                                                         flex_trit_t const *const hash, bool *const spent) {
  retcode_t ret = RC_OK;

  if (transaction_value(tx) < 0) {
    flex_trit_t tail[FLEX_TRIT_SIZE_243];

    // Transaction is confirmed
    if (transaction_snapshot_index(tx) != 0) {
      *spent = true;
      return RC_OK;
    }

    // Transaction is pending
    if ((ret = iota_tangle_find_tail(tangle, hash, tail, spent)) != RC_OK) {
      return ret;
    }
    if (*spent) {
      bundle_transactions_t *bundle = NULL;
      bundle_status_t status = BUNDLE_NOT_INITIALIZED;

      bundle_transactions_new(&bundle);
      ret = iota_consensus_bundle_validator_validate(tangle, tail, bundle, &status);
      bundle_transactions_free(&bundle);
      // TODO: What if the bundle is invalid but signature is still OK ?
      // TODO: What if incomplete bundle ?
      *spent = (status == BUNDLE_VALID);
    }

  } else {
    *spent = false;
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas, iota_consensus_conf_t *const conf) {
  sas->conf = conf;
  logger_id = logger_helper_enable(SPENT_ADDRESSES_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  return iota_spent_addresses_service_read_files(sas);
}

retcode_t iota_spent_addresses_service_destroy(spent_addresses_service_t *const sas) {
  UNUSED(sas);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t iota_spent_addresses_service_was_address_spent_from(spent_addresses_service_t const *const sas,
                                                              spent_addresses_provider_t const *const sap,
                                                              tangle_t const *const tangle,
                                                              flex_trit_t const *const address, bool *const spent) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;
  DECLARE_PACK_SINGLE_TX(tx, txp, tx_pack);

  if (sas == NULL || sap == NULL || tangle == NULL || address == NULL || spent == NULL) {
    return RC_NULL_PARAM;
  }

  *spent = false;

  if (memcmp(address, sas->conf->genesis_hash, FLEX_TRIT_SIZE_243) == 0 ||
      memcmp(address, sas->conf->coordinator_address, FLEX_TRIT_SIZE_243) == 0) {
    return RC_OK;
  }

  if ((ret = iota_spent_addresses_provider_exist(sap, address, spent)) != RC_OK) {
    return ret;
  }

  if (*spent) {
    return RC_OK;
  }

  hash_pack_init(&pack, 8);

  if ((ret = iota_tangle_transaction_load_hashes_by_address(tangle, address, &pack)) != RC_OK) {
    goto done;
  }

  // TODO: If the hash set returned contains more than 100 000 entries, it likely will not be a spent address.
  // To avoid unnecessary overhead while processing, the loop will return false

  for (size_t i = 0; i < pack.num_loaded; ++i) {
    if ((ret = iota_tangle_transaction_load_partial(tangle, ((flex_trit_t **)(pack.models))[i], &tx_pack,
                                                    PARTIAL_TX_MODEL_ESSENCE_METADATA)) != RC_OK) {
      goto done;
    }
    if ((ret = iota_spent_addresses_service_was_tx_spent_from(tangle, txp, ((flex_trit_t **)(pack.models))[i],
                                                              spent)) != RC_OK) {
      goto done;
    }
    if (*spent) {
      goto done;
    }
  }

done:
  hash_pack_free(&pack);

  return ret;
}
