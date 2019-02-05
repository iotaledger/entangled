/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/handles/signal.h"

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

/*
 * @para universal_signal_num_t SIG: assign the signal number which is going to
 * handle.
 * @para bool (*signal_handler)(): signal handler of each signal needs
 */
__sighandler_t register_signal(universal_signal_num_t SIG,
                               bool (*register_signal_handler)()) {
  return signal((int)SIG, register_signal_handler);
}

#elif defined(_WIN32)

BOOL signal_handler_WIN(DWORD dwCtrlType) {
  universal_signal_num_t uni_signo = null;
  if (dwCtrlType == CTRL_C_SIGNAL) {
    uni_signo = ctrl_c;
  }

  if (uni_signo != null) {
    signal_handler(uni_signo);
    return TRUE;
  }

  return signal_error;
}

static inline __sighandler_t register_signal(int SIG) {
  return SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler_WIN, TRUE);
}

#endif
