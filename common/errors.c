/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/errors.h"

const char* error_2_string(retcode_t err) {
  switch (err) {
    case RC_OK:
      return STR_OK;

    // Storage SQLite3 Module
    case RC_STORAGE_OOM:
    case RC_SQLITE3_FAILED_OPEN_DB:
    case RC_SQLITE3_FAILED_INSERT_DB:
    case RC_SQLITE3_FAILED_CREATE_INDEX_DB:
    case RC_SQLITE3_FAILED_WRITE_STATEMENT:
    case RC_SQLITE3_FAILED_SELECT_DB:
    case RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED:
    case RC_SQLITE3_FAILED_NOT_IMPLEMENTED:
    case RC_SQLITE3_FAILED_START_TRANSACTION:
    case RC_SQLITE3_FAILED_END_TRANSACTION:
    case RC_SQLITE3_FAILED_BINDING:
    case RC_SQLITE3_FAILED_PREPARED_STATEMENT:
    case RC_SQLITE3_FAILED_FINALIZE:
    case RC_SQLITE3_FAILED_STEP:
    // Storage SQL Module
    case RC_SQL_FAILED_WRITE_STATEMENT:
    // Core Module
    case RC_CORE_NULL_CORE:
    case RC_CORE_FAILED_DATABASE_INIT:
    case RC_CORE_FAILED_DATABASE_DESTROY:
    case RC_CORE_FAILED_NODE_INIT:
    case RC_CORE_FAILED_NODE_START:
    case RC_CORE_FAILED_NODE_STOP:
    case RC_CORE_FAILED_NODE_DESTROY:
    case RC_CORE_FAILED_API_INIT:
    case RC_CORE_FAILED_API_START:
    case RC_CORE_FAILED_API_STOP:
    case RC_CORE_FAILED_API_DESTROY:
    case RC_CORE_FAILED_SNAPSHOT_INIT:
    case RC_CORE_FAILED_SNAPSHOT_DESTROY:
    case RC_CORE_STILL_RUNNING:
    // Node Module
    case RC_NODE_NULL_NODE:
    case RC_NODE_NULL_CORE:
    case RC_NODE_FAILED_NEIGHBORS_INIT:
    case RC_NODE_FAILED_NEIGHBORS_DESTROY:
    case RC_NODE_FAILED_BROADCASTER_INIT:
    case RC_NODE_FAILED_BROADCASTER_START:
    case RC_NODE_FAILED_BROADCASTER_STOP:
    case RC_NODE_FAILED_BROADCASTER_DESTROY:
    case RC_NODE_FAILED_PROCESSOR_INIT:
    case RC_NODE_FAILED_PROCESSOR_START:
    case RC_NODE_FAILED_PROCESSOR_STOP:
    case RC_NODE_FAILED_PROCESSOR_DESTROY:
    case RC_NODE_FAILED_RECEIVER_INIT:
    case RC_NODE_FAILED_RESPONDER_START:
    case RC_NODE_FAILED_RESPONDER_STOP:
    case RC_NODE_FAILED_RESPONDER_DESTROY:
    case RC_NODE_STILL_RUNNING:
    case RC_NODE_FAILED_RECEIVER_STOP:
    case RC_NODE_FAILED_RECEIVER_DESTROY:
    case RC_NODE_FAILED_RESPONDER_INIT:
    // Neighbor Module
    case RC_NEIGHBOR_NULL_NEIGHBOR:
    case RC_NEIGHBOR_NULL_URI:
    case RC_NEIGHBOR_NULL_HASH:
    case RC_NEIGHBOR_NULL_PACKET:
    case RC_NEIGHBOR_NULL_NODE:
    case RC_NEIGHBOR_FAILED_URI_PARSING:
    case RC_NEIGHBOR_INVALID_PROTOCOL:
    case RC_NEIGHBOR_INVALID_HOST:
    case RC_NEIGHBOR_FAILED_SEND:
    case RC_NEIGHBOR_FAILED_REQUESTER:
    case RC_NEIGHBOR_FAILED_ENDPOINT_INIT:
      return "String not defined.";

    // Cclient Module
    case RC_CCLIENT_JSON_CREATE:
      return STR_CCLIENT_JSON_CREATE;
    case RC_CCLIENT_JSON_PARSE:
      return STR_CCLIENT_JSON_PARSE;
    case RC_CCLIENT_OOM:
      return STR_CCLIENT_OOM;
    case RC_CCLIENT_HTTP:
      return STR_CCLIENT_HTTP;
    case RC_CCLIENT_HTTP_REQ:
      return STR_CCLIENT_HTTP_REQ;
    case RC_CCLIENT_HTTP_RES:
      return STR_CCLIENT_HTTP_RES;
    case RC_CCLIENT_RES_ERROR:
      return STR_CCLIENT_RES_ERROR;
    case RC_CCLIENT_JSON_KEY:
      return STR_CCLIENT_JSON_KEY;
    case RC_CCLIENT_FLEX_TRITS:
      return STR_CCLIENT_FLEX_TRITS;
    case RC_CCLIENT_NULL_PTR:
      return STR_CCLIENT_NULL_PTR;
    case RC_CCLIENT_UNIMPLEMENTED:
      return STR_CCLIENT_UNIMPLEMENTED;
    case RC_CCLIENT_INVALID_SECURITY:
      return STR_CCLIENT_INVALID_SECURITY;

    // Consensus module
    case RC_CONSENSUS_NOT_IMPLEMENTED:
    case RC_CONSENSUS_CW_FAILED_IN_DFS_FROM_DB:
    case RC_CONSENSUS_CW_FAILED_IN_LIGHT_DFS:
    case RC_CONSENSUS_OOM:
    case RC_CONSENSUS_NULL_PTR:
    case RC_CONSENSUS_EXIT_PROBABILITIES_INVALID_ENTRYPOINT:
    // Utils module
    case RC_UTILS_FAILED_REMOVE_FILE:
    case RC_UTILS_FAILED_TO_COPY_FILE:
    // Broadcaster module
    case RC_BROADCASTER_FAILED_PUSH_QUEUE:
    case RC_BROADCASTER_STILL_RUNNING:
    // Processor component module
    case RC_PROCESSOR_COMPONENT_FAILED_INIT_QUEUE:
    case RC_PROCESSOR_COMPONENT_FAILED_DESTROY_QUEUE:
    case RC_PROCESSOR_COMPONENT_FAILED_THREAD_SPAWN:
    case RC_PROCESSOR_COMPONENT_FAILED_THREAD_JOIN:
    case RC_PROCESSOR_COMPONENT_FAILED_ADD_QUEUE:
    case RC_PROCESSOR_COMPONENT_STILL_RUNNING:
    case RC_PROCESSOR_COMPONENT_INVALID_TX:
    case RC_PROCESSOR_COMPONENT_OOM:
    // Receiver component module
    case RC_RECEIVER_COMPONENT_NULL_STATE:
    case RC_RECEIVER_COMPONENT_NULL_NODE:
    case RC_RECEIVER_COMPONENT_FAILED_THREAD_SPAWN:
    case RC_RECEIVER_COMPONENT_FAILED_THREAD_JOIN:
    case RC_RECEIVER_COMPONENT_STILL_RUNNING:
    case RC_RECEIVER_COMPONENT_INVALID_PROCESSOR:
    // Requester component module
    case RC_REQUESTER_COMPONENT_NULL_STATE:
    case RC_REQUESTER_COMPONENT_NULL_TANGLE:
    case RC_REQUESTER_COMPONENT_NULL_HASH:
    case RC_REQUESTER_COMPONENT_FAILED_INIT_LIST:
    case RC_REQUESTER_COMPONENT_FAILED_DESTROY_LIST:
    case RC_REQUESTER_COMPONENT_FAILED_ADD_LIST:
    case RC_REQUESTER_COMPONENT_FAILED_REMOVE_LIST:
    // Responder component module
    case RC_RESPONDER_COMPONENT_NULL_STATE:
    case RC_RESPONDER_COMPONENT_NULL_NODE:
    case RC_RESPONDER_COMPONENT_NULL_PACKET:
    case RC_RESPONDER_COMPONENT_NULL_NEIGHBOR:
    case RC_RESPONDER_COMPONENT_NULL_TX:
    case RC_RESPONDER_COMPONENT_NULL_REQ:
    case RC_RESPONDER_COMPONENT_FAILED_INIT_QUEUE:
    case RC_RESPONDER_COMPONENT_FAILED_DESTROY_QUEUE:
    case RC_RESPONDER_COMPONENT_FAILED_THREAD_SPAWN:
    case RC_RESPONDER_COMPONENT_FAILED_THREAD_JOIN:
    case RC_RESPONDER_COMPONENT_FAILED_ADD_QUEUE:
    case RC_RESPONDER_COMPONENT_STILL_RUNNING:
    case RC_RESPONDER_COMPONENT_FAILED_TX_PROCESSING:
    case RC_RESPONDER_COMPONENT_FAILED_REQ_PROCESSING:
    case RC_RESPONDER_COMPONENT_INVALID_TX:
    case RC_RESPONDER_COMPONENT_OOM:
    // Conf Module
    case RC_CIRI_CONF_NULL_CONF:
    case RC_CIRI_CONF_INVALID_ARGUMENTS:
    // Conf Module
    case RC_API_NULL_SELF:
    case RC_API_FAILED_THREAD_SPAWN:
    case RC_API_FAILED_THREAD_JOIN:
    case RC_API_STILL_RUNNING:
    // Snapshot Module
    case RC_SNAPSHOT_NULL_SELF:
    case RC_SNAPSHOT_NULL_STATE:
    case RC_SNAPSHOT_NULL_HASH:
    case RC_SNAPSHOT_NULL_BALANCE:
    case RC_SNAPSHOT_FILE_NOT_FOUND:
    case RC_SNAPSHOT_INVALID_FILE:
    case RC_SNAPSHOT_INVALID_SUPPLY:
    case RC_SNAPSHOT_INCONSISTENT_SNAPSHOT:
    case RC_SNAPSHOT_OOM:
    case RC_SNAPSHOT_INCONSISTENT_PATCH:
    case RC_SNAPSHOT_BALANCE_NOT_FOUND:
      return "String not defined.";
    default:
      return "WTF";
  }
}
