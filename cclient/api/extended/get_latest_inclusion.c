/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/get_inclusion_states.h"
#include "cclient/api/core/get_node_info.h"

#include "cclient/api/extended/get_latest_inclusion.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_get_latest_inclusion(iota_client_service_t const* const serv, hash243_queue_t const transactions,
                                           get_inclusion_states_res_t* out_states) {
  retcode_t ret_code = RC_ERROR;
  get_inclusion_states_req_t* inclusion_req = NULL;
  get_node_info_res_t* node_info = get_node_info_res_new();

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!node_info) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s create node info request failed: %s\n", __func__,
              error_2_string(ret_code));
    return ret_code;
  }

  ret_code = iota_client_get_node_info(serv, node_info);
  if (ret_code) {
    goto done;
  }

  inclusion_req = get_inclusion_states_req_new();
  if (!inclusion_req) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s create get inclusion state request failed: %s\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  inclusion_req->transactions = transactions;
  ret_code = hash243_queue_push(&inclusion_req->tips, node_info->latest_solid_subtangle_milestone);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s hash queue push failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  // check txs with latest solid subtangle milestone
  ret_code = iota_client_get_inclusion_states(serv, inclusion_req, out_states);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s hash queue push failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

done:
  get_node_info_res_free(&node_info);
  // no need to free it, it should free via caller.
  inclusion_req->transactions = NULL;
  get_inclusion_states_req_free(&inclusion_req);
  return ret_code;
}
