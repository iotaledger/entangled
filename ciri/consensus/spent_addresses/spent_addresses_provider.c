/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/logger_helper.h"

#define SPENT_ADDRESSES_PROVIDER_LOGGER_ID "spent_addresses_provider"

static logger_id_t logger_id;

retcode_t iota_spent_addresses_provider_init(spent_addresses_provider_t *const sap,
                                             connection_config_t const *const conf) {
  logger_id = logger_helper_enable(SPENT_ADDRESSES_PROVIDER_LOGGER_ID, LOGGER_DEBUG, true);

  return storage_connection_init(&sap->connection, conf, STORAGE_CONNECTION_SPENT_ADDRESSES);
}

retcode_t iota_spent_addresses_provider_destroy(spent_addresses_provider_t *const sap) {
  logger_helper_release(logger_id);

  return storage_connection_destroy(&sap->connection);
}

retcode_t iota_spent_addresses_provider_store(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address) {
  return storage_spent_address_store(&sap->connection, address);
}

retcode_t iota_spent_addresses_provider_batch_store(spent_addresses_provider_t const *const sap,
                                                    hash243_set_t const addresses) {
  return storage_spent_addresses_store(&sap->connection, addresses);
}

retcode_t iota_spent_addresses_provider_exist(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address, bool *const exist) {
  return storage_spent_address_exist(&sap->connection, address, exist);
}

retcode_t iota_spent_addresses_provider_import(spent_addresses_provider_t const *const sap, char const *const file) {
  retcode_t ret = RC_OK;
  char *line = NULL;
  size_t len = 0;
  ssize_t rd = 0;
  FILE *fp = NULL;
  flex_trit_t address[FLEX_TRIT_SIZE_243];

  if ((fp = fopen(file, "r")) == NULL) {
    log_critical(logger_id, "Opening spent addresses file failed\n");
    ret = RC_UTILS_FAILED_TO_OPEN_FILE;
    goto done;
  }

  while ((rd = getline(&line, &len, fp)) > 0) {
    line[--rd] = '\0';
    flex_trits_from_trytes(address, HASH_LENGTH_TRIT, (tryte_t *)line, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if ((ret = iota_spent_addresses_provider_store(sap, address)) != RC_OK) {
      goto done;
    }
  }

done:
  if (line) {
    free(line);
  }
  if (fp) {
    fclose(fp);
  }

  return ret;
}
