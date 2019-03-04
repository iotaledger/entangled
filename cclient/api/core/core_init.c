/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/core_init.h"
#include "cclient/api/core/logger.h"

retcode_t iota_client_core_init(iota_client_service_t* const serv) {
  logger_init_client_core();
  return iota_client_service_init(serv);
}

void iota_client_core_destroy(iota_client_service_t* const serv) {
  logger_destroy_client_core();
  iota_client_service_destroy(serv);
}
