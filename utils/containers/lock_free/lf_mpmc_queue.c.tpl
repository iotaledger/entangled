/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "utils/containers/lock_free/lf_mpmc_queue_{TYPE}.h"

retcode_t lf_mpmc_queue_{TYPE}_init(lf_mpmc_queue_{TYPE}_t* const queue) {
  ck_fifo_mpmc_entry_t* stub = NULL;

  if (queue == NULL) {
    return RC_NULL_PARAM;
  }

  if ((stub = malloc(sizeof(ck_fifo_mpmc_entry_t))) == NULL) {
    return RC_OOM;
  }

  ck_fifo_mpmc_init(queue, stub);

  return RC_OK;
}

retcode_t lf_mpmc_queue_{TYPE}_destroy(lf_mpmc_queue_{TYPE}_t* const queue) {
  ck_fifo_mpmc_entry_t* garbage = NULL;

  if (queue == NULL) {
    return RC_NULL_PARAM;
  }

  ck_fifo_mpmc_deinit(queue, &garbage);

  if (garbage == NULL) {
    return RC_NULL_PARAM;
  }

  free(garbage);

  return RC_OK;
}

retcode_t lf_mpmc_queue_{TYPE}_enqueue(lf_mpmc_queue_{TYPE}_t* const queue,
                                       {TYPE} const* const element) {
  {TYPE}* entry = NULL;
  ck_fifo_mpmc_entry_t* fifo_entry = NULL;

  if (queue == NULL || element == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = malloc(sizeof({TYPE}))) == NULL) {
    return RC_OOM;
  }

  memcpy(entry, element, sizeof({TYPE}));

  if ((fifo_entry = malloc(sizeof(ck_fifo_mpmc_entry_t))) == NULL) {
    free(entry);
    return RC_OOM;
  }

  ck_fifo_mpmc_enqueue(queue, fifo_entry, entry);

  return RC_OK;
}

retcode_t lf_mpmc_queue_{TYPE}_dequeue(lf_mpmc_queue_{TYPE}_t* const queue,
                                       {TYPE}* const element) {
  {TYPE}* entry = NULL;
  ck_fifo_mpmc_entry_t* fifo_entry = NULL;

  if (queue == NULL || element == NULL) {
    return RC_NULL_PARAM;
  }

  if (ck_fifo_mpmc_dequeue(queue, &entry, &fifo_entry) == false) {
    return RC_NULL_PARAM;
  }

  memcpy(element, entry, sizeof({TYPE}));

  free(entry);
  free(fifo_entry);

  return RC_OK;
}

retcode_t lf_mpmc_queue_{TYPE}_trydequeue(lf_mpmc_queue_{TYPE}_t* const queue,
                                          {TYPE}* const element, bool *status) {
  {TYPE}* entry = NULL;
  ck_fifo_mpmc_entry_t* fifo_entry = NULL;

  if (queue == NULL || element == NULL) {
    return RC_NULL_PARAM;
  }

  if ((*status = ck_fifo_mpmc_trydequeue(queue, &entry, &fifo_entry)) == false) {
    return RC_OK;
  }

  memcpy(element, entry, sizeof({TYPE}));

  free(entry);
  free(fifo_entry);

  return RC_OK;
}
