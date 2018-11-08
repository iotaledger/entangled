/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_RAND_H__
#define __UTILS_HANDLES_RAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <stdlib.h>

static inline void rand_handle_seed(unsigned int seed) { srand(seed); }

static inline int rand_handle_rand() { return rand(); }

static inline double rand_handle_probability() {
  return ((double)rand_handle_rand() / (double)RAND_MAX);
}

#else

#error "Unhandled plateform"

#endif

/**
 * Initializes random number generation with a seed
 *
 * @param seed The seed value
 */
static inline void rand_handle_seed(unsigned int seed);

/**
 * Generates a random number
 *
 * @return a random number
 */
static inline int rand_handle_rand();

/**
 * Generates a random probability in [0;1]
 *
 * @return a random probability
 */
static inline double rand_handle_probability();

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_RAND_H__
