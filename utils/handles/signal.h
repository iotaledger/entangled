/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_SIGNAL_H__
#define __UTILS_HANDLES_SIGNAL_H__

/**
 * We declare a function signal_handle_register() to handle signals while the
 * program is running
 */

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)

typedef void (*signal_handle_t)(int);

static inline signal_handle_t signal_handle_register(int sig, signal_handle_t handler) { return signal(sig, handler); }

#elif defined(unix) || defined(__unix) || defined(__unix__)

typedef void (*signal_handle_t)(int);

static inline signal_handle_t signal_handle_register(int sig, signal_handle_t handler) { return signal(sig, handler); }

#elif defined(__APPLE__) || defined(__MACH__)

typedef sig_t signal_handle_t;

static inline signal_handle_t signal_handle_register(int sig, signal_handle_t handler) { return signal(sig, handler); }

#else

#error "No signal primitives found"

#endif

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HANDLES_SIGNAL_H__
