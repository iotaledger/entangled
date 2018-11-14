/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "http.h"
#include <string.h>
#include "cclient/service.h"
#include "http_parser.h"
#include "network/sockets.h"
#include "utils/macros.h"

typedef enum {
  IOTA_REQUEST_STATUS_OK,
  IOTA_REQUEST_STATUS_DONE,
  IOTA_REQUEST_STATUS_ERROR
} IOTA_REQUEST_STATUS;

struct _response_ctx {
  http_parser* parser;
  char_buffer_t* response;
  size_t offset;
  IOTA_REQUEST_STATUS status;
};

const char* khttp_ApplicationJson = "application/json";
const char* khttp_ApplicationFormUrlencoded =
    "application/x-www-form-urlencoded";

// Callback declarations for parser
static int request_parse_header_complete(struct _response_ctx* response);
static int request_parse_data(struct _response_ctx* response,
                              const unsigned char* at, size_t length);
static int request_parse_message_complete(struct _response_ctx* response);

// Callback implementation for parser
static int request_parse_header_complete_cb(http_parser* parser) {
  return request_parse_header_complete((struct _response_ctx*)parser->data);
}

static int request_parse_data_cb(http_parser* parser, const char* at,
                                 size_t length) {
  return request_parse_data((struct _response_ctx*)parser->data,
                            (const unsigned char*)at, length);
}

static int request_parse_message_complete_cb(http_parser* parser) {
  return request_parse_message_complete((struct _response_ctx*)parser->data);
}

// Callback implementation for context
static int request_parse_header_complete(struct _response_ctx* response) {
  size_t data_len = response->parser->content_length;
  if (!data_len) {
    response->status = IOTA_REQUEST_STATUS_ERROR;
    return -1;
  }
  if (char_buffer_allocate(response->response, data_len) != RC_OK) {
    response->status = IOTA_REQUEST_STATUS_ERROR;
    return -1;
  }
  response->offset = 0;
  return RC_OK;
}

static int request_parse_data(struct _response_ctx* response,
                              const unsigned char* at, size_t length) {
  memcpy(response->response->data + response->offset, at, length);
  response->offset += length;
  return RC_OK;
}

static int request_parse_message_complete(struct _response_ctx* response) {
  response->status = IOTA_REQUEST_STATUS_DONE;
  return RC_OK;
}

// Socket creation
static retcode_t connect_to_iota_service(char const* const hostname,
                                         const size_t port, int* sockfd) {
  if ((*sockfd = open_client_socket(hostname, port)) == -1) {
    return RC_CCLIENT_HTTP;
  }
  return RC_OK;
}

static retcode_t read_data_from_iota_service(int sockfd,
                                             char_buffer_t* response) {
  char buffer[RECEIVE_BUFFER_SIZE] = {0};
  ssize_t num_received = 0;
  // Setup parser settings - callbacks
  http_parser_settings settings = {0};
  settings.on_headers_complete = &request_parse_header_complete_cb;
  settings.on_body = &request_parse_data_cb;
  settings.on_message_complete = &request_parse_message_complete_cb;
  // Setup parser
  http_parser parser = {0};
  // Setup response structure
  struct _response_ctx response_context = {0};
  response_context.parser = &parser;
  response_context.response = response;
  response_context.status = IOTA_REQUEST_STATUS_OK;
  response_context.offset = 0;
  // Initialize parser
  http_parser_init(&parser, HTTP_RESPONSE);
  parser.data = &response_context;
  // Loop over received data
  while ((num_received = receive_on_socket_wait(sockfd, buffer,
                                                RECEIVE_BUFFER_SIZE)) > 0) {
    int parsed = http_parser_execute(&parser, &settings, buffer, num_received);
    // A parsing error occured, or an error in a callback
    if (parsed < num_received ||
        response_context.status == IOTA_REQUEST_STATUS_ERROR) {
      return RC_CCLIENT_HTTP_RES;
    } else if (response_context.status == IOTA_REQUEST_STATUS_DONE) {
      return RC_OK;
    }
    memset(buffer, 0, sizeof(buffer));
  }
  if (num_received <= 0) {
    return RC_CCLIENT_HTTP;
  }
  return RC_OK;
}

static retcode_t send_data_to_iota_service(int sockfd, char const* const data,
                                           size_t data_length) {
  char* ptr = (char*)data;
  while (data_length > 0) {
    ssize_t num_sent = send_on_socket_wait(sockfd, ptr, data_length);
    if (num_sent < 0) {
      return RC_CCLIENT_HTTP;
    }
    ptr += num_sent;
    data_length -= num_sent;
  }
  return RC_OK;
}

static retcode_t send_headers_to_iota_service(int sockfd,
                                              http_info_t const* http_settings,
                                              size_t data_length) {
  static char* header_template =
      "POST %s HTTP/1.1\r\n"
      "Host: %s\r\n"
      "X-IOTA-API-Version: %d\r\n"
      "Content-Type: %s\r\n"
      "Accept: %s\r\n"
      "Content-Length: %lu\r\n"
      "\r\n";
  char header[256];
  sprintf(header, header_template, http_settings->path, http_settings->host,
          http_settings->api_version, http_settings->content_type,
          http_settings->accept, data_length);
  return send_data_to_iota_service(sockfd, header, strlen(header));
}

retcode_t iota_service_query(const void* const service_opaque,
                             char_buffer_t* obj, char_buffer_t* response) {
  int sockfd = -1;
  retcode_t result = RC_OK;
  const iota_client_service_t* const service =
      (const iota_client_service_t* const)service_opaque;
  const http_info_t* http_settings = &service->http;

  if ((result = connect_to_iota_service(
           http_settings->host, http_settings->port, &sockfd)) != RC_OK) {
    goto cleanup;
  }

  if ((result = send_headers_to_iota_service(sockfd, http_settings,
                                             obj->length)) != RC_OK) {
    goto cleanup;
  }
  if ((result = send_data_to_iota_service(sockfd, obj->data, obj->length)) !=
      RC_OK) {
    goto cleanup;
  }
  if ((result = read_data_from_iota_service(sockfd, response)) != RC_OK) {
    goto cleanup;
  }
cleanup:
  if (sockfd != -1) {
    close_socket(sockfd);
  }
  return result;
}
