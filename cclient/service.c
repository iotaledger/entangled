/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "service.h"
#include "serializer/json/json_serializer.h"
#include "utils/logger_helper.h"

void iota_client_service_init(iota_client_service_t* serv) {
  // enable logger
  logger_init_json_serializer();
  logger_init_types();
  // init serializer
  init_json_serializer(&serv->serializer);
}

void iota_client_service_destroy() {
  // clear logger
  logger_destroy_json_serializer();
  logger_destroy_types();
}
