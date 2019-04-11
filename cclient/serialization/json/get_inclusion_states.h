/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H
#define CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/get_inclusion_states.h"
#include "cclient/response/get_inclusion_states.h"
#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/serializer.h"

retcode_t json_get_inclusion_states_serialize_request(serializer_t const* const s,
                                                      get_inclusion_states_req_t const* const req, char_buffer_t* out);
retcode_t json_get_inclusion_states_deserialize_request(serializer_t const* const s, char const* const obj,
                                                        get_inclusion_states_req_t* const req);
retcode_t json_get_inclusion_states_serialize_response(serializer_t const* const s,
                                                       get_inclusion_states_res_t const* const res, char_buffer_t* out);
retcode_t json_get_inclusion_states_deserialize_response(serializer_t const* const s, char const* const obj,
                                                         get_inclusion_states_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H
