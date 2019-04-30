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
#ifndef CCLIENT_SERIALIZATION_JSON_FIND_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_FIND_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/find_transactions.h"
#include "cclient/response/find_transactions.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a find transactions request to a JSON string.
 *
 * @param[in] obj A find transactions request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_find_transactions_serialize_request(find_transactions_req_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a find transactions request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A find transactions request object.
 * @return #retcode_t
 */
retcode_t json_find_transactions_deserialize_request(char const* const obj, find_transactions_req_t* out);

/**
 * @brief Converts a find transactions response to a JSON string.
 *
 * @param[in] obj A find transactions request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_find_transactions_serialize_response(find_transactions_res_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a find transactions response.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A find transactions response object.
 * @return #retcode_t
 */
retcode_t json_find_transactions_deserialize_response(char const* const obj, find_transactions_res_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_FIND_TRANSACTIONS_H

/** @} */