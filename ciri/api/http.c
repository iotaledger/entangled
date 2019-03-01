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
#include "cclient/types/types.h"
#include "ciri/api/api.h"
#include "ciri/api/http.h"
#include "utils/logger_helper.h"

#define API_HTTP_LOGGER_ID "api_http"

static logger_id_t logger_id;
static _Thread_local tangle_t *tangle;

typedef struct iota_api_http_session_s {
  bool valid_api_version;
  bool valid_content_type;
  char_buffer_t *request;
} iota_api_http_session_t;

static retcode_t iota_api_http_process_request(iota_api_http_t *http,
                                               const char *command,
                                               const char *payload,
                                               char_buffer_t *const out) {
  retcode_t ret = RC_OK;

  if (!tangle) {
    log_error(logger_id, "Instantiating new HTTP API database connection\n");

    tangle = (tangle_t *)calloc(1, sizeof(tangle_t));
    utarray_push_back(http->db_connections, tangle);
    iota_tangle_init(tangle, http->db_config);
  }

  if (strcmp(command, "getNodeInfo") == 0) {
    get_node_info_res_t *res = get_node_info_res_new();
    if (!res) {
      ret = RC_OOM;
      goto gni;
    }
    ret = iota_api_get_node_info(http->api, res);
    if (ret) goto gni;

    ret = http->serializer.vtable.get_node_info_serialize_response(
        &http->serializer, res, out);

  gni:
    get_node_info_res_free(&res);
  } else if (strcmp(command, "checkConsistency") == 0) {
    check_consistency_req_t *req = check_consistency_req_new();
    check_consistency_res_t *res = check_consistency_res_new();

    ret = http->serializer.vtable.check_consistency_deserialize_request(
        &http->serializer, payload, req);
    if (ret) goto cc;

    ret = iota_api_check_consistency(http->api, tangle, req, res);
    if (ret) goto cc;

    ret = http->serializer.vtable.check_consistency_serialize_response(
        &http->serializer, res, out);

  cc:
    check_consistency_req_free(&req);
    check_consistency_res_free(&res);
  } else {
    ret = RC_API_INVALID_COMMAND;
  }

  return ret;
}

static int iota_api_http_header_iter(void *cls, enum MHD_ValueKind kind,
                                     const char *key, const char *value) {
  iota_api_http_session_t *sess = cls;

  if (0 == strcmp(MHD_HTTP_HEADER_CONTENT_TYPE, key)) {
    sess->valid_content_type = !strcmp("application/json", value);
  } else if (0 == strcmp("X-IOTA-API-Version", key)) {
    sess->valid_api_version = !strcmp("1", value);
  }

  return MHD_YES;
}

static int iota_api_http_handler(void *cls, struct MHD_Connection *connection,
                                 const char *url, const char *method,
                                 const char *version, const char *upload_data,
                                 size_t *upload_data_size, void **ptr) {
  int ret = MHD_NO;
  iota_api_http_t *api = (iota_api_http_t *)cls;
  iota_api_http_session_t *sess = *ptr;
  struct MHD_Response *response = NULL;
  cJSON *json_obj = NULL, *json_item = NULL;
  char_buffer_t *response_buf = NULL;
  char *command_str = NULL;

  if (strncmp(method, MHD_HTTP_METHOD_POST, 4) != 0) {
    return MHD_NO;
  }

  if (sess == NULL) {
    sess = calloc(1, sizeof(iota_api_http_session_t));
    *ptr = sess;

    MHD_get_connection_values(connection, MHD_HEADER_KIND,
                              iota_api_http_header_iter, sess);
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

  json_obj = cJSON_Parse(sess->request->data);
  if (!json_obj) {
    ret = MHD_NO;
    goto cleanup;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "command");
  if (!cJSON_IsString(json_item) || (json_item->valuestring == NULL)) {
    cJSON_Delete(json_obj);
    ret = MHD_NO;
    goto cleanup;
  }

  command_str = strdup(json_item->valuestring);
  cJSON_Delete(json_obj);

  response_buf = char_buffer_new();
  if (iota_api_http_process_request(api, command_str, sess->request->data,
                                    response_buf)) {
    free(command_str);
    char_buffer_free(response_buf);
    ret = MHD_NO;
    goto cleanup;
  }
  free(command_str);

  response = MHD_create_response_from_buffer(
      response_buf->length, response_buf->data, MHD_RESPMEM_MUST_COPY);
  MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE,
                          "application/json");
  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

  char_buffer_free(response_buf);
cleanup:
  if (sess) {
    if (sess->request) {
      char_buffer_free(sess->request);
    }

    free(sess);
  }
  *ptr = NULL;
  return ret;
}

retcode_t iota_api_http_init(iota_api_http_t *const http, iota_api_t *const api,
                             connection_config_t *const db_config) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(API_HTTP_LOGGER_ID, LOGGER_DEBUG, true);

  http->running = false;
  http->api = api;
  http->db_config = db_config;

  utarray_new(http->db_connections, &ut_ptr_icd);

  init_json_serializer(&http->serializer);

  return RC_OK;
}

retcode_t iota_api_http_start(iota_api_http_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running == true) {
    return RC_OK;
  }

  api->state = MHD_start_daemon(
      MHD_USE_AUTO_INTERNAL_THREAD | MHD_USE_ERROR_LOG | MHD_USE_DEBUG, 14265,
      NULL, NULL, iota_api_http_handler, api, MHD_OPTION_END);
  api->running = true;

  return RC_OK;
}

retcode_t iota_api_http_stop(iota_api_http_t *const api) {
  retcode_t ret = RC_OK;
  tangle_t *tangle;

  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running == false) {
    return RC_OK;
  }

  MHD_stop_daemon(api->state);

  for (tangle = (tangle_t *)utarray_back(api->db_connections); tangle != NULL;
       tangle = (tangle_t *)utarray_back(api->db_connections)) {
    log_error(logger_id, "Destroying HTTP API database connection\n");
    iota_tangle_destroy(tangle);
    free(tangle);
    utarray_pop_back(api->db_connections);
  }

  api->running = false;
  return ret;
}

retcode_t iota_api_http_destroy(iota_api_http_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running) {
    return RC_STILL_RUNNING;
  }

  utarray_free(api->db_connections);

  logger_helper_release(logger_id);
  return RC_OK;
}
