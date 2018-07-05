/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_LOCK_HANDLE_H__
#define __COMMON_NETWORK_LOCK_HANDLE_H__

/**
 * We declare a type lock_handle_t that depends on the system available lock
 * primitives
 */

#include <unistd.h>

#ifdef _POSIX_THREADS

#include <pthread.h>
typedef pthread_rwlock_t lock_handle_t;

#else

#error "No thread primitives found"

#endif  // _POSIX_THREADS

/**
 * The following functions are defined contextually with macros depending on the
 * system available lock primitives, some of them might have no effect if not
 * needed by the underlying API
 */

/**
 * Initializes a lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int lock_handle_init(lock_handle_t* const lock);

/**
 * Acquires ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int lock_handle_lock(lock_handle_t* const lock);

/**
 * Releases ownership of the given lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int lock_handle_unlock(lock_handle_t* const lock);

/**
 * Destroys the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int lock_handle_destroy(lock_handle_t* const lock);

#endif  // __COMMON_NETWORK_LOCK_HANDLE_H____
