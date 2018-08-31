/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "consensus/tangle/tangle.h"
#include "consensus/test_definitions/defs.h"
#include "utils/files.h"

#define PATH_LENGTH 256

static char *test_db = "consensus/cw_rating_calculator/tests/test.db";
static char *ciri_db = "consensus/cw_rating_calculator/tests/ciri.db";

// void get_test_db_path(char *path) {
//   char path[PATH_LENGTH];
//
//   getcwd(path, PATH_LENGTH);
//   fprintf(stderr, "%s\n", path);
//   // strcpy(path, test_db);
// }
//
// void get_ciri_db_path(char *path) { strcpy(path, ciri_db); }

// void get_db_path(bool debug_mode) {}

retcode_t test_setup(tangle_t *const tangle,
                     connection_config_t *const config) {
  retcode_t ret = RC_OK;

  if ((ret = copy_file(test_db, ciri_db))) {
    return ret;
  }
  if ((ret = iota_tangle_init(tangle, config))) {
    return ret;
  }
  return ret;
}

retcode_t test_cleanup(tangle_t *const tangle) {
  retcode_t ret = RC_OK;

  if ((ret = iota_tangle_destroy(tangle))) {
    return ret;
  }
  if ((ret = remove_file(test_db))) {
    return ret;
  }
  return ret;
}
