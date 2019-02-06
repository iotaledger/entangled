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
 * @para void (*signal_handler)(): signal handler of each signal needs
 */
__sighandler_t register_signal(universal_signal_num_t SIG,
                               void (*register_signal_handler)()) {
  return signal((int)SIG, register_signal_handler);
}

#elif defined(_WIN32)

BOOL WINAPI signal_handler_WIN(DWORD dwCtrlType) {
  switch (fdwCtrlType) {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
      printf("Ctrl-C event\n\n");

      return SIGNAL_SUCCESS;
    default:
      return SIGNAL_ERROR;
  }
}

__sighandler_t register_signal(universal_signal_num_t SIG,
                               void (*register_signal_handler)()) {
  return SetConsoleCtrlHandler(signal_handler_WIN, TRUE);
}

#endif
