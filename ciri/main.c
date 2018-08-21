/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/core.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "ciri/conf/provider.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "main"

static core_t core_g;
static ciri_config_t* config;

int main(int argc, char* argv[]) {
  int ret = EXIT_SUCCESS;

  if (LOGGER_VERSION != logger_version()) {
    return EXIT_FAILURE;
  }
  logger_init();
  logger_output_register(stdout);
  logger_output_level_set(stdout, LOGGER_DEBUG);
  logger_helper_init(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  // configuration argument parser
  if ((config = ciri_conf_init()) == NULL) {
      return EXIT_FAILURE;
  }

  if (ciri_conf_parse(config, argc, argv)) {
      return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Initializing cIRI core\n");
  if (core_init(&core_g, config)) {
    log_critical(MAIN_LOGGER_ID, "Initializing cIRI core failed\n");
    return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Starting cIRI core\n");
  if (core_start(&core_g)) {
    log_critical(MAIN_LOGGER_ID, "Starting cIRI core failed\n");
    return EXIT_FAILURE;
  }

  // TODO(thibault) neighbors configuration
  neighbor_t neighbor;
  neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:14262");
  neighbor_add(core_g.node.neighbors, neighbor);
  neighbor_init_with_uri(&neighbor, "udp://127.0.0.1:14263");
  neighbor_add(core_g.node.neighbors, neighbor);

  sleep(1000);

  log_info(MAIN_LOGGER_ID, "Stopping cIRI core\n");
  if (core_stop(&core_g)) {
    log_error(MAIN_LOGGER_ID, "Stopping cIRI core failed\n");
    ret = EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Destroying cIRI core\n");
  if (core_destroy(&core_g)) {
    log_error(MAIN_LOGGER_ID, "Destroying cIRI core\n");
    ret = EXIT_FAILURE;
  }

  return ret;
}
