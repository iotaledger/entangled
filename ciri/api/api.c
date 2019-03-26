/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/api/api.h"
#include "gossip/node.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define API_LOGGER_ID "api"

static logger_id_t logger_id;

/*
 * Private functions
 */

static bool invalid_subtangle_status(iota_api_t const *const api) {
  return (api->core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index ==
          api->core->consensus.milestone_tracker.milestone_start_index);
}

typedef enum iota_api_command_e {
  CMD_GET_NODE_INFO,
  CMD_GET_NEIGHBORS,
  CMD_ADD_NEIGHBORS,
  CMD_REMOVE_NEIGHBORS,
  CMD_GET_TIPS,
  CMD_FIND_TRANSACTIONS,
  CMD_GET_TRYTES,
  CMD_GET_INCLUSION_STATES,
  CMD_GET_BALANCES,
  CMD_GET_TRANSACTIONS_TO_APPROVE,
  CMD_ATTACH_TO_TANGLE,
  CMD_INTERRUPT_ATTACHING_TO_TANGLE,
  CMD_BROADCAST_TRANSACTIONS,
  CMD_STORE_TRANSACTIONS,
  CMD_WERE_ADDRESSES_SPENT_FROM,
  CMD_CHECK_CONSISTENCY,
  CMD_UNKNOWN
} iota_api_command_t;

static iota_api_command_t get_command(char const *const command) {
  static struct iota_api_command_map_s {
    char const *const string;
    iota_api_command_t const value;
  } map[] = {
      {"getNodeInfo", CMD_GET_NODE_INFO},
      {"getNeighbors", CMD_GET_NEIGHBORS},
      {"addNeighbors", CMD_ADD_NEIGHBORS},
      {"removeNeighbors", CMD_REMOVE_NEIGHBORS},
      {"getTips", CMD_GET_TIPS},
      {"findTransactions", CMD_FIND_TRANSACTIONS},
      {"getTrytes", CMD_GET_TRYTES},
      {"getInclusionStates", CMD_GET_INCLUSION_STATES},
      {"getBalances", CMD_GET_BALANCES},
      {"getTransactionsToApprove", CMD_GET_TRANSACTIONS_TO_APPROVE},
      {"attachToTangle", CMD_ATTACH_TO_TANGLE},
      {"interruptAttachingToTangle", CMD_INTERRUPT_ATTACHING_TO_TANGLE},
      {"broadcastTransactions", CMD_BROADCAST_TRANSACTIONS},
      {"storeTransactions", CMD_STORE_TRANSACTIONS},
      {"wereAddressesSpentFrom", CMD_WERE_ADDRESSES_SPENT_FROM},
      {"checkConsistency", CMD_CHECK_CONSISTENCY},
      {NULL, CMD_UNKNOWN},
  };
  struct iota_api_command_map_s *p = map;
  for (; p->string != NULL && strcmp(p->string, command) != 0; ++p)
    ;
  return p->value;
}

/*
 * Public functions
 */

retcode_t iota_api_get_node_info(iota_api_t const *const api, get_node_info_res_t *const res,
                                 error_res_t **const error) {
  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  char_buffer_set(res->app_name, CIRI_NAME);
  char_buffer_set(res->app_version, CIRI_VERSION);
  memcpy(res->latest_milestone, api->core->consensus.milestone_tracker.latest_milestone, FLEX_TRIT_SIZE_243);
  res->latest_milestone_index = api->core->consensus.milestone_tracker.latest_milestone_index;
  memcpy(res->latest_solid_subtangle_milestone, api->core->consensus.milestone_tracker.latest_solid_subtangle_milestone,
         FLEX_TRIT_SIZE_243);
  res->latest_solid_subtangle_milestone_index =
      api->core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index;
  res->milestone_start_index = api->core->consensus.milestone_tracker.milestone_start_index;
  rw_lock_handle_rdlock(&api->core->node.neighbors_lock);
  res->neighbors = neighbors_count(api->core->node.neighbors);
  rw_lock_handle_unlock(&api->core->node.neighbors_lock);
  res->packets_queue_size = broadcaster_size(&api->core->node.broadcaster);
  res->time = current_timestamp_ms();
  res->tips = tips_cache_size(&api->core->node.tips);
  res->transactions_to_request = requester_size(&api->core->node.transaction_requester);
  memcpy(res->coordinator_address, api->core->consensus.conf.coordinator_address, FLEX_TRIT_SIZE_243);

  return RC_OK;
}

retcode_t iota_api_get_neighbors(iota_api_t const *const api, get_neighbors_res_t *const res,
                                 error_res_t **const error) {
  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_add_neighbors(iota_api_t const *const api, add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res, error_res_t **const error) {
  char **uri;
  neighbor_t neighbor;
  retcode_t ret = RC_OK;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  res->added_neighbors = 0;
  for (uri = (char **)utarray_front(req->uris); uri != NULL; uri = (char **)utarray_next(req->uris, uri)) {
    if ((ret = neighbor_init_with_uri(&neighbor, *uri)) != RC_OK) {
      return ret;
    }
    log_info(logger_id, "Adding neighbor %s\n", *uri);
    rw_lock_handle_wrlock(&api->core->node.neighbors_lock);
    if (neighbors_add(&api->core->node.neighbors, &neighbor) == RC_OK) {
      res->added_neighbors++;
    } else {
      log_warning(logger_id, "Adding neighbor %s failed\n", *uri);
    }
    rw_lock_handle_unlock(&api->core->node.neighbors_lock);
  }

  return ret;
}

retcode_t iota_api_remove_neighbors(iota_api_t const *const api, remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res, error_res_t **const error) {
  char **uri;
  neighbor_t neighbor;
  retcode_t ret = RC_OK;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  res->removed_neighbors = 0;
  for (uri = (char **)utarray_front(req->uris); uri != NULL; uri = (char **)utarray_next(req->uris, uri)) {
    if ((ret = neighbor_init_with_uri(&neighbor, *uri)) != RC_OK) {
      return ret;
    }
    log_info(logger_id, "Removing neighbor %s\n", *uri);
    rw_lock_handle_wrlock(&api->core->node.neighbors_lock);
    if (neighbors_remove(&api->core->node.neighbors, &neighbor) == RC_OK) {
      res->removed_neighbors++;
    } else {
      log_warning(logger_id, "Removing neighbor %s failed\n", *uri);
    }
    rw_lock_handle_unlock(&api->core->node.neighbors_lock);
  }

  return ret;
}

retcode_t iota_api_get_tips(iota_api_t const *const api, get_tips_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  hash243_set_t tips = NULL;
  hash243_set_entry_t *iter = NULL;
  hash243_set_entry_t *tmp = NULL;

  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = tips_cache_get_tips(&api->core->node.tips, &tips)) != RC_OK) {
    goto done;
  }

  HASH_ITER(hh, tips, iter, tmp) {
    if ((ret = hash243_stack_push(&res->hashes, iter->hash)) != RC_OK) {
      goto done;
    }
  }

done:
  hash243_set_free(&tips);
  return ret;
}

retcode_t iota_api_find_transactions(iota_api_t const *const api, tangle_t *const tangle,
                                     find_transactions_req_t const *const req, find_transactions_res_t *const res,
                                     error_res_t **const error) {
  retcode_t ret = RC_OK;
  iota_stor_pack_t pack;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (hash243_queue_count(req->bundles) == 0 && hash243_queue_count(req->addresses) == 0 &&
      hash81_queue_count(req->tags) == 0 && hash243_queue_count(req->approvees) == 0) {
    return RC_API_FIND_TRANSACTIONS_NO_INPUT;
  }

  // TODO Refactor stor_pack #618
  hash_pack_init(&pack, 1024);

  if ((ret = iota_tangle_transaction_find(tangle, req->bundles, req->addresses, req->tags, req->approvees, &pack)) !=
      RC_OK) {
    goto done;
  }

  if (pack.num_loaded > api->conf.max_find_transactions) {
    ret = RC_API_MAX_FIND_TRANSACTIONS;
    goto done;
  }

  for (size_t i = 0; i < pack.num_loaded; i++) {
    if ((ret = hash243_queue_push(&res->hashes, (pack.models)[i])) != RC_OK) {
      goto done;
    }
  }

done:
  hash_pack_free(&pack);
  return ret;
}

retcode_t iota_api_get_trytes(iota_api_t const *const api, tangle_t *const tangle, get_trytes_req_t const *const req,
                              get_trytes_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  hash243_queue_entry_t *iter = NULL;
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (hash243_queue_count(req->hashes) > api->conf.max_get_trytes) {
    return RC_API_MAX_GET_TRYTES;
  }

  CDL_FOREACH(req->hashes, iter) {
    hash_pack_reset(&pack);
    // NOTE Concurrency needs to be taken care of
    if ((ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, iter->hash, &pack)) != RC_OK) {
      return ret;
    }

    if (pack.num_loaded != 0) {
      transaction_serialize_on_flex_trits(txp, tx_trits);
    } else {
      memset(tx_trits, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
    }

    hash8019_queue_push(&res->trytes, tx_trits);
  }

  return ret;
}

retcode_t iota_api_get_inclusion_states(iota_api_t const *const api, get_inclusion_states_req_t const *const req,
                                        get_inclusion_states_res_t *const res, error_res_t **const error) {
  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_get_balances(iota_api_t const *const api, get_balances_req_t const *const req,
                                get_balances_res_t *const res, error_res_t **const error) {
  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_get_transactions_to_approve(iota_api_t const *const api, tangle_t *const tangle,
                                               get_transactions_to_approve_req_t const *const req,
                                               get_transactions_to_approve_res_t *const res,
                                               error_res_t **const error) {
  retcode_t ret = RC_OK;
  tips_pair_t tips;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (req->depth > api->core->consensus.conf.max_depth) {
    return RC_API_INVALID_DEPTH_INPUT;
  }

  if (invalid_subtangle_status(api)) {
    return RC_API_INVALID_SUBTANGLE_STATUS;
  }

  if ((ret = iota_consensus_tip_selector_get_transactions_to_approve(&api->core->consensus.tip_selector, tangle,
                                                                     req->depth, req->reference, &tips)) != RC_OK) {
    return ret;
  }

  get_transactions_to_approve_res_set_branch(res, tips.branch);
  get_transactions_to_approve_res_set_trunk(res, tips.trunk);

  return ret;
}

retcode_t iota_api_attach_to_tangle(iota_api_t const *const api, attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res, error_res_t **const error) {
  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_interrupt_attaching_to_tangle(iota_api_t const *const api, error_res_t **const error) {
  if (api == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_broadcast_transactions(iota_api_t const *const api, broadcast_transactions_req_t const *const req,
                                          error_res_t **const error) {
  retcode_t ret = RC_OK;
  flex_trit_t *elt = NULL;
  iota_transaction_t tx;

  if (api == NULL || req == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (req->trytes == NULL) {
    return ret;
  }

  HASH_ARRAY_FOREACH(req->trytes, elt) {
    transaction_deserialize_from_trits(&tx, elt, true);
    if (!iota_consensus_transaction_validate(&api->core->consensus.transaction_validator, &tx)) {
      continue;
    }
    // TODO priority queue on weight_magnitude
    if ((ret = broadcaster_on_next(&api->core->node.broadcaster, elt)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}

retcode_t iota_api_store_transactions(iota_api_t const *const api, tangle_t *const tangle,
                                      store_transactions_req_t const *const req, error_res_t **const error) {
  retcode_t ret = RC_OK;
  flex_trit_t *elt = NULL;
  iota_transaction_t tx;
  bool exists;

  if (api == NULL || req == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  HASH_ARRAY_FOREACH(req->trytes, elt) {
    transaction_deserialize_from_trits(&tx, elt, true);
    if (!iota_consensus_transaction_validate(&api->core->consensus.transaction_validator, &tx)) {
      continue;
    }
    if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, transaction_hash(&tx), &exists)) !=
        RC_OK) {
      return ret;
    }
    if (exists) {
      continue;
    }
    // NOTE Concurrency needs to be taken care of
    if ((ret = iota_tangle_transaction_store(tangle, &tx)) != RC_OK) {
      return ret;
    }
    if ((ret = iota_consensus_transaction_solidifier_update_status(&api->core->consensus.transaction_solidifier, tangle,
                                                                   &tx)) != RC_OK) {
      log_warning(logger_id, "Updating transaction status failed\n");
      return ret;
    }
    // TODO store metadata: arrival_time, status, sender (#407)
  }

  return ret;
}

retcode_t iota_api_were_addresses_spent_from(iota_api_t const *const api, check_consistency_req_t const *const req,
                                             check_consistency_res_t *const res, error_res_t **const error) {
  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_check_consistency(iota_api_t const *const api, tangle_t *const tangle,
                                     check_consistency_req_t const *const req, check_consistency_res_t *const res,
                                     error_res_t **const error) {
  retcode_t ret = RC_OK;
  hash243_queue_entry_t *iter = NULL;
  bundle_transactions_t *bundle = NULL;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  exit_prob_transaction_validator_t walker_validator;
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  res->state = false;

  if (invalid_subtangle_status(api)) {
    return RC_API_INVALID_SUBTANGLE_STATUS;
  }

  CDL_FOREACH(req->tails, iter) {
    bundle_transactions_new(&bundle);
    hash_pack_reset(&pack);
    if ((ret = iota_tangle_transaction_load_partial(tangle, iter->hash, &pack, PARTIAL_TX_MODEL_ESSENCE_METADATA)) !=
        RC_OK) {
    } else if (pack.num_loaded == 0) {
      ret = RC_API_TAIL_MISSING;
    } else if (tx.essence.current_index != 0) {
      ret = RC_API_NOT_TAIL;
    } else if (!tx.metadata.solid) {
      check_consistency_res_info_set(res, API_TAILS_NOT_SOLID);
    } else if ((ret = iota_consensus_bundle_validator_validate(tangle, iter->hash, bundle, &bundle_status)) != RC_OK) {
    } else if (bundle_status != BUNDLE_VALID || bundle_transactions_size(bundle) == 0) {
      check_consistency_res_info_set(res, API_TAILS_BUNDLE_INVALID);
    }
    bundle_transactions_free(&bundle);
    if (ret != RC_OK || (res->info != NULL && res->info->data != NULL)) {
      return ret;
    }
  }

  rw_lock_handle_rdlock(&api->core->consensus.milestone_tracker.latest_snapshot->rw_lock);

  if ((ret = iota_consensus_exit_prob_transaction_validator_init(
           &api->core->consensus.conf, &api->core->consensus.milestone_tracker, &api->core->consensus.ledger_validator,
           &walker_validator)) == RC_OK) {
    CDL_FOREACH(req->tails, iter) {
      if ((ret = iota_consensus_exit_prob_transaction_validator_is_valid(&walker_validator, tangle, iter->hash,
                                                                         &res->state)) != RC_OK) {
        break;
      } else if (!res->state) {
        check_consistency_res_info_set(res, API_TAILS_NOT_CONSISTENT);
        break;
      }
    }
  }

  iota_consensus_exit_prob_transaction_validator_destroy(&walker_validator);

  rw_lock_handle_unlock(&api->core->consensus.milestone_tracker.latest_snapshot->rw_lock);

  return ret;
}

retcode_t iota_api_init(iota_api_t *const api, core_t *const core) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(API_LOGGER_ID, LOGGER_DEBUG, true);
  api->core = core;

  return RC_OK;
}

retcode_t iota_api_destroy(iota_api_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_release(logger_id);

  return RC_OK;
}
