/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/consensus/test_utils/spent_addresses.h"
#include "utils/files.h"

retcode_t spent_addresses_setup(spent_addresses_provider_t *const sap, storage_connection_config_t *const config,
                                char *test_db_path) {
  retcode_t ret = RC_OK;

  if ((ret = iota_utils_copy_file(test_db_path, "common/storage/sql/sqlite3/spent-addresses.db"))) {
    return ret;
  }
  if ((ret = iota_spent_addresses_provider_init(sap, config))) {
    return ret;
  }
  return ret;
}

retcode_t spent_addresses_cleanup(spent_addresses_provider_t *const sap, char *test_db_path) {
  retcode_t ret = RC_OK;

  if ((ret = iota_spent_addresses_provider_destroy(sap))) {
    return ret;
  }
  if ((ret = iota_utils_remove_file(test_db_path))) {
    return ret;
  }
  return ret;
}
