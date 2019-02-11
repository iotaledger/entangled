/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __SOCKETS_H
#define __SOCKETS_H

#ifdef __cplusplus
extern "C" {
#endif

// Windows headers
#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>
#define RECEIVE_BUFFER_SIZE 4096
// ESP headers
#elif __XTENSA__
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#define RECEIVE_BUFFER_SIZE 2048
// Should cover all POSIX complient platforms
// Linux, macOS...
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define RECEIVE_BUFFER_SIZE 4096
#endif

int open_client_socket(char const *const hostname, const size_t port);
static inline void close_socket(int sockfd) {
  if (sockfd != -1) {
    close(sockfd);
  }
}
static inline int receive_on_socket_wait(int sockfd, void *buffer, size_t len) {
  return recv(sockfd, buffer, len, 0);
}
static inline int send_on_socket_wait(int sockfd, const void *buffer,
                                      size_t len) {
  return send(sockfd, buffer, len, 0);
}

#ifdef __cplusplus
}
#endif

#endif  // __SOCKETS_H
