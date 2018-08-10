/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/core.h"
#include "ciri/node.h"
#include "utils/logger_helper.h"

#define MAIN_LOGGER_ID "main"

static node_t node_g;
static core_t core_g;

int main() {
  if (LOGGER_VERSION != logger_version()) {
    return EXIT_FAILURE;
  }
  logger_init();
  logger_output_register(stdout);
  logger_output_level_set(stdout, LOGGER_DEBUG);
  logger_helper_init(MAIN_LOGGER_ID, LOGGER_DEBUG, true);

  if (core_init(&core_g) == false) {
    return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Initializing cIRI node\n");
  if (node_init(&node_g) == false) {
    return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Starting cIRI node\n");
  if (node_start(&node_g) == false) {
    return EXIT_FAILURE;
  }

  // TODO(thibault) Remove, for broadcaster testing purpose
  neighbor_t neighbor;
  neighbor_init_with_uri(&neighbor, "tcp://127.0.0.1:14262");
  neighbor_add(node_g.neighbors, neighbor);
  neighbor_init_with_uri(&neighbor, "udp://127.0.0.1:14263");
  neighbor_add(node_g.neighbors, neighbor);
  flex_trit_t raw_trits[] = {-1, 0, 1, 1, 0, 1, 1,  0, -1,
                             -1, 1, 0, 1, 1, 0, -1, 1};
  while (1) {
    trit_array_p trits = trit_array_new(17);
    trit_array_set_trits(trits, raw_trits, 17);
    broadcaster_on_next(&node_g.broadcaster, trits);
    sleep(1);
  }

  log_info(MAIN_LOGGER_ID, "Stopping cIRI node\n");
  if (node_stop(&node_g) == false) {
    return EXIT_FAILURE;
  }

  log_info(MAIN_LOGGER_ID, "Destroying cIRI node\n");
  if (node_destroy(&node_g) == false) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
