/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/get_bundle.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/traverse_bundle.h"

retcode_t iota_client_get_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                 bundle_transactions_t* const bundle, bundle_status_t* const bundle_status) {
  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  retcode_t ret_code = iota_client_traverse_bundle(serv, tail_hash, bundle);
  if (ret_code == RC_OK) {
    bundle_validate(bundle, bundle_status);
  }
  return ret_code;
}
