/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_GET_MISSING_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_GET_MISSING_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/get_missing_transactions.h"
#include "cclient/serialization/serializer.h"

retcode_t json_get_missing_transactions_serialize_response(get_missing_transactions_res_t const* const res,
                                                           char_buffer_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_MISSING_TRANSACTIONS_H
