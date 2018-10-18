/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_API_H__
#define __CIRI_API_API_H__

#include <stdbool.h>
#include <stdint.h>

#include "cclient/serialization/serializer.h"
#include "common/errors.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_api_s {
  thread_handle_t thread;
  bool running;
  uint16_t port;
  serializer_t serializer;
  serializer_type_t serializer_type;
} iota_api_t;

/**
 * Initializes an API
 *
 * @param api The API
 * @param port The API port
 * @param serializer_type A serializer type
 *
 * @return a status code
 */
retcode_t iota_api_init(iota_api_t *const api, uint16_t port,
                        serializer_type_t serializer_type);

/**
 * Starts an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_start(iota_api_t *const api);

/**
 * Stops an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_stop(iota_api_t *const api);

/**
 * Destroys an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_destroy(iota_api_t *const api);

retcode_t iota_api_get_node_info(get_node_info_res_t *const res);
retcode_t iota_api_get_neighbors(get_neighbors_res_t *const res);
retcode_t iota_api_add_neighbors(add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res);
retcode_t iota_api_remove_neighbors(remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res);
retcode_t iota_api_get_tips(get_tips_res_t *const res);
retcode_t iota_api_find_transactions(find_transactions_req_t const *const req,
                                     find_transactions_res_t *const res);
retcode_t iota_api_get_trytes(get_trytes_req_t const *const req,
                              get_trytes_res_t *const res);
retcode_t iota_api_get_inclusion_states(
    get_inclusion_state_req_t const *const req,
    get_inclusion_state_res_t *const res);
retcode_t iota_api_get_balances(get_balances_req_t const *const req,
                                get_balances_res_t *const res);
retcode_t iota_api_get_transactions_to_approve(
    get_transactions_to_approve_req_t const *const req,
    get_transactions_to_approve_res_t *const res);
retcode_t iota_api_attach_to_tangle(attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res);
retcode_t iota_api_interrupt_attaching_to_tangle();
retcode_t iota_api_broadcast_transactions(
    broadcast_transactions_req_t const *const req);
retcode_t iota_api_store_transactions(
    store_transactions_req_t const *const req);
retcode_t iota_api_check_consistency(check_consistency_req_t const *const req,
                                     check_consistency_res_t *const res);

#ifdef __cplusplus
}
#endif

#endif
