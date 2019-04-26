/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/store_transactions.h"
#include "cclient/serialization/serializer.h"

retcode_t json_store_transactions_serialize_request(store_transactions_req_t const* const obj, char_buffer_t* out);

retcode_t json_store_transactions_deserialize_request(char const* const obj, store_transactions_req_t* const out);
#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H
