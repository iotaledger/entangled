/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_COND_H__
#define __UTILS_HANDLES_COND_H__

/**
 * We define a type cond_handle_t mapping to a system available condition
 * variable primitive and its associated functions, some of them might have no
 * effect if not needed by the underlying API
 */

#include <unistd.h>

#include "utils/handles/lock.h"

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_cond_t cond_handle_t;

static inline int cond_handle_init(cond_handle_t* const cond) {
  return pthread_cond_init(cond, NULL);
}

static inline int cond_handle_signal(cond_handle_t* const cond) {
  return pthread_cond_signal(cond);
}

static inline int cond_handle_broadcast(cond_handle_t* const cond) {
  return pthread_cond_broadcast(cond);
}

static inline int cond_handle_wait(cond_handle_t* const cond,
                                   lock_handle_t* const lock) {
  return pthread_cond_wait(cond, lock);
}

static inline int cond_handle_timedwait(cond_handle_t* const cond,
                                        lock_handle_t* const lock,
                                        unsigned int timeout) {
  struct timespec ts = {time(NULL) + timeout, 0};
  return pthread_cond_timedwait(cond, lock, &ts);
}

static inline int cond_handle_destroy(cond_handle_t* const cond) {
  return pthread_cond_destroy(cond);
}

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
static inline int cond_handle_wait(cond_handle_t* const cond,
                                   lock_handle_t* const lock);

/**
 * Blocks the calling thread, waiting for the condition specified by cond to be
 * signaled or broadcast to, or until the timeout is reached
 *
 * @param cond The condition variable
 * @param lock The associated lock
 * @param timeout The timeout in seconds
 *
 * @return exit status
 */
static inline int cond_handle_timedwait(cond_handle_t* const cond,
                                        lock_handle_t* const lock,
                                        unsigned int timeout);

/**
 * Destroys the condition variable specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
static inline int cond_handle_destroy(cond_handle_t* const cond);

#endif  // __UTILS_HANDLES_COND_H__
