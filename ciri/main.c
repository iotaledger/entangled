/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "ciri/core.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "main"

static core_t core_g;

int main(int argc, char* argv[]) {
  int ret = EXIT_SUCCESS;

  rand_handle_seed(time(NULL));

  // Default configuration

  if (iota_ciri_conf_default(&core_g.conf, &core_g.consensus.conf,
                             &core_g.node.conf, &core_g.api.conf) != RC_OK) {
    return EXIT_FAILURE;
  }

  // File configuration

  // CLI Configuration

  if (iota_ciri_conf_cli(&core_g.conf, &core_g.consensus.conf,
                         &core_g.node.conf, &core_g.api.conf, argc,
                         argv) != RC_OK) {
    return EXIT_FAILURE;
  }

  if (LOGGER_VERSION != logger_version()) {
    return EXIT_FAILURE;
  }
  logger_init();
  logger_output_register(stdout);
  logger_output_level_set(stdout, core_g.conf.log_level);
  logger_helper_init(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  log_info(MAIN_LOGGER_ID, "Initializing cIRI core\n");
  if (core_init(&core_g) != RC_OK) {
    log_critical(MAIN_LOGGER_ID, "Initializing cIRI core failed\n");
    return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Starting cIRI core\n");
  if (core_start(&core_g) != RC_OK) {
    log_critical(MAIN_LOGGER_ID, "Starting cIRI core failed\n");
    return EXIT_FAILURE;
  }

  // TODO remove
  // Dummy broadcasted packet to begin receiving from UDP neighbors
  sleep(2);
  flex_trit_t dummy[FLEX_TRIT_SIZE_8019];
  memset(dummy, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
  broadcaster_on_next(&core_g.node.broadcaster, dummy);
  sleep(1000);

  log_info(MAIN_LOGGER_ID, "Stopping cIRI core\n");
  if (core_stop(&core_g) != RC_OK) {
    log_error(MAIN_LOGGER_ID, "Stopping cIRI core failed\n");
    ret = EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Destroying cIRI core\n");
  if (core_destroy(&core_g) != RC_OK) {
    log_error(MAIN_LOGGER_ID, "Destroying cIRI core failed\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
