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
    case RC_ERROR:
      return STR_ERROR;
    case RC_NULL_PARAM:
      return STR_NULL_PARAM;

    // Storage Module
    case RC_STORAGE_FAILED_OPEN_DB:
    case RC_STORAGE_FAILED_INSERT_DB:
    case RC_STORAGE_NO_PATH_FOR_DB_SPECIFIED:
    case RC_STORAGE_FAILED_NOT_IMPLEMENTED:
    case RC_STORAGE_FAILED_BINDING:
    case RC_STORAGE_FAILED_FINALIZE:
    case RC_STORAGE_FAILED_STEP:

    // Neighbor Module
    case RC_NEIGHBOR_FAILED_URI_PARSING:
    case RC_NEIGHBOR_INVALID_PROTOCOL:
    case RC_NEIGHBOR_INVALID_HOST:
    case RC_NEIGHBOR_FAILED_SEND:
    case RC_NEIGHBOR_FAILED_ENDPOINT_INIT:
      return "String not defined.";

    // Cclient Module
    case RC_CCLIENT_JSON_CREATE:
      return STR_CCLIENT_JSON_CREATE;
    case RC_CCLIENT_JSON_PARSE:
      return STR_CCLIENT_JSON_PARSE;
    case RC_CCLIENT_HTTP:
      return STR_CCLIENT_HTTP;
    case RC_CCLIENT_HTTP_REQ:
      return STR_CCLIENT_HTTP_REQ;
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
    case RC_CCLIENT_TX_DESERIALIZE_FAILED:
      return STR_CCLIENT_TX_DESERIALIZE_FAILED;
    case RC_CCLIENT_INSUFFICIENT_BALANCE:
      return STR_CCLIENT_INSUFFICIENT_BALANCE;
    case RC_CCLIENT_POW_FAILED:
      return STR_CCLIENT_POW_FAILED;
    case RC_CCLIENT_INVALID_TRANSFER:
      return SRT_CCLIENT_INVALID_TRANSFER;
    case RC_CCLIENT_INVALID_TAIL_HASH:
      return STR_CCLIENT_INVALID_TAIL_HASH;
    case RC_CCLIENT_INVALID_BUNDLE:
      return SRT_CCLIENT_INVALID_BUNDLE;
    case RC_CCLIENT_CHECK_BALANCE:
      return SRT_CCLIENT_CHECK_BALANCE;
    case RC_CCLIENT_NOT_PROMOTABLE:
      return SRT_CCLIENT_NOT_PROMOTABLE;
    case RC_CCLIENT_NO_LOCAL_POW:
      return SRT_CCLIENT_NO_LOCAL_POW;

    // Consensus module
    case RC_CONSENSUS_NOT_IMPLEMENTED:
    case RC_CW_FAILED_IN_DFS_FROM_DB:
    case RC_CW_FAILED_IN_LIGHT_DFS:
    case RC_EXIT_PROBABILITIES_INVALID_ENTRYPOINT:

    // Utils module
    case RC_UTILS_FAILED_REMOVE_FILE:
    case RC_UTILS_FAILED_TO_COPY_FILE:
      return "String not defined.";
    case RC_UTILS_SOCKET_TLS_RNG:
      return STR_UTILS_SOCKET_TLS_RNG;
    case RC_UTILS_SOCKET_TLS_CA:
      return STR_UTILS_SOCKET_TLS_CA;
    case RC_UTILS_SOCKET_TLS_CLIENT_PEM:
      return STR_UTILS_SOCKET_TLS_CLIENT_PEM;
    case RC_UTILS_SOCKET_TLS_CLIENT_PK:
      return STR_UTILS_SOCKET_TLS_CLIENT_PK;
    case RC_UTILS_SOCKET_TLS_CONF:
      return STR_UTILS_SOCKET_TLS_CONF;
    case RC_UTILS_SOCKET_TLS_AUTHMODE:
      return STR_UTILS_SOCKET_TLS_AUTHMODE;
    case RC_UTILS_SOCKET_CLIENT_AUTH:
      return STR_UTILS_SOCKET_CLIENT_AUTH;
    case RC_UTILS_SOCKET_TLS_HANDSHAKE:
      return STR_UTILS_SOCKET_TLS_HANDSHAKE;
    case RC_UTILS_SOCKET_CONNECT:
      return STR_UTILS_SOCKET_CONNECT;
    case RC_UTILS_SOCKET_RECV:
      return STR_UTILS_SOCKET_RECV;
    case RC_UTILS_SOCKET_SEND:
      return STR_UTILS_SOCKET_SEND;

    // Processor component module
    case RC_PROCESSOR_INVALID_TRANSACTION:

    // Conf Module
    case RC_CONF_INVALID_ARGUMENT:

    // Snapshot Module
    case RC_SNAPSHOT_FILE_NOT_FOUND:
    case RC_SNAPSHOT_INVALID_FILE:
    case RC_SNAPSHOT_INVALID_SUPPLY:
    case RC_SNAPSHOT_INCONSISTENT_SNAPSHOT:
    case RC_SNAPSHOT_INCONSISTENT_PATCH:
    case RC_SNAPSHOT_BALANCE_NOT_FOUND:
      return "String not defined.";

    // Common Module
    case RC_COMMON_BUNDLE_SIGN:
      return SRT_COMMON_BUNDLE_SIGN;

    default:
      return "Unknown error";
  }
}
