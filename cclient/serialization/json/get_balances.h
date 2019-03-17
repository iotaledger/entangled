/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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

retcode_t json_get_balances_serialize_request(serializer_t const* const s, get_balances_req_t const* const req,
                                              char_buffer_t* out);
retcode_t json_get_balances_deserialize_request(serializer_t const* const s, char const* const obj,
                                                get_balances_req_t* const req);
retcode_t json_get_balances_serialize_response(serializer_t const* const s, const get_balances_res_t* const res,
                                               char_buffer_t* out);
retcode_t json_get_balances_deserialize_response(const serializer_t* const s, const char* const obj,
                                                 get_balances_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_BALANCES_H
