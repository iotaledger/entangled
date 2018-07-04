#ifndef __COMMON_NETWORK_COND_HANDLE_H__
#define __COMMON_NETWORK_COND_HANDLE_H__

/**
 * We declare a type cond_handle_t that depends on the system conditionnal
 * variable primitives
 */

#include <unistd.h>

#ifdef _POSIX_THREADS

#include <pthread.h>
typedef pthread_cond_t cond_handle_t;

#else

#error "No conditionnal variable primitives found"

#endif  // _POSIX_THREADS

/**
 * The following functions are defined contextually with macros depending on the
 * system available conditionnal variable primitives, some of them might have
 * no effect if not needed by the underlying API
 */

/**
 * Initializes a condition variable object with the specified attributes for
 * use
 *
 * @param cond The conditionnal variable
 *
 * @return exit status
 */
int cond_handle_init(cond_handle_t* const cond);

/**
 * Wakes up at least one thread that is currently waiting on the condition
 * variable specified by cond
 *
 * @param cond The conditionnal variable
 *
 * @return exit status
 */
int cond_handle_signal(cond_handle_t* const cond);

/**
 * Wakes up all threads that are currently waiting on the condition variable
 * specified by cond
 *
 * @param cond The conditionnal variable
 *
 * @return exit status
 */
int cond_handle_broadcast(cond_handle_t* const cond);

/**
 * Blocks the calling thread, waiting for the condition specified by cond to be
 * signaled or broadcast to
 *
 * @param cond The conditionnal variable
 *
 * @return exit status
 */
int cond_handle_wait(cond_handle_t* const cond);

/**
 * Destroys the condition variable specified by cond
 *
 * @param cond The conditionnal variable
 *
 * @return exit status
 */
int cond_handle_destroy(cond_handle_t* const cond);

#endif  // __COMMON_NETWORK_COND_HANDLE_H____
