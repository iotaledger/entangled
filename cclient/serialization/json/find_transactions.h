/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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

retcode_t json_find_transactions_serialize_request(serializer_t const* const s,
                                                   find_transactions_req_t const* const req, char_buffer_t* out);
retcode_t json_find_transactions_deserialize_request(serializer_t const* const s, char const* const obj,
                                                     find_transactions_req_t* req);
retcode_t json_find_transactions_serialize_response(serializer_t const* const s, find_transactions_res_t* const res,
                                                    char_buffer_t* out);
retcode_t json_find_transactions_deserialize_response(serializer_t const* const s, const char* const obj,
                                                      find_transactions_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_FIND_TRANSACTIONS_H
