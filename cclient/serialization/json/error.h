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
#ifndef CCLIENT_SERIALIZATION_JSON_ERROR_H
#define CCLIENT_SERIALIZATION_JSON_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/error.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts an error response object to a JSON string.
 *
 * @param[in] obj An error response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_error_serialize_response(error_res_t *const obj, char_buffer_t *out);

#ifdef __cplusplus
}
#endif

#endif

/** @} */