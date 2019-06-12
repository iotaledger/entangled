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
#ifndef CCLIENT_SERIALIZATION_JSON_ATTACH_TO_TANGLE_H
#define CCLIENT_SERIALIZATION_JSON_ATTACH_TO_TANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/attach_to_tangle.h"
#include "cclient/response/attach_to_tangle.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts an attach to tangle request to a JSON string.
 *
 * @param[in] obj An attach to tangle request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_attach_to_tangle_serialize_request(attach_to_tangle_req_t const *const obj, char_buffer_t *out);

/**
 * @brief Converts an attach to tangle response to a JSON string.
 *
 * @param[in] obj An attach to tangle response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_attach_to_tangle_serialize_response(attach_to_tangle_res_t const *const obj, char_buffer_t *out);

/**
 * @brief Converts a JSON string to an attach to tangle request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out An attach to tangle request object.
 * @return #retcode_t
 */
retcode_t json_attach_to_tangle_deserialize_request(char const *const obj, attach_to_tangle_req_t *const out);

/**
 * @brief Converts a JSON string to an attach to tangle response.
 *
 * @param[in] obj A JSON string.
 * @param[out] out An attach to tangle response object.
 * @return #retcode_t
 */
retcode_t json_attach_to_tangle_deserialize_response(char const *const obj, attach_to_tangle_res_t *const out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_ATTACH_TO_TANGLE_H

/** @} */