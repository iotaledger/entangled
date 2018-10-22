/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/service.h"
#include "cclient/serialization/json/json_serializer.h"
#include "utils/logger_helper.h"

retcode_t iota_client_service_init(iota_client_service_t* const serv) {
  // enable logger
  logger_init_types();
  // init serializer
  if (serv->serializer_type == SR_JSON) {
    logger_init_json_serializer();
    init_json_serializer(&serv->serializer);
  } else {
    return RC_CCLIENT_UNIMPLEMENTED;
  }
  return RC_OK;
}

void iota_client_service_destroy(iota_client_service_t* const serv) {
  // clear logger
  if (serv->serializer_type == SR_JSON) {
    logger_destroy_json_serializer();
  }
  logger_destroy_types();
}
