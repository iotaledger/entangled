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
