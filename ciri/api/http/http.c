/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <microhttpd.h>
#include "cJSON.h"

#include "cclient/request/requests.h"
#include "cclient/response/responses.h"
#include "cclient/serialization/json/json_serializer.h"
#include "ciri/api/api.h"
#include "ciri/api/http/http.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define API_HTTP_LOGGER_ID "api_http"

static logger_id_t logger_id;
static _Thread_local tangle_t *tangle;
static _Thread_local spent_addresses_provider_t *sap;

typedef struct iota_api_http_session_s {
  bool valid_api_version;
  bool valid_content_type;
  char_buffer_t *request;
} iota_api_http_session_t;

static retcode_t error_serialize_response(iota_api_http_t *const http, error_res_t **const error,
                                          char const *const message, char_buffer_t *const out) {
  retcode_t ret = RC_OK;

  if (*error == NULL) {
    if (message == NULL) {
      *error = error_res_new("Internal server error");
    } else {
      *error = error_res_new(message);
    }
  }
  ret = http->serializer.vtable.error_serialize_response(*error, out);
  error_res_free(error);

  return ret;
}

static inline retcode_t process_add_neighbors_request(iota_api_http_t *const http, char const *const payload,
                                                      char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  add_neighbors_req_t *req = add_neighbors_req_new();
  add_neighbors_res_t *res = add_neighbors_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.add_neighbors_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_add_neighbors(http->api, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.add_neighbors_serialize_response(res, out);
  }

done:
  add_neighbors_req_free(&req);
  add_neighbors_res_free(&res);

  return ret;
}

static inline retcode_t process_attach_to_tangle_request(iota_api_http_t *const http, char const *const payload,
                                                         char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  attach_to_tangle_req_t *req = attach_to_tangle_req_new();
  attach_to_tangle_res_t *res = attach_to_tangle_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.attach_to_tangle_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_attach_to_tangle(http->api, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.attach_to_tangle_serialize_response(res, out);
  }

done:
  attach_to_tangle_req_free(&req);
  attach_to_tangle_res_free(&res);

  return ret;
}

static inline retcode_t process_broadcast_transactions_request(iota_api_http_t *const http, char const *const payload,
                                                               char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  error_res_t *error = NULL;

  if (req == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.broadcast_transactions_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_broadcast_transactions(http->api, req, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  }

done:
  broadcast_transactions_req_free(&req);

  return ret;
}

static inline retcode_t process_check_consistency_request(iota_api_http_t *const http, char const *const payload,
                                                          char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.check_consistency_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_check_consistency(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.check_consistency_serialize_response(res, out);
  }

done:
  check_consistency_req_free(&req);
  check_consistency_res_free(&res);

  return ret;
}

static inline retcode_t process_find_transactions_request(iota_api_http_t *const http, char const *const payload,
                                                          char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.find_transactions_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_find_transactions(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.find_transactions_serialize_response(res, out);
  }

done:
  find_transactions_req_free(&req);
  find_transactions_res_free(&res);

  return ret;
}

static inline retcode_t process_get_balances_request(iota_api_http_t *const http, char const *const payload,
                                                     char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_balances_req_t *req = get_balances_req_new();
  get_balances_res_t *res = get_balances_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.get_balances_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_get_balances(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_balances_serialize_response(res, out);
  }

done:
  get_balances_req_free(&req);
  get_balances_res_free(&res);

  return ret;
}

static inline retcode_t process_get_inclusion_states_request(iota_api_http_t *const http, char const *const payload,
                                                             char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_inclusion_states_req_t *req = get_inclusion_states_req_new();
  get_inclusion_states_res_t *res = get_inclusion_states_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.get_inclusion_states_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_get_inclusion_states(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_inclusion_states_serialize_response(res, out);
  }

done:
  get_inclusion_states_req_free(&req);
  get_inclusion_states_res_free(&res);

  return ret;
}

static inline retcode_t process_get_missing_transactions_request(iota_api_http_t *const http, char const *const payload,
                                                                 char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_missing_transactions_res_t *res = get_missing_transactions_res_new();
  error_res_t *error = NULL;
  UNUSED(payload);

  if (res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = iota_api_get_missing_transactions(http->api, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_missing_transactions_serialize_response(res, out);
  }

done:
  get_missing_transactions_res_free(&res);

  return ret;
}

static inline retcode_t process_get_neighbors_request(iota_api_http_t *const http, char const *const payload,
                                                      char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_neighbors_res_t *res = get_neighbors_res_new();
  error_res_t *error = NULL;
  UNUSED(payload);

  if (res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = iota_api_get_neighbors(http->api, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_neighbors_serialize_response(res, out);
  }

done:
  get_neighbors_res_free(&res);

  return ret;
}

static inline retcode_t process_get_node_info_request(iota_api_http_t *const http, char const *const payload,
                                                      char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_node_info_res_t *res = get_node_info_res_new();
  error_res_t *error = NULL;
  UNUSED(payload);

  if (res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = iota_api_get_node_info(http->api, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_node_info_serialize_response(res, out);
  }

done:
  get_node_info_res_free(&res);

  return ret;
}

static inline retcode_t process_get_tips_request(iota_api_http_t *const http, char const *const payload,
                                                 char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_tips_res_t *res = get_tips_res_new();
  error_res_t *error = NULL;
  UNUSED(payload);

  if (res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = iota_api_get_tips(http->api, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_tips_serialize_response(res, out);
  }

done:
  get_tips_res_free(&res);

  return ret;
}

static inline retcode_t process_get_transactions_to_approve_request(iota_api_http_t *const http,
                                                                    char const *const payload,
                                                                    char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_transactions_to_approve_req_t *req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *res = get_transactions_to_approve_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.get_transactions_to_approve_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_get_transactions_to_approve(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_transactions_to_approve_serialize_response(res, out);
  }

done:
  get_transactions_to_approve_req_free(&req);
  get_transactions_to_approve_res_free(&res);

  return ret;
}

static inline retcode_t process_get_trytes_request(iota_api_http_t *const http, char const *const payload,
                                                   char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  get_trytes_req_t *req = get_trytes_req_new();
  get_trytes_res_t *res = get_trytes_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.get_trytes_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_get_trytes(http->api, tangle, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.get_trytes_serialize_response(res, out);
  }

done:
  get_trytes_req_free(&req);
  get_trytes_res_free(&res);

  return ret;
}

static inline retcode_t process_remove_neighbors_request(iota_api_http_t *const http, char const *const payload,
                                                         char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  remove_neighbors_req_t *req = remove_neighbors_req_new();
  remove_neighbors_res_t *res = remove_neighbors_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.remove_neighbors_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_remove_neighbors(http->api, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.remove_neighbors_serialize_response(res, out);
  }

done:
  remove_neighbors_req_free(&req);
  remove_neighbors_res_free(&res);

  return ret;
}

static inline retcode_t process_store_transactions_request(iota_api_http_t *const http, char const *const payload,
                                                           char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  store_transactions_req_t *req = store_transactions_req_new();
  error_res_t *error = NULL;

  if (req == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.store_transactions_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_store_transactions(http->api, tangle, req, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  }

done:
  store_transactions_req_free(&req);

  return ret;
}

static inline retcode_t process_were_addresses_spent_from_request(iota_api_http_t *const http,
                                                                  char const *const payload, char_buffer_t *const out) {
  retcode_t ret = RC_OK;
  were_addresses_spent_from_req_t *req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_res_t *res = were_addresses_spent_from_res_new();
  error_res_t *error = NULL;

  if (req == NULL || res == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if ((ret = http->serializer.vtable.were_addresses_spent_from_deserialize_request(payload, req)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_api_were_addresses_spent_from(http->api, tangle, sap, req, res, &error)) != RC_OK) {
    error_serialize_response(http, &error, NULL, out);
  } else {
    ret = http->serializer.vtable.were_addresses_spent_from_serialize_response(res, out);
  }

done:
  were_addresses_spent_from_req_free(&req);
  were_addresses_spent_from_res_free(&res);

  return ret;
}

static retcode_t iota_api_http_process_request(iota_api_http_t *const http, char const *const command,
                                               char const *const payload, char_buffer_t *const out) {
  error_res_t *error = NULL;

  if (!tangle) {
    storage_connection_config_t db_conf = {.db_path = http->api->conf.tangle_db_path};

    tangle = (tangle_t *)calloc(1, sizeof(tangle_t));
    utarray_push_back(http->tangle_db_connections, tangle);
    iota_tangle_init(tangle, &db_conf);
  }

  if (!sap) {
    storage_connection_config_t db_conf = {.db_path = http->api->conf.spent_addresses_db_path};

    sap = (spent_addresses_provider_t *)calloc(1, sizeof(spent_addresses_provider_t));
    utarray_push_back(http->spent_addresses_db_connections, sap);
    iota_spent_addresses_provider_init(sap, &db_conf);
  }

  for (size_t i = 0; http->api->conf.remote_limit_api[i]; i++) {
    if (strcmp(http->api->conf.remote_limit_api[i], command) == 0) {
      error_serialize_response(http, &error, "This command is not available on this node", out);
      return RC_OK;
    }
  }

  if (strcmp(command, "addNeighbors") == 0) {
    return process_add_neighbors_request(http, payload, out);
  } else if (strcmp(command, "attachToTangle") == 0) {
    return process_attach_to_tangle_request(http, payload, out);
  } else if (strcmp(command, "broadcastTransactions") == 0) {
    return process_broadcast_transactions_request(http, payload, out);
  } else if (strcmp(command, "checkConsistency") == 0) {
    return process_check_consistency_request(http, payload, out);
  } else if (strcmp(command, "findTransactions") == 0) {
    return process_find_transactions_request(http, payload, out);
  } else if (strcmp(command, "getBalances") == 0) {
    return process_get_balances_request(http, payload, out);
  } else if (strcmp(command, "getInclusionStates") == 0) {
    return process_get_inclusion_states_request(http, payload, out);
  } else if (strcmp(command, "getMissingTransactions") == 0) {
    return process_get_missing_transactions_request(http, payload, out);
  } else if (strcmp(command, "getNeighbors") == 0) {
    return process_get_neighbors_request(http, payload, out);
  } else if (strcmp(command, "getNodeInfo") == 0) {
    return process_get_node_info_request(http, payload, out);
  } else if (strcmp(command, "getTips") == 0) {
    return process_get_tips_request(http, payload, out);
  } else if (strcmp(command, "getTransactionsToApprove") == 0) {
    return process_get_transactions_to_approve_request(http, payload, out);
  } else if (strcmp(command, "getTrytes") == 0) {
    return process_get_trytes_request(http, payload, out);
  } else if (strcmp(command, "removeNeighbors") == 0) {
    return process_remove_neighbors_request(http, payload, out);
  } else if (strcmp(command, "storeTransactions") == 0) {
    return process_store_transactions_request(http, payload, out);
  } else if (strcmp(command, "wereAddressesSpentFrom") == 0) {
    return process_were_addresses_spent_from_request(http, payload, out);
  } else {
    error_serialize_response(http, &error, "Unknown command", out);
  }

  return RC_OK;
}

static int iota_api_http_header_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
  iota_api_http_session_t *sess = cls;
  UNUSED(kind);

  if (0 == strcmp(MHD_HTTP_HEADER_CONTENT_TYPE, key)) {
    sess->valid_content_type = !strcmp("application/json", value);
  } else if (0 == strcmp("X-IOTA-API-Version", key)) {
    sess->valid_api_version = !strcmp("1", value);
  }

  return MHD_YES;
}

static int iota_api_http_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
                                 const char *version, const char *upload_data, size_t *upload_data_size, void **ptr) {
  int ret = MHD_NO;
  iota_api_http_t *api = (iota_api_http_t *)cls;
  iota_api_http_session_t *sess = *ptr;
  struct MHD_Response *response = NULL;
  cJSON *json_obj = NULL, *json_item = NULL;
  char_buffer_t *response_buf = NULL;
  error_res_t *error = NULL;
  UNUSED(url);
  UNUSED(version);

  if (strncmp(method, MHD_HTTP_METHOD_POST, 4) != 0) {
    return MHD_NO;
  }

  if (sess == NULL) {
    sess = calloc(1, sizeof(iota_api_http_session_t));
    *ptr = sess;

    MHD_get_connection_values(connection, MHD_HEADER_KIND, iota_api_http_header_iter, sess);
    return MHD_YES;
  }

  if (!sess->valid_api_version || !sess->valid_content_type) {
    ret = MHD_NO;
    goto cleanup;
  }

  // While upload_data_size > 0 process upload_data
  if (*upload_data_size > 0) {
    if (sess->request == NULL) {
      sess->request = char_buffer_new();
      char_buffer_allocate(sess->request, *upload_data_size);
      strncpy(sess->request->data, upload_data, *upload_data_size);
    } else {
      log_error(logger_id,
                "cIRI HTTP API currently does not support parsing such large "
                "requests.\n");
      ret = MHD_NO;
      goto cleanup;
    }

    *upload_data_size = 0;
    return MHD_YES;
  }

  if (sess->request == NULL) {
    // POST but no body, so we skip this request
    ret = MHD_NO;
    goto cleanup;
  }

  response_buf = char_buffer_new();
  json_obj = cJSON_Parse(sess->request->data);

  if (!json_obj) {
    error_serialize_response(api, &error, "Invalid JSON request", response_buf);
  } else if (!cJSON_HasObjectItem(json_obj, "command")) {
    error_serialize_response(api, &error, "Command parameter has not been specified in the request", response_buf);
  } else {
    json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "command");
    if (!cJSON_IsString(json_item) || (json_item->valuestring == NULL)) {
      cJSON_Delete(json_obj);
      ret = MHD_NO;
      goto cleanup;
    }

    iota_api_http_process_request(api, json_item->valuestring, sess->request->data, response_buf);
  }

  response = MHD_create_response_from_buffer(response_buf->length, response_buf->data, MHD_RESPMEM_MUST_COPY);
  MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

cleanup:
  char_buffer_free(response_buf);
  cJSON_Delete(json_obj);
  if (sess) {
    if (sess->request) {
      char_buffer_free(sess->request);
    }

    free(sess);
  }
  *ptr = NULL;
  return ret;
}

retcode_t iota_api_http_init(iota_api_http_t *const http, iota_api_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(API_HTTP_LOGGER_ID, LOGGER_DEBUG, true);

  http->running = false;
  http->api = api;

  utarray_new(http->tangle_db_connections, &ut_ptr_icd);
  utarray_new(http->spent_addresses_db_connections, &ut_ptr_icd);

  init_json_serializer(&http->serializer);

  return RC_OK;
}

retcode_t iota_api_http_start(iota_api_http_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running == true) {
    return RC_OK;
  }

  api->state = MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD | MHD_USE_ERROR_LOG | MHD_USE_DEBUG,
                                api->api->conf.http_port, NULL, NULL, iota_api_http_handler, api, MHD_OPTION_END);
  api->running = true;

  return RC_OK;
}

retcode_t iota_api_http_stop(iota_api_http_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running == false) {
    return RC_OK;
  }

  MHD_stop_daemon(api->state);

  api->running = false;

  return RC_OK;
}

retcode_t iota_api_http_destroy(iota_api_http_t *const api) {
  tangle_t *tangle_iter = NULL;
  spent_addresses_provider_t *spent_addresses_iter = NULL;

  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running) {
    return RC_STILL_RUNNING;
  }

  for (tangle_iter = (tangle_t *)utarray_back(api->tangle_db_connections); tangle_iter != NULL;
       tangle_iter = (tangle_t *)utarray_back(api->tangle_db_connections)) {
    iota_tangle_destroy(tangle_iter);
    free(tangle_iter);
  }

  utarray_free(api->tangle_db_connections);

  for (spent_addresses_iter = (spent_addresses_provider_t *)utarray_back(api->spent_addresses_db_connections);
       spent_addresses_iter != NULL;
       spent_addresses_iter = (spent_addresses_provider_t *)utarray_back(api->spent_addresses_db_connections)) {
    iota_spent_addresses_provider_destroy(spent_addresses_iter);
    free(spent_addresses_iter);
  }

  utarray_free(api->spent_addresses_db_connections);

  logger_helper_release(logger_id);

  return RC_OK;
}
