/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_LOCK_H__
#define __UTILS_HANDLES_LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * We define a type lock_handle_t mapping to a system available lock primitive
 * and its associated functions, some of them might have no effect if not needed
 * by the underlying API
 */

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#elif defined(_WIN32)
#include "utils/windows.h"
#endif

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_mutex_t lock_handle_t;

static inline int lock_handle_init(lock_handle_t* const lock) { return pthread_mutex_init(lock, NULL); }

static inline int lock_handle_lock(lock_handle_t* const lock) { return pthread_mutex_lock(lock); }

static inline int lock_handle_unlock(lock_handle_t* const lock) { return pthread_mutex_unlock(lock); }

static inline int lock_handle_destroy(lock_handle_t* const lock) { return pthread_mutex_destroy(lock); }

#elif defined(_WIN32)

typedef CRITICAL_SECTION lock_handle_t;

static inline int lock_handle_init(lock_handle_t* const lock) {
  InitializeCriticalSection(lock);
  return 0;
}
static inline int lock_handle_lock(lock_handle_t* const lock) {
  EnterCriticalSection(lock);
  return 0;
}
static inline int lock_handle_unlock(lock_handle_t* const lock) {
  LeaveCriticalSection(lock);
  return 0;
}
static inline int lock_handle_destroy(lock_handle_t* const lock) {
  DeleteCriticalSection(lock);
  return 0;
}

#else

#error "No lock primitive found"

#endif  // _POSIX_THREADS

/**
 * Initializes a lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_init(lock_handle_t* const lock);

/**
 * Acquires ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_lock(lock_handle_t* const lock);

/**
 * Releases ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_unlock(lock_handle_t* const lock);

/**
 * Destroys the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int lock_handle_destroy(lock_handle_t* const lock);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_LOCK_H__
