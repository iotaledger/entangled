/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cond_handle.h"

#ifdef _POSIX_THREADS

int cond_handle_init(cond_handle_t* const cond) {
  return pthread_cond_init(cond, NULL);
}

int cond_handle_signal(cond_handle_t* const cond) {
  return pthread_cond_signal(cond);
}

int cond_handle_broadcast(cond_handle_t* const cond) {
  return pthread_cond_broadcast(cond);
}

int cond_handle_wait(cond_handle_t* const cond, lock_handle_t* const lock) {
  return pthread_cond_wait(cond, lock);
}

int cond_handle_destroy(cond_handle_t* const cond) {
  return pthread_cond_destroy(cond);
}

#else

#error "No condition variable primitives found"

#endif  // _POSIX_THREADS
