#ifndef __COMMON_NETWORK_CONCURRENT_QUEUE_TRYTES_H__
#define __COMMON_NETWORK_CONCURRENT_QUEUE_TRYTES_H__

#include "common/trinary/tryte.h"

#include "concurrent_queue.h.inc"

typedef tryte_t* tryte_p;

DECLARE_CONCURRENT_QUEUE_OF(tryte_p);

#endif  // __COMMON_NETWORK_CONCURRENT_QUEUE_TRYTES_H__
