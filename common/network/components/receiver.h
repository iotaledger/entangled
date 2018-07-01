#ifndef __COMMON_NETWORK_COMPONENTS_RECEIVER_H__
#define __COMMON_NETWORK_COMPONENTS_RECEIVER_H__

#include <stdint.h>

typedef struct {
  uint16_t port;
  int socket;
  int packet_size;
  void *packet;
} receiver_ctx_t;

void initialize_receiver_ctx(receiver_ctx_t *const ctx);
void *receiver_routine(void *arg);

#endif  //__COMMON_NETWORK_COMPONENTS_RECEIVER_H__
