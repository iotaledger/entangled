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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_SERIALIZATION_JSON_GET_TRANSACTIONS_TO_APPROVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/get_transactions_to_approve.h"
#include "cclient/response/get_transactions_to_approve.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get transactions to approve request to a JSON string.
 *
 * @param[in] req A get transactions to approve request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_transactions_to_approve_serialize_request(get_transactions_to_approve_req_t const* const req,
                                                             char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get transactions to approve request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A get transactions to approve request object.
 * @return #retcode_t
 */
retcode_t json_get_transactions_to_approve_deserialize_request(char const* const obj,
                                                               get_transactions_to_approve_req_t* out);

/**
 * @brief Converts a get transactions to approve response to a JSON string.
 *
 * @param[in] res A get transactions to approve response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_transactions_to_approve_serialize_response(get_transactions_to_approve_res_t const* const obj,
                                                              char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get transactions to approve response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get transactions to approve response object.
 * @return #retcode_t
 */
retcode_t json_get_transactions_to_approve_deserialize_response(char const* const obj,
                                                                get_transactions_to_approve_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_TRANSACTIONS_TO_APPROVE_H

/** @} */