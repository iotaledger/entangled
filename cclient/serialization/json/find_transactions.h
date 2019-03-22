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

retcode_t json_find_transactions_serialize_request(const serializer_t* const s,
                                                   find_transactions_req_t const* const req, char_buffer_t* out);
retcode_t json_find_transactions_deserialize_response(const serializer_t* const s, const char* const obj,
                                                      find_transactions_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_FIND_TRANSACTIONS_H
