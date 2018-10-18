/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "cclient/serialization/json/json_serializer.h"
#include "ciri/api/api.h"
#include "request/requests.h"
#include "response/responses.h"
#include "utils/logger_helper.h"

#define API_LOGGER_ID "api"

/*
 * Private functions
 */

static retcode_t get_node_info(get_node_info_res_t *const res) { return RC_OK; }

static retcode_t get_neighbors(get_neighbors_res_t *const res) { return RC_OK; }

static retcode_t add_neighbors(add_neighbors_req_t const *const req,
                               add_neighbors_res_t *const res) {
  return RC_OK;
}

static retcode_t remove_neighbors(remove_neighbors_req_t const *const req,
                                  remove_neighbors_res_t *const res) {
  return RC_OK;
}

static retcode_t get_tips(get_tips_res_t *const res) { return RC_OK; }

static retcode_t find_transactions(find_transactions_req_t const *const req,
                                   find_transactions_res_t *const res) {
  return RC_OK;
}

static retcode_t get_trytes(get_trytes_req_t const *const req,
                            get_trytes_res_t *const res) {
  return RC_OK;
}

static retcode_t get_inclusion_states(
    get_inclusion_state_req_t const *const req,
    get_inclusion_state_res_t *const res) {
  return RC_OK;
}

static retcode_t get_balances(get_balances_req_t const *const req,
                              get_balances_res_t *const res) {
  return RC_OK;
}

static retcode_t get_transactions_to_approve(
    get_transactions_to_approve_req_t const *const req,
    get_transactions_to_approve_res_t *const res) {
  return RC_OK;
}

static retcode_t attach_to_tangle(attach_to_tangle_req_t const *const req,
                                  attach_to_tangle_res_t *const res) {
  return RC_OK;
}

static retcode_t interrupt_attaching_to_tangle() { return RC_OK; }

static retcode_t broadcast_transactions(
    broadcast_transactions_req_t const *const req) {
  return RC_OK;
}

static retcode_t store_transactions(store_transactions_req_t const *const req) {
  return RC_OK;
}

static retcode_t check_consistency(check_consistency_req_t const *const req,
                                   check_consistency_res_t *const res) {
  return RC_OK;
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
      {"checkConsistency", CMD_CHECK_CONSISTENCY},
      {NULL, CMD_UNKNOWN},
  };
  struct iota_api_command_map_s *p = map;
  for (; p->string != NULL && strcmp(p->string, command) != 0; ++p)
    ;
  return p->value;
}

static void *iota_api_routine(void *arg) {
  iota_api_t *api = (iota_api_t *)arg;
  retcode_t ret = RC_OK;

  while (api->running) {
    // TODO Get request from client
    // TODO Parse request
    switch (get_command("getNeighbors")) {
      case CMD_GET_NODE_INFO: {
      } break;
      case CMD_GET_NEIGHBORS: {
        get_neighbors_res_t *res = get_neighbors_res_new();
        get_neighbors(res);
        // TODO serialize response
        get_neighbors_res_free(res);
      } break;
      case CMD_ADD_NEIGHBORS: {
      } break;
      case CMD_REMOVE_NEIGHBORS: {
      } break;
      case CMD_GET_TIPS: {
      } break;
      case CMD_FIND_TRANSACTIONS: {
      } break;
      case CMD_GET_TRYTES: {
      } break;
      case CMD_GET_INCLUSION_STATES: {
      } break;
      case CMD_GET_BALANCES: {
      } break;
      case CMD_GET_TRANSACTIONS_TO_APPROVE: {
      } break;
      case CMD_ATTACH_TO_TANGLE: {
      } break;
      case CMD_INTERRUPT_ATTACHING_TO_TANGLE: {
      } break;
      case CMD_BROADCAST_TRANSACTIONS: {
      } break;
      case CMD_STORE_TRANSACTIONS: {
      } break;
      case CMD_CHECK_CONSISTENCY: {
      } break;
      default: { } break; }
    // TODO Send response back to client
    sleep(1);
  }
  return NULL;
}

/*
 * Public functions
 */

retcode_t iota_api_init(iota_api_t *const api, uint16_t port,
                        serializer_type_t serializer_type) {
  if (api == NULL) {
    return RC_API_NULL_SELF;
  }

  logger_helper_init(API_LOGGER_ID, LOGGER_DEBUG, true);
  memset(api, 0, sizeof(iota_api_t));
  api->running = false;
  api->port = port;
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
  log_info(API_LOGGER_ID, "Spawning cIRI API thread\n");
  if (thread_handle_create(&api->thread, (thread_routine_t)iota_api_routine,
                           api) != 0) {
    log_critical(API_LOGGER_ID, "Spawning cIRI API thread failed\n");
    return RC_API_FAILED_THREAD_SPAWN;
  }
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

  log_info(API_LOGGER_ID, "Shutting down cIRI API thread\n");
  if (thread_handle_join(api->thread, NULL) != 0) {
    log_error(API_LOGGER_ID, "Shutting down cIRI API thread failed\n");
    ret = RC_API_FAILED_THREAD_JOIN;
  }
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
