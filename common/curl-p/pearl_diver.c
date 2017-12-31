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
  pthread_rwlock_t *rwlock;
  SearchStatus *status;
} SearchInstance;

void *run_search_thread(void *);
void pt_start(pthread_t *const, SearchInstance *const, unsigned short);
short do_pd_search(short (*)(PCurl *, unsigned short), SearchInstance *const,
                   PCurl *const);
void init_inst(SearchInstance *const, SearchStatus *, pthread_rwlock_t *,
               unsigned short, PCurl *const, unsigned short, unsigned short,
               unsigned short, short (*)(PCurl *, unsigned short));

void join_all(pthread_t *const tid, unsigned short index, unsigned short *,
              short *);

PearlDiverStatus pd_search(Curl *const ctx, unsigned short offset,
                           unsigned short end,
                           short (*test)(PCurl *, unsigned short),
                           unsigned short param) {
  unsigned short found_thread = 0, n_procs = sysconf(_SC_NPROCESSORS_ONLN);
  signed short found_index = -1;
  SearchInstance inst[n_procs];
  SearchStatus status = SEARCH_RUNNING;

  pthread_t tid[n_procs];
  pthread_rwlock_t rwlock;
  pthread_rwlock_init(&rwlock, NULL);

  {
    PCurl curl;
    trits_to_ptrits(ctx->state, curl.state, STATE_LENGTH);
    curl.type = ctx->type;
    init_inst(inst, &status, &rwlock, n_procs, &curl, offset, end, param, test);
  }

  pt_start(tid, inst, n_procs - 1);

  join_all(tid, n_procs - 1, &found_thread, &found_index);
  pthread_rwlock_destroy(&rwlock);

  switch (found_index) {
    case -1:
      return PEARL_DIVER_ERROR;
  }

  ptrits_to_trits(&inst[found_thread].curl.state[offset], &ctx->state[offset],
                  found_index, end - offset);

  return PEARL_DIVER_SUCCESS;
}

void init_inst(SearchInstance *const inst, SearchStatus *status,
               pthread_rwlock_t *rwlock, unsigned short index,
               PCurl *const curl, unsigned short offset, unsigned short end,
               unsigned short param, short (*test)(PCurl *, unsigned short)) {
  if (index == 0) {
    return;
  }
  *inst = (SearchInstance){.index = index - 1,
                           .offset = offset,
                           .end = end,
                           .param = param,
                           .status = status,
                           .rwlock = rwlock,
                           .test = test};
  memcpy(&(inst->curl), curl, sizeof(PCurl));
  init_inst(&inst[1], status, rwlock, index - 1, curl, offset, end, param,
            test);
}

void pt_start(pthread_t *const tid, SearchInstance *const inst,
              unsigned short index) {
  if (pthread_create(&tid[index], NULL, run_search_thread,
                     ((void *)&inst[index]))) {
    // tid[thread_count] = 0;
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
  return (void *)do_pd_search(inst->test, inst, &copy);
}

short do_pd_search(short (*test)(PCurl *, unsigned short), SearchInstance *inst,
                   PCurl *copy) {
  SearchStatus status;
  status = *inst->status;
  while (status == SEARCH_RUNNING) {
    pthread_rwlock_rdlock(inst->rwlock);
    pthread_rwlock_unlock(inst->rwlock);
    memcpy(copy, &inst->curl, sizeof(PCurl));
    ptrit_transform(copy);
    short index = test(copy, inst->param);
    if (index >= 0) {
      pthread_rwlock_wrlock(inst->rwlock);
      *inst->status = SEARCH_FINISHED;
      pthread_rwlock_unlock(inst->rwlock);
      return index;
    }
    ptrit_increment(inst->curl.state, inst->offset + 1, HASH_LENGTH);
  }
  return -1;
  // return do_pd_search(test, inst, copy);
}

void join_all(pthread_t *const tid, unsigned short index,
              unsigned short *found_thread, short *found_index) {
  short int join_result;
  switch (*found_index) {
    case -1:
      join_result = pthread_join(tid[index], (void *)found_index);
      if (*found_index >= 0) {
        *found_thread = index;
      }
      break;
    default:
      join_result = pthread_join(tid[index], NULL);
  }
  if (join_result || index == 0) {
    return;
  }
  join_all(tid, index - 1, found_thread, found_index);
}
