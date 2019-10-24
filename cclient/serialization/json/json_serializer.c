/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include "cJSON.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/json_serializer.h"
#include "cclient/serialization/json/logger.h"

#include "cclient/serialization/json/add_neighbors.h"
#include "cclient/serialization/json/attach_to_tangle.h"
#include "cclient/serialization/json/broadcast_transactions.h"
#include "cclient/serialization/json/check_consistency.h"
#include "cclient/serialization/json/error.h"
#include "cclient/serialization/json/find_transactions.h"
#include "cclient/serialization/json/get_balances.h"
#include "cclient/serialization/json/get_inclusion_states.h"
#include "cclient/serialization/json/get_missing_transactions.h"
#include "cclient/serialization/json/get_neighbors.h"
#include "cclient/serialization/json/get_node_api_conf.h"
#include "cclient/serialization/json/get_node_info.h"
#include "cclient/serialization/json/get_tips.h"
#include "cclient/serialization/json/get_transactions_to_approve.h"
#include "cclient/serialization/json/get_trytes.h"
#include "cclient/serialization/json/remove_neighbors.h"
#include "cclient/serialization/json/store_transactions.h"
#include "cclient/serialization/json/were_addresses_spent_from.h"

static serializer_vtable json_vtable = {
    .add_neighbors_serialize_request = json_add_neighbors_serialize_request,
    .add_neighbors_serialize_response = json_add_neighbors_serialize_response,
    .add_neighbors_deserialize_request = json_add_neighbors_deserialize_request,
    .add_neighbors_deserialize_response = json_add_neighbors_deserialize_response,

    .attach_to_tangle_serialize_request = json_attach_to_tangle_serialize_request,
    .attach_to_tangle_serialize_response = json_attach_to_tangle_serialize_response,
    .attach_to_tangle_deserialize_request = json_attach_to_tangle_deserialize_request,
    .attach_to_tangle_deserialize_response = json_attach_to_tangle_deserialize_response,

    .broadcast_transactions_serialize_request = json_broadcast_transactions_serialize_request,
    .broadcast_transactions_deserialize_request = json_broadcast_transactions_deserialize_request,

    .check_consistency_serialize_request = json_check_consistency_serialize_request,
    .check_consistency_serialize_response = json_check_consistency_serialize_response,
    .check_consistency_deserialize_request = json_check_consistency_deserialize_request,
    .check_consistency_deserialize_response = json_check_consistency_deserialize_response,

    .error_serialize_response = json_error_serialize_response,

    .find_transactions_serialize_request = json_find_transactions_serialize_request,
    .find_transactions_deserialize_request = json_find_transactions_deserialize_request,
    .find_transactions_serialize_response = json_find_transactions_serialize_response,
    .find_transactions_deserialize_response = json_find_transactions_deserialize_response,

    .get_balances_serialize_request = json_get_balances_serialize_request,
    .get_balances_deserialize_request = json_get_balances_deserialize_request,
    .get_balances_serialize_response = json_get_balances_serialize_response,
    .get_balances_deserialize_response = json_get_balances_deserialize_response,

    .get_inclusion_states_serialize_request = json_get_inclusion_states_serialize_request,
    .get_inclusion_states_deserialize_request = json_get_inclusion_states_deserialize_request,
    .get_inclusion_states_serialize_response = json_get_inclusion_states_serialize_response,
    .get_inclusion_states_deserialize_response = json_get_inclusion_states_deserialize_response,

    .get_missing_transactions_serialize_response = json_get_missing_transactions_serialize_response,

    .get_neighbors_serialize_request = json_get_neighbors_serialize_request,
    .get_neighbors_serialize_response = json_get_neighbors_serialize_response,
    .get_neighbors_deserialize_response = json_get_neighbors_deserialize_response,

    .get_node_api_conf_serialize_request = json_get_node_api_conf_serialize_request,
    .get_node_api_conf_serialize_response = json_get_node_api_conf_serialize_response,
    .get_node_api_conf_deserialize_response = json_get_node_api_conf_deserialize_response,

    .get_node_info_serialize_request = json_get_node_info_serialize_request,
    .get_node_info_serialize_response = json_get_node_info_serialize_response,
    .get_node_info_deserialize_response = json_get_node_info_deserialize_response,

    .get_tips_serialize_request = json_get_tips_serialize_request,
    .get_tips_serialize_response = json_get_tips_serialize_response,
    .get_tips_deserialize_response = json_get_tips_deserialize_response,

    .get_transactions_to_approve_serialize_request = json_get_transactions_to_approve_serialize_request,
    .get_transactions_to_approve_deserialize_request = json_get_transactions_to_approve_deserialize_request,
    .get_transactions_to_approve_serialize_response = json_get_transactions_to_approve_serialize_response,
    .get_transactions_to_approve_deserialize_response = json_get_transactions_to_approve_deserialize_response,

    .get_trytes_serialize_request = json_get_trytes_serialize_request,
    .get_trytes_deserialize_request = json_get_trytes_deserialize_request,
    .get_trytes_serialize_response = json_get_trytes_serialize_response,
    .get_trytes_deserialize_response = json_get_trytes_deserialize_response,

    .remove_neighbors_serialize_request = json_remove_neighbors_serialize_request,
    .remove_neighbors_deserialize_request = json_remove_neighbors_deserialize_request,
    .remove_neighbors_serialize_response = json_remove_neighbors_serialize_response,
    .remove_neighbors_deserialize_response = json_remove_neighbors_deserialize_response,

    .store_transactions_serialize_request = json_store_transactions_serialize_request,
    .store_transactions_deserialize_request = json_store_transactions_deserialize_request,

    .were_addresses_spent_from_serialize_request = json_were_addresses_spent_from_serialize_request,
    .were_addresses_spent_from_deserialize_request = json_were_addresses_spent_from_deserialize_request,
    .were_addresses_spent_from_serialize_response = json_were_addresses_spent_from_serialize_response,
    .were_addresses_spent_from_deserialize_response = json_were_addresses_spent_from_deserialize_response,
};

void init_json_serializer(serializer_t *serializer) { serializer->vtable = json_vtable; }
