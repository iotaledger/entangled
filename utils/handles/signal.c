/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/handles/signal.h"

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#elif defined(_WIN32)

static inline BOOL WINAPI signal_handler_WIN(DWORD dwCtrlType) {
  switch (fdwCtrlType) {
    case CTRL_C_EVENT:
      // TODO get the funtion with map(ctrl_c)

      log_info(logger_id, "Ctrl-C event\n\n");

      return SIGNAL_SUCCESS;
    default:
      return SIGNAL_ERROR;
  }
}

#endif
