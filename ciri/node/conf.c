/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/conf.h"
#include "common/defs.h"

retcode_t iota_node_conf_init(iota_node_conf_t* const conf) {
  flex_trit_t coordinator_address[FLEX_TRIT_SIZE_243];

  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->mwm = DEFAULT_MWN;
  conf->neighbors = DEFAULT_NEIGHBORS;
  conf->p_send_milestone = DEFAULT_PROBABILITY_SEND_MILESTONE;
  conf->recent_seen_bytes_cache_size = DEFAULT_RECENT_SEEN_BYTES_CACHE_SIZE;
  conf->requester_queue_size = DEFAULT_REQUESTER_QUEUE_SIZE;
  conf->tips_cache_size = DEFAULT_TIPS_CACHE_SIZE;
  flex_trits_from_trytes(coordinator_address, HASH_LENGTH_TRIT, (tryte_t*)COORDINATOR_ADDRESS, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  flex_trits_to_bytes(conf->coordinator_address, HASH_LENGTH_TRIT, coordinator_address, HASH_LENGTH_TRIT,
                      HASH_LENGTH_TRIT);
  strncpy(conf->neighboring_address, DEFAULT_NEIGHBORING_ADDRESS, sizeof(conf->neighboring_address));
  conf->neighboring_port = DEFAULT_NEIGHBORING_PORT;
  conf->auto_tethering_enabled = DEFAULT_AUTO_TETHERING_ENABLED;
  conf->max_neighbors = DEFAULT_MAX_NEIGHBORS;
  conf->reconnect_attempt_interval = DEFAULT_RECONNECT_ATTEMPT_INTERVAL;

  return RC_OK;
}
