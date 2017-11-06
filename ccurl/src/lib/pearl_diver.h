
#ifndef PEARLDIVER_H
#define PEARLDIVER_H

#include "curl.h"

#if defined(_WIN32)
#include <windows.h>
#endif
#if defined(_WIN32) && !defined(__MINGW32__)
#else
#include <pthread.h>
#endif
#include "hash.h"
#include <stdbool.h>

#define Invalid_transaction_trits_length 0x63
#define Invalid_min_weight_magnitude 0x64
#define InterruptedException 0x65
//#define HIGH_BITS
//0b1111111111111111111111111111111111111111111111111111111111111111L
//#define LOW_BITS
//0b0000000000000000000000000000000000000000000000000000000000000000L
#define HIGH_BITS 0xFFFFFFFFFFFFFFFF
#define LOW_BITS 0x0000000000000000
#define LOW_0 0xDB6DB6DB6DB6DB6D
#define HIGH_0 0xB6DB6DB6DB6DB6DB
#define LOW_1 0xF1F8FC7E3F1F8FC7
#define HIGH_1 0x8FC7E3F1F8FC7E3F
#define LOW_2 0x7FFFE00FFFFC01FF
#define HIGH_2 0xFFC01FFFF803FFFF
#define LOW_3 0xFFC0000007FFFFFF
#define HIGH_3 0x003FFFFFFFFFFFFF

#if defined(_WIN32) && !defined(__MINGW32__)
#define pthread_mutex_init(A, B) InitializeCriticalSection(A)
#define pthread_mutex_destroy(A) DeleteCriticalSection(A)
#define pthread_t HANDLE
#define pthread_create(A, B, C, D) *A = CreateThread(B, 0, C, D, 0, NULL)
#define sched_yield() SwitchToThread()
#define pthread_join(A, B) WaitForSingleObject(A, INFINITE)
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_unlock(A) LeaveCriticalSection(A)
// http://stackoverflow.com/questions/800383/what-is-the-difference-between-mutex-and-critical-section
#define pthread_mutex_lock(A) EnterCriticalSection(A)
#endif

typedef struct {
  bc_trit_t mid_low[STATE_LENGTH];
  bc_trit_t mid_high[STATE_LENGTH];
  bc_trit_t low[STATE_LENGTH];
  bc_trit_t high[STATE_LENGTH];
} States;

typedef enum {
  PD_FINISHED,
  PD_SEARCHING,
  PD_FAILED,
  PD_FOUND,
  PD_INTERRUPTED,
  PD_INVALID
} PDStatus;

typedef struct {
  PDStatus status;
  pthread_mutex_t new_thread_search;
} PearlDiver;

void init_pearldiver(PearlDiver* ctx);
void interrupt(PearlDiver* ctx);
void pd_search(PearlDiver* ctx, curl_t* const curl,
               const int min_weight_magnitude, int numberOfThreads);
void pd_transform(bc_trit_t* const stateLow, bc_trit_t* const stateHigh,
                  bc_trit_t* const scratchpadLow, bc_trit_t* const scratchpadHigh);
void pd_increment(bc_trit_t* const midStateCopyLow, bc_trit_t* const midStateCopyHigh,
                  const int fromIndex, const int toIndex);
void pd_search_init(States* states, curl_t *const curl, size_t offset);

#endif
