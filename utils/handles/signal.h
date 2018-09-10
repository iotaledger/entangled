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
#include <stdlib.h>

typedef enum UNIVERSAL_SINGAL_NUM {
    null,
    ctrl_c
} universal_singal_num_t;

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>
#define ctrl_c_signal SIGINT
#define signal_error SIG_ERR

void signal_handler_posix(int signo);
void (*end_core)(universal_singal_num_t);

static inline __sighandler_t register_signal(
    int SIG, void (*handler_core_end)(universal_singal_num_t)) {
        end_core = handler_core_end;
  return signal(SIG, signal_handler_posix);
}

#elif defined(_WIN32)
#include <Windows.h>

#define ctrl_c_signal CTRL_C_EVENT
#define signal_error FALSE

BOOL signal_handler_WIN(DWORD dwCtrlType);
void (*end_core)(universal_singal_num_t);

static inline __sighandler_t register_signal(
    int SIG, void (*handler_core_end)(universal_singal_num_t)) {
        end_core = handler_core_end;
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
    int SIG, void (*handler_core_end)(universal_singal_num_t));

#endif  // __UTILS_HANDLES_SIG_H__