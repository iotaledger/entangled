/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node.h"
#include "common/network/logger.h"

static node_t node_g;

int main() {
  logger_init(LOG_DEBUG, false, stdout);

  log_info("Initializing cIRI node");
  if (node_init(&node_g) == false) {
    return EXIT_FAILURE;
  }

  log_info("Starting cIRI node");
  if (node_start(&node_g) == false) {
    return EXIT_FAILURE;
  }

  sleep(10);

  log_info("Stopping cIRI node");
  if (node_stop(&node_g) == false) {
    return EXIT_FAILURE;
  }

  log_info("Destroying cIRI node");
  if (node_destroy(&node_g) == false) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
