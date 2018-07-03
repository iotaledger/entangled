#ifndef __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
#define __COMMON_NETWORK_COMPONENTS_BROADCASTER_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

void broadcaster_on_next(concurrent_queue_of_trit_array_p *const queue,
                         trit_array_t const *const hash);
void *broadcaster_routine(void *arg);

#endif  //__COMMON_NETWORK_COMPONENTS_BROADCASTER_H__
