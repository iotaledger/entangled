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

typedef enum UNIVERSAL_SIGNAL_NUM { null, ctrl_c } universal_signal_num_t;

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>
#define CTRL_C_SIGNAL SIGINT
#define SIGNAL_ERROR SIG_ERR

void signal_handler_posix(int signo);
void (*handler)(universal_signal_num_t);

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler)(universal_signal_num_t)) {
  handler = signal_handler;
  return signal(SIG, signal_handler_posix);
}

#elif defined(_WIN32)
#include <Windows.h>

#define CTRL_C_SIGNAL CTRL_C_EVENT
#define SIGNAL_ERROR FALSE

BOOL signal_handler_WIN(DWORD dwCtrlType);
void (*handler)(universal_signal_num_t);

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler)(universal_signal_num_t)) {
  handler = signal_handler;
  return SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler_WIN, TRUE);
}

#else

#error "No signal process library found"

#endif  // __unix__
        /*
         * Register signal
         *
         * @param the signal(interrupt) we hope to be caught
         *
         * @param the function stop and destroy core
         *
         * @return signal_error if singal_handler went wrong
         */

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler)(universal_signal_num_t));

#endif  // __UTILS_HANDLES_SIGNAL_H__
