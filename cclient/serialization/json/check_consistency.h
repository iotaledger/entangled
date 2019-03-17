/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_CHECK_CONSISTENCY_H
#define CCLIENT_SERIALIZATION_JSON_CHECK_CONSISTENCY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/check_consistency.h"
#include "cclient/response/check_consistency.h"
#include "cclient/serialization/serializer.h"

retcode_t json_check_consistency_serialize_request(const serializer_t *const s, check_consistency_req_t *const obj,
                                                   char_buffer_t *out);
retcode_t json_check_consistency_serialize_response(const serializer_t *const s, check_consistency_res_t *const obj,
                                                    char_buffer_t *out);

retcode_t json_check_consistency_deserialize_request(const serializer_t *const s, const char *const obj,
                                                     check_consistency_req_t *out);
retcode_t json_check_consistency_deserialize_response(const serializer_t *const s, const char *const obj,
                                                      check_consistency_res_t *out);

#ifdef __cplusplus
}
#endif

#endif
