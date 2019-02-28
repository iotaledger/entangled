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
#include "ciri/api/http.h"
#include "utils/logger_helper.h"

#define API_HTTP_LOGGER_ID "api_http"

static logger_id_t logger_id;

typedef struct iota_api_http_sess_s {
  bool valid_api_version;
  bool valid_content_type;
  char_buffer_t *request;
} iota_api_http_sess_t;

static int iota_api_http_header_iter(void *cls, enum MHD_ValueKind kind,
                                     const char *key, const char *value) {
  iota_api_http_sess_t *sess = cls;

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
  int ret;
  iota_api_http_t *api = (iota_api_http_t *)cls;
  iota_api_http_sess_t *sess = *ptr;
  struct MHD_Response *response;

  if (strncmp(method, MHD_HTTP_METHOD_POST, 4) != 0) {
    return MHD_NO;
  }

  if (sess == NULL) {
    sess = calloc(1, sizeof(iota_api_http_sess_t));
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

  cJSON *json_obj = cJSON_Parse(sess->request->data);
  cJSON *json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "command");

  if (!cJSON_IsString(json_item) || (json_item->valuestring == NULL)) {
    cJSON_Delete(json_obj);
    ret = MHD_NO;
    goto cleanup;
  }

  response = MHD_create_response_from_buffer(strlen(json_item->valuestring),
                                             json_item->valuestring,
                                             MHD_RESPMEM_MUST_COPY);
  MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE,
                          "application/json");
  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

  cJSON_Delete(json_obj);

cleanup:
  if (sess) {
    free(sess);
  }
  *ptr = NULL;
  return ret;
}

retcode_t iota_api_http_init(iota_api_http_t *const http,
                             iota_api_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(API_HTTP_LOGGER_ID, LOGGER_DEBUG, true);

  http->api = api;
  http->running = false;

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

  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running == false) {
    return RC_OK;
  }

  MHD_stop_daemon(api->state);

  api->running = false;
  return ret;
}

retcode_t iota_api_http_destroy(iota_api_http_t *const api) {
  if (api == NULL) {
    return RC_NULL_PARAM;
  } else if (api->running) {
    return RC_STILL_RUNNING;
  }

  logger_helper_release(logger_id);
  return RC_OK;
}
