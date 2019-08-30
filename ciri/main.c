/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "ciri/api/api.h"
#include "ciri/api/http/http.h"
#include "ciri/core.h"
#include "utils/handles/rand.h"
#include "utils/handles/signal.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "ciri"
#define STATS_LOG_INTERVAL_S 10

static core_t ciri_core;
static iota_api_t api;
static iota_api_http_t http;
static logger_id_t logger_id;
static tangle_t tangle;

static retcode_t ciri_init() {
  retcode_t ret = RC_OK;

  log_info(logger_id, "Initializing cIRI core\n");
  if ((ret = core_init(&ciri_core, &tangle)) != RC_OK) {
    log_critical(logger_id, "Initializing cIRI core failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing API\n");
  if ((ret = iota_api_init(&api, &ciri_core)) != RC_OK) {
    log_critical(logger_id, "Initializing API failed\n");
    return ret;
  }

  log_info(logger_id, "Initializing API HTTP\n");
  if ((ret = iota_api_http_init(&http, &api)) != RC_OK) {
    log_critical(logger_id, "Initializing API HTTP failed\n");
    return ret;
  }

  return ret;
}

static retcode_t ciri_start() {
  retcode_t ret = RC_OK;

  log_info(logger_id, "Starting cIRI core\n");
  if ((ret = core_start(&ciri_core, &tangle)) != RC_OK) {
    log_critical(logger_id, "Starting cIRI core failed\n");
    return ret;
  }

  log_info(logger_id, "Starting API HTTP\n");
  if ((ret = iota_api_http_start(&http)) != RC_OK) {
    log_critical(logger_id, "Starting API HTTP failed\n");
    return ret;
  }

  return ret;
}

static void ciri_stop(int sig) {
  if (ciri_core.running && sig == SIGINT) {
    log_info(logger_id, "Stopping API HTTP\n");
    if (iota_api_http_stop(&http) != RC_OK) {
      log_error(logger_id, "Stopping API HTTP failed\n");
    }

    log_info(logger_id, "Stopping cIRI core\n");
    if (core_stop(&ciri_core) != RC_OK) {
      log_error(logger_id, "Stopping cIRI core failed\n");
    }
  }
}

static retcode_t ciri_destroy() {
  retcode_t ret = RC_OK;

  log_info(logger_id, "Destroying API HTTP\n");
  if ((ret = iota_api_http_destroy(&http)) != RC_OK) {
    log_error(logger_id, "Destroying API HTTP failed\n");
  }

  log_info(logger_id, "Destroying API\n");
  if ((ret = iota_api_destroy(&api)) != RC_OK) {
    log_error(logger_id, "Destroying API failed\n");
  }

  log_info(logger_id, "Destroying cIRI core\n");
  if ((ret = core_destroy(&ciri_core)) != RC_OK) {
    log_error(logger_id, "Destroying cIRI core failed\n");
  }

  return ret;
}

int main(int argc, char* argv[]) {
  int ret = EXIT_SUCCESS;

  if (signal_handle_register(SIGINT, ciri_stop) == SIG_ERR) {
    return EXIT_FAILURE;
  }

  rand_handle_seed(time(NULL));

  if (logger_helper_init(LOGGER_WARNING) != RC_OK) {
    return EXIT_FAILURE;
  }
  logger_id = logger_helper_enable(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  // Default configuration

  if (iota_ciri_conf_default_init(&ciri_core.conf, &ciri_core.consensus.conf, &ciri_core.node.conf, &api.conf) !=
      RC_OK) {
    return EXIT_FAILURE;
  }

  // File configuration

  if (iota_ciri_conf_file_init(&ciri_core.conf, &ciri_core.consensus.conf, &ciri_core.node.conf, &api.conf, argc,
                               argv) != RC_OK) {
    return EXIT_FAILURE;
  }

  // CLI configuration

  if (iota_ciri_conf_cli_init(&ciri_core.conf, &ciri_core.consensus.conf, &ciri_core.node.conf, &api.conf, argc,
                              argv) != RC_OK) {
    return EXIT_FAILURE;
  }

  logger_output_level_set(stdout, ciri_core.conf.log_level);

  log_info(logger_id, "Welcome to %s v%s\n", CIRI_NAME, CIRI_VERSION);

  log_info(logger_id, "Initializing storage\n");
  if (storage_init() != RC_OK) {
    log_critical(logger_id, "Initializing storage failed\n");
    return EXIT_FAILURE;
  }

  {
    storage_connection_config_t db_conf = {.db_path = ciri_core.conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return EXIT_FAILURE;
    }
  }

  if (ciri_core.conf.tangle_db_revalidate) {
    log_info(logger_id, "Revalidating database\n");
    if (iota_tangle_milestone_clear(&tangle) != RC_OK || iota_tangle_transaction_metadata_clear(&tangle) != RC_OK) {
      log_critical(logger_id, "Revalidating database failed\n");
      return EXIT_FAILURE;
    }
  }

  log_info(logger_id, "Initializing cIRI\n");
  if (ciri_init() != RC_OK) {
    log_critical(logger_id, "Initializing cIRI failed\n");
    return EXIT_FAILURE;
  }

  log_info(logger_id, "Starting cIRI\n");
  if (ciri_start() != RC_OK) {
    log_critical(logger_id, "Starting cIRI failed\n");
    ret = EXIT_FAILURE;
    goto destroy;
  }

  {
    size_t count = 0;

    while (ciri_core.running) {
      if (iota_tangle_transaction_count(&tangle, &count) != RC_OK) {
        ret = EXIT_FAILURE;
        break;
      }
      log_info(logger_id,
               "Transactions: to process %d, to hash %d, to validate %d, to broadcast %d, to request %d, "
               "to reply %d, count %d\n",
               processor_stage_size(&ciri_core.node.processor), hasher_stage_size(&ciri_core.node.hasher),
               validator_stage_size(&ciri_core.node.validator), broadcaster_stage_size(&ciri_core.node.broadcaster),
               requester_size(&ciri_core.node.transaction_requester), responder_stage_size(&ciri_core.node.responder),
               count);
      sleep(STATS_LOG_INTERVAL_S);
    }
  }

destroy:

  log_info(logger_id, "Destroying cIRI\n");
  if (ciri_destroy() != RC_OK) {
    log_error(logger_id, "Destroying cIRI failed\n");
    ret = EXIT_FAILURE;
  }

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_error(logger_id, "Destroying tangle connection failed\n");
    ret = EXIT_FAILURE;
  }

  log_info(logger_id, "Destroying storage\n");
  if (storage_destroy() != RC_OK) {
    log_error(logger_id, "Destroying storage failed\n");
    ret = EXIT_FAILURE;
  }

  if (iota_ciri_conf_destroy(&ciri_core.conf, &ciri_core.consensus.conf, &ciri_core.node.conf, &api.conf) != RC_OK) {
    log_error(logger_id, "Destroying configuration failed\n");
    ret = EXIT_FAILURE;
  }

  logger_helper_release(logger_id);
  if (logger_helper_destroy() != RC_OK) {
    ret = EXIT_FAILURE;
  }

  return ret;
}
