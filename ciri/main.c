/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "ciri/core.h"
#include "utils/handles/rand.h"
#include "utils/handles/signal.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "main"
#define STATS_LOG_INTERVAL_S 10

static core_t ciri_core;
static logger_id_t logger_id;

static void signal_handler(int sig) {
  if (ciri_core.running && sig == SIGINT) {
    log_info(logger_id, "Stopping cIRI core\n");
    if (core_stop(&ciri_core) != RC_OK) {
      log_error(logger_id, "Stopping cIRI core failed\n");
    }
  }
}

int main(int argc, char* argv[]) {
  int ret = EXIT_SUCCESS;
  tangle_t tangle;
  connection_config_t db_conf;

  if (signal_handle_register(SIGINT, signal_handler) == SIG_ERR) {
    return EXIT_FAILURE;
  }

  rand_handle_seed(time(NULL));

  if (logger_helper_init() != RC_OK) {
    return EXIT_FAILURE;
  }
  logger_id = logger_helper_enable(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  // Default configuration

  if (iota_ciri_conf_default(&ciri_core.conf, &ciri_core.consensus.conf,
                             &ciri_core.node.conf,
                             &ciri_core.api.conf) != RC_OK) {
    return EXIT_FAILURE;
  }

  // File configuration

  if (iota_ciri_conf_file(&ciri_core.conf, &ciri_core.consensus.conf,
                          &ciri_core.node.conf, &ciri_core.api.conf) != RC_OK) {
    return EXIT_FAILURE;
  }

  // CLI configuration

  if (iota_ciri_conf_cli(&ciri_core.conf, &ciri_core.consensus.conf,
                         &ciri_core.node.conf, &ciri_core.api.conf, argc,
                         argv) != RC_OK) {
    return EXIT_FAILURE;
  }

  logger_output_level_set(stdout, ciri_core.conf.log_level);

  log_info(logger_id, "Initializing storage\n");
  if (storage_init() != RC_OK) {
    log_critical(logger_id, "Initializing storage failed\n");
    return EXIT_FAILURE;
  }

  db_conf.db_path = ciri_core.conf.db_path;
  if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
    log_critical(logger_id, "Initializing tangle connection failed\n");
    return EXIT_FAILURE;
  }

  log_info(logger_id, "Initializing cIRI core\n");
  if (core_init(&ciri_core, &tangle) != RC_OK) {
    log_critical(logger_id, "Initializing cIRI core failed\n");
    return EXIT_FAILURE;
  }

  log_info(logger_id, "Starting cIRI core\n");
  if (core_start(&ciri_core, &tangle) != RC_OK) {
    log_critical(logger_id, "Starting cIRI core failed\n");
    return EXIT_FAILURE;
  }

  size_t count = 0;
  while (ciri_core.running) {
    if (iota_tangle_transaction_count(&tangle, &count) != RC_OK) {
      ret = EXIT_FAILURE;
      break;
    }
    log_info(logger_id,
             "Transactions: to process %d, to broadcast %d, to request %d, "
             "to reply %d, count %d\n",
             processor_size(&ciri_core.node.processor),
             broadcaster_size(&ciri_core.node.broadcaster),
             requester_size(&ciri_core.node.transaction_requester),
             responder_size(&ciri_core.node.responder), count);
    sleep(STATS_LOG_INTERVAL_S);
  }

  log_info(logger_id, "Destroying cIRI core\n");
  if (core_destroy(&ciri_core) != RC_OK) {
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
