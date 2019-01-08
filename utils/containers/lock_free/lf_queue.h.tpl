/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/*
 * UMM - unbounded queue, multiple producers, multiple consumers
 */

#ifndef __UTILS_CONTAINERS_LOCK_FREE_LF_QUEUE_UMM_{TYPE}_H__
#define __UTILS_CONTAINERS_LOCK_FREE_LF_QUEUE_UMM_{TYPE}_H__

#include <stddef.h>
#include <stdint-gcc.h>

#include <liblfds711.h>
#include "common/errors.h"
#include "utils/containers/lock_free/queue_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_lf_queue_umm_{TYPE}_s {
  {TYPE} data;
  struct lfds711_queue_umm_element queue_element;
  struct lfds711_freelist_element freelist_element;
} iota_lf_queue_umm_{TYPE}_t;

typedef struct {
    struct lfds711_queue_umm_element queue_element_dummy;
    struct lfds711_queue_umm_state queue;
    struct lfds711_freelist_state freelist;
} iota_lf_umm_queue_{TYPE}_t;

/**
 * Init owner function
 * should be called only from the owner thread
 *
 * @param queue The queue to initialize
 * @param element_size The queued element's size
 *
 * @return void
 */

void iota_lf_umm_queue_{TYPE}_init_owner(iota_lf_umm_queue_t* const queue);

/**
 * Init user function
 * should be called from any of the queue users
 *
 * @param queue The queue to initialize
 *
 * @return void
 */

void iota_lf_umm_queue_{TYPE}_init_user(iota_lf_umm_queue_t* const queue);

/**
 * Frees the queue
 *
 * @param queue The queue
 *
 * @return void
 */

void* iota_lf_umm_queue_{TYPE}_free(iota_lf_umm_queue_t* const queue);

/**
 * Enqueues an element
 *
 * @param queue The queue
 * @param element The element to queue
 *
 * @return error code
 */

retcode_t iota_lf_umm_queue_{TYPE}_enqueue(iota_lf_umm_queue_t* const queue,
                                            {TYPE} const* const data);

/**
 * Dequeues an element
 *
 * @param queue The queue
 *
 * @return the dequeued element or NULL
 */

iota_lf_queue_umm_{TYPE}_t* iota_lf_umm_queue_{TYPE}_dequeue(
        iota_lf_umm_queue_t* const queue);

/**
 * count number of elements
 *
 * @param queue The queue
 *
 * @return the number of elements
 */

size_t iota_lf_umm_queue_{TYPE}_count(const iota_lf_umm_queue_t* const queue);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LOCK_FREE_LF_QUEUE_UMM_{TYPE}_H__
