/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/logger_helper.h"

#define SPENT_ADDRESSES_PROVIDER_LOGGER_ID "spent_addresses_provider"

static logger_id_t logger_id;

retcode_t iota_spent_addresses_provider_init(spent_addresses_provider_t* const sap) {
  logger_id = logger_helper_enable(SPENT_ADDRESSES_PROVIDER_LOGGER_ID, LOGGER_DEBUG, true);

  return RC_OK;
}

retcode_t iota_spent_addresses_provider_destroy(spent_addresses_provider_t* const sap) {
  logger_helper_release(logger_id);

  return RC_OK;
}
