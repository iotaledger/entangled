/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socket.h"

int socket_connect(char const *const hostname, const size_t port) {
  struct addrinfo hints, *serverinfo, *info;
  char port_string[6] = {};
  int sockfd = -1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;  // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;
  sprintf(port_string, "%zu", port);
  if (getaddrinfo(hostname, port_string, &hints, &serverinfo) != 0) {
    return -1;
  }
  // loop through all the results and connect to the first we can
  for (info = serverinfo; info; info = info->ai_next) {
    if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1) {
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

static void tls_init(mbedtls_ctx_t *tls_ctx) {
  mbedtls_entropy_init(&tls_ctx->entropy);
  mbedtls_ctr_drbg_init(&tls_ctx->ctr_drbg);
  mbedtls_ssl_init(&tls_ctx->ssl);
  mbedtls_ssl_config_init(&tls_ctx->ssl_conf);
  mbedtls_x509_crt_init(&tls_ctx->cacert);
  mbedtls_x509_crt_init(&tls_ctx->client_cacert);
  mbedtls_pk_init(&tls_ctx->pk_ctx);
  mbedtls_net_init(&tls_ctx->net_ctx);
}

static void tls_destroy(mbedtls_ctx_t *tls_ctx) {
  mbedtls_entropy_free(&tls_ctx->entropy);
  mbedtls_ctr_drbg_free(&tls_ctx->ctr_drbg);
  mbedtls_ssl_free(&tls_ctx->ssl);
  mbedtls_ssl_config_free(&tls_ctx->ssl_conf);
  mbedtls_x509_crt_free(&tls_ctx->cacert);
  mbedtls_x509_crt_free(&tls_ctx->client_cacert);
  mbedtls_pk_free(&tls_ctx->pk_ctx);
  mbedtls_net_free(&tls_ctx->net_ctx);
}

void tls_socket_close(mbedtls_ctx_t *tls_ctx) { tls_destroy(tls_ctx); }

int tls_socket_send(mbedtls_ctx_t *ctx, char const *data, size_t size) {
  return mbedtls_ssl_write(&ctx->ssl, (const unsigned char *)data, size);
}

int tls_socket_recv(mbedtls_ctx_t *ctx, char *data, size_t size) {
  return mbedtls_ssl_read(&ctx->ssl, (unsigned char *)data, size);
}

int tls_socket_connect(mbedtls_ctx_t *tls_ctx, char const *host, uint16_t port, char const *ca_pem,
                       char const *client_cert_pem, char const *client_pk_pem, retcode_t *error) {
  int mbedtls_ret = -1;
  char const drgb_pres[] = "iota_tls_client";
  bool is_client_auth = false;
  char port_string[6] = {};
  sprintf(port_string, "%d", port);

  tls_init(tls_ctx);

  // init RNG
  if (mbedtls_ctr_drbg_seed(&tls_ctx->ctr_drbg, mbedtls_entropy_func, &tls_ctx->entropy,
                            (unsigned char const *)drgb_pres, strlen(drgb_pres)) != 0) {
    *error = RC_UTILS_SOCKET_TLS_RNG;
    return -1;
  }

  // parsing CA
  if (ca_pem) {
    if (mbedtls_x509_crt_parse(&tls_ctx->cacert, (unsigned char *)ca_pem, strlen(ca_pem) + 1) != 0) {
      *error = RC_UTILS_SOCKET_TLS_CA;
      return -1;
    }
  }

  // parsing client CA and PK if it exists
  if ((NULL != client_cert_pem) && (NULL != client_pk_pem)) {
    if (mbedtls_x509_crt_parse(&tls_ctx->client_cacert, (unsigned char *)client_cert_pem,
                               strlen(client_cert_pem) + 1) != 0) {
      *error = RC_UTILS_SOCKET_TLS_CLIENT_PEM;
      return -1;
    }
    if (mbedtls_pk_parse_key(&tls_ctx->pk_ctx, (unsigned char *)client_pk_pem, strlen(client_pk_pem) + 1, NULL, 0) !=
        0) {
      *error = RC_UTILS_SOCKET_TLS_CLIENT_PK;
      return -1;
    }
    is_client_auth = true;
  }

  // Start the connection
  if (mbedtls_net_connect(&tls_ctx->net_ctx, host, port_string, MBEDTLS_NET_PROTO_TCP) != 0) {
    *error = RC_UTILS_SOCKET_CONNECT;
    return -1;
  }
  // client setup
  if (mbedtls_ssl_config_defaults(&tls_ctx->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                  MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
    *error = RC_UTILS_SOCKET_TLS_CONF;
    return -1;
  }

  mbedtls_ssl_conf_ca_chain(&tls_ctx->ssl_conf, &tls_ctx->cacert, NULL);
  mbedtls_ssl_conf_rng(&tls_ctx->ssl_conf, mbedtls_ctr_drbg_random, &tls_ctx->ctr_drbg);
  // tls authentication mode
  mbedtls_ssl_conf_authmode(&tls_ctx->ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);

  if (mbedtls_ssl_setup(&tls_ctx->ssl, &tls_ctx->ssl_conf) != 0) {
    *error = RC_UTILS_SOCKET_TLS_AUTHMODE;
    return -1;
  }

  mbedtls_ssl_set_hostname(&tls_ctx->ssl, host);

  // BIO callbacks
  mbedtls_ssl_set_bio(&tls_ctx->ssl, &tls_ctx->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

  if (is_client_auth) {
    if (mbedtls_ssl_conf_own_cert(&tls_ctx->ssl_conf, &tls_ctx->client_cacert, &tls_ctx->pk_ctx) != 0) {
      *error = RC_UTILS_SOCKET_CLIENT_AUTH;
      return -1;
    }
  }

  while ((mbedtls_ret = mbedtls_ssl_handshake(&tls_ctx->ssl)) != 0) {
    if (mbedtls_ret != MBEDTLS_ERR_SSL_WANT_READ && mbedtls_ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      // handshake failed
      *error = RC_UTILS_SOCKET_TLS_HANDSHAKE;
      return -1;
    }
  }

  // Verify the server certificate
  if (tls_ctx->ssl_conf.authmode != MBEDTLS_SSL_VERIFY_NONE) {
    mbedtls_ssl_get_verify_result(&tls_ctx->ssl);
  }
  return tls_ctx->net_ctx.fd;
}
