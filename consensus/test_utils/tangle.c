/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "consensus/tangle/tangle.h"
#include "consensus/test_utils/tangle.h"
#include "utils/files.h"

retcode_t tangle_setup(tangle_t *const tangle, connection_config_t *const config,
                       char *test_db_path, char *ciri_db_path) {
  retcode_t ret = RC_OK;

  if ((ret = copy_file(test_db_path, ciri_db_path))) {
    return ret;
  }
  if ((ret = iota_tangle_init(tangle, config))) {
    return ret;
  }
  return ret;
}

retcode_t tangle_cleanup(tangle_t *const tangle, char *test_db_path) {
  retcode_t ret = RC_OK;

  if ((ret = iota_tangle_destroy(tangle))) {
    return ret;
  }
  if ((ret = remove_file(test_db_path))) {
    return ret;
  }
  return ret;
}
