/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/lock_free/lf_queue_{TYPE}.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void iota_lf_umm_queue_{TYPE}_init_owner(iota_lf_umm_queue_{TYPE}_t* const queue,
                                         uint32_t element_size) {
  lfds711_queue_umm_init_valid_on_current_logical_core(
      &queue->queue, &queue->queue_element_dummy, NULL);

  lfds711_freelist_init_valid_on_current_logical_core(&queue->freelist, NULL, 0,
                                                      NULL);
}

void iota_lf_umm_queue_{TYPE}_init_user(iota_lf_umm_queue_{TYPE}_t* const queue) {
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
}

void* iota_lf_umm_queue_{TYPE}_free(iota_lf_umm_queue_{TYPE}_t* const queue) {
  struct lfds711_freelist_element* fe;
  iota_lf_queue_umm_{TYPE}_t* queue_element;
  if (queue == NULL) {
    return;
  }

  //Dequeue will add all allocated queued element to the freelist
  while (iota_lf_umm_queue_{TYPE}_dequeue(queue));
  lfds711_queue_umm_cleanup(&queue->queue, NULL);

  while (lfds711_freelist_pop(&queue->freelist, &fe, NULL)) {
    queue_element = LFDS711_FREELIST_GET_VALUE_FROM_ELEMENT(*fe);
    free(queue_element);
  }

  lfds711_freelist_cleanup(&queue->freelist, NULL);
}

retcode_t iota_lf_umm_queue_{TYPE}_enqueue(iota_lf_umm_queue_{TYPE}_t* const queue,
        {TYPE} const* const data) {
  iota_lf_queue_umm_{TYPE}_t* p = NULL;
  bool reused_element = false;

  if (queue == NULL || data == NULL) {
    return RC_NULL_PARAM;
  }

  struct lfds711_freelist_element* fe;
  if (lfds711_freelist_pop(&queue->freelist, &fe, NULL)) {
    reused_element = true;
    p = LFDS711_FREELIST_GET_VALUE_FROM_ELEMENT(*fe);
  } else if ((p = (iota_lf_queue_umm_{TYPE}_t*)malloc(sizeof(iota_lf_queue_umm_{TYPE}_t))) == NULL) {
    return RC_OOM;
  }

  if (!reused_element){
   LFDS711_FREELIST_SET_VALUE_IN_ELEMENT(p->freelist_element, p);
  }

  memcpy(&p->data, data, sizeof(p->data));
  LFDS711_QUEUE_UMM_SET_VALUE_IN_ELEMENT(p->queue_element, p);
  lfds711_queue_umm_enqueue(&queue->queue, &p->queue_element);

  return RC_OK;
}

iota_lf_queue_umm_{TYPE}_t* iota_lf_umm_queue_{TYPE}_dequeue(
        iota_lf_umm_queue_{TYPE}_t* const queue) {
  struct lfds711_queue_umm_element* qe;
  iota_lf_queue_umm_{TYPE}_t* p = NULL;

  if (queue == NULL) {
    return NULL;
  }

  if (!lfds711_queue_umm_dequeue(&queue->queue, &qe)) {
    return NULL;
  }

  p = LFDS711_QUEUE_UMM_GET_VALUE_FROM_ELEMENT(*qe);
  lfds711_freelist_push(&queue->freelist, &p->freelist_element, NULL);

  return p;
}

size_t iota_lf_umm_queue_{TYPE}_count(const iota_lf_umm_queue_{TYPE}_t* const queue) {
  lfds711_pal_uint_t count;
  lfds711_queue_umm_query(&queue->queue,
                          LFDS711_QUEUE_UMM_QUERY_SINGLETHREADED_GET_COUNT,
                          NULL, &count);
  return count;
}
