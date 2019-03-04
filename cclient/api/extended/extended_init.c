/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/extended_init.h"
#include "cclient/api/extended/logger.h"

void iota_client_extended_init() { logger_init_client_extended(); }

void iota_client_extended_destroy() { logger_destroy_client_extended(); }
