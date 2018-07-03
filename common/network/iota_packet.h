#ifndef __COMMON_NETWORK_IOTA_PACKET_H__
#define __COMMON_NETWORK_IOTA_PACKET_H__

#include "common/network/socket.h"
#include "common/trinary/tryte.h"

#define TRANSACTION_PACKET_SIZE 1650

typedef struct {
  tryte_t data[TRANSACTION_PACKET_SIZE];
  socket_addr_t source;
} iota_packet_t;

#endif  // __COMMON_NETWORK_IOTA_PACKET_H__
