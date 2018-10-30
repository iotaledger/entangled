/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CONF_H__
#define __CONSENSUS_CONF_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

// Miscellanous conf

#define IOTA_SUPPLY 2779530283277761LL

// Tip selection conf

#define DEFAULT_TIP_SELECTION_MAX_DEPTH 15
#define DEFAULT_TIP_SELECTION_ALPHA 0.001
#define DEFAULT_TIP_SELECTION_BELOW_MAX_DEPTH 20000
#define DEFAULT_TIP_SELECTION_CW_CALC_IMPL DFS_FROM_ENTRY_POINT
#define DEFAULT_TIP_SELECTION_EP_RAND_IMPL EP_RANDOM_WALK

// Snapshot conf

#define DEFAULT_SNAPSHOT_CONF_FILE SNAPSHOT_CONF_FILE
#define DEFAULT_SNAPSHOT_SIG_FILE SNAPSHOT_SIG_FILE
#define DEFAULT_SNAPSHOT_FILE SNAPSHOT_FILE

// Milestone conf

#define DEFAULT_NUM_KEYS_IN_MILESTONE NUM_KEYS_IN_MILESTONE
#define DEFAULT_MWN MWM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_consensus_conf_s {
  // Miscellanous conf
  flex_trit_t genesis_hash[FLEX_TRIT_SIZE_243];
  // Tip selection conf
  size_t max_depth;
  double alpha;
  size_t below_max_depth;
  // Snapshot conf
  char snapshot_conf_file[128];
  char snapshot_sig_file[128];
  char snapshot_file[128];
  uint64_t snapshot_timestamp_sec;
  uint64_t snapshot_signature_index;
  uint64_t snapshot_signature_depth;
  flex_trit_t snapshot_signature_pubkey[FLEX_TRIT_SIZE_243];
  // Milestone conf
  flex_trit_t coordinator[FLEX_TRIT_SIZE_243];
  uint64_t last_milestone;
  size_t num_keys_in_milestone;
  uint8_t mwm;
} iota_consensus_conf_t;

retcode_t iota_consensus_conf_init(iota_consensus_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CONF_H__
