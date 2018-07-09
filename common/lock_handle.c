/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "lock_handle.h"

#ifdef _POSIX_THREADS

int lock_handle_init(lock_handle_t* const lock) {
  return pthread_mutex_init(lock, NULL);
}

int lock_handle_lock(lock_handle_t* const lock) {
  return pthread_mutex_lock(lock);
}

int lock_handle_unlock(lock_handle_t* const lock) {
  return pthread_mutex_unlock(lock);
}

int lock_handle_destroy(lock_handle_t* const lock) {
  return pthread_mutex_destroy(lock);
}

#else

#error "No lock primitives found"

#endif  // _POSIX_THREADS
