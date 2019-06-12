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
#ifndef CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/broadcast_transactions.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a broadcast transactions request to a JSON string.
 *
 * @param[in] obj A broadcast transactions request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_broadcast_transactions_serialize_request(broadcast_transactions_req_t *const req, char_buffer_t *out);

/**
 * @brief Converts a JSON string to a broadcast transactions response.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A broadacst transactions response object.
 * @return #retcode_t
 */
retcode_t json_broadcast_transactions_deserialize_request(char const *const obj,
                                                          broadcast_transactions_req_t *const out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H

/** @} */