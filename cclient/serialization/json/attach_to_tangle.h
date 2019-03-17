/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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

retcode_t json_attach_to_tangle_serialize_request(const serializer_t *const s, const attach_to_tangle_req_t *const obj,
                                                  char_buffer_t *out);
retcode_t json_attach_to_tangle_serialize_response(const serializer_t *const s, const attach_to_tangle_res_t *const obj,
                                                   char_buffer_t *out);

retcode_t json_attach_to_tangle_deserialize_request(const serializer_t *const s, const char *const obj,
                                                    attach_to_tangle_req_t *const out);
retcode_t json_attach_to_tangle_deserialize_response(const serializer_t *const s, const char *const obj,
                                                     attach_to_tangle_res_t *const out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_ATTACH_TO_TANGLE_H
