/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "signal.h"

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

void signal_handler_posix(int signo) {
  if (signo == CTRL_C_SIGNAL) {
    handler(ctrl_c);
  }
}

#elif defined(_WIN32)

BOOL signal_handler_WIN(DWORD dwCtrlType) {
  if (dwCtrlType == CTRL_C_SIGNAL) {
    handler(ctrl_c);
    return TRUE;
  }
  return signal_error;
}

#endif
