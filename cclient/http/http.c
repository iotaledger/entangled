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
#include "utils/macros.h"
#include "utils/sockets.h"

#define RECEIVE_BUFFER_SIZE 1024

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

static int request_parse_header_complete(struct _response_ctx* response);
static int request_parse_data(struct _response_ctx* response,
                              const unsigned char* at, size_t length);
static int request_parse_message_complete(struct _response_ctx* response);

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

static int connect_to_iota_service(char const* const hostname,
                                   const size_t port) {
  int sockfd;
  struct addrinfo hints, *serverinfo, *info;
  int res;
  char port_string[6];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;  // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;
  sprintf(port_string, "%lu", port);
  if ((res = getaddrinfo(hostname, port_string, &hints, &serverinfo)) != 0) {
    return -1;
  }
  // loop through all the results and connect to the first we can
  for (info = serverinfo; info; info = info->ai_next) {
    if ((sockfd = socket(info->ai_family, info->ai_socktype,
                         info->ai_protocol)) == -1) {
      continue;
    }
    if (connect(sockfd, info->ai_addr, info->ai_addrlen) == -1) {
      close(sockfd);
      continue;
    }
    // Successfully connection
    break;
  }
  // If no info was found return error
  if (!info) {
    return -1;
  }
  // Free resources allocated by getaddrinfo
  freeaddrinfo(serverinfo);
  // Return socket fd - 0 or greater
  return sockfd;
}

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
  while ((num_received = recv(sockfd, buffer, RECEIVE_BUFFER_SIZE, 0)) > 0) {
    int parsed = http_parser_execute(&parser, &settings, buffer, num_received);
    // A parsing error occured, or an error in a callback
    if (parsed < num_received) {
      return -1;
    } else if (response_context.status == IOTA_REQUEST_STATUS_ERROR) {
      return -1;
    } else if (response_context.status == IOTA_REQUEST_STATUS_DONE) {
      return RC_OK;
    }
    memset(buffer, 0, sizeof(buffer));
  }
  if (num_received <= 0) {
    return -1;
  }
  return RC_OK;
}

static retcode_t send_data_to_iota_service(int sockfd, char const* const data,
                                           size_t data_length) {
  char* ptr = (char*)data;
  while (data_length > 0) {
    ssize_t num_sent = send(sockfd, ptr, data_length, 0);
    if (num_sent < 0) {
      return -1;
    }
    ptr += num_sent;
    data_length -= num_sent;
  }
  return RC_OK;
}

static retcode_t send_headers_to_iota_service(int sockfd,
                                              http_info_t const* http_settings,
                                              size_t data_length,
                                              char const* const path) {
  char header[100];
  sprintf(header, "POST %s HTTP/1.1\r\n", path);
  if (send_data_to_iota_service(sockfd, header, strlen(header)) < 0) {
    return -1;
  }
  sprintf(header, "Host: %s\r\n", http_settings->host);
  if (send_data_to_iota_service(sockfd, header, strlen(header)) < 0) {
    return -1;
  }
  sprintf(header, "X-IOTA-API-Version: %d\r\n", http_settings->api_version);
  if (send_data_to_iota_service(sockfd, header, strlen(header)) < 0) {
    return -1;
  }
  sprintf(header, "Content-Type: application/x-www-form-urlencoded\r\n");
  if (send_data_to_iota_service(sockfd, header, strlen(header)) < 0) {
    return -1;
  }
  sprintf(header, "Content-Length: %lu\r\n", data_length);
  if (send_data_to_iota_service(sockfd, header, strlen(header)) < 0) {
    return -1;
  }
  if (send_data_to_iota_service(sockfd, "\r\n", 2) < 0) {
    return -1;
  }
  return RC_OK;
}

retcode_t _iota_service_query(const void* const service_opaque,
                              char_buffer_t* obj, char_buffer_t* response,
                              char const* const path) {
  int sockfd = -1;
  retcode_t result = RC_OK;
  const iota_client_service_t* const service =
      (const iota_client_service_t* const)service_opaque;
  const http_info_t* http_settings = &service->http;

  if ((sockfd = connect_to_iota_service(http_settings->host,
                                        http_settings->port)) < 0) {
    return -1;
  }

  if (send_headers_to_iota_service(sockfd, http_settings, obj->length, path) !=
      RC_OK) {
    result = -1;
    goto cleanup;
  }
  if (send_data_to_iota_service(sockfd, obj->data, obj->length) != RC_OK) {
    result = -1;
    goto cleanup;
  }
  if (read_data_from_iota_service(sockfd, response) != RC_OK) {
    result = -1;
    goto cleanup;
  }
cleanup:
  close(sockfd);
  return result;
}

retcode_t iota_service_query(const void* const service_opaque,
                             char_buffer_t* obj, char_buffer_t* response) {
  return _iota_service_query(service_opaque, obj, response, "/");
}
