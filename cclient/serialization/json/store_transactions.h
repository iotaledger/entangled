/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup serialization_json
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/store_transactions.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a store transactions request to a JSON string.
 *
 * @param[in] obj A store transactions request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_store_transactions_serialize_request(store_transactions_req_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a store transactions request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A store transactions request object.
 * @return #retcode_t
 */
retcode_t json_store_transactions_deserialize_request(char const* const obj, store_transactions_req_t* const out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_STORE_TRANSACTIONS_H

/** @} */