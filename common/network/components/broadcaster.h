#ifndef __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
#define __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__

#include "common/network/concurrent_queue_transaction.h"
#include "common/trinary/tryte.h"

void broadcaster_on_next(tryte_t const *const hash);
void *broadcaster_routine(void *arg);

extern concurrent_queue_of_iota_transaction_t broadcaster_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
