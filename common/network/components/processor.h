#ifndef __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
#define __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__

#include "common/network/concurrent_queue_transaction.h"
#include "common/trinary/tryte.h"

void processor_on_next(tryte_t const *const hash);
void *processor_routine(void *arg);

extern concurrent_queue_of_iota_transaction_t processor_queue;

#endif  //__COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
