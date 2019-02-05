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
 *  This enum is declared for consistent name of signal for c5 ross platfrom
 * developing the naming order is followed by POSIX.1-1990 the standard signals
 * table is at http://man7.org/linux/man-pages/man7/signal.7.html
 *
 */

typedef enum UNIVERSAL_SIGNAL_NUM {
  null,
  signal_hang_up,
  ctrl_c
} universal_signal_num_t;

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>
#define CTRL_C_SIGNAL SIGINT
#define SIGNAL_ERROR SIG_ERR

#elif defined(_WIN32)

#include <Windows.h>

#define CTRL_C_SIGNAL CTRL_C_EVENT
#define SIGNAL_ERROR FALSE

BOOL signal_handler_WIN(DWORD dwCtrlType);

#else

#error "No signal process library found"

#endif  // __unix__
        /*
         * Register signal, call register_signal wherever we need to catch signals
         *
         * @param the signal(interrupt) we hope to be caught
         *
         * @param the function used to be signal_handler i.e. function used to stop and
         * destroy core
         *
         * @return signal_error if singal_handler went wrong
         */

__sighandler_t register_signal(universal_signal_num_t SIG,
                               bool (*register_signal_handler)());

#endif  // __UTILS_HANDLES_SIGNAL_H__
