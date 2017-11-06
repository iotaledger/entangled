#include "pearl_diver.h"
#include "constants.h"
#include "hash.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) && !defined(__MINGW32__)
#include <intrin.h>
#else
#include <sched.h>
#endif


#define OFFSET_LENGTH 4
#define NONCE_START HASH_LENGTH - NONCE_LENGTH
#define NONCE_INIT_START NONCE_START + OFFSET_LENGTH
#define NONCE_INCREMENT_START NONCE_INIT_START + INT_LENGTH

typedef struct {
  States* states;
  char* trits;
  int min_weight_magnitude;
  int threadIndex;
  PearlDiver* ctx;
} PDThread;

#if defined(_WIN32) && !defined(__MINGW32__)
DWORD WINAPI find_nonce(void* data);
#else
void* find_nonce(void* states);
#endif

void interrupt(PearlDiver* ctx) {
  pthread_mutex_lock(&ctx->new_thread_search);
  if (ctx->status == PD_SEARCHING) {
    ctx->status = PD_INTERRUPTED;
  }
  pthread_mutex_unlock(&ctx->new_thread_search);
}

void pd_search(
    PearlDiver* ctx,
    curl_t *const curl,
    const int min_weight_magnitude,
    int numberOfThreads
    ) {
  int k, thread_count;

  if (min_weight_magnitude < 0 ||
      min_weight_magnitude > HASH_LENGTH) {
    ctx->status = PD_INVALID;

#ifdef DEBUG
    fprintf(stderr, "E: Invalid arguments.\n");
#endif
    return;
  }

  ctx->status = PD_SEARCHING;

  States states;
  pd_search_init(&states, curl, HASH_LENGTH - NONCE_LENGTH);

  if (numberOfThreads <= 0) {

#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    numberOfThreads = sysinfo.dwNumberOfProcessors;
#else
    numberOfThreads = sysconf(_SC_NPROCESSORS_ONLN) - 1;
#endif
    if (numberOfThreads < 1)
      numberOfThreads = 1;
  }

  pthread_mutex_init(&ctx->new_thread_search, NULL);
  pthread_t tid[numberOfThreads];

  PDThread pdthreads[numberOfThreads];
  thread_count = 0;
#ifdef DEBUG
  fprintf(stderr, "I: Starting search threads.\n");
#endif
  while (thread_count < numberOfThreads) {

    pdthreads[thread_count] =
        (PDThread){.states = &states,
                   .trits = curl->state,
                   .min_weight_magnitude = min_weight_magnitude,
                   .threadIndex = thread_count,
                   .ctx = ctx};
    if(pthread_create(&tid[thread_count], NULL, &find_nonce,
                       (void*)&(pdthreads[thread_count]))) {
      //tid[thread_count] = 0;
    }
    thread_count++;
  }

  for (k = 0; k < thread_count; k++) {
    // Could be that thread creation has failed.
    if(tid[k]) {
      pthread_join(tid[k], NULL);
    }
  }

#ifdef DEBUG
  fprintf(stderr, "I: Found threads. Returning.\n");
#endif
  return; // ctx->status == PD_INTERRUPTED;
}

void pd_search_init(States* states, curl_t* curl, size_t offset) {
  int i;
  for (i = 0; i < STATE_LENGTH; i++) {
    switch (curl->state[i]) {
      case 0: {
                states->mid_low[i] = HIGH_BITS;
                states->mid_high[i] = HIGH_BITS;
              } break;
      case 1: {
                states->mid_low[i] = LOW_BITS;
                states->mid_high[i] = HIGH_BITS;
              } break;
      default: {
                 states->mid_low[i] = HIGH_BITS;
                 states->mid_high[i] = LOW_BITS;
               }
    }
  }
  states->mid_low[offset] = LOW_0;
  states->mid_high[offset] = HIGH_0;
  states->mid_low[offset + 1] = LOW_1;
  states->mid_high[offset + 1] = HIGH_1;
  states->mid_low[offset + 2] = LOW_2;
  states->mid_high[offset + 2] = HIGH_2;
  states->mid_low[offset + 3] = LOW_3;
  states->mid_high[offset + 3] = HIGH_3;
      
}

int is_found(bc_trit_t* low, bc_trit_t* high, int index, int min_weight_magnitude) {
  int i;
  for (i = min_weight_magnitude; i-- > 0;) {
    if ((((bc_trit_t)(low[HASH_LENGTH - 1 - i])) & (1 << index)) !=
        (((bc_trit_t)(high[HASH_LENGTH - 1 - i])) & (1 << index))) {
      return 0;
    }
  }
  return 1;
}

int is_found_fast(bc_trit_t* low, bc_trit_t* high, int min_weight_magnitude) {
  int i;
  bc_trit_t lastMeasurement = HIGH_BITS; /* (low[index] >> bitIndex & 1) !=
                                         (high[index] >> bitIndex & 1) */
  for (i = min_weight_magnitude; i-- > 0;) {
    lastMeasurement &= ~(low[HASH_LENGTH - 1 - i] ^ high[HASH_LENGTH - 1 - i]);
    if (lastMeasurement == 0) {
      return 0;
    }
  }
  return lastMeasurement;
}

char ctxStatusEq(PDThread* thread, PearlDiver* ctx, int cmp) {
#if defined(_WIN32) && !defined(__MINGW32__)
  EnterCriticalSection(&thread->ctx->new_thread_search);
#else
  pthread_mutex_lock(&thread->ctx->new_thread_search);
#endif

  char eq = ctx->status == cmp;
  
#if defined(_WIN32) && !defined(__MINGW32__)
  LeaveCriticalSection(&thread->ctx->new_thread_search);
#else
  pthread_mutex_unlock(&thread->ctx->new_thread_search);
#endif

  return eq;
}

#if defined(_WIN32) && !defined(__MINGW32__)
DWORD WINAPI find_nonce(void* data) {
#else
void* find_nonce(void* data) {
#endif
  bc_trit_t midStateCopyLow[STATE_LENGTH], midStateCopyHigh[STATE_LENGTH];
  int i, shift;
  bc_trit_t nonce_probe, nonce_output;
  PDThread* my_thread = (PDThread*)data;
  char* trits = my_thread->trits;

  memset(midStateCopyLow, 0, STATE_LENGTH * sizeof(bc_trit_t));
  memset(midStateCopyHigh, 0, STATE_LENGTH * sizeof(bc_trit_t));
  PearlDiver* ctx = my_thread->ctx;
  memcpy(midStateCopyLow, my_thread->states->mid_low,
         STATE_LENGTH * sizeof(bc_trit_t));
  memcpy(midStateCopyHigh, my_thread->states->mid_high,
         STATE_LENGTH * sizeof(bc_trit_t));

  for (i = my_thread->threadIndex; i-- > 0;) {
    pd_increment(midStateCopyLow, midStateCopyHigh, NONCE_INIT_START,
        NONCE_INCREMENT_START);
  }

  bc_trit_t scratchpadLow[STATE_LENGTH], scratchpadHigh[STATE_LENGTH],
      stateLow[STATE_LENGTH], stateHigh[STATE_LENGTH];
  memset(stateLow, 0, STATE_LENGTH * sizeof(bc_trit_t));
  memset(stateHigh, 0, STATE_LENGTH * sizeof(bc_trit_t));
  memset(scratchpadLow, 0, STATE_LENGTH * sizeof(bc_trit_t));
  memset(scratchpadHigh, 0, STATE_LENGTH * sizeof(bc_trit_t));

  while (ctxStatusEq(my_thread, ctx, PD_SEARCHING)) {
    pd_increment(midStateCopyLow, midStateCopyHigh, NONCE_INCREMENT_START,
                 HASH_LENGTH);
    memcpy(stateLow, midStateCopyLow, STATE_LENGTH * sizeof(bc_trit_t));
    memcpy(stateHigh, midStateCopyHigh, STATE_LENGTH * sizeof(bc_trit_t));
    pd_transform(stateLow, stateHigh, scratchpadLow, scratchpadHigh);

    if ((nonce_probe = is_found_fast(stateLow, stateHigh,
                                     my_thread->min_weight_magnitude)) == 0)
      continue;

#if defined(_WIN32) && !defined(__MINGW32__)
#ifdef _WIN64
    _BitScanForward64(&shift, nonce_probe);
#else
    _BitScanForward(&shift, nonce_probe);
#endif
    nonce_output = 1 << shift;
    EnterCriticalSection(&my_thread->ctx->new_thread_search);
#else
    shift = __builtin_ctzll(nonce_probe);
    nonce_output = 1 << shift;
    pthread_mutex_lock(&my_thread->ctx->new_thread_search);
#endif

    if (ctx->status != PD_FOUND) {
      ctx->status = PD_FOUND;
      for (i = 0; i < HASH_LENGTH; i++) {
        trits[i] =
            (((bc_trit_t)(midStateCopyLow[i]) & nonce_output) == 0)
                ? 1
                : ((((bc_trit_t)(midStateCopyHigh[i]) & nonce_output) == 0) ? -1
                                                                         : 0);
      }
    }

#if defined(_WIN32) && !defined(__MINGW32__)
    LeaveCriticalSection(&my_thread->ctx->new_thread_search);
#else
    pthread_mutex_unlock(&my_thread->ctx->new_thread_search);
#endif

    return 0;
  }
  return 0;
}

void pd_transform(bc_trit_t* const stateLow, bc_trit_t* const stateHigh,
                  bc_trit_t* const scratchpadLow, bc_trit_t* const scratchpadHigh) {

  int scratchpadIndex = 0, round, stateIndex;
  bc_trit_t alpha, beta, gamma, delta;

  for (round = NUMBER_OF_ROUNDS; round-- > 0;) {
    memcpy(scratchpadLow, stateLow, STATE_LENGTH * sizeof(bc_trit_t));
    memcpy(scratchpadHigh, stateHigh, STATE_LENGTH * sizeof(bc_trit_t));

    for (stateIndex = 0; stateIndex < STATE_LENGTH; stateIndex++) {

      alpha = scratchpadLow[scratchpadIndex];
      beta = scratchpadHigh[scratchpadIndex];
      gamma = scratchpadHigh[scratchpadIndex +=
                             (scratchpadIndex < 365 ? 364 : -365)];
      delta = (alpha | (~gamma)) & (scratchpadLow[scratchpadIndex] ^ beta);

      stateLow[stateIndex] = ~delta;
      stateHigh[stateIndex] = (alpha ^ gamma) | delta;
    }
  }
}
void pd_increment(bc_trit_t* const mid_low, bc_trit_t* const mid_high,
                  const int from_index, const int to_index) {

  size_t i;
  bc_trit_t carry = 1;
  bc_trit_t low, hi;
  for (i = from_index; i < to_index && carry != 0; i++) {
    low = mid_low[i];
    hi = mid_high[i];
    mid_low[i] = hi ^ low;
    mid_high[i] = low;
    carry = hi & (~low);
  }
}
