#ifndef __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
#define __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

void processor_on_next(trit_array_t const *const hash);
void *processor_routine(void *arg);

extern concurrent_queue_of_trit_array_p processor_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
