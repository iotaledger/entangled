/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup serialization_json
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_SERIALIZATION_JSON_SERIALIZER_H
#define CCLIENT_SERIALIZATION_JSON_SERIALIZER_H

#include <stdlib.h>

#include "cclient/serialization/serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes json serializer function table.
 *
 * @param serializer
 */
void init_json_serializer(serializer_t* serializer);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_SERIALIZER_H

/** @} */