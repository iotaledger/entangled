/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_THREAD_H__
#define __UTILS_HANDLES_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * We define a type thread_handle_t mapping to a system available thread
 * primitive and its associated functions, some of them might have no effect if
 * not needed by the underlying API
 */

typedef void *(*thread_routine_t)(void *);

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#elif defined(_WIN32)
#include "utils/windows.h"
#endif

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_t thread_handle_t;

static inline int thread_handle_create(thread_handle_t *const thread, thread_routine_t routine, void *arg) {
  return pthread_create(thread, NULL, routine, arg);
}

static inline int thread_handle_join(thread_handle_t thread, void **status) { return pthread_join(thread, status); }

#elif defined(_WIN32)

typedef HANDLE thread_handle_t;

static inline int thread_handle_create(thread_handle_t *const thread, thread_routine_t routine, void *arg) {
  *thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)routine, arg, 0, NULL);
  return 0;
}
static inline int thread_handle_join(thread_handle_t thread, void **status) {
  WaitForSingleObject(thread, INFINITE);

  if (status) {
    *status = NULL;
    return !GetExitCodeThread(thread, (LPDWORD)status);
  }
  return 0;
}

#else

#error "No thread primitive found"

#endif  // _POSIX_THREADS

/**
 * Creates a new thread
 *
 * @param thread The thread
 * @param routine The routine to launch
 * @param arg Argument to give to the routine
 *
 * @return exit status
 */
static inline int thread_handle_create(thread_handle_t *const thread, thread_routine_t routine, void *arg);

/**
 * Waits for a thread termination
 *
 * @param thread The thread
 * @param status Exit status of the thread
 *
 * @return exit status
 */
static inline int thread_handle_join(thread_handle_t thread, void **status);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_THREAD_H__
