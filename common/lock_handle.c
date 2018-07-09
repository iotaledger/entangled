/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unistd.h>

#include "lock_handle.h"

#ifdef _POSIX_THREADS

int lock_handle_init(lock_handle_t* const lock) {
  return pthread_rwlock_init(lock, NULL);
}

int lock_handle_lock(lock_handle_t* const lock) {
  return pthread_rwlock_wrlock(lock);
}

int lock_handle_rdlock(lock_handle_t* const lock) {
  return pthread_rwlock_rdlock(lock);
}

int lock_handle_wrlock(lock_handle_t* const lock) {
  return pthread_rwlock_wrlock(lock);
}

int lock_handle_unlock(lock_handle_t* const lock) {
  return pthread_rwlock_unlock(lock);
}

int lock_handle_destroy(lock_handle_t* const lock) {
  return pthread_rwlock_destroy(lock);
}

#else

#error "No thread primitives found"

#endif  // _POSIX_THREADS
