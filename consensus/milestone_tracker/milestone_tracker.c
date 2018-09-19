/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "ciri/conf/conf_values.h"
#include "common/model/milestone.h"
#include "common/sign/v1/iss_curl.h"
#include "common/storage/sql/defs.h"
#include "common/trinary/trit_long.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "mam/v1/merkle.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/time.h"

#define MILESTONE_TRACKER_LOGGER_ID "consensus_milestone_tracker"

static retcode_t validate_milestone(milestone_tracker_t* const mt,
                                    iota_milestone_t* const candidate) {
  retcode_t ret = RC_OK;
  bool exists = false;

  if (candidate->index >= 0x200000) {
    return RC_OK;
  }

  // Checking if milestone is already persisted/validated in database
  TRIT_ARRAY_DECLARE(hash, NUM_TRITS_HASH);
  memcpy(hash.trits, candidate->hash, FLEX_TRIT_SIZE_243);
  if ((ret = iota_tangle_milestone_exist(mt->tangle, MILESTONE_COL_HASH, &hash,
                                         &exists))) {
    goto done;
  } else if (exists) {
    goto valid;
  }

  {  // TODO remove
    iota_tangle_milestone_store(mt->tangle, candidate);
    goto valid;
  }
  // final List<List<TransactionViewModel>> bundleTransactions =
  //     BundleValidator.validate(tangle, transactionViewModel.getHash());
  // if (bundleTransactions.size() == 0) {
  //   return INCOMPLETE;
  // } else {
  //   for (final List<TransactionViewModel> bundleTransactionViewModels :
  //        bundleTransactions) {
  //     // if
  //     //
  //     (Arrays.equals(bundleTransactionViewModels.get(0).getHash(),transactionViewModel.getHash()))
  //     // {
  //     if (bundleTransactionViewModels.get(0).getHash().equals(
  //             transactionViewModel.getHash())) {
  //       // final TransactionViewModel transactionViewModel2 =
  //       //
  //       StorageTransactions.instance().loadTransaction(transactionViewModel.trunkTransactionPointer);
  //       final TransactionViewModel transactionViewModel2 =
  //           transactionViewModel.getTrunkTransaction(tangle);
  //       if (transactionViewModel2.getType() ==
  //               TransactionViewModel.FILLED_SLOT &&
  //           transactionViewModel.getBranchTransactionHash().equals(
  //               transactionViewModel2.getTrunkTransactionHash()) &&
  //           transactionViewModel.getBundleHash().equals(
  //               transactionViewModel2.getBundleHash())) {
  //         final byte[] trunkTransactionTrits =
  //             transactionViewModel.getTrunkTransactionHash().trits();
  //         final byte[] signatureFragmentTrits = Arrays.copyOfRange(
  //             transactionViewModel.trits(),
  //             TransactionViewModel.SIGNATURE_MESSAGE_FRAGMENT_TRINARY_OFFSET,
  //             TransactionViewModel.SIGNATURE_MESSAGE_FRAGMENT_TRINARY_OFFSET
  //             +
  //                 TransactionViewModel.SIGNATURE_MESSAGE_FRAGMENT_TRINARY_SIZE);
  //
  //         final byte[] merkleRoot = ISS.getMerkleRoot(
  //             mode,
  //             ISS.address(
  //                 mode, ISS.digest(mode,
  //                                  Arrays.copyOf(ISS.normalizedBundle(
  //                                                    trunkTransactionTrits),
  //                                                ISS.NUMBER_OF_FRAGMENT_CHUNKS),
  //                                  signatureFragmentTrits)),
  //             transactionViewModel2.trits(), 0, index, numOfKeysInMilestone);
  //         if ((testnet && acceptAnyTestnetCoo) ||
  //             (new Hash(merkleRoot)).equals(coordinator)) {
  //           new MilestoneViewModel(index, transactionViewModel.getHash())
  //               .store(tangle);
  //           return VALID;
  //         } else {
  //           return INVALID;
  //         }
  //       }
  //     }
  //   }
  // }
  // return INVALID;

valid:
  if (candidate->index > mt->latest_milestone_index) {
    mt->latest_milestone_index = candidate->index;
    memcpy(mt->latest_milestone->trits, candidate->hash, FLEX_TRIT_SIZE_243);
  }
done:
  return ret;
}

static uint64_t get_milestone_index(iota_transaction_t const tx) {
  trit_t buffer[NUM_TRITS_OBSOLETE_TAG];

  flex_trits_to_trits(buffer, NUM_TRITS_OBSOLETE_TAG, tx->obsolete_tag,
                      NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
  return trits_to_long(buffer, NUM_TRITS_VALUE);
}

static void* latest_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  iota_stor_pack_t hash_pack;
  iota_milestone_t candidate;
  struct _iota_transaction tx;
  iota_transaction_t tx_ptr = &tx;
  iota_stor_pack_t tx_pack = {(void**)&tx_ptr, 1, 0, false};
  uint64_t scan_time, previous_latest_milestone_index;

  if (mt == NULL) {
    return NULL;
  }

  hash_pack_init(&hash_pack, MILESTONE_TRACKER_INITIAL_HASH_PACK_SIZE);
  while (mt->running) {
    log_debug(MILESTONE_TRACKER_LOGGER_ID, "Scanning for latest milestone\n");
    scan_time = current_timestamp_ms();
    previous_latest_milestone_index = mt->latest_milestone_index;

    hash_pack.num_loaded = 0;
    hash_pack.insufficient_capacity = false;
    if (iota_tangle_transaction_load_hashes(mt->tangle, TRANSACTION_COL_ADDRESS,
                                            mt->coordinator,
                                            &hash_pack) == RC_OK) {
      for (size_t i = 0; i < hash_pack.num_loaded; ++i) {
        tx_pack.num_loaded = 0;
        tx_pack.insufficient_capacity = false;
        if (iota_tangle_transaction_load(mt->tangle, TRANSACTION_COL_HASH,
                                         hash_pack.models[i],
                                         &tx_pack) == RC_OK) {
          if (tx_pack.num_loaded > 0 && tx.current_index == 0) {
            candidate.index = get_milestone_index(&tx);
            memcpy(candidate.hash, tx.hash, FLEX_TRIT_SIZE_243);
            if (validate_milestone(mt, &candidate) != RC_OK) {
              log_warning(MILESTONE_TRACKER_LOGGER_ID,
                          "Validating milestone failed\n");
            }
          }
        }
      }
      if (previous_latest_milestone_index != mt->latest_milestone_index) {
        // TODO messageQ publish lmi
        log_info(MILESTONE_TRACKER_LOGGER_ID,
                 "Latest milestone has changed from #%" PRIu64 " to #%" PRIu64
                 "\n",
                 previous_latest_milestone_index, mt->latest_milestone_index);
      }
    }
    sleep_ms(MAX(1, LATEST_MILESTONE_RESCAN_INTERVAL -
                        (current_timestamp_ms() - scan_time)));
  }
  hash_pack_free(&hash_pack);
  return NULL;
}

static retcode_t update_latest_solid_subtangle_milestone(
    milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;
  iota_milestone_t milestone, latest_milestone;
  iota_milestone_t *milestone_ptr = &milestone,
                   *latest_milestone_ptr = &latest_milestone;
  iota_stor_pack_t pack = {(void**)&latest_milestone_ptr, 1, 0, false};

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }
  if ((ret = iota_tangle_milestone_load_latest(mt->tangle, &pack)) != RC_OK) {
    return ret;
  }
  if (pack.num_loaded != 0) {
    pack.num_loaded = 0;
    pack.models = (void**)&milestone_ptr;
    if ((ret = iota_tangle_milestone_load_next(
             mt->tangle, mt->latest_solid_subtangle_milestone_index, &pack)) !=
        RC_OK) {
      return ret;
    }
    while (pack.num_loaded != 0 && milestone.index <= latest_milestone.index &&
           mt->running) {
      if (true  // TODO
                // transactionValidator.checkSolidity(milestoneViewModel.getHash(),
                // true)
          && milestone.index >= mt->latest_solid_subtangle_milestone_index &&
          true /* TODO ledgerValidator.updateSnapshot(milestoneViewModel) */) {
        mt->latest_solid_subtangle_milestone_index = milestone.index;
        memcpy(mt->latest_solid_subtangle_milestone->trits, milestone.hash,
               FLEX_TRIT_SIZE_243);
      } else {
        break;
      }
      pack.num_loaded = 0;
      if ((ret = iota_tangle_milestone_load_next(
               mt->tangle, mt->latest_solid_subtangle_milestone_index,
               &pack)) != RC_OK) {
        return ret;
      }
    }
  }
  return ret;
}

static void* solid_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  uint64_t scan_time, previous_solid_subtangle_latest_milestone_index;

  if (mt != NULL) {
    while (mt->running) {
      log_debug(MILESTONE_TRACKER_LOGGER_ID,
                "Scanning for latest solid subtangle milestone\n");
      scan_time = current_timestamp_ms();
      previous_solid_subtangle_latest_milestone_index =
          mt->latest_solid_subtangle_milestone_index;
      if (mt->latest_solid_subtangle_milestone_index <
          mt->latest_milestone_index) {
        if (update_latest_solid_subtangle_milestone(mt) != RC_OK) {
          log_warning(MILESTONE_TRACKER_LOGGER_ID,
                      "Updating latest solid subtangle milestone failed\n");
        }
      }
      if (previous_solid_subtangle_latest_milestone_index !=
          mt->latest_solid_subtangle_milestone_index) {
        // TODO messageQ publish lmsi/lmhs
        log_info(MILESTONE_TRACKER_LOGGER_ID,
                 "Latest solid subtangle milestone has changed from #%" PRIu64
                 " to #%" PRIu64 "\n",
                 previous_solid_subtangle_latest_milestone_index,
                 mt->latest_solid_subtangle_milestone_index);
      }
      sleep_ms(MAX(1, SOLID_MILESTONE_RESCAN_INTERVAL -
                          (current_timestamp_ms() - scan_time)));
    }
  }
  return NULL;
}

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                      tangle_t* const tangle,
                                      snapshot_t* const snapshot,
                                      bool testnet) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (tangle == NULL) {
    return RC_CONSENSUS_MT_NULL_TANGLE;
  }

  logger_helper_init(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(mt, 0, sizeof(milestone_tracker_t));
  mt->running = false;
  mt->testnet = testnet;
  mt->tangle = tangle;
  mt->latest_snapshot = snapshot;
  if ((mt->latest_milestone = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    goto oom;
  }
  if ((mt->latest_solid_subtangle_milestone = trit_array_new(NUM_TRITS_HASH)) ==
      NULL) {
    goto oom;
  }
  if (mt->testnet) {
    if ((mt->coordinator = trit_array_new_from_trytes(
             (tryte_t*)TESTNET_COORDINATOR_ADDRESS)) == NULL) {
      goto oom;
    }
    mt->milestone_start_index = TESTNET_MILESTONE_START_INDEX;
    mt->num_keys_in_milestone = TESTNET_NUM_KEYS_IN_MILESTONE;
  } else {
    if ((mt->coordinator = trit_array_new_from_trytes(
             (tryte_t*)MAINNET_COORDINATOR_ADDRESS)) == NULL) {
      goto oom;
    }
    mt->milestone_start_index = MAINNET_MILESTONE_START_INDEX;
    mt->num_keys_in_milestone = MAINNET_NUM_KEYS_IN_MILESTONE;
  }
  mt->latest_milestone_index = mt->milestone_start_index;
  mt->latest_solid_subtangle_milestone_index = mt->milestone_start_index;

  return RC_OK;

oom:
  if (mt->latest_milestone) {
    trit_array_free(mt->latest_milestone);
  }
  if (mt->latest_solid_subtangle_milestone) {
    trit_array_free(mt->latest_solid_subtangle_milestone);
  }
  if (mt->coordinator) {
    trit_array_free(mt->coordinator);
  }
  return RC_CONSENSUS_MT_OOM;
}

retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  mt->running = true;

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning latest milestone tracker thread\n");
  if (thread_handle_create(&mt->latest_milestone_tracker,
                           (thread_routine_t)latest_milestone_tracker,
                           mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning latest milestone tracker thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning solid milestone tracker thread\n");
  if (thread_handle_create(&mt->solid_milestone_tracker,
                           (thread_routine_t)solid_milestone_tracker,
                           mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning solid milestone tracker thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (mt->running == false) {
    return RC_OK;
  }

  mt->running = false;

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down latest milestone tracker thread\n");
  if (thread_handle_join(mt->latest_milestone_tracker, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down latest milestone tracker thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down solid milestone tracker thread\n");
  if (thread_handle_join(mt->solid_milestone_tracker, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down solid milestone tracker thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (mt->running) {
    return RC_CONSENSUS_MT_STILL_RUNNING;
  }

  trit_array_free(mt->latest_milestone);
  trit_array_free(mt->latest_solid_subtangle_milestone);
  trit_array_free(mt->coordinator);
  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);

  return ret;
}
