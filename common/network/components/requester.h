#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

void request_transaction(trit_array_t const *const hash);
trit_array_t *get_transaction_to_request();

extern concurrent_queue_of_trit_array_p requester_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
