/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H
#define CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/broadcast_transactions.h"
#include "cclient/serialization/serializer.h"

retcode_t json_broadcast_transactions_serialize_request(const serializer_t *const s,
                                                        broadcast_transactions_req_t *const req, char_buffer_t *out);

retcode_t json_broadcast_transactions_deserialize_request(const serializer_t *const s, const char *const obj,
                                                          broadcast_transactions_req_t *const out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_BROADCAST_TRANSACTIONS_H
