/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HANDLES_SIG_H__
#define __UTILS_HANDLES_SIG_H__

/**
 * We declare a function register_signal() to a system available condition
 * variable primitive and its associated functions, some of them might have no
 * effect if not needed by the underlying API
 */

#include <signal.h>
void signal_handler_core_end(int signo);

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>
#define ctrl_c_signal SIGINT
#define signal_error SIG_ERR

void signal_handler_posix(int signo);

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler_core_end)(int)) {
  return signal(SIG, signal_handler_posix);
}

#elif defined(_WIN32)
#include <Windows.h>

#define ctrl_c_signal CTRL_C_EVENT
#define signal_error FALSE

BOOL signal_handler_WIN(DWORD dwCtrlType);

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler_core_end)(int)) {
  return SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler_WIN, TRUE);
}

#else

#error "No lock primitive found"

#endif  // __unix__
/*
 * Register signal
 *
 * @param the signal(interrupt) eed to be caught
 *
 * @param the function stop and destroy core
 *
 * @return signal_error if singal_handler went wrong
 */

static inline __sighandler_t register_signal(
    int SIG, void (*signal_handler_core_end)(int));

#endif  // __UTILS_HANDLES_SIG_H__