/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_CAS_H__
#define __UTILS_HANDLES_CAS_H__

#include <stdbool.h>

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#elif defined(_WIN32)
#include <Windows.h>
#endif
/*
 * Taken from:
 * https://stackoverflow.com/questions/1158374/portable-compare-and-swap-atomic-operations-c-c-library/1189634#1189634
 */

#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) > 40100
static inline bool compare_and_swap(volatile void** ptr, void* comp_val,
                                    void* new_val) {
  return __sync_bool_compare_and_swap(ptr, comp_val, new_val);
}
#elif defined(_MSC_VER)
static inline bool compare_and_swap(volatile void** ptr, void* comp_val,
                                    void* new_val) {
  if (InterlockedCompareExchangePointer(ptr, new_val, comp_val) == comp_val) {
    return true;
  }
  return false;
}
#elif __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050
static inline bool compare_and_swap(volatile void** ptr, void* comp_val,
                                    void* new_val) {
  return OSAtomicCompareAndSwapPtr(comp_val, new_val, ptr);
}

#error "No atomic swap primitives found"

#endif

/**
 * Atomically compare and swap pointers
 *
 * @param ptr - The pointer to swap
 * @param comp_val - The pointer's value the atomic swap should compare against
 * @param new_val - The value to assign to the ptr
 *
 * @return True if swap succeeded
 */

static inline bool compare_and_swap(volatile void** ptr, void* comp_val,
                                    void* new_val);

#endif  // __UTILS_HANDLES_CAS_H__
