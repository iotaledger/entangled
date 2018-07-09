/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_COND_HANDLE_H_
#define COMMON_COND_HANDLE_H_

/**
 * We declare a type cond_handle_t that depends on the system condition variable
 * primitives
 */

#include <unistd.h>

#include "lock_handle.h"

#ifdef _POSIX_THREADS

#include <pthread.h>
typedef pthread_cond_t cond_handle_t;

#else

#error "No condition variable primitives found"

#endif  // _POSIX_THREADS

/**
 * The following functions are defined contextually with macros depending on the
 * system available condition variable primitives, some of them might have no
 * effect if not needed by the underlying API
 */

/**
 * Initializes a condition variable object with the specified attributes for use
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
int cond_handle_init(cond_handle_t* const cond);

/**
 * Wakes up at least one thread that is currently waiting on the condition
 * variable specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
int cond_handle_signal(cond_handle_t* const cond);

/**
 * Wakes up all threads that are currently waiting on the condition variable
 * specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
int cond_handle_broadcast(cond_handle_t* const cond);

/**
 * Blocks the calling thread, waiting for the condition specified by cond to be
 * signaled or broadcast to
 *
 * @param cond The condition variable
 * @param lock The associated lock
 *
 * @return exit status
 */
int cond_handle_wait(cond_handle_t* const cond, lock_handle_t* const lock);

/**
 * Destroys the condition variable specified by cond
 *
 * @param cond The condition variable
 *
 * @return exit status
 */
int cond_handle_destroy(cond_handle_t* const cond);

#endif  // COMMON_COND_HANDLE_H_
