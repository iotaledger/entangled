/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "cclient/serialization/json/json_serializer.h"
#include "ciri/api/api.h"
#include "gossip/node.h"
#include "request/requests.h"
#include "response/responses.h"
#include "utils/logger_helper.h"

#define API_LOGGER_ID "api"

/*
 * Private functions
 */

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

retcode_t iota_api_get_node_info(iota_api_t const *const api,
                                 get_node_info_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_get_neighbors(iota_api_t const *const api,
                                 get_neighbors_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_add_neighbors(iota_api_t const *const api,
                                 add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_remove_neighbors(iota_api_t const *const api,
                                    remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_get_tips(iota_api_t const *const api,
                            get_tips_res_t *const res) {
  retcode_t ret = RC_OK;
  hash243_set_t tips = NULL;
  hash243_set_entry_t *iter = NULL;
  hash243_set_entry_t *tmp = NULL;
  tryte_t tip_trytes[HASH_LENGTH_TRYTE + 1];

  if ((ret = tips_cache_get_tips(&api->node->tips, &tips))) {
    goto done;
  }

  HASH_ITER(hh, tips, iter, tmp) {
    // TODO Not necessary when get_tips_res_t will have a hash container
    flex_trits_to_trytes(tip_trytes, HASH_LENGTH_TRYTE, iter->hash,
                         HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
    tip_trytes[HASH_LENGTH_TRYTE] = '\0';
    get_tips_res_add_tip(res, tip_trytes);
  }

done:
  hash243_set_free(&tips);
  return ret;
}

retcode_t iota_api_find_transactions(iota_api_t const *const api,
                                     find_transactions_req_t const *const req,
                                     find_transactions_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_get_trytes(iota_api_t const *const api,
                              get_trytes_req_t const *const req,
                              get_trytes_res_t *const res) {
  retcode_t ret = RC_OK;
  flex_hash_array_t *iter = NULL;
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);

  if (flex_hash_array_count(req->hashes) > api->conf.max_get_trytes) {
    return RC_API_MAX_GET_TRYTES;
  }
  LL_FOREACH(req->hashes, iter) {
    hash_pack_reset(&pack);
    // NOTE Concurrency needs to be taken care of
    if ((ret = iota_tangle_transaction_load(api->tangle, TRANSACTION_FIELD_HASH,
                                            iter->hash, &pack)) != RC_OK) {
      return ret;
    }
    if (pack.num_loaded != 0) {
      transaction_serialize_on_flex_trits(txp, tx_trits);
      flex_trits_to_trytes(tx_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                           tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                           NUM_TRITS_SERIALIZED_TRANSACTION);
    } else {
      memset(tx_trytes, '9', NUM_TRYTES_SERIALIZED_TRANSACTION);
    }
    // TODO Remove when cclient uses hash containers
    tx_trytes[NUM_TRYTES_SERIALIZED_TRANSACTION] = '\0';
    get_trytes_res_add_trytes(res, tx_trytes);
  }
  return ret;
}

retcode_t iota_api_get_inclusion_states(
    iota_api_t const *const api, get_inclusion_state_req_t const *const req,
    get_inclusion_state_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_get_balances(iota_api_t const *const api,
                                get_balances_req_t const *const req,
                                get_balances_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_get_transactions_to_approve(
    iota_api_t const *const api,
    get_transactions_to_approve_req_t const *const req,
    get_transactions_to_approve_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_attach_to_tangle(iota_api_t const *const api,
                                    attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_interrupt_attaching_to_tangle(iota_api_t const *const api) {
  return RC_OK;
}

retcode_t iota_api_broadcast_transactions(
    iota_api_t const *const api,
    broadcast_transactions_req_t const *const req) {
  retcode_t ret = RC_OK;
  flex_hash_array_t *iter = NULL;
  struct _iota_transaction tx;

  LL_FOREACH(req->trytes, iter) {
    transaction_deserialize_from_trits(&tx, iter->hash->trits);
    if (iota_consensus_transaction_validate(api->transaction_validator, &tx)) {
      // TODO priority queue on weight_magnitude
      if ((ret = broadcaster_on_next(&api->node->broadcaster,
                                     iter->hash->trits)) != RC_OK) {
        return ret;
      }
    }
  }
  return ret;
}

retcode_t iota_api_store_transactions(
    iota_api_t const *const api, store_transactions_req_t const *const req) {
  retcode_t ret = RC_OK;
  flex_hash_array_t *iter = NULL;
  struct _iota_transaction tx;
  struct _trit_array hash = {.trits = NULL,
                             .num_trits = HASH_LENGTH_TRIT,
                             .num_bytes = FLEX_TRIT_SIZE_243,
                             .dynamic = 0};
  tx.solid = 0;
  tx.snapshot_index = 0;

  bool exists;
  LL_FOREACH(req->trytes, iter) {
    transaction_deserialize_from_trits(&tx, iter->hash->trits);
    if (iota_consensus_transaction_validate(api->transaction_validator, &tx)) {
      hash.trits = tx.hash;
      if ((ret = iota_tangle_transaction_exist(
               api->tangle, TRANSACTION_FIELD_HASH, &hash, &exists)) != RC_OK) {
        return ret;
      }
      if (!exists) {
        // NOTE Concurrency needs to be taken care of
        if ((ret = iota_tangle_transaction_store(api->tangle, &tx)) != RC_OK) {
          return ret;
        }
        // TODO store metadata: arrival_time, status, sender (#407)
      }
    }
  }
  return ret;
}

retcode_t iota_api_were_addresses_spent_from(
    iota_api_t const *const api, check_consistency_req_t const *const req,
    check_consistency_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_check_consistency(iota_api_t const *const api,
                                     check_consistency_req_t const *const req,
                                     check_consistency_res_t *const res) {
  return RC_OK;
}

retcode_t iota_api_init(iota_api_t *const api, node_t *const node,
                        tangle_t *const tangle,
                        transaction_validator_t *const transaction_validator,
                        serializer_type_t const serializer_type) {
  if (api == NULL) {
    return RC_API_NULL_SELF;
  }

  logger_helper_init(API_LOGGER_ID, LOGGER_DEBUG, true);
  api->running = false;
  api->node = node;
  api->tangle = tangle;
  api->transaction_validator = transaction_validator;
  api->serializer_type = serializer_type;
  if (api->serializer_type == SR_JSON) {
    init_json_serializer(&api->serializer);
  } else {
    return RC_API_SERIALIZER_NOT_IMPLEMENTED;
  }
  return RC_OK;
}

retcode_t iota_api_start(iota_api_t *const api) {
  if (api == NULL) {
    return RC_API_NULL_SELF;
  }

  api->running = true;
  return RC_OK;
}

retcode_t iota_api_stop(iota_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_API_NULL_SELF;
  } else if (api->running == false) {
    return RC_OK;
  }

  api->running = false;
  return ret;
}

retcode_t iota_api_destroy(iota_api_t *const api) {
  if (api == NULL) {
    return RC_API_NULL_SELF;
  } else if (api->running) {
    return RC_API_STILL_RUNNING;
  }

  logger_helper_destroy(API_LOGGER_ID);
  return RC_OK;
}
