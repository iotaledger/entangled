#ifndef __COMMON_NETWORK_NEIGHBOR_H__
#define __COMMON_NETWORK_NEIGHBOR_H__

#include <stdint.h>
#include <stdlib.h>

typedef enum connection_type_e { TCP, UDP } connection_type_t;

typedef struct {
  char *host;
  uint16_t port;
  int socket;
  connection_type_t connection_type;
  unsigned int number_of_all_transactions;
  unsigned int number_of_new_transactions;
  unsigned int number_of_invalid_transactions;
  unsigned int number_of_sent_transactions;
  unsigned int random_transaction_requests;
  int flagged;
} neighbor_t;

int neighbor_send(neighbor_t *const neighbor, void *const packet,
                  size_t const packet_size);
int tcp_neighbor_send(neighbor_t *const neighbor, void *const packet,
                      size_t const packet_size);
int udp_neigbhor_send(neighbor_t *const neighbor, void *const packet,
                      size_t const packet_size);

#endif  // __COMMON_NETWORK_NEIGHBOR_H__
