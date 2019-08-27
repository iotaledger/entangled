/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

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
  pcurl_t *pcurl;  // must be aligned, hence allocated separately
  size_t index;
  size_t begin;
  size_t end;
  test_fun_t test;
  test_arg_t param;
  test_result_t result;
  search_status_t status;
  search_status_t volatile *overall_status;
  rw_lock_handle_t *statusLock;
} SearchInstance;

static void init_inst(size_t n, SearchInstance *inst, pcurl_t *pcurls, pcurl_t *pcurl, size_t begin, size_t end,
                      test_fun_t test, test_arg_t param, search_status_t volatile *overall_status,
                      rw_lock_handle_t *statusLock);
static void pt_start(size_t n, thread_handle_t *tid, SearchInstance *inst);
static void *run_search_thread(void *const data);

static PearlDiverStatus pd_search_n(Curl *ctx, size_t begin, size_t end, test_fun_t test, test_arg_t param,
                                    size_t n_procs) {
  PearlDiverStatus pd_status = PEARL_DIVER_ERROR;
  search_status_t overall_status;
  rw_lock_handle_t statusLock;
  pcurl_t *pcurls = NULL;
  pcurl_t *pcurls_aligned = NULL;
  SearchInstance *inst = NULL;
  thread_handle_t *tid = NULL;

  do {
    if (end < begin + PTRIT_SIZE_LOG3 + 1) {
      break;
    }

    if (rw_lock_handle_init(&statusLock)) {
      break;
    }

    uintptr_t const pad = PTRIT_SIZE / 8 - 1;
    pcurls = (pcurl_t *)malloc(n_procs * sizeof(pcurl_t) + (size_t)pad);
    if (NULL == pcurls) {
      break;
    }
    pcurls_aligned = (pcurl_t *)(((uintptr_t)pcurls + pad) & ~pad);

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
      init_inst(n_procs, inst, pcurls_aligned, &pcurl, begin + PTRIT_SIZE_LOG3, end, test, param, &overall_status,
                &statusLock);
    }

    overall_status = SEARCH_RUNNING;
    pt_start(n_procs, tid, inst);

    SearchInstance const *found_inst = NULL;
    for (size_t i = n_procs; i--;) {
      if (tid[i] == 0) continue;

      test_result_t result = -1;
      thread_handle_join(tid[i], (void **)&result);
      result = inst[i].result;

      if (!found_inst && result >= 0) {
        size_t found_index = (size_t)result;
        found_inst = inst + i;
        pd_status = PEARL_DIVER_SUCCESS;

        ptrits_get_slice(end - begin, ctx->state + begin, found_inst->pcurl->state + begin, found_index);
      }
    }
  } while (0);

  rw_lock_handle_destroy(&statusLock);

  free(tid);
  free(inst);
  free(pcurls);

  return pd_status;
}

PearlDiverStatus pd_search(Curl *ctx, size_t begin, size_t end, test_fun_t test, test_arg_t param) {
  size_t n_procs = system_cpu_available();
  return pd_search_n(ctx, begin, end, test, param, n_procs);
}

void init_inst(size_t n, SearchInstance *inst, pcurl_t *pcurls, pcurl_t *pcurl, size_t begin, size_t end,
               test_fun_t test, test_arg_t param, search_status_t volatile *overall_status,
               rw_lock_handle_t *statusLock) {
  for (; n--; ++inst) {
    *inst = (SearchInstance){.pcurl = pcurls++,
                             .index = n,
                             .begin = begin,
                             .end = end,
                             .test = test,
                             .param = param,
                             .result = -1,
                             .overall_status = overall_status,
                             .statusLock = statusLock};
    memcpy(inst->pcurl, pcurl, sizeof(pcurl_t));
    // in order to make search instances different from the initial `pcurl` state, `pcurl` state is incremented starting
    // from `begin`
    ptrit_increment(pcurl->state, begin, HASH_LENGTH_TRIT);
  }
}

void pt_start(size_t n, thread_handle_t *tid, SearchInstance *inst) {
  for (; n--; ++tid, ++inst) {
    if (thread_handle_create(tid, run_search_thread, (void *)inst)) {
      *tid = 0;
    }
  }
}

void *run_search_thread(void *const data) {
  SearchInstance *inst = ((SearchInstance *)data);
  pcurl_t copy;

  for (;;) {
    rw_lock_handle_rdlock(inst->statusLock);
    inst->status = *inst->overall_status;
    rw_lock_handle_unlock(inst->statusLock);

    if (inst->status != SEARCH_RUNNING) {
      break;
    }

    memcpy(&copy, inst->pcurl, sizeof(pcurl_t));
    ptrit_transform(&copy);
    // test a transformed pcurl state
    inst->result = inst->test(&copy, inst->param);

    if (inst->result >= 0) {
      rw_lock_handle_wrlock(inst->statusLock);
      *inst->overall_status = SEARCH_FINISHED;
      rw_lock_handle_unlock(inst->statusLock);

      break;
    }

    // TODO: super strange why pcurl state is incremented from begin+1
    // can't they overlap???
    // TODO: end = HASH_LENGTH_TRIT?
    ptrit_increment(inst->pcurl->state, inst->begin + 1, inst->end);
  }

  return (void *)inst->result;
}
