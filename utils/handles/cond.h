/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_COND_H__
#define __UTILS_HANDLES_COND_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * We define a type cond_handle_t mapping to a system available condition
 * variable primitive and its associated functions, some of them might have no
 * effect if not needed by the underlying API
 */

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>
#include <unistd.h>
#elif defined(_WIN32)
#include "utils/windows.h"
#endif

#include <stdint.h>

#include "utils/handles/lock.h"

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_cond_t cond_handle_t;

static inline int cond_handle_init(cond_handle_t* const cond) { return pthread_cond_init(cond, NULL); }

static inline int cond_handle_signal(cond_handle_t* const cond) { return pthread_cond_signal(cond); }

static inline int cond_handle_broadcast(cond_handle_t* const cond) { return pthread_cond_broadcast(cond); }

static inline int cond_handle_wait(cond_handle_t* const cond, lock_handle_t* const lock) {
  return pthread_cond_wait(cond, lock);
}

static inline int cond_handle_timedwait(cond_handle_t* const cond, lock_handle_t* const lock,
                                        uint64_t const timeout_ms) {
  struct timespec ts;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  ts.tv_sec = tv.tv_sec + timeout_ms / 1000ULL;
  ts.tv_nsec = tv.tv_usec * 1000ULL + (timeout_ms % 1000ULL) * 1000000ULL;
  if (ts.tv_nsec >= 1000000000LL) {
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000ULL;
  }

  return pthread_cond_timedwait(cond, lock, &ts);
}

static inline int cond_handle_destroy(cond_handle_t* const cond) { return pthread_cond_destroy(cond); }

#elif defined(_WIN32)

typedef CONDITION_VARIABLE cond_handle_t;

static inline int cond_handle_init(cond_handle_t* const cond) {
  InitializeConditionVariable(cond);
  return 0;
}

static inline int cond_handle_signal(cond_handle_t* const cond) {
  WakeConditionVariable(cond);
  return 0;
}

static inline int cond_handle_broadcast(cond_handle_t* const cond) {
  WakeAllConditionVariable(cond);
  return 0;
}

static inline int cond_handle_wait(cond_handle_t* const cond, lock_handle_t* const lock) {
  SleepConditionVariableCS(cond, lock, INFINITE);
  return 0;
}

static inline int cond_handle_timedwait(cond_handle_t* const cond, lock_handle_t* const lock,
                                        uint64_t const timeout_ms) {
  if (!SleepConditionVariableCS(cond, lock, timeout_ms)) {
    return ETIMEDOUT;
  }
  return 0;
}

static inline int cond_handle_destroy(cond_handle_t* const cond) { return 0; }

#else

#error "No condition variable primitive found"

#endif  // _POSIX_THREADS

/**
 * Initializes a condition variable object with the specified attributes for use
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
static inline int cond_handle_init(cond_handle_t* const cond);

/**
 * Wakes up at least one thread that is currently waiting on the condition
 * variable specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
static inline int cond_handle_signal(cond_handle_t* const cond);

/**
 * Wakes up all threads that are currently waiting on the condition variable
 * specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
static inline int cond_handle_broadcast(cond_handle_t* const cond);

/**
 * Blocks the calling thread, waiting for the condition specified by cond to be
 * signaled or broadcast to
 *
 * @param cond The condition variable
 * @param lock The associated lock
 *
 * @return exit status
 */
static inline int cond_handle_wait(cond_handle_t* const cond, lock_handle_t* const lock);

/**
 * Blocks the calling thread, waiting for the condition specified by cond to be
 * signaled or broadcast to, or until the timeout is reached
 *
 * @param cond The condition variable
 * @param lock The associated lock
 * @param timeout The timeout in milliseconds
 *
 * @return exit status
 */
static inline int cond_handle_timedwait(cond_handle_t* const cond, lock_handle_t* const lock,
                                        uint64_t const timeout_ms);

/**
 * Destroys the condition variable specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
static inline int cond_handle_destroy(cond_handle_t* const cond);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_COND_H__
