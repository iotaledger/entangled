/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_RW_LOCK_H__
#define __UTILS_HANDLES_RW_LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * We define a type rw_lock_handle_t mapping to a system available read/write
 * lock primitive and its associated functions, some of them might have no
 * effect if not needed by the underlying API
 */
#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__) || defined(__XTENSA__))
#include <unistd.h>
#elif defined(_WIN32)
#include "utils/windows.h"
#endif

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_rwlock_t rw_lock_handle_t;

static inline int rw_lock_handle_init(rw_lock_handle_t* const lock) { return pthread_rwlock_init(lock, NULL); }

static inline int rw_lock_handle_rdlock(rw_lock_handle_t* const lock) { return pthread_rwlock_rdlock(lock); }

static inline int rw_lock_handle_wrlock(rw_lock_handle_t* const lock) { return pthread_rwlock_wrlock(lock); }

static inline int rw_lock_handle_unlock(rw_lock_handle_t* const lock) { return pthread_rwlock_unlock(lock); }

static inline int rw_lock_handle_destroy(rw_lock_handle_t* const lock) { return pthread_rwlock_destroy(lock); }

#elif defined(_WIN32)

typedef SRWLOCK rw_lock_handle_t;

static inline int rw_lock_handle_init(rw_lock_handle_t* const lock) {
  InitializeSRWLock(lock);
  return 0;
}

static inline int rw_lock_handle_rdlock(rw_lock_handle_t* const lock) {
  AcquireSRWLockShared(lock);
  return 0;
}

static inline int rw_lock_handle_wrlock(rw_lock_handle_t* const lock) {
  AcquireSRWLockExclusive(lock);
  return 0;
}

static inline int rw_lock_handle_unlock(rw_lock_handle_t* const lock) {
  void* state = *(void**)lock;

  if (state == (void*)1) {
    ReleaseSRWLockExclusive(lock);
  } else {
    ReleaseSRWLockShared(lock);
  }
  return 0;
}

static inline int rw_lock_handle_destroy(rw_lock_handle_t* const lock) { return 0; }

#else

#error "No read/write lock primitive found"

#endif  // _POSIX_THREADS

/**
 * Initializes a read/write lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int rw_lock_handle_init(rw_lock_handle_t* const lock);

/**
 * Acquires reading ownership of the given read/write lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int rw_lock_handle_rdlock(rw_lock_handle_t* const lock);

/**
 * Acquires writing ownership of the given read/write lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int rw_lock_handle_wrlock(rw_lock_handle_t* const lock);

/**
 * Releases ownership of the given read/write lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int rw_lock_handle_unlock(rw_lock_handle_t* const lock);

/**
 * Destroys the read/write lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
static inline int rw_lock_handle_destroy(rw_lock_handle_t* const lock);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_RW_LOCK_H__
