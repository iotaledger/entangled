/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include "utils/containers/lists/concurrent_list_trit_array.h"

typedef concurrent_list_of_trit_array_p requester_list_t;
typedef struct node_s node_t;

typedef struct requester_state_s {
  requester_list_t *list;
  node_t *node;
} requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool requester_init(requester_state_t *const state, node_t *const node);
size_t transactions_to_request_number(requester_state_t *const state);
bool clear_transaction_request(requester_state_t *const state,
                               trit_array_p const hash);
bool transactions_to_request_is_full(requester_state_t *const state);
bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash);
bool get_transaction_to_request(requester_state_t *const state,
                                trit_array_p *hash);
bool requester_destroy(requester_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
