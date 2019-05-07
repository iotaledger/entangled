/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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

retcode_t json_get_transactions_to_approve_serialize_request(get_transactions_to_approve_req_t const* const req,
                                                             char_buffer_t* out);
retcode_t json_get_transactions_to_approve_deserialize_request(char const* const obj,
                                                               get_transactions_to_approve_req_t* out);
retcode_t json_get_transactions_to_approve_serialize_response(get_transactions_to_approve_res_t const* const obj,
                                                              char_buffer_t* out);
retcode_t json_get_transactions_to_approve_deserialize_response(char const* const obj,
                                                                get_transactions_to_approve_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_TRANSACTIONS_TO_APPROVE_H
