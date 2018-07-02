#ifndef __COMMON_NETWORK_COMPONENTS_RESPONDER_H__
#define __COMMON_NETWORK_COMPONENTS_RESPONDER_H__

#include "common/network/queues/concurrent_queue_hash_request.h"

void responder_on_next(trit_array_t const *const hash,
                       neighbor_t *const neighbor);
void *responder_routine(void *arg);

extern concurrent_queue_of_hash_request_t responder_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_RESPONDER_H__
