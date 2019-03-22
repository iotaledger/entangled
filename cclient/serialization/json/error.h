/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_ERROR_H
#define CCLIENT_SERIALIZATION_JSON_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/error.h"
#include "cclient/serialization/serializer.h"

retcode_t json_error_serialize_response(serializer_t const *const s, error_res_t *const obj, char_buffer_t *out);

#ifdef __cplusplus
}
#endif

#endif
