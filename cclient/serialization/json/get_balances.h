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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_BALANCES_H
#define CCLIENT_SERIALIZATION_JSON_GET_BALANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/get_balances.h"
#include "cclient/response/get_balances.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get balances request to a JSON string.
 *
 * @param[in] obj A get balances request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_balances_serialize_request(get_balances_req_t const* const req, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get balances request.
 *
 * @param[in] obj A JSON string.
 * @param[out] req A get balances request object.
 * @return #retcode_t
 */
retcode_t json_get_balances_deserialize_request(char const* const obj, get_balances_req_t* const req);

/**
 * @brief Converts a get balances response to a JSON string.
 *
 * @param[in] obj A get balances response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_balances_serialize_response(get_balances_res_t const* const res, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get balances response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get balances response object.
 * @return #retcode_t
 */
retcode_t json_get_balances_deserialize_response(char const* const obj, get_balances_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_BALANCES_H

/** @} */