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

#define IOTA_SUPPLY 2779530283277761LL

#define DEFAULT_TIP_SELECTION_MAX_DEPTH 15
#define DEFAULT_TIP_SELECTION_ALPHA 0.001
#define DEFAULT_TIP_SELECTION_BELOW_MAX_DEPTH 20000
#define DEFAULT_TIP_SELECTION_CW_CALC_IMPL DFS_FROM_ENTRY_POINT
#define DEFAULT_TIP_SELECTION_EP_RAND_IMPL EP_RANDOM_WALK
#define DEFAULT_SNAPSHOT_CONF_FILE SNAPSHOT_CONF_FILE
#define DEFAULT_SNAPSHOT_SIG_FILE SNAPSHOT_SIG_FILE
#define DEFAULT_SNAPSHOT_FILE SNAPSHOT_FILE
#define DEFAULT_NUM_KEYS_IN_MILESTONE NUM_KEYS_IN_MILESTONE
#define DEFAULT_MWN MWM

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA consensus
typedef struct iota_consensus_conf_s {
  // The hash of the genesis transaction
  flex_trit_t genesis_hash[FLEX_TRIT_SIZE_243];
  // Limits how many milestones behind the current one the random walk can start
  size_t max_depth;
  // Randomness of the tip selection. Value must be in [0, inf] where 0 is most
  // random and inf is most deterministic
  double alpha;
  // Maximum number of unconfirmed transactions that may be analysed to find the
  // latest referenced milestone by the currently visited transaction during the
  // random walk
  size_t below_max_depth;
  // Path of the snapshot configuration file
  char snapshot_conf_file[128];
  // Path to the file that contains a signature for the snapshot file
  char snapshot_sig_file[128];
  // Path to the file that contains the state of the ledger at the last snapshot
  char snapshot_file[128];
  // Epoch time of the last snapshot
  uint64_t snapshot_timestamp_sec;
  // Index of the snapshot signature
  uint64_t snapshot_signature_index;
  // Depth of the snapshot signature
  uint64_t snapshot_signature_depth;
  // Public key of the snapshot signature
  flex_trit_t snapshot_signature_pubkey[FLEX_TRIT_SIZE_243];
  // The address of the coordinator
  flex_trit_t coordinator[FLEX_TRIT_SIZE_243];
  // The index of the last milestone issues by the corrdinator before the
  // snapshot last
  uint64_t last_milestone;
  // The depth of the Merkle tree which in turn determines the number of leaves
  // (private keys) that the coordinator can use to sign a message
  size_t num_keys_in_milestone;
  // Number of trailing ternary 0s that must appear at the end of a transaction
  // hash. Difficulty can be described as 3^mwm
  uint8_t mwm;
} iota_consensus_conf_t;

/**
 * Initializes consensus configuration with default values
 *
 * @param conf Consensus configuration variables
 *
 * @return a status code
 */
retcode_t iota_consensus_conf_init(iota_consensus_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CONF_H__
