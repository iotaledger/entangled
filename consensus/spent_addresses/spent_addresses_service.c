/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/spent_addresses/spent_addresses_service.h"
#include "consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/logger_helper.h"

#define SPENT_ADDRESSES_SERVICE_LOGGER_ID "spent_addresses_service"

static logger_id_t logger_id;

retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;
  spent_addresses_provider_t sap;

  sas->conf = conf;
  logger_id = logger_helper_enable(SPENT_ADDRESSES_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  {
    connection_config_t db_conf = {.db_path = sas->conf->spent_addresses_db_path};

    if ((ret = iota_spent_addresses_provider_init(&sap, &db_conf)) != RC_OK) {
      log_critical(logger_id, "Initializing spent addresses database connection failed\n");
      return ret;
    }
  }


done:
  if ((ret = iota_spent_addresses_provider_destroy(&sap)) != RC_OK) {
    log_critical(logger_id, "Destroying spent addresses database connection failed\n");
  }

  return ret;
}

retcode_t iota_spent_addresses_service_destroy(spent_addresses_service_t *const sas) {
  logger_helper_release(logger_id);

  return RC_OK;
}
