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

retcode_t iota_spent_addresses_provider_init(spent_addresses_provider_t *const sap,
                                             connection_config_t const *const conf) {
  logger_id = logger_helper_enable(SPENT_ADDRESSES_PROVIDER_LOGGER_ID, LOGGER_DEBUG, true);

  return connection_init(&sap->connection, conf, STORAGE_CONNECTION_SPENT_ADDRESSES);
}

retcode_t iota_spent_addresses_provider_destroy(spent_addresses_provider_t *const sap) {
  logger_helper_release(logger_id);

  return connection_destroy(&sap->connection);
}

/*
 * Spent address operations
 */

retcode_t iota_spent_addresses_provider_store(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address) {
  return iota_store_spent_address_store(&sap->connection, address);
}

retcode_t iota_spent_addresses_provider_batch_store(spent_addresses_provider_t const *const sap,
                                                    hash243_set_t const *const addresses) {
  return iota_stor_spent_addresses_store(&sap->connection, addresses);
}

retcode_t iota_spent_addresses_provider_exist(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address) {
  return iota_stor_spent_address_exist(&sap->connection, address);
}
