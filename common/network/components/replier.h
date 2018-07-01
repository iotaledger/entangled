#ifndef __COMMON_NETWORK_COMPONENTS_REPLIER_H__
#define __COMMON_NETWORK_COMPONENTS_REPLIER_H__

#include "common/network/concurrent_queue_hash_request.h"
#include "common/trinary/tryte.h"

void replier_on_next(tryte_t const *const hash);
void *replier_routine(void *arg);

extern concurrent_queue_of_hash_request_t replier_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_REPLIER_H__
