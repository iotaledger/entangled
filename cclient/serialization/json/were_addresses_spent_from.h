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
#ifndef CCLIENT_SERIALIZATION_JSON_WERE_ADDRESSES_SPENT_FROM_H
#define CCLIENT_SERIALIZATION_JSON_WERE_ADDRESSES_SPENT_FROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/were_addresses_spent_from.h"
#include "cclient/response/were_addresses_spent_from.h"
#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a were addresses spent from request to a JSON string.
 *
 * @param[in] req A were addresses septn from request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_were_addresses_spent_from_serialize_request(were_addresses_spent_from_req_t const* const req,
                                                           char_buffer_t* out);

/**
 * @brief Converts a JSON string to a were address spent from request.
 *
 * @param[in] obj A JSON string.
 * @param[out] req A were addresses spent from request object.
 * @return #retcode_t
 */
retcode_t json_were_addresses_spent_from_deserialize_request(char const* const obj,
                                                             were_addresses_spent_from_req_t* const req);

/**
 * @brief Converts a were addresses spent from response to a JSON string.
 *
 * @param[in] res A were addresses spent from response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_were_addresses_spent_from_serialize_response(were_addresses_spent_from_res_t const* const res,
                                                            char_buffer_t* out);

/**
 * @brief Converts a JSON string to a were addresses spent from response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A were addresses spent from response object.
 * @return #retcode_t
 */
retcode_t json_were_addresses_spent_from_deserialize_response(char const* const obj,
                                                              were_addresses_spent_from_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_WERE_ADDRESSES_SPENT_FROM_H

/** @} */