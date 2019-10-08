/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup response
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_RESPONSE_GET_NODE_CONF_H
#define CCLIENT_RESPONSE_GET_NODE_CONF_H

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"
#include "utils/char_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The data structure of the get node configuration response.
 *
 */
typedef struct get_node_api_conf_res_s {
  /**
   * The maximum number of transactions that may be returned.
   */
  uint32_t max_find_transactions;

  /**
   * The maximum number of parameters in an API call.
   */
  uint32_t max_requests_list;

  /**
   * a maximum number of trytes that may be returned by the #iota_client_get_trytes.
   */
  uint32_t max_get_trytes;

  /**
   * The maximum number of characters that the body of an API call may contain.
   */
  uint32_t max_body_length;

  /**
   * The starting milestone index that the node uses to validate and confirm transactions.
   */
  uint32_t milestone_start_index;

  /**
   * The type of this node, True runs on testnet else mainnet.
   */
  bool test_net;

} get_node_api_conf_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_NODE_CONF_H

/** @} */
