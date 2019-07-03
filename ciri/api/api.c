/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/api/api.h"
#include "ciri/api/feature.h"
#include "common/helpers/pow.h"
#include "common/network/uri.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define API_LOGGER_ID "api"

static logger_id_t logger_id;

/*
 * Private functions
 */

static bool invalid_subtangle_status(iota_api_t const *const api, error_res_t **const error) {
  bool invalid = api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot.metadata.index ==
                 api->core->consensus.milestone_tracker.snapshots_provider->inital_snapshot.metadata.index;

  if (invalid) {
    *error = error_res_new(API_ERROR_INVALID_SUBTANGLE);
  }

  return invalid;
}

/*
 * Public functions
 */

retcode_t iota_api_add_neighbors(iota_api_t const *const api, add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  neighbor_t neighbor;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  res->added_neighbors = 0;

  URIS_FOREACH(req->uris, uri) {
    if ((ret = neighbor_init_with_uri(&neighbor, *uri)) != RC_OK) {
      *error = error_res_new(API_ERROR_INVALID_URI_SCHEME);
      return ret;
    }
    rw_lock_handle_wrlock(&api->core->node.neighbors_lock);
    if (neighbors_add(&api->core->node.neighbors, &neighbor) == RC_OK) {
      log_info(logger_id, "Added neighbor %s\n", *uri);
      res->added_neighbors++;
    } else {
      log_warning(logger_id, "Adding neighbor %s failed\n", *uri);
    }
    rw_lock_handle_unlock(&api->core->node.neighbors_lock);
  }

  return ret;
}

retcode_t iota_api_attach_to_tangle(iota_api_t const *const api, attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  bundle_transactions_t *bundle = NULL;
  iota_transaction_t *tx_iter = NULL;
  flex_trit_t *trytes_iter = NULL;
  iota_transaction_t tx;
  flex_trit_t tx_trytes[FLEX_TRIT_SIZE_8019];

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  bundle_transactions_new(&bundle);

  HASH_ARRAY_FOREACH(req->trytes, trytes_iter) {
    transaction_deserialize_from_trits(&tx, trytes_iter, false);
    bundle_transactions_add(bundle, &tx);
  }

  if ((ret = iota_pow_bundle(bundle, req->trunk, req->branch, req->mwm)) != RC_OK) {
    goto done;
  }

  BUNDLE_FOREACH(bundle, tx_iter) {
    transaction_serialize_on_flex_trits(tx_iter, tx_trytes);
    hash_array_push(res->trytes, tx_trytes);
  }

done:
  bundle_transactions_free(&bundle);

  return RC_OK;
}

retcode_t iota_api_broadcast_transactions(iota_api_t const *const api, broadcast_transactions_req_t const *const req,
                                          error_res_t **const error) {
  retcode_t ret = RC_OK;
  flex_trit_t *elt = NULL;
  iota_transaction_t tx;
  iota_packet_t packet;

  if (api == NULL || req == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (req->trytes == NULL) {
    return ret;
  }

  memset(&packet, 0, sizeof(iota_packet_t));

  HASH_ARRAY_FOREACH(req->trytes, elt) {
    transaction_deserialize_from_trits(&tx, elt, true);
    if (!iota_consensus_transaction_validate(&api->core->consensus.transaction_validator, &tx)) {
      continue;
    }
    flex_trits_to_bytes(packet.content, NUM_TRITS_SERIALIZED_TRANSACTION, elt, NUM_TRITS_SERIALIZED_TRANSACTION,
                        NUM_TRITS_SERIALIZED_TRANSACTION);
    // TODO priority queue on weight_magnitude
    if ((ret = broadcaster_add(&api->core->node.broadcaster, &packet)) != RC_OK) {
      return ret;
    }
  }

  return ret;
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

  if (invalid_subtangle_status(api, error)) {
    return RC_API_INVALID_SUBTANGLE_STATUS;
  }

  CDL_FOREACH(req->tails, iter) {
    bundle_transactions_new(&bundle);
    hash_pack_reset(&pack);
    if ((ret = iota_tangle_transaction_load_partial(tangle, iter->hash, &pack, PARTIAL_TX_MODEL_ESSENCE_METADATA)) !=
        RC_OK) {
      goto done;
    }
    if (pack.num_loaded == 0) {
      ret = RC_API_TAIL_MISSING;
      goto done;
    }
    if (tx.essence.current_index != 0) {
      ret = RC_API_NOT_TAIL;
      goto done;
    }
    if (!tx.metadata.solid) {
      check_consistency_res_info_set(res, API_ERROR_TAILS_NOT_SOLID);
      goto done;
    }
    if ((ret = iota_consensus_bundle_validator_validate(tangle, iter->hash, bundle, &bundle_status)) != RC_OK) {
      goto done;
    }
    if (bundle_status != BUNDLE_VALID || bundle_transactions_size(bundle) == 0) {
      check_consistency_res_info_set(res, API_ERROR_TAILS_BUNDLE_INVALID);
      goto done;
    }
    bundle_transactions_free(&bundle);
  }

  iota_snapshot_read_lock(&api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot);

  if ((ret = iota_consensus_exit_prob_transaction_validator_init(
           &api->core->consensus.conf, &api->core->consensus.milestone_tracker, &api->core->consensus.ledger_validator,
           &walker_validator)) == RC_OK) {
    CDL_FOREACH(req->tails, iter) {
      if ((ret = iota_consensus_exit_prob_transaction_validator_is_valid(&walker_validator, tangle, iter->hash,
                                                                         &res->state)) != RC_OK) {
        break;
      }
      if (!res->state) {
        check_consistency_res_info_set(res, API_ERROR_TAILS_NOT_CONSISTENT);
        break;
      }
    }
  }

  iota_consensus_exit_prob_transaction_validator_destroy(&walker_validator);

  iota_snapshot_unlock(&api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot);

done:
  bundle_transactions_free(&bundle);

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

retcode_t iota_api_get_balances(iota_api_t const *const api, tangle_t *const tangle,
                                get_balances_req_t const *const req, get_balances_res_t *const res,
                                error_res_t **const error) {
  retcode_t ret = RC_OK;
  state_delta_t balances = NULL;
  hash243_queue_t tips = NULL;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (req->threshold <= 0 || req->threshold > 100) {
    return RC_API_GET_BALANCES_INVALID_THRESHOLD;
  }

  iota_snapshot_read_lock(&api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot);

  res->milestone_index =
      iota_snapshot_get_index(&api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot);

  if (req->tips == NULL) {
    if ((ret = hash243_queue_push(&tips, api->core->consensus.milestone_tracker.latest_solid_subtangle_milestone)) !=
        RC_OK) {
      goto done;
    }
  } else {
    tips = req->tips;
  }

  {
    hash243_queue_entry_t *address = NULL;
    int64_t balance = 0;

    CDL_FOREACH(req->addresses, address) {
      balance = 0;
      if ((ret = iota_snapshot_get_balance(&api->core->consensus.snapshots_provider.latest_snapshot, address->hash,
                                           &balance)) != RC_OK &&
          ret != RC_SNAPSHOT_BALANCE_NOT_FOUND) {
        goto done;
      }
      if ((ret = state_delta_add(&balances, address->hash, balance)) != RC_OK) {
        goto done;
      }
    }
  }

  // TODO only if tips provided ?
  {
    hash243_queue_entry_t *tip = NULL;
    hash243_set_t visited_hashes = NULL;
    state_delta_t diff = NULL;
    bool is_valid = false;

    CDL_FOREACH(tips, tip) {
      is_valid = false;
      if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, tip->hash, &is_valid)) != RC_OK) {
        goto done;
      }
      if (!is_valid) {
        ret = RC_API_GET_BALANCES_UNKNOWN_TIP;
        goto done;
      }
      is_valid = false;
      if ((ret = iota_consensus_ledger_validator_update_delta(&api->core->consensus.ledger_validator, tangle,
                                                              &visited_hashes, &diff, tip->hash, &is_valid)) != RC_OK) {
        goto done;
      }
      if (!is_valid) {
        ret = RC_API_GET_BALANCES_INCONSISTENT_TIP;
        goto done;
      }
      if ((ret = get_balances_res_reference_add(res, tip->hash)) != RC_OK) {
        goto done;
      }
    }

    if ((ret = state_delta_apply_patch_if_present(&balances, &diff)) != RC_OK) {
      goto done;
    }
  }

  {
    hash243_queue_entry_t *address = NULL;
    state_delta_entry_t *entry = NULL;

    CDL_FOREACH(req->addresses, address) {
      state_delta_find(balances, address->hash, entry);
      if (entry) {
        if ((ret = get_balances_res_balances_add(res, entry->value)) != RC_OK) {
          goto done;
        }
      }
    }
  }

done:
  iota_snapshot_unlock(&api->core->consensus.milestone_tracker.snapshots_provider->latest_snapshot);
  if (tips != req->tips) {
    hash243_queue_free(&tips);
  }

  return ret;
}

retcode_t iota_api_get_inclusion_states(iota_api_t const *const api, tangle_t *const tangle,
                                        get_inclusion_states_req_t const *const req,
                                        get_inclusion_states_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;

  if (api == NULL || tangle == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  if (invalid_subtangle_status(api, error)) {
    return RC_API_INVALID_SUBTANGLE_STATUS;
  }

  {
    hash243_queue_entry_t *iter = NULL;
    DECLARE_PACK_SINGLE_TX(tx, txp, pack);
    uint64_t lssm_index = api->core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index;

    CDL_FOREACH(req->transactions, iter) {
      hash_pack_reset(&pack);
      if ((ret = iota_tangle_transaction_load_partial(tangle, iter->hash, &pack, PARTIAL_TX_MODEL_METADATA)) != RC_OK) {
        return ret;
      }

      if (pack.num_loaded == 0 || transaction_snapshot_index(txp) == 0 ||
          transaction_snapshot_index(txp) > lssm_index) {
        get_inclusion_states_res_states_add(res, false);
      } else if (transaction_snapshot_index(txp) <= lssm_index) {
        get_inclusion_states_res_states_add(res, true);
      }
    }
  }

  return RC_OK;
}

retcode_t iota_api_get_missing_transactions(iota_api_t const *const api, get_missing_transactions_res_t *const res,
                                            error_res_t **const error) {
  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return requester_get_requested_transactions(&api->core->node.transaction_requester, &res->hashes);
}

retcode_t iota_api_get_neighbors(iota_api_t const *const api, get_neighbors_res_t *const res,
                                 error_res_t **const error) {
  retcode_t ret = RC_OK;
  neighbor_t *iter = NULL;
  char address[MAX_HOST_LENGTH + MAX_PORT_LENGTH + 1];

  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&api->core->node.neighbors_lock);
  LL_FOREACH(api->core->node.neighbors, iter) {
    snprintf(address, MAX_HOST_LENGTH + MAX_PORT_LENGTH + 1, "%s:%d", iter->endpoint.host, iter->endpoint.port);
    if ((ret = get_neighbors_res_add_neighbor(
             res, address, iter->nbr_all_txs, iter->nbr_random_tx_reqs, iter->nbr_new_txs, iter->nbr_invalid_txs,
             iter->nbr_stale_txs, iter->nbr_sent_txs, (iter->endpoint.protocol == PROTOCOL_TCP ? "TCP" : "UDP"))) !=
        RC_OK) {
      break;
    }
  }
  rw_lock_handle_unlock(&api->core->node.neighbors_lock);

  return ret;
}

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
  node_features_set(api->features, res->features);
  memcpy(res->coordinator_address, api->core->consensus.conf.coordinator_address, FLEX_TRIT_SIZE_243);

  return RC_OK;
}

retcode_t iota_api_get_tips(iota_api_t const *const api, get_tips_res_t *const res, error_res_t **const error) {
  if (api == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return tips_cache_get_tips(&api->core->node.tips, &res->hashes);
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

  if (invalid_subtangle_status(api, error)) {
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

retcode_t iota_api_interrupt_attaching_to_tangle(iota_api_t const *const api, error_res_t **const error) {
  if (api == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  return RC_OK;
}

retcode_t iota_api_remove_neighbors(iota_api_t const *const api, remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  neighbor_t neighbor;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  res->removed_neighbors = 0;

  URIS_FOREACH(req->uris, uri) {
    if ((ret = neighbor_init_with_uri(&neighbor, *uri)) != RC_OK) {
      *error = error_res_new(API_ERROR_INVALID_URI_SCHEME);
      return ret;
    }

    rw_lock_handle_wrlock(&api->core->node.neighbors_lock);
    if (neighbors_remove(&api->core->node.neighbors, &neighbor) == RC_OK) {
      log_info(logger_id, "Removed neighbor %s\n", *uri);
      res->removed_neighbors++;
    } else {
      log_warning(logger_id, "Removing neighbor %s failed\n", *uri);
    }
    rw_lock_handle_unlock(&api->core->node.neighbors_lock);
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
    if ((ret = iota_tangle_transaction_store(tangle, &tx)) != RC_OK) {
      return ret;
    }
    if ((ret = iota_consensus_transaction_solidifier_update_status(&api->core->consensus.transaction_solidifier, tangle,
                                                                   &tx)) != RC_OK) {
      log_warning(logger_id, "Updating transaction status failed\n");
      return ret;
    }
    if (transaction_current_index(&tx) == 0 &&
        memcmp(transaction_address(&tx), api->core->consensus.milestone_tracker.conf->coordinator_address,
               FLEX_TRIT_SIZE_243) == 0) {
      ret = iota_milestone_tracker_add_candidate(&api->core->consensus.milestone_tracker, transaction_hash(&tx));
    }
    // TODO store metadata: arrival_time, status, sender (#407)
  }

  return ret;
}

retcode_t iota_api_were_addresses_spent_from(iota_api_t const *const api, tangle_t *const tangle,
                                             spent_addresses_provider_t *const sap,
                                             were_addresses_spent_from_req_t const *const req,
                                             were_addresses_spent_from_res_t *const res, error_res_t **const error) {
  retcode_t ret = RC_OK;
  bool spent = false;
  hash243_queue_entry_t *iter = NULL;

  if (api == NULL || req == NULL || res == NULL || error == NULL) {
    return RC_NULL_PARAM;
  }

  CDL_FOREACH(req->addresses, iter) {
    if ((ret = iota_spent_addresses_service_was_address_spent_from(&api->core->consensus.spent_addresses_service, sap,
                                                                   tangle, iter->hash, &spent)) != RC_OK) {
      return ret;
    }
    if ((ret = were_addresses_spent_from_res_states_add(res, spent)) != RC_OK) {
      return ret;
    }
  }

  return RC_OK;
}

retcode_t iota_api_init(iota_api_t *const api, core_t *const core) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(API_LOGGER_ID, LOGGER_DEBUG, true);
  api->core = core;
  node_features_clear(&api->features);

  return RC_OK;
}

retcode_t iota_api_destroy(iota_api_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_release(logger_id);

  return RC_OK;
}
