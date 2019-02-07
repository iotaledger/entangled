/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/iota_client_core_api.h"
#include "cclient/http/http.h"
#include "cclient/service.h"
#include "common/helpers/pow.h"
#include "utils/time.h"

#define CCLIENT_CORE_LOGGER_ID "cclient_core_api"

static logger_id_t logger_id;

retcode_t iota_client_core_init(iota_client_service_t* const serv) {
  logger_id = logger_helper_enable(CCLIENT_CORE_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__,
           CCLIENT_CORE_LOGGER_ID);
  return iota_client_service_init(serv);
}

void iota_client_core_destroy(iota_client_service_t* const serv) {
  log_info(logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__,
           CCLIENT_CORE_LOGGER_ID);
  logger_helper_release(logger_id);
  iota_client_service_destroy(serv);
}

retcode_t iota_client_get_node_info(const iota_client_service_t* const service,
                                    get_node_info_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_node_info_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_node_info_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_get_neighbors(const iota_client_service_t* const service,
                                    get_neighbors_res_t* const res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_neighbors_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_add_neighbors(const iota_client_service_t* const service,
                                    const add_neighbors_req_t* const req,
                                    add_neighbors_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.add_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.add_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_remove_neighbors(
    const iota_client_service_t* const service,
    const remove_neighbors_req_t* const req, remove_neighbors_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.remove_neighbors_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.remove_neighbors_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_get_tips(const iota_client_service_t* const service,
                               get_tips_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_tips_serialize_request(
      &service->serializer, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_tips_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_find_transactions(
    iota_client_service_t const* const service,
    find_transactions_req_t const* const req,
    find_transactions_res_t* const res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.find_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.find_transactions_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_get_trytes(const iota_client_service_t* const service,
                                 get_trytes_req_t* const req,
                                 get_trytes_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_trytes_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_trytes_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_get_inclusion_states(
    const iota_client_service_t* const service,
    get_inclusion_state_req_t* const req, get_inclusion_state_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.get_inclusion_state_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_inclusion_state_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_get_balances(iota_client_service_t const* const service,
                                   get_balances_req_t const* const req,
                                   get_balances_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.get_balances_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.get_balances_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_client_get_transactions_to_approve(
    const iota_client_service_t* const service,
    const get_transactions_to_approve_req_t* const req,
    get_transactions_to_approve_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* req_buff = char_buffer_new();
  char_buffer_t* res_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result =
      service->serializer.vtable.get_transactions_to_approve_serialize_request(
          &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable
               .get_transactions_to_approve_deserialize_response(
                   &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}

retcode_t iota_client_attach_to_tangle(
    const iota_client_service_t* const service,
    const attach_to_tangle_req_t* const req, attach_to_tangle_res_t* res) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = NULL;
  char_buffer_t* req_buff = NULL;
  bundle_transactions_t* bundle = NULL;

  if (service == NULL) {
    log_info(logger_id, "[%s:%d] local PoW\n", __func__, __LINE__);
    iota_transaction_t tx = {};
    flex_trit_t* array_elt = NULL;
    flex_trit_t* tx_trytes = NULL;
    // create bundle
    bundle_transactions_new(&bundle);
    HASH_ARRAY_FOREACH(req->trytes, array_elt) {
      transaction_deserialize_from_trits(&tx, array_elt, false);
      bundle_transactions_add(bundle, &tx);
    }

    // PoW on bundle
    if (iota_pow_bundle(bundle, req->trunk, req->branch, req->mwm) != RC_OK) {
      log_info(logger_id, "[%s:%d] PoW failed.\n", __func__, __LINE__);
      result = RC_CCLIENT_POW_FAILED;
      goto done;
    }

    // bundle to trytes
    iota_transaction_t* curr_tx = NULL;
    BUNDLE_FOREACH(bundle, curr_tx) {
      tx_trytes = transaction_serialize(curr_tx);
      if (tx_trytes) {
        hash_array_push(res->trytes, tx_trytes);
        free(tx_trytes);
        tx_trytes = NULL;
      } else {
        log_info(logger_id, "[%s:%d] transaction serialize failed.\n", __func__,
                 __LINE__);
        result = RC_CCLIENT_TX_DESERIALIZE_FAILED;
        goto done;
      }
    }
  } else {
    log_info(logger_id, "[%s:%d] remote PoW\n", __func__, __LINE__);
    res_buff = char_buffer_new();
    req_buff = char_buffer_new();
    if (req_buff == NULL || res_buff == NULL) {
      log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                   STR_CCLIENT_OOM);
      result = RC_CCLIENT_OOM;
      goto done;
    }

    result = service->serializer.vtable.attach_to_tangle_serialize_request(
        &service->serializer, req, req_buff);
    if (result != RC_OK) {
      goto done;
    }

    result = iota_service_query(service, req_buff, res_buff);
    if (result != RC_OK) {
      log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                error_2_string(result));
      goto done;
    }

    result = service->serializer.vtable.attach_to_tangle_deserialize_response(
        &service->serializer, res_buff->data, res);
  }

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  if (bundle) {
    bundle_transactions_free(&bundle);
  }
  return result;
}

retcode_t iota_client_interrupt_attaching_to_tangle(
    const iota_client_service_t* const service) {
  retcode_t result = RC_OK;
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  // TODO
  return result;
}

retcode_t iota_client_broadcast_transactions(
    const iota_client_service_t* const service,
    broadcast_transactions_req_t* req) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.broadcast_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_client_store_transactions(
    const iota_client_service_t* const service,
    store_transactions_req_t const* const req) {
  retcode_t result = RC_OK;
  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }
  result = service->serializer.vtable.store_transactions_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }

  return result;
}

retcode_t iota_client_check_consistency(
    const iota_client_service_t* const service,
    check_consistency_req_t* const req, check_consistency_res_t* res) {
  retcode_t result = RC_OK;

  char_buffer_t* res_buff = char_buffer_new();
  char_buffer_t* req_buff = char_buffer_new();
  log_info(logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (req_buff == NULL || res_buff == NULL) {
    log_critical(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_OOM);
    result = RC_CCLIENT_OOM;
    goto done;
  }

  result = service->serializer.vtable.check_consistency_serialize_request(
      &service->serializer, req, req_buff);
  if (result != RC_OK) {
    goto done;
  }

  result = iota_service_query(service, req_buff, res_buff);
  if (result != RC_OK) {
    log_error(logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              error_2_string(result));
    goto done;
  }

  result = service->serializer.vtable.check_consistency_deserialize_response(
      &service->serializer, res_buff->data, res);

done:
  if (req_buff) {
    char_buffer_free(req_buff);
  }
  if (res_buff) {
    char_buffer_free(res_buff);
  }
  return result;
}
