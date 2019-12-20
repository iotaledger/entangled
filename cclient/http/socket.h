/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CCLIENT_HTTP_SOCKETS_H__
#define __CCLIENT_HTTP_SOCKETS_H__

#include "mbedtls/certs.h"
#include "mbedtls/config.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#include "common/errors.h"

// Windows headers
#ifdef __WIN32__
#include "utils/windows.h"

#include <ws2tcpip.h>

// ESP headers
#elif __XTENSA__
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
// Should cover all POSIX complient platforms
// Linux, macOS...
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int socket_connect(char const *const hostname, const size_t port);
static inline void socket_close(int sockfd) {
  if (sockfd != -1) {
    close(sockfd);
  }
}
static inline int socket_recv(int sockfd, void *buffer, size_t len) { return recv(sockfd, buffer, len, 0); }
static inline int socket_send(int sockfd, const void *buffer, size_t len) { return send(sockfd, buffer, len, 0); }

typedef struct mbedtls_ctx_s {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config ssl_conf;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt client_cacert;
  mbedtls_pk_context pk_ctx;
  mbedtls_net_context net_ctx;
} mbedtls_ctx_t;

int tls_socket_connect(mbedtls_ctx_t *tls_ctx, char const *host, uint16_t port, char const *ca_pem,
                       char const *client_cert_pem, char const *client_pk_pem, retcode_t *error);
int tls_socket_send(mbedtls_ctx_t *ctx, char const *data, size_t size);
int tls_socket_recv(mbedtls_ctx_t *ctx, char *data, size_t size);
void tls_socket_close(mbedtls_ctx_t *tls_ctx);

#ifdef __cplusplus
}
#endif

#endif  // __CCLIENT_HTTP_SOCKETS_H__
