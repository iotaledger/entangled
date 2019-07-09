/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/conf.h"
#include "common/defs.h"

retcode_t iota_node_conf_init(iota_node_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->mwm = DEFAULT_MWN;
  conf->neighbors = DEFAULT_NEIGHBORS;
  conf->p_remove_request = DEFAULT_PROBABILITY_REMOVE_REQUEST;
  conf->p_reply_random_tip = DEFAULT_PROBABILITY_REPLY_RANDOM_TIP;
  conf->p_select_milestone = DEFAULT_PROBABILITY_SELECT_MILESTONE;
  conf->p_send_milestone = DEFAULT_PROBABILITY_SEND_MILESTONE;
  conf->recent_seen_bytes_cache_size = DEFAULT_RECENT_SEEN_BYTES_CACHE_SIZE;
  conf->requester_queue_size = DEFAULT_REQUESTER_QUEUE_SIZE;
  conf->tips_cache_size = DEFAULT_TIPS_CACHE_SIZE;
  conf->tips_solidifier_enabled = DEFAULT_TIPS_SOLIDIFIER_ENABLED;
  flex_trits_from_trytes(conf->coordinator_address, HASH_LENGTH_TRIT, (tryte_t*)COORDINATOR_ADDRESS, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  strncpy(conf->neighboring_address, DEFAULT_NEIGHBORING_ADDRESS, sizeof(conf->neighboring_address));
  conf->neighboring_port = DEFAULT_NEIGHBORING_PORT;
  conf->auto_tethering_enabled = DEFAULT_AUTO_TETHERING_ENABLED;
  conf->max_neighbors = DEFAULT_MAX_NEIGHBORS;
  conf->reconnect_attempt_interval = DEFAULT_RECONNECT_ATTEMPT_INTERVAL;

  return RC_OK;
}
