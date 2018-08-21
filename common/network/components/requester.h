/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include <stdbool.h>
#include <stdlib.h>

// Forward declarations
typedef struct concurrent_list_trit_array_p_s requester_list_t;
typedef struct _trit_array *trit_array_p;
typedef struct node_s node_t;

typedef struct requester_state_s {
  requester_list_t *list;
  node_t *node;
} requester_state_t;

#ifdef __cplusplus
extern "C" {
#endif

bool requester_init(requester_state_t *const state, node_t *const node);
size_t requester_size(requester_state_t *const state);
bool requester_clear_request(requester_state_t *const state,
                             trit_array_p const hash);
bool requester_is_full(requester_state_t *const state);
bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash);
bool get_transaction_to_request(requester_state_t *const state,
                                trit_array_p *hash);
bool requester_destroy(requester_state_t *const state);

#ifdef __cplusplus
}
#endif

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
