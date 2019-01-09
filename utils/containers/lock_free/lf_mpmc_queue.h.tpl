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
{INCLUDE}

#define LF_MPMC_QUEUE_IS_EMPTY(queue) CK_FIFO_MPMC_ISEMPTY(&(queue)->fifo)
#define LF_MPMC_QUEUE_FIRST(queue) CK_FIFO_MPMC_FIRST(&(queue)->fifo)
#define LF_MPMC_QUEUE_NEXT(queue) CK_FIFO_MPMC_NEXT(&(queue)->fifo)
#define LF_MPMC_QUEUE_FOREACH(queue, entry) CK_FIFO_MPMC_FOREACH(&(queue)->fifo, entry)
#define LF_MPMC_QUEUE_FOREACH_SAFE(queue, entry, T) \
  CK_FIFO_MPMC_FOREACH_SAFE(&(queue)->fifo, entry, T)

#ifdef __cplusplus
extern "C" {
#endif

// A generic concurrent lock-free Multi Producer Multi Consumer queue
typedef struct lf_mpmc_queue_{TYPE}_s {
  ck_fifo_mpmc_t fifo;
  size_t element_size;
} lf_mpmc_queue_{TYPE}_t;

/**
 * Initializes a lock-free MPMC queue
 *
 * @param queue The queue
 * @param element_size The size of an element
 *
 * @return a status code
 */
retcode_t lf_mpmc_queue_{TYPE}_init(lf_mpmc_queue_{TYPE}_t* const queue, size_t const element_size);

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

/**
 * Tries to dequeue an element from a lock-free MPMC queue
 *
 * @param queue The queue
 * @param element The element to dequeue into
 *
 * @return a status code
 */
retcode_t lf_mpmc_queue_{TYPE}_trydequeue(lf_mpmc_queue_{TYPE}_t* const queue,
                                          {TYPE} * const element, bool * const status);

/**
 * Counts the number of elements in a lock-free MPMC queue
 *
 * @param queue The queue
 *
 * @return the number of elements
 */
static inline size_t lf_mpmc_queue_{TYPE}_count(lf_mpmc_queue_{TYPE}_t const * const queue) {
  size_t count = 0;
  ck_fifo_mpmc_entry_t* entry = NULL;

  if (queue == NULL) {
    return 0;
  }

  LF_MPMC_QUEUE_FOREACH(queue, entry) {
    count++;
  }

  return count;
}

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LOCK_FREE_LF_MPMC_QUEUE_{TYPE}_H__
