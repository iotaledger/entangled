/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/crypto/curl-p/ptrit.h"
#include "common/crypto/curl-p/search.h"
#include "common/trinary/ptrit_incr.h"
#include "common/trinary/trit_ptrit.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"
#include "utils/system.h"

typedef enum { SEARCH_RUNNING, SEARCH_INTERRUPT, SEARCH_FINISHED } search_status_t;

typedef struct {
  size_t index;
  pcurl_t pcurl;
  size_t begin;
  size_t end;
  test_fun_t test;
  test_arg_t param;
  test_result_t result;
  search_status_t status;
  search_status_t volatile *overall_status;
  rw_lock_handle_t *statusLock;
} SearchInstance;

void init_inst(size_t n, SearchInstance *const inst, pcurl_t *pcurl, size_t begin, size_t end, test_fun_t test,
               test_arg_t param, search_status_t volatile *overall_status, rw_lock_handle_t *statusLock);
void pt_start(size_t n, thread_handle_t *tid, SearchInstance *inst);
void *run_search_thread(void *const data);

PearlDiverStatus pd_search(Curl const *ctx, size_t begin, size_t end, test_fun_t test, test_arg_t param) {
  PearlDiverStatus pd_status = PEARL_DIVER_ERROR;
  size_t n_procs = system_cpu_available();
  search_status_t overall_status;
  rw_lock_handle_t statusLock;
  SearchInstance *inst = NULL;
  thread_handle_t *tid = NULL;

  do {
    if (end < begin + PTRIT_SIZE_LOG3) {
      break;
    }

    if (rw_lock_handle_init(&statusLock)) {
      break;
    }

    inst = (SearchInstance *)calloc(n_procs, sizeof(SearchInstance));
    if (NULL == inst) {
      break;
    }

    tid = (thread_handle_t *)calloc(n_procs, sizeof(thread_handle_t));
    if (NULL == tid) {
      break;
    }

    // Initialize search instances
    {
      pcurl_t pcurl;
      ptrit_curl_init(&pcurl, ctx->type);
      // `pcurl` state is filled with the initial `ctx` state
      ptrits_fill(STATE_LENGTH, pcurl.state, ctx->state);
      // but rewritten from `begin` with values [N,N+1,...,N+PTRIT_SIZE) for some N, so all slices in `pcurl` state are
      // already different
      ptrits_set_iota(pcurl.state + begin);
      // now initialize each thread search instance with a shorter search range [begin + PTRIT_SIZE_LOG3, end)
      init_inst(n_procs, inst, &pcurl, begin + PTRIT_SIZE_LOG3, end, test, param, &overall_status, &statusLock);
    }

    overall_status = SEARCH_RUNNING;
    pt_start(n_procs, tid, inst);

    SearchInstance const *found_inst = NULL;
    for (size_t i = n_procs; i--;) {
      if (tid[i] == 0) continue;

      test_result_t result = -1;
      thread_handle_join(tid[i], (void **)&result);

      if (!found_inst && result >= 0) {
        size_t found_index = (size_t)result;
        found_inst = inst + i;
        pd_status = PEARL_DIVER_SUCCESS;

        ptrits_set_slice(end - begin, inst->pcurl.state + begin, found_index, ctx->state + begin);
      }
    }
  } while (0);

  rw_lock_handle_destroy(&statusLock);

  free(inst);
  free(tid);

  return pd_status;
}

void init_inst(size_t n, SearchInstance *const inst, pcurl_t *pcurl, size_t begin, size_t end, test_fun_t test,
               test_arg_t param, search_status_t volatile *overall_status, rw_lock_handle_t *statusLock) {
  for (; n--;) {
    *inst = (SearchInstance){.index = n,
                             .begin = begin,
                             .end = end,
                             .test = test,
                             .param = param,
                             .result = -1,
                             .overall_status = overall_status,
                             .statusLock = statusLock};
    memcpy(&(inst->pcurl), pcurl, sizeof(pcurl_t));
    // in order to make search instances different from the initial `pcurl` state, `pcurl` state is incremented starting
    // from `begin`
    ptrit_increment(pcurl->state, begin, HASH_LENGTH_TRIT);
  }
}

void pt_start(size_t n, thread_handle_t *tid, SearchInstance *inst) {
  for (; n--; ++tid, ++inst) {
    if (thread_handle_create(tid, run_search_thread, (void *)inst)) {
      // TODO: check thread_handle_create
      *tid = 0;
    }
  }
}

void *run_search_thread(void *const data) {
  SearchInstance *inst = ((SearchInstance *)data);
  pcurl_t copy;

  for (;;) {
    // TODO: why rdlock is needed?
    // `*inst->overall_status` is assigned with a correct value
    rw_lock_handle_rdlock(inst->statusLock);
    inst->status = *inst->overall_status;
    rw_lock_handle_unlock(inst->statusLock);

    if (inst->status != SEARCH_RUNNING) {
      break;
    }

    memcpy(&copy, &inst->pcurl, sizeof(pcurl_t));
    ptrit_transform(&copy);
    // test a transformed pcurl state
    // TODO: make sure test returns negative result if failed
    inst->result = inst->test(&copy, inst->param);

    if (inst->result >= 0) {
      // TODO: why wrlock is needed?
      // `*inst->overall_status` is only assigned `SEARCH_FINISHED`
      // if it were to be assigned another overall_status it should be checked within the lock
      rw_lock_handle_wrlock(inst->statusLock);
      // TODO: check for other overall_status values
      *inst->overall_status = SEARCH_FINISHED;
      rw_lock_handle_unlock(inst->statusLock);

      break;
    }

    // super strange why pcurl state is incremented from begin+1
    // can't they overlap???
    ptrit_increment(inst->pcurl.state, inst->begin + 1, HASH_LENGTH_TRIT);
  }

  return (void *)inst->result;
}
