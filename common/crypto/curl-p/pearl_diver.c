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

typedef enum { SEARCH_RUNNING, SEARCH_INTERRUPT, SEARCH_FINISHED, SEARCH_NOT_FOUND } search_status_t;

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
      // Now initialize each thread search instance based on this `pcurl` state
      init_inst(n_procs, inst, pcurls_aligned, &pcurl, begin, end, test, param, &overall_status, &statusLock);
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

        // Copy slice found into `ctx` state
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

static size_t min__(size_t a, size_t b) { return a < b ? a : b; }

void init_inst(size_t n, SearchInstance *inst, pcurl_t *pcurls, pcurl_t *pcurl, size_t begin, size_t end,
               test_fun_t test, test_arg_t param, search_status_t volatile *overall_status,
               rw_lock_handle_t *statusLock) {
  // Size of the initial fixed value range
  size_t n_log3 = min__(ptrit_log3(PTRIT_SIZE * n), end - begin);
  trit_t value[CURL_STATE_SIZE];
  // Initial `value` is `-(3^n_log3-1)/2`
  memset(value, -1, sizeof(value));
  // Clear the variable value range
  ptrits_fill(end - (begin + n_log3), pcurl->state + (begin + n_log3), value);

  for (; n--; ++inst) {
    // Initial state value is just `pcurl` ...
    memcpy(pcurls, pcurl, sizeof(pcurl_t));
    // ... but rewritten from `begin` with values `[value .. value+PTRIT_SIZE)`, so all slices in `pcurl` state are
    // already different
    ptrit_set_iota(n_log3, pcurls->state + begin, value);

    // Start search range for `inst` from `begin + n_log3` skipping initial fixed value
    *inst = (SearchInstance){.pcurl = pcurls++,
                             .index = n,
                             .begin = begin + n_log3,
                             .end = end,
                             .test = test,
                             .param = param,
                             .result = -1,
                             .overall_status = overall_status,
                             .statusLock = statusLock};
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

    // Transform a copy
    memcpy(&copy, inst->pcurl, sizeof(pcurl_t));
    ptrit_transform(&copy);
    // Test a transformed pcurl state
    inst->result = inst->test(&copy, inst->param);

    if (inst->result >= 0) {
      rw_lock_handle_wrlock(inst->statusLock);
      *inst->overall_status = SEARCH_FINISHED;
      rw_lock_handle_unlock(inst->statusLock);

      break;
    }

    // Update/increment range
    if (ptrit_hincr(inst->end - inst->begin, inst->pcurl->state + inst->begin)) {
      // Overflow, search range exhausted, set fail status and exit thread
      inst->status = SEARCH_NOT_FOUND;
      break;
    }
  }

  return (void *)inst->result;
}
