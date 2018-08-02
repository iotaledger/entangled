/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_RW_LOCK_HANDLE_H_
#define COMMON_RW_LOCK_HANDLE_H_

/**
 * We define a type rw_lock_handle_t mapping to a system available read/write
 * lock primitive and its associated functions, some of them might have no
 * effect if not needed by the underlying API
 */

#include <unistd.h>

#ifdef _POSIX_THREADS

#include <pthread.h>

typedef pthread_rwlock_t rw_lock_handle_t;

static inline int rw_lock_handle_init(rw_lock_handle_t* const lock) {
  return pthread_rwlock_init(lock, NULL);
}

static inline int rw_lock_handle_rdlock(rw_lock_handle_t* const lock) {
  return pthread_rwlock_rdlock(lock);
}

static inline int rw_lock_handle_wrlock(rw_lock_handle_t* const lock) {
  return pthread_rwlock_wrlock(lock);
}

static inline int rw_lock_handle_unlock(rw_lock_handle_t* const lock) {
  return pthread_rwlock_unlock(lock);
}

static inline int rw_lock_handle_destroy(rw_lock_handle_t* const lock) {
  return pthread_rwlock_destroy(lock);
}

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

#endif  // COMMON_RW_LOCK_HANDLE_H_
