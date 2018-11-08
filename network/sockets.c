/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include "sockets.h"

int open_client_socket(char const *const hostname, const size_t port) {
  struct addrinfo hints, *serverinfo, *info;
  char port_string[6];
  int sockfd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;  // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;
  sprintf(port_string, "%lu", port);
  if (getaddrinfo(hostname, port_string, &hints, &serverinfo) != 0) {
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

void close_socket(int sockfd) {
  if (sockfd != -1) {
    close(sockfd);
  }
}

int receive_on_socket_wait(int sockfd, void *buffer, size_t len) {
  return recv(sockfd, buffer, len, 0);
}

int send_on_socket_wait(int sockfd, const void *buffer, size_t len) {
  return send(sockfd, buffer, len, 0);
}
