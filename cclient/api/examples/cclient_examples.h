/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_EXAMPLES_H
#define CCLIENT_EXAMPLES_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "cclient/api/core/core_api.h"
#include "cclient/api/extended/extended_api.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#ifdef __cplusplus
extern "C" {
#endif

static tryte_t const *const MY_SEED =
    (tryte_t *)"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

void example_attach_to_tangle(iota_client_service_t *s);
void example_broadcast_transactions(iota_client_service_t *s);
void example_check_consistency(iota_client_service_t *s);
void example_find_transactions(iota_client_service_t *s);
void example_get_balance(iota_client_service_t *s);
void example_get_inclusion_states(iota_client_service_t *s);
void example_get_tips(iota_client_service_t *s);
void example_get_transactions_to_approve(iota_client_service_t *s);
void example_get_trytes(iota_client_service_t *s);
void example_node_info(iota_client_service_t *s);
void example_prepare_transfer(iota_client_service_t *s);
void example_store_transactions(iota_client_service_t *s);
void example_were_addresses_spent_from(iota_client_service_t *s);

void example_get_new_address(iota_client_service_t *s);
void example_get_inputs(iota_client_service_t *s);
void example_get_account_data(iota_client_service_t *s);
void example_find_transaction_objects(iota_client_service_t *s);
void example_is_promotable(iota_client_service_t *s);
void example_get_latest_inclusion(iota_client_service_t *s);
void example_send_trytes(iota_client_service_t *s);
void example_traverse_bundle(iota_client_service_t *s);
void example_get_bundle(iota_client_service_t *s);
void example_replay_bundle(iota_client_service_t *s);
void example_broadcast_bundle(iota_client_service_t *s);
void example_promote_transaction(iota_client_service_t *s);

void example_send_data(iota_client_service_t *s);
void example_send_balance(iota_client_service_t *s);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_EXAMPLES_H