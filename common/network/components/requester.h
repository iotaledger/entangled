#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include "common/network/queues/concurrent_queue_trytes.h"

void request_transaction(tryte_t *const hash);
tryte_t *get_transaction_to_request();

extern concurrent_queue_of_tryte_p requester_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
