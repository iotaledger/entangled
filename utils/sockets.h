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
#endif

#ifdef __cplusplus
}
#endif

#endif  // __SOCKETS_H
