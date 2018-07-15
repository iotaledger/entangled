/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_THREAD_HANDLE_H_
#define COMMON_THREAD_HANDLE_H_

/**
 * We define a type thread_handle_t mapping to a system available thread
 * primitive and its associated functions, some of them might have no effect if
 * not needed by the underlying API
 */

typedef void *(*thread_routine_t)(void *);

#include <unistd.h>

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_t thread_handle_t;

/**
 * Creates a new thread
 *
 * @param thread The thread
 * @param routine The routine to launch
 * @param arg Argument to give to the routine
 *
 * @return exit status
 */
static inline int thread_handle_create(thread_handle_t *const thread,
                                       thread_routine_t routine, void *arg) {
  return pthread_create(thread, NULL, routine, arg);
}

/**
 * Waits for a thread termination
 *
 * @param thread The thread
 * @param status Exit status of the thread
 *
 * @return exit status
 */
static inline int thread_handle_join(thread_handle_t thread, void **status) {
  return pthread_join(thread, status);
}

#else

#error "No thread primitive found"

#endif  // _POSIX_THREADS

#endif  // COMMON_THREAD_HANDLE_H_
