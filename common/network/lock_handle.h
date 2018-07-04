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
int initialize(lock_handle_t* const lock);

/**
 * Locks the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int lock(lock_handle_t* const lock);

/**
 * Unlocks the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int unlock(lock_handle_t* const lock);

/**
 * Destroy the lock
 *
 * @param lock The lock
 *
 * @return exit status
 */
int destroy(lock_handle_t* const lock);

#endif  // __COMMON_NETWORK_LOCK_HANDLE_H____
