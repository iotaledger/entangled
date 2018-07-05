/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
#define __COMMON_NETWORK_COMPONENTS_PROCESSOR_H__

#include "common/network/queues/concurrent_queue_trit_array.h"

typedef concurrent_queue_of_trit_array_p processor_queue_t;

void processor_on_next(processor_queue_t *const queue,
                       trit_array_t const *const hash);
void *processor_routine(void *arg);

#endif  //__COMMON_NETWORK_COMPONENTS_PROCESSOR_H__
