/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "ciri/core.h"
#include "ciri/perceptive_node/perceptive_node.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "main"
#define STATS_LOG_INTERVAL_S 10

static core_t core_g;
static logger_id_t logger_id;

int main(int argc, char* argv[]) {
  int ret = EXIT_SUCCESS;
  tangle_t tangle;
  connection_config_t db_conf;

  rand_handle_seed(time(NULL));

  if (logger_helper_init() != RC_OK) {
    return EXIT_FAILURE;
  }
  logger_id = logger_helper_enable(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  // Default configuration

  if (iota_ciri_conf_default(&core_g.conf, &core_g.consensus.conf,
                             &core_g.node.conf, &core_g.api.conf,
                             &core_g.perceptive_node.conf) != RC_OK) {
    return EXIT_FAILURE;
  }

  // File configuration

  if (iota_ciri_conf_file(&core_g.conf, &core_g.consensus.conf,
                          &core_g.node.conf, &core_g.api.conf,
                          &core_g.perceptive_node.conf) != RC_OK) {
    return EXIT_FAILURE;
  }

  // CLI configuration

  if (iota_ciri_conf_cli(&core_g.conf, &core_g.consensus.conf,
                         &core_g.node.conf, &core_g.api.conf,
                         &core_g.perceptive_node.conf, argc, argv) != RC_OK) {
    return EXIT_FAILURE;
  }

  logger_output_level_set(stdout, core_g.conf.log_level);

  log_info(logger_id, "Initializing storage\n");
  if (storage_init() != RC_OK) {
    log_critical(logger_id, "Initializing storage failed\n");
    return EXIT_FAILURE;
  }

  db_conf.db_path = core_g.conf.db_path;
  if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
    log_critical(logger_id, "Initializing tangle connection failed\n");
    return EXIT_FAILURE;
  }

  log_info(logger_id, "Initializing cIRI core\n");
  if (core_init(&core_g, &tangle) != RC_OK) {
    log_critical(logger_id, "Initializing cIRI core failed\n");
    return EXIT_FAILURE;
  }

  log_info(logger_id, "Starting cIRI core\n");
  if (core_start(&core_g, &tangle) != RC_OK) {
    log_critical(logger_id, "Starting cIRI core failed\n");
    return EXIT_FAILURE;
  }

  size_t count = 0;
  while (true) {
    if (iota_tangle_transaction_count(&tangle, &count) != RC_OK) {
      ret = EXIT_FAILURE;
      break;
    }
    log_info(logger_id,
             "Transactions: to process %d, to broadcast %d, to request %d, "
             "to reply %d, count %d\n",
             processor_size(&core_g.node.processor),
             broadcaster_size(&core_g.node.broadcaster),
             requester_size(&core_g.node.transaction_requester),
             responder_size(&core_g.node.responder), count);
    sleep(STATS_LOG_INTERVAL_S);
  }

  log_info(logger_id, "Stopping cIRI core\n");
  if (core_stop(&core_g) != RC_OK) {
    log_error(logger_id, "Stopping cIRI core failed\n");
    ret = EXIT_FAILURE;
  }

  log_info(logger_id, "Destroying cIRI core\n");
  if (core_destroy(&core_g) != RC_OK) {
    log_error(logger_id, "Destroying cIRI core failed\n");
    ret = EXIT_FAILURE;
  }

  log_info(logger_id, "Destroying storage\n");
  if (storage_destroy() != RC_OK) {
    log_error(logger_id, "Destroying storage failed\n");
    ret = EXIT_FAILURE;
  }

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_error(logger_id, "Destroying tangle connection failed\n");
    ret = EXIT_FAILURE;
  }

  logger_helper_release(logger_id);
  if (logger_helper_destroy() != RC_OK) {
    ret = EXIT_FAILURE;
  }

  return ret;
}
