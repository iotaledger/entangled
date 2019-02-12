/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_SIGNAL_H__
#define __UTILS_HANDLES_SIGNAL_H__

/**
 * We declare a function register_signal() to handle signals while the program
 * executing. This function will catch some exceptional behavior with the
 * program.
 */

#include <signal.h>
#include <stdlib.h>
#include "utils/logger_helper.h"

/*
 *  This enum is declared for consistent name of signal for cross platfrom
 * developing the naming order is followed by POSIX.1-1990 the standard signals
 * table is at http://man7.org/linux/man-pages/man7/signal.7.html
 *
 */

typedef enum UNIVERSAL_SIGNAL_NUM {
  null,
  signal_hang_up,
  ctrl_c
} universal_signal_num_t;

#define SIGNAL_ERROR SIG_ERR

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>

/*
 * Call regiter_signal with assigned signal_handler, then the signal handler can
 * work
 *
 * @para universal_signal_num_t sig: signal(interrupt) we hope to be caught
 *
 * @para void (*signal_handler)(): signal handler of each signal needs
 *
 * @return signal_error if singal_handler went wrong
 */
static inline __sighandler_t register_signal(
    universal_signal_num_t sig, void (*register_signal_handler)()) {
  return signal((int)sig, register_signal_handler);
}

#elif defined(_WIN32)

typedef void (*SignalHandlerPointer)(int);

SignalHandlerPointer register_signal(universal_signal_num_t sig,
                                     void (*register_signal_handler)()) {
  return signal((int)sig, register_signal_handler);
}

#else

#error "No signal process library found"

#endif  // platform validate

#endif  // __UTILS_HANDLES_SIGNAL_H__
