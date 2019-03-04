/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_EXTENDED_LOGGER_H
#define CCLIENT_API_EXTENDED_LOGGER_H

#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern logger_id_t client_extended_logger_id;

void logger_init_client_extended();
void logger_destroy_client_extended();

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_EXTENDED_LOGGER_H
