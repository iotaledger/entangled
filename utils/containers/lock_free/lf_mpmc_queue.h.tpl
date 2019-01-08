/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_LOCK_FREE_LF_MPMC_QUEUE_{TYPE}_H__
#define __UTILS_CONTAINERS_LOCK_FREE_LF_MPMC_QUEUE_{TYPE}_H__

#include <ck_fifo.h>

#include "common/errors.h"

#define LF_MPMC_QUEUE_IS_EMPTY(f) CK_FIFO_MPMC_ISEMPTY(f)
#define LF_MPMC_QUEUE_FIRST(f) CK_FIFO_MPMC_FIRST(f)
#define LF_MPMC_QUEUE_NEXT(f) CK_FIFO_MPMC_NEXT(m)
#define LF_MPMC_QUEUE_FOREACH(fifo, entry) CK_FIFO_MPMC_FOREACH(fifo, entry)
#define LF_MPMC_QUEUE_FOREACH_SAFE(fifo, entry, T) \
  CK_FIFO_MPMC_FOREACH_SAFE(fifo, entry, T)

#ifdef __cplusplus
extern "C" {
#endif

// A generic concurrent lock-free Multi Producer Multi Consumer queue
typedef ck_fifo_mpmc_t lf_mpmc_queue_{TYPE}_t;

/**
 * Initializes a lock-free MPMC queue
 *
 * @param queue The queue
 *
 * @return a status code
 */

retcode_t lf_mpmc_queue_{TYPE}_init(lf_mpmc_queue_{TYPE}_t* const queue);

/**
 * Destroys a lock-free MPMC queue
 *
 * @param queue The queue
 *
 * @return a status code
 */

retcode_t lf_mpmc_queue_{TYPE}_destroy(lf_mpmc_queue_{TYPE}_t* const queue);

/**
 * Enqueues an element in a lock-free MPMC queue
 *
 * @param queue The queue
 * @param element The element to enqueue
 *
 * @return a status code
 */

retcode_t lf_mpmc_queue_{TYPE}_enqueue(lf_mpmc_queue_{TYPE}_t* const queue,
                                       {TYPE} const* const element);

/**
 * Dequeues an element from a lock-free MPMC queue
 *
 * @param queue The queue
 * @param element The element to dequeue into
 *
 * @return a status code
 */

retcode_t lf_mpmc_queue_{TYPE}_dequeue(lf_mpmc_queue_{TYPE}_t* const queue,
                                       {TYPE} * const element);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LOCK_FREE_LF_MPMC_QUEUE_{TYPE}_H__
