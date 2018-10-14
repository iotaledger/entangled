/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/curl-p/ptrit.h"
#include "common/curl-p/search.h"
#include "common/trinary/ptrit_incr.h"
#include "common/trinary/trit_ptrit.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"
#include "utils/system.h"

typedef enum { SEARCH_RUNNING, SEARCH_INTERRUPT, SEARCH_FINISHED } SearchStatus;

typedef struct {
  unsigned short index;
  unsigned short offset;
  unsigned short end;
  PCurl curl;
  short (*test)(PCurl *, unsigned short);
  unsigned short param;
  SearchStatus *status;
  rw_lock_handle_t *statusLock;
} SearchInstance;

void init_inst(SearchInstance *const inst, SearchStatus *const status,
               rw_lock_handle_t *const statusLock, unsigned short const index,
               PCurl *const curl, unsigned short const offset,
               unsigned short const end, unsigned short const param,
               short (*test)(PCurl *const, unsigned short const));
void pt_start(thread_handle_t *const tid, SearchInstance *const inst,
              unsigned short const index);
void *run_search_thread(void *const data);
short do_pd_search(short (*test)(PCurl *const, unsigned short const),
                   SearchInstance *const inst, PCurl *const copy);

PearlDiverStatus pd_search(Curl *const ctx, unsigned short const offset,
                           unsigned short const end,
                           short (*test)(PCurl *const, unsigned short const),
                           unsigned short const param) {
  unsigned short found_thread = 0, n_procs = system_cpu_available();

  intptr_t found_index = -1;
  SearchStatus status = SEARCH_RUNNING;
  rw_lock_handle_t statusLock;

  if (rw_lock_handle_init(&statusLock)) {
    return PEARL_DIVER_ERROR;
  }

  SearchInstance *inst = calloc(n_procs, sizeof(SearchInstance));
  thread_handle_t *tid = calloc(n_procs, sizeof(thread_handle_t));

  {
    PCurl curl;
    trits_to_ptrits(ctx->state, curl.state, STATE_LENGTH);
    ptrit_offset(&curl.state[offset], 4);
    curl.type = ctx->type;
    init_inst(inst, &status, &statusLock, n_procs, &curl, offset + 4, end,
              param, test);
  }

  pt_start(tid, inst, n_procs - 1);

  for (int i = n_procs; --i >= 0;) {
    if (tid[i] == 0) continue;

    if (found_index == -1) {
      thread_handle_join(tid[i], (void *)&found_index);

      if (found_index != -1) {
        found_thread = i;
      }
    } else {
      thread_handle_join(tid[i], NULL);
    }
  }

  switch (found_index) {
    case -1:
      free(inst);
      free(tid);

      return PEARL_DIVER_ERROR;
  }

  rw_lock_handle_destroy(&statusLock);

  ptrits_to_trits(&inst[found_thread].curl.state[offset], &ctx->state[offset],
                  found_index, end - offset);

  free(inst);
  free(tid);

  return PEARL_DIVER_SUCCESS;
}

void init_inst(SearchInstance *const inst, SearchStatus *const status,
               rw_lock_handle_t *const statusLock, unsigned short const index,
               PCurl *const curl, unsigned short const offset,
               unsigned short const end, unsigned short const param,
               short (*test)(PCurl *const, unsigned short const)) {
  if (index == 0) {
    return;
  }
  *inst = (SearchInstance){.index = index - 1,
                           .offset = offset,
                           .end = end,
                           .param = param,
                           .status = status,
                           .statusLock = statusLock,
                           .test = test};
  memcpy(&(inst->curl), curl, sizeof(PCurl));
  init_inst(&inst[1], status, statusLock, index - 1, curl, offset, end, param,
            test);
}

void pt_start(thread_handle_t *const tid, SearchInstance *const inst,
              unsigned short const index) {
  if (thread_handle_create(&tid[index], run_search_thread,
                           ((void *)&inst[index]))) {
    tid[index] = 0;
  }
  if (index == 0) {
    return;
  }
  pt_start(tid, inst, index - 1);
}

void *run_search_thread(void *const data) {
  unsigned short i;
  PCurl copy;

  SearchInstance *inst = ((SearchInstance *)data);

  for (i = 0; i < inst->index; i++) {
    ptrit_increment(inst->curl.state, inst->offset, HASH_LENGTH_TRIT);
  }

  intptr_t ret = do_pd_search(inst->test, inst, &copy);
  return (void *)ret;
}

short do_pd_search(short (*test)(PCurl *const, unsigned short const),
                   SearchInstance *const inst, PCurl *const copy) {
  short index;

  SearchStatus status = SEARCH_RUNNING;
  while (status == SEARCH_RUNNING) {
    memcpy(copy, &inst->curl, sizeof(PCurl));
    ptrit_transform(copy);
    index = test(copy, inst->param);
    if (index >= 0) {
      rw_lock_handle_wrlock(inst->statusLock);
      *inst->status = SEARCH_FINISHED;
      rw_lock_handle_unlock(inst->statusLock);

      return index;
    }
    ptrit_increment(inst->curl.state, inst->offset + 1, HASH_LENGTH_TRIT);

    rw_lock_handle_rdlock(inst->statusLock);
    status = *inst->status;
    rw_lock_handle_unlock(inst->statusLock);
  }
  return -1;
  // return do_pd_search(test, inst, copy);
}
