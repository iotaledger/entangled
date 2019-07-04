/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include <uv.h>

#include "ciri/node/network/tcp_server.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"

#define TCP_SERVER_LOGGER_ID "tcp_server"

static logger_id_t logger_id;
static uv_loop_t *loop;
static uv_tcp_t server;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

void echo_write(uv_write_t *req, int status) {
  if (status) {
    fprintf(stderr, "Write error %s\n", uv_strerror(status));
  }
  free(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  if (nread < 0) {
    if (nread != UV_EOF) {
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
      uv_close((uv_handle_t *)client, NULL);
    }
  } else if (nread > 0) {
    uv_write_t *req = (uv_write_t *)malloc(sizeof(uv_write_t));
    uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
    uv_write(req, client, &wrbuf, 1, echo_write);
  }

  if (buf->base) {
    free(buf->base);
  }
}

void on_new_connection(uv_stream_t *server, int status) {
  fprintf(stderr, "NEW CONN\n");
  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    return;
  }

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
  } else {
    uv_close((uv_handle_t *)client, NULL);
  }
}

/*



int r =
if (r) {
  fprintf(stderr, "Listen error %s\n", uv_strerror(r));
  return 1;
}
return uv_run(loop, UV_RUN_DEFAULT);
*/

/*
 * Private functions
 */

static void *tcp_server_routine(tcp_server_t *const tcp_server) {
  if (uv_run(loop, UV_RUN_DEFAULT) != 0) {
    log_critical(logger_id, "Starting tcp server loop failed\n");
    return NULL;
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t tcp_server_init(tcp_server_t *const tcp_server, node_t *const node) {
  struct sockaddr_in addr;

  if (tcp_server == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(TCP_SERVER_LOGGER_ID, LOGGER_DEBUG, true);
  loop = uv_default_loop();

  tcp_server->running = false;
  tcp_server->node = node;

  uv_tcp_init(loop, &server);
  uv_ip4_addr(tcp_server->node->conf.neighboring_address, tcp_server->node->conf.neighboring_port, &addr);
  uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
  uv_listen((uv_stream_t *)&server, 128, on_new_connection);

  return RC_OK;
}

retcode_t tcp_server_start(tcp_server_t *const tcp_server) {
  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(logger_id, "Spawning TCP server thread\n");
  tcp_server->running = true;
  if (thread_handle_create(&tcp_server->thread, (thread_routine_t)tcp_server_routine, tcp_server) != 0) {
    log_critical(logger_id, "Spawning TCP server thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t tcp_server_stop(tcp_server_t *const tcp_server) {
  retcode_t ret = RC_OK;

  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  } else if (tcp_server->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down TCP server thread\n");
  tcp_server->running = false;
  uv_stop(loop);
  if (thread_handle_join(tcp_server->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down TCP server thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t tcp_server_destroy(tcp_server_t *const tcp_server) {
  if (tcp_server == NULL) {
    return RC_NULL_PARAM;
  } else if (tcp_server->running) {
    return RC_STILL_RUNNING;
  }

  logger_helper_release(logger_id);

  return RC_OK;
}
