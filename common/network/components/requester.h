#ifndef __COMMON_NETWORK_COMPONENTS_REQUESTER_H__
#define __COMMON_NETWORK_COMPONENTS_REQUESTER_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

typedef concurrent_queue_of_trit_array_p request_queue_t;

void request_transaction(request_queue_t *const queue,
                         trit_array_t const *const hash);
trit_array_t *get_transaction_to_request();

#endif  //__COMMON_NETWORK_COMPONENTS_REQUESTER_H__
