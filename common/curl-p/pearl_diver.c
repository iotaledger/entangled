#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common/trinary/ptrit_incr.h"
#include "common/trinary/trit_ptrit.h"

#include "ptrit.h"
#include "search.h"

typedef enum { SEARCH_RUNNING, SEARCH_INTERRUPT, SEARCH_FINISHED } SearchStatus;

typedef struct {
  unsigned short index;
  unsigned short offset;
  unsigned short end;
  PCurl curl;
  short (*test)(PCurl *, unsigned short);
  unsigned short param;
  SearchStatus *status;
  pthread_rwlock_t *statusLock;
} SearchInstance;

void *run_search_thread(void *);
void pt_start(pthread_t *const, SearchInstance *const, unsigned short);
short do_pd_search(short (*)(PCurl *, unsigned short), SearchInstance *const,
                   PCurl *const);
void init_inst(SearchInstance *const, SearchStatus *, pthread_rwlock_t*, unsigned short,
               PCurl *const, unsigned short, unsigned short, unsigned short,
               short (*)(PCurl *, unsigned short));

PearlDiverStatus pd_search(Curl *const ctx, unsigned short offset,
                           unsigned short end,
                           short (*test)(PCurl *, unsigned short),
                           unsigned short param) {
  unsigned short found_thread = 0, n_procs = sysconf(_SC_NPROCESSORS_ONLN);
  intptr_t found_index = -1;
  SearchInstance inst[n_procs];
  SearchStatus status = SEARCH_RUNNING;
  pthread_rwlock_t statusLock;
  pthread_t tid[n_procs];

  if(pthread_rwlock_init(&statusLock, NULL)) {
    return PEARL_DIVER_ERROR;
  }


  {
    PCurl curl;
    trits_to_ptrits(ctx->state, curl.state, STATE_LENGTH);
    ptrit_offset(&curl.state[offset], 4);
    curl.type = ctx->type;
    init_inst(inst, &status, &statusLock, n_procs, &curl, offset + 4, end, param, test);
  }

  pt_start(tid, inst, n_procs - 1);

  for (int i = n_procs; --i >= 0;) {
    if (tid[i] == 0) continue;

    if (found_index < 0) {
      pthread_join(tid[i], (void *)&found_index);

      if (found_index >= 0) {
        found_thread = i;
      }
    } else {
      pthread_join(tid[i], NULL);
    }
  }

  switch (found_index) {
    case -1:
      return PEARL_DIVER_ERROR;
  }

  pthread_rwlock_destroy(&statusLock);

  ptrits_to_trits(&inst[found_thread].curl.state[offset], &ctx->state[offset],
                  found_index, end - offset);

  return PEARL_DIVER_SUCCESS;
}

void init_inst(SearchInstance *const inst, SearchStatus *status, pthread_rwlock_t *statusLock,
               unsigned short index, PCurl *const curl, unsigned short offset,
               unsigned short end, unsigned short param,
               short (*test)(PCurl *, unsigned short)) {
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
  init_inst(&inst[1], status, statusLock, index - 1, curl, offset, end, param, test);
}

void pt_start(pthread_t *const tid, SearchInstance *const inst,
              unsigned short index) {
  if (pthread_create(&tid[index], NULL, run_search_thread,
                     ((void *)&inst[index]))) {
    tid[index] = 0;
  }
  if (index == 0) {
    return;
  }
  pt_start(tid, inst, index - 1);
}

void *run_search_thread(void *data) {
  unsigned short i;
  PCurl copy;

  SearchInstance *inst = ((SearchInstance *)data);

  for (i = 0; i < inst->index; i++) {
    ptrit_increment(inst->curl.state, inst->offset, HASH_LENGTH);
  }
  return (void *)(intptr_t)do_pd_search(inst->test, inst, &copy);
}

short do_pd_search(short (*test)(PCurl *, unsigned short), SearchInstance *inst,
                   PCurl *copy) {
  short index;

  SearchStatus status = SEARCH_RUNNING;
  while (status == SEARCH_RUNNING) {
    memcpy(copy, &inst->curl, sizeof(PCurl));
    ptrit_transform(copy);
    index = test(copy, inst->param);
    if (index >= 0) {
      pthread_rwlock_wrlock(inst->statusLock);
      *inst->status = SEARCH_FINISHED;
      pthread_rwlock_unlock(inst->statusLock);

      return index;
    }
    ptrit_increment(inst->curl.state, inst->offset + 1, HASH_LENGTH);

    pthread_rwlock_rdlock(inst->statusLock);
    status = *inst->status;
    pthread_rwlock_unlock(inst->statusLock);
  }
  return -1;
  // return do_pd_search(test, inst, copy);
}
